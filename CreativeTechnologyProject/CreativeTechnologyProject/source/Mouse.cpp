#include "..\includes\Mouse.h"
#include "WindowsCustomInclude.h"

std::pair<int, int> Mouse::GetPos() const noexcept
{
	return { _x, _y };
}

int Mouse::GetPosX() const noexcept
{
	return _x;
}

int Mouse::GetPosY() const noexcept
{
	return _y;
}

bool Mouse::IsInWindow() const noexcept
{
	return _isInWindow;
}

bool Mouse::LeftIsPressed() const noexcept
{
	return _leftIsPressed;
}

bool Mouse::RightIsPressed() const noexcept
{
	return _rightIsPressed;
}

Mouse::Event Mouse::Read() noexcept
{
	if (_buffer.size() > 0u)
	{
		Mouse::Event e = _buffer.front();
		_buffer.pop();
		return e;
	}
	else
	{
		return Mouse::Event();
	}
}

bool Mouse::IsEmpty() const noexcept
{
	return _buffer.empty();
}

void Mouse::Flush() noexcept
{
	_buffer = std::queue<Event>();
}

void Mouse::OnMouseMove(int x, int y) noexcept
{
	_x = x;
	_y = y;
	_buffer.push(Event(Event::Type::MOVE, *this));
	TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
	_isInWindow = true;
	_buffer.push(Event(Event::Type::ENTER, *this));
	TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
	_isInWindow = false;
	_buffer.push(Event(Event::Type::LEAVE, *this));
	TrimBuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
	_leftIsPressed = true;
	_buffer.push(Event(Event::Type::L_PRESS, *this));
	TrimBuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
	_leftIsPressed = false;
	_buffer.push(Event(Event::Type::L_RELEASE, *this));
	TrimBuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
	_rightIsPressed = true;
	_buffer.push(Event(Event::Type::R_PRESS, *this));
	TrimBuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
	_rightIsPressed = false;
	_buffer.push(Event(Event::Type::R_RELEASE, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
	_buffer.push(Event(Event::Type::WHEEL_UP, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
	_buffer.push(Event(Event::Type::WHEEL_DOWN, *this));
	TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
	while (_buffer.size() > _bufferSize)
	{
		_buffer.pop();
	}
}

void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
	_wheelDeltaCarry += delta;
	while (_wheelDeltaCarry >= WHEEL_DELTA)
	{
		_wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(x, y);
	}
	while (_wheelDeltaCarry <= -WHEEL_DELTA)
	{
		_wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(x, y);
	}
}
