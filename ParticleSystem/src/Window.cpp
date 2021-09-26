#include "Window.h"
#include "../imgui/imgui_impl_win32.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass()
    : hInst(GetModuleHandle(nullptr))
{
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = HandleMsgSetup;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetInstance();
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = GetName();
    wc.hIconSm = nullptr;
    
    RegisterClassEx(&wc);
}

const wchar_t* Window::WindowClass::GetName() noexcept
{
    return wndclassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
    return wndClass.hInst;
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass(wndclassName, GetInstance());
}

Window::Window(int width, int height, const wchar_t* name)
    :
    width(width), height(height)
{
    hWnd = ::CreateWindow(
        WindowClass::GetName(),
        name,
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        width,
        height,
        nullptr,
        nullptr,
        WindowClass::GetInstance(),
        this
    );
    ShowWindow(hWnd, SW_SHOWDEFAULT);
    ImGui_ImplWin32_Init(hWnd);
    pGfx = std::make_unique<d3dApp>(hWnd);
}

Window::~Window()
{
    ImGui_ImplWin32_Shutdown();
    DestroyWindow(hWnd);
}

void Window::SetTitle(const std::wstring& title)
{
}

std::optional<int> Window::ProcessMessages() noexcept
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            return 0;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return {};
}

d3dApp& Window::Gfx()
{
    return *pGfx;
}

LRESULT __stdcall Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (msg == WM_NCCREATE)
    {
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsg));
        return pWnd->WndProc(hWnd, msg, wParam, lParam);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT __stdcall Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    return pWnd->WndProc(hWnd, msg, wParam, lParam);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    const auto& ImGuiIO = ImGui::GetIO();

    switch (uMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

    case WM_KILLFOCUS:
        kbd.ClearState();
        break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (!(lParam & 0x40000000) || kbd.IsAutoRepeatEnabled())
        {
            kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
        }
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
        break;

    case WM_CHAR:
        kbd.OnChar(static_cast<unsigned char>(wParam));
        break;

    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
