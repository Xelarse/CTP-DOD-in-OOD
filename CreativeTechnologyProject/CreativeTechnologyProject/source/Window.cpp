#include "Window.h"
#include <sstream>

////Window class related functions
Window::WindowClass Window::WindowClass::_windowClass;

Window::WindowClass::WindowClass() noexcept : _hInstance(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	//Register Window Class
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hIconSm = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(_windowClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return _windowClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return _windowClass._hInstance;
}

//Window Related Functions

Window::Window(int width, int height, const char* name) : _width(width), _height(height)
{
	//Calc window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	
	if (AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false) == 0)
	{
		throw WND_LAST_EXCEPT();
	}

	//Create window and store hWnd
	_hWnd = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, _width, _height,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	if (_hWnd == nullptr)
	{
		throw WND_LAST_EXCEPT();
	}

	//Show Window
	ShowWindow(_hWnd, SW_SHOWDEFAULT);

	//Creates graphics object
	_pGfx = std::make_unique<Graphics>(_hWnd);
}

Window::~Window()
{
	DestroyWindow(_hWnd);
}

void Window::SetTitle(const std::string newTitle)
{
	if (SetWindowText(_hWnd, newTitle.c_str()) == 0)
	{
		throw WND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages()
{
	//Message loop
	MSG msg;
	//Takes messages, removes off queue and dispatches but not stop execution of program
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		//Check for quit because peekmessage does not signal this via return
		if (msg.message == WM_QUIT)
		{
			return msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//return empty optional when not quitting app
	return {};
}

Graphics& Window::Gfx()
{
	if (!_pGfx)
	{
		throw WND_NOGFX_EXCEPT();
	}
	return *_pGfx;
}

LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	//Check if the message is the window being created and highjack it to point to our member function by adding the reference "this" to the create window function
	if (msg == WM_NCCREATE)
	{
		//Extract ptr to window class from creation data
		const CREATESTRUCTW* const createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam); //Recasting lParam to a createstruct so we can access our "this" from the create window
		Window* const wnd = static_cast<Window*>(createStruct->lpCreateParams);

		//Set WinAPI-managed user data to store ptr to window class. Creates a link between the window itself and the class we want to control it
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(wnd));

		//Set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

		//Forward message to the window class handler
		return wnd->HandleMsg(hWnd, msg, wParam, lParam);

		//Messages after this point AKA initialising the window will be handled by HandleMsgThunk
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	//Since the WINAPI cant call member functions directly we use this as a middle man to HandleMsg

	//retrieve ptr to window class
	Window* const wnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	//Forward the message to our member function because we changed GWLP_Userdata to point to our window class instance
	return wnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
		case WM_CLOSE:
			PostQuitMessage(1337);
			//Add a return to reprevent the default window handler from destroying our window and allow our destructor to handle it instead
			return 0;
		case WM_KILLFOCUS:
			//If window looses focus clear all key states to prevent ghost keys still being registered
			_keyboard.ClearState();
			break;
	//////////// KEYBOARD MESSAGES ///////////////
		case WM_KEYDOWN:
		//Added sys key to take into account the ALT key
		case WM_SYSKEYDOWN:
			//If bit 30 of lParam is false or autorepeat is enabled then process the keypress, otherwise ignore it 
			if (!(lParam & 0x40000000) || _keyboard.AutorepeatIsEnabled())
			{
				_keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
			}
			break;
		case WM_KEYUP:
		//Added sys key to take into account the ALT key
		case WM_SYSKEYUP:
			_keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
			break;
		case WM_CHAR:
			//Returns Character pressed with modifers (shift + d == D)
			_keyboard.OnChar(static_cast<unsigned char>(wParam));
			break;
	////////////////////////////////////////////////
	///////////// MOUSE MESSAGES //////////////////
		case WM_MOUSEMOVE:
			const POINTS pt1 = MAKEPOINTS(lParam);
			//If mouse is in client region log movement and log enter into window and capture if it wasnt previous in window
			if (pt1.x >= 0 && pt1.x < _width && pt1.y >= 0 && pt1.y < _height)
			{
				_mouse.OnMouseMove(pt1.x, pt1.y);
				if (!_mouse.IsInWindow())
				{
					SetCapture(_hWnd);
					_mouse.OnMouseEnter();
				}
			}
			//Not in client -> log move / maintain capture if button down
			else
			{
				if (wParam & (MK_LBUTTON | MK_RBUTTON))
				{
					_mouse.OnMouseMove(pt1.x, pt1.y);
				}
				//button up == release the capture and log that the mouse has left
				else
				{
					ReleaseCapture();
					_mouse.OnMouseLeave();
				}
			}
			break;
		case WM_LBUTTONDOWN:
			const POINTS pt2 = MAKEPOINTS(lParam);
			_mouse.OnLeftPressed(pt2.x, pt2.y);
			break;
		case WM_LBUTTONUP:
			const POINTS pt3 = MAKEPOINTS(lParam);
			_mouse.OnLeftReleased(pt3.x, pt3.y);
			break;
		case WM_RBUTTONDOWN:
			const POINTS pt4 = MAKEPOINTS(lParam);
			_mouse.OnRightPressed(pt4.x, pt4.y);
			break;
		case WM_RBUTTONUP:
			const POINTS pt5 = MAKEPOINTS(lParam);
			_mouse.OnRightReleased(pt5.x, pt5.y);
			break;
		case WM_MOUSEWHEEL:
			const POINTS pt6 = MAKEPOINTS(lParam);
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			_mouse.OnWheelDelta(pt6.x, pt6.y, delta);
			break;
	//////////////////////////////////////////////
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//Custom window exception functions
std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	// windows will allocate memory for err string and make our pointer point to it
	const DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	// 0 string length returned indicates a failure
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = pMsgBuf;
	// free windows buffer
	LocalFree(pMsgBuf);
	return errorString;
}


Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{
}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "Window Exception";
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode(hr);
}


const char* Window::NoGfxException::GetType() const noexcept
{
	return "Window Exception [No Graphics]";
}
