#pragma once
#include <queue>

class Mouse
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			L_PRESS,
			L_RELEASE,
			R_PRESS,
			R_RELEASE,
			WHEEL_UP,
			WHEEL_DOWN,
			MOVE,
			ENTER,
			LEAVE,
			INVALID
		};
		Event() noexcept : _type(Type::INVALID), _leftIsPressed(false), _rightIsPressed(false), _x(0), _y(0) { }
		Event(Type type, const Mouse& parent) noexcept :
			_type(type), _leftIsPressed(parent._leftIsPressed), _rightIsPressed(parent._rightIsPressed), _x(parent._x), _y(parent._y) { }

		inline bool IsValid() const noexcept { return _type != Type::INVALID; }
		inline Type GetType() const noexcept { return _type; }
		inline std::pair<int,int> GetPos() const noexcept { return {_x,_y}; }
		inline int GetPosX() const noexcept { return _x; }
		inline int GetPosY() const noexcept { return _y; }
		inline bool LeftIsPressed() const noexcept { return _leftIsPressed; }
		inline bool RightIsPressed() const noexcept { return _rightIsPressed; }

	private:
		Type _type;
		bool _leftIsPressed;
		bool _rightIsPressed;
		int _x;
		int _y;
	};
public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	bool IsInWindow() const noexcept;
	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	Mouse::Event Read() noexcept;
	bool IsEmpty() const noexcept;
	void Flush() noexcept;
private:
	void OnMouseMove(int x, int y) noexcept;
	void OnMouseEnter() noexcept;
	void OnMouseLeave() noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void TrimBuffer() noexcept;
	void OnWheelDelta(int x, int y, int delta) noexcept;
private:
	static constexpr unsigned int _bufferSize = 16u;
	int _x;
	int _y;
	bool _leftIsPressed = false;
	bool _rightIsPressed = false;
	bool _isInWindow = false;
	int _wheelDeltaCarry = 0;
	std::queue<Event> _buffer;
};

