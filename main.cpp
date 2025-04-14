#include "nwpwin.h"

class Ship : public vsite::nwp::window
{
public:
    std::string class_name() override { return "STATIC"; }
};

static const int size = 20;

class main_window : public vsite::nwp::window
{
protected:
    void on_left_button_down(POINT p) override
    {
        position = p;
        if (!ship.operator HWND())
            ship.create(*this, WS_CHILD | WS_VISIBLE | SS_CENTER, "x", 0, p.x, p.y, size, size);
        else
            SetWindowPos(ship, 0, p.x, p.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

    void on_key_up(int vk) override
    {
        if (ship.operator HWND())
        {
            DWORD style = GetWindowLong(ship, GWL_STYLE);
            style &= ~WS_BORDER;
            SetWindowLong(ship, GWL_STYLE, style);
            SetWindowPos(ship, 0, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }
    }

    void on_key_down(int vk) override
    {
        if (ship.operator HWND() && (vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN))
        {
            RECT parent;
            GetClientRect(*this, &parent);

            int step = 5;
            if (GetKeyState(VK_CONTROL) & 0x8000)
                step *= 3;

            switch (vk)
            {
            case VK_LEFT:
                position.x = max(position.x - step, 0);
                break;
            case VK_RIGHT:
                position.x = min(position.x + step, parent.right - size);
                break;
            case VK_UP:
                position.y = max(position.y - step, 0);
                break;
            case VK_DOWN:
                position.y = min(position.y + step, parent.bottom - size);
                break;
            }

            DWORD style = GetWindowLong(ship, GWL_STYLE);
            style |= WS_BORDER;
            SetWindowLong(ship, GWL_STYLE, style);
            SetWindowPos(ship, 0, position.x, position.y, 0, 0,
                SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }
    }

    void on_destroy() override
    {
        ::PostQuitMessage(0);
    }

private:
    Ship ship;
    POINT position;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
    vsite::nwp::application app;
    main_window w;
    w.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, "NWP 4");
    return app.run();
}