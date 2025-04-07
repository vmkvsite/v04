#include "nwpwin.h"

class Ship : public vsite::nwp::window
{
	bool moving = false;
	int step = 5;

public:
	bool create(HWND parent, POINT position)
	{
		return vsite::nwp::window::create(parent, WS_CHILD | WS_VISIBLE | SS_CENTER, "x", 0,
			position.x, position.y, 20, 20);
	}

	void set_moving(bool m)
	{
		if (moving != m)
		{
			moving = m;
			HWND hwnd = *this;
			LONG style = ::GetWindowLong(hwnd, GWL_STYLE);
			if (moving)
				style |= WS_BORDER;
			else
				style &= ~WS_BORDER;
			::SetWindowLong(hwnd, GWL_STYLE, style);
			::SetWindowPos(hwnd, 0, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
	}

	void move(int direction, bool ctrl_pressed)
	{
		HWND hwnd = *this;
		RECT client;
		::GetWindowRect(hwnd, &client);
		POINT p = { client.left, client.top };
		::ScreenToClient(::GetParent(hwnd), &p);

		int actual_step = ctrl_pressed ? step * 3 : step;

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
		::GetClientRect(::GetParent(hwnd), &parent);

		if (p.x < 0) p.x = 0;
		if (p.y < 0) p.y = 0;
		if (p.x > parent.right - 20) p.x = parent.right - 20;
		if (p.y > parent.bottom - 20) p.y = parent.bottom - 20;

		::SetWindowPos(hwnd, 0, p.x, p.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
};

class main_window : public vsite::nwp::window
{
protected:
	void on_left_button_down(POINT p) override
	{
		if (!ship)
		{
			ship = new Ship();
			ship->create(*this, p);
		}
		else
		{
			HWND hwnd = *ship;
			::SetWindowPos(hwnd, 0, p.x, p.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
	}

	void on_key_up(int vk) override
	{
		if (ship)
			ship->set_moving(false);
	}

	void on_key_down(int vk) override
	{
		if (ship && (vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN))
		{
			ship->set_moving(true);
			bool ctrl_pressed = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
			ship->move(vk, ctrl_pressed);
		}
	}

	void on_destroy() override
	{
		delete ship;
		::PostQuitMessage(0);
	}

private:
	Ship* ship = nullptr;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
	vsite::nwp::application app;
	main_window w;
	w.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, "NWP 4");
	return app.run();
}