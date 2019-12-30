#pragma once
#include "WindowsCustomInclude.h"
#include "ExceptionHandler.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <optional>
#include <memory>

class Window
{
private:
	//Specialised instance of the exception handler specific to windows
	class Exception : public ExceptionHandler
	{
		using ExceptionHandler::ExceptionHandler;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};

	//Singleton manages register and cleanup of window class
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* _windowClassName = "AT Task 1 Plantu";
		static WindowClass _windowClass;
		HINSTANCE _hInstance;
	};

public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string newTitle);
	static std::optional<int> ProcessMessages();
	Graphics& Gfx();
public:
	Keyboard _keyboard;
	Mouse _mouse;
private:
	//Cannot set windows callback to member functions so static is like making it global, we'll then use these to call our member function
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	int _width;
	int _height;
	HWND _hWnd;
	std::unique_ptr<Graphics> _pGfx;
};

//Error exception helper macro
#define WND_EXCEPT( hr ) Window::HrException( __LINE__,__FILE__,(hr) )
#define WND_LAST_EXCEPT() Window::HrException( __LINE__,__FILE__,GetLastError() )
#define WND_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ )

