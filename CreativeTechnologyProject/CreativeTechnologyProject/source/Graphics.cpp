#include "..\includes\Graphics.h"
#include "dxerr/dxerr.h"
#include <sstream>
#include "imgui/imgui_impl_dx11.h" 
#include "imgui/imgui_impl_win32.h" 

#define GFX_THROW_FAILED(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )

namespace wrl = Microsoft::WRL;

Graphics::Graphics(HWND hWnd)
{
	//Descriptor for swap chain
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferDesc.Width = 0; //Looks at the window and figure it out
	scd.BufferDesc.Height = 0;
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 0; //pick whatever freshrate you need to
	scd.BufferDesc.RefreshRate.Denominator = 0;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0;

	HRESULT hr;

	//create device and front/back buffers, swapchain and rendering context
	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&scd,
		&_pSwapChain,
		&_pDevice,
		nullptr,
		&_pContext
	));

	//gain access to texture subresource in swapchain (back buffer)
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
	GFX_THROW_FAILED(_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer));
	GFX_THROW_FAILED(_pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		nullptr,
		&_pTarget
	));

	//Create z buffer / depth buffer
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDDState;
	D3D11_DEPTH_STENCIL_DESC dsd = {};
	dsd.DepthEnable = TRUE;
	dsd.StencilEnable = FALSE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;

	GFX_THROW_FAILED(_pDevice->CreateDepthStencilState(&dsd, &pDDState));

	//bind depth state
	_pContext->OMSetDepthStencilState(pDDState.Get(), 1u);

	//Create depth stensil texture
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC td = {};
	td.Width = 800u;
	td.Height = 600u;
	td.MipLevels = 1u;
	td.ArraySize = 1u;
	td.Format = DXGI_FORMAT_D32_FLOAT;
	td.SampleDesc.Count = 1u;			//SampleDesc values must match swapchains values!!!
	td.SampleDesc.Quality = 0u;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_FAILED(_pDevice->CreateTexture2D(&td, nullptr, &pDepthStencil));

	//Create view of depth stencil texture
	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0u;
	GFX_THROW_FAILED(_pDevice->CreateDepthStencilView(pDepthStencil.Get(), &dsvd, &_pDepthStencilView));

	//Bind the depth stencil view to OM
	_pContext->OMSetRenderTargets(1u, _pTarget.GetAddressOf(), _pDepthStencilView.Get());

	//Configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800.0f;
	vp.Height = 600.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	_pContext->RSSetViewports(1u, &vp);

	//Init imgui d3d implementation
	ImGui_ImplDX11_Init(_pDevice.Get(), _pContext.Get());
}

void Graphics::BeginFrame(float r, float g, float b) noexcept
{
	//Imgui begin frame 
	if (_imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	const float color[] = { r,g,b,1.0f };
	_pContext->ClearRenderTargetView(_pTarget.Get(), color);
}

void Graphics::EndFrame()
{
	//Imgui Frame End
	if (_imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	HRESULT hr;

	if (FAILED(hr = _pSwapChain->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(_pDevice->GetDeviceRemovedReason());
		}
		else
		{
			GFX_THROW_FAILED(hr);
		}
	}
}

void Graphics::EnableImgui() noexcept
{
	_imguiEnabled = true;
}

void Graphics::DisableImgui() noexcept
{
	_imguiEnabled = false;
}

bool Graphics::IsImguiEnabled() const noexcept
{
	return _imguiEnabled;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return _projectionMatrix;
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	_projectionMatrix = proj;
}

//Exception handling below here
Graphics::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	_hr(hr)
{
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return _hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(_hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(_hr, buf, sizeof(buf));
	return buf;
}


const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}