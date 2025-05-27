#define NOMINMAX
#include "nwpwin.h"
#include <algorithm>

class Ship : public vsite::nwp::window
{
	bool moving = false;
	static const int size = 20;

public:
	std::string class_name() override
	{
		return "STATIC";
	}

	bool create(HWND parent, POINT position)
	{
		return vsite::nwp::window::create(parent, WS_CHILD | WS_VISIBLE | SS_CENTER, "x", 0,
			position.x, position.y, size, size);
	}

	void set_moving(bool m)
	{
		if (moving != m)
		{
			moving = m;
			LONG style = ::GetWindowLong(*this, GWL_STYLE);
			if (moving)
				style |= WS_BORDER;
			else
				style &= ~WS_BORDER;
			::SetWindowLong(*this, GWL_STYLE, style);
			::SetWindowPos(*this, 0, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
	}

	void set_position(POINT p)
	{
		::SetWindowPos(*this, 0, p.x, p.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	void move(int direction, bool ctrl_pressed)
	{
		RECT client;
		::GetWindowRect(*this, &client);
		POINT p = { client.left, client.top };
		::ScreenToClient(::GetParent(*this), &p);

		int actual_step = ctrl_pressed ? 15 : 5;

		switch (direction)
		{
		case VK_LEFT:
			p.x -= actual_step;
			break;
		case VK_RIGHT:
			p.x += actual_step;
			break;
		case VK_UP:
			p.y -= actual_step;
			break;
		case VK_DOWN:
			p.y += actual_step;
			break;
		}

		RECT parent;
		::GetClientRect(::GetParent(*this), &parent);

		p.x = std::max(0L, std::min(p.x, parent.right - size));
		p.y = std::max(0L, std::min(p.y, parent.bottom - size));

		set_position(p);
	}
};

class main_window : public vsite::nwp::window
{
protected:
	void on_left_button_down(POINT p) override
	{
		if (!ship)
		{
			ship.create(*this, p);
		}
		else
		{
			ship.set_position(p);
		}
	}

	void on_key_up(int vk) override
	{
		ship.set_moving(false);
	}

	void on_key_down(int vk) override
	{
		if (vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN)
		{
			ship.set_moving(true);
			bool ctrl_pressed = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
			ship.move(vk, ctrl_pressed);
		}
	}

	void on_destroy() override
	{
		::PostQuitMessage(0);
	}

private:
	Ship ship;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
	vsite::nwp::application app;
	main_window w;
	w.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, "NWP 4");
	return app.run();
}