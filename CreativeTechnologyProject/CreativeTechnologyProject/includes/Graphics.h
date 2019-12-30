#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "WindowsCustomInclude.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "ExceptionHandler.h"
#include <wrl.h>

class Graphics
{
public:
	class Exception : public ExceptionHandler
	{
		using ExceptionHandler::ExceptionHandler;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT _hr;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	};
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;

	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11Device> _pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> _pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> _pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _pTarget;
};

