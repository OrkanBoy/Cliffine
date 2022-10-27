#include "Core/Asserts.h"
#include "Platform/Platform.h"

#include <windowsx.h>

#ifdef CLF_PLATFORM_WIN32
namespace Clf
{
    HINSTANCE Platform::instance;
    HWND Platform::window;

    u64 Platform::clockFrequency;
    u64 Platform::startTime;

    void Platform::Init(const char* appName, i32 x, i32 y, u32 width, u32 height)
    {
        instance = GetModuleHandleA(0);

        // Setup and register window class.
        HICON icon = LoadIcon(instance, IDI_APPLICATION);
        WNDCLASSA wc;
        memset(&wc, 0, sizeof(wc));
        wc.style = CS_DBLCLKS;  // Get double-clicks
        wc.lpfnWndProc = OnEvent;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = instance;
        wc.hIcon = icon;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);  // NULL; // Manage the cursor manually
        wc.hbrBackground = NULL;                   // Transparent
        wc.lpszClassName = "kohi_window_class";

        CLF_ASSERT(RegisterClassA(&wc), "Window registration failed");

        // Create window
        u32 clientX = x,
            clientY = y,
            clientWidth = width,
            clientHeight = height;

        u32 windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
        u32 windowStyleEx = WS_EX_APPWINDOW;

        windowStyle |= WS_MAXIMIZEBOX;
        windowStyle |= WS_MINIMIZEBOX;
        windowStyle |= WS_THICKFRAME;

        // Obtain the size of the border.
        RECT borderRect = { 0, 0, 0, 0 };
        AdjustWindowRectEx(&borderRect, windowStyle, 0, windowStyleEx);

        // In this case, the border rectangle is negative.
        y += borderRect.left;
        y += borderRect.top;

        // Grow by the size of the OS border.
        width += borderRect.right - borderRect.left;
        height += borderRect.bottom - borderRect.top;

        CLF_ASSERT(window = CreateWindowExA(
            windowStyleEx, "kohi_window_class", appName,
            windowStyle, x, y, width, height,
            0, 0, instance, 0),
            "Window creation Failed!");

        // Show the window
        bool should_activate = 1;  // TODO: if the window should not accept input, this should be false.
        i32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
        // If initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
        // If initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE
        ShowWindow(window, show_window_command_flags);

        // Clock setup
        LARGE_INTEGER clockFrequency, startTime;
        QueryPerformanceFrequency(&clockFrequency);
        Platform::clockFrequency = (u64)clockFrequency.QuadPart;
        QueryPerformanceCounter(&startTime);
        Platform::startTime = (u64)startTime.QuadPart;
    }

    void Platform::Deinit() 
    {
        CLF_ASSERT(window, "Window already unintialized!");
        DestroyWindow(window);
        window = nullptr;
    }

    void Platform::PollEvents()
    {
        MSG message;
        while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
    }

    void* Platform::AllocMem(u64 size)
    {
        return malloc(size);
    }

    void Platform::FreeMem(void* block)
    {
        free(block);
    }

    void* Platform::ZeroMem(void* block, u64 size) {
        return memset(block, 0, size);
    }

    void* Platform::CopyMem(void* dest, const void* source, u64 size) {
        return memcpy(dest, source, size);
    }

    void* Platform::SetMem(void* dest, i32 value, u64 size) {
        return memset(dest, value, size);
    }

    void Platform::ConsoleOut(const char* message, Log::Level level) {
        DWORD type = level < 2 ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE;
        HANDLE console = GetStdHandle(type);

        constexpr u8 levelColors[Log::LOG_LEVEL_MAX]{ 64, 4, 6, 2, 1, 8 };
        SetConsoleTextAttribute(console, levelColors[level]);
        OutputDebugStringA(message);

        LPDWORD writtenChars = 0;
        WriteConsoleA(GetStdHandle(type), message, (DWORD)strlen(message), writtenChars, 0);
    }

    u64 Platform::GetAbsTime() {
        LARGE_INTEGER time;
        QueryPerformanceCounter(&time);
        return (u64)time.QuadPart - startTime;
    }

    void Platform::Sleep(u64 ms) {
        SleepEx(ms, false);
    }

    LRESULT CALLBACK Platform::OnEvent(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param) {
        switch (msg) {
        case WM_ERASEBKGND:
            // Notify the OS that erasing will be handled by the application to prevent flicker.
            return 1;
        case WM_CLOSE:
            // TODO: Fire an event for the application to quit.
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE: {
            // Get the updated size.
            // RECT r;
            // GetClientRect(hwnd, &r);
            // u32 width = r.right - r.left;
            // u32 height = r.bottom - r.top;

            // TODO: Fire an event for window resize.
        } break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            // Key pressed/released
            //b8 pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            // TODO: input processing

        } break;
        case WM_MOUSEMOVE: {
            // Mouse move
            //i32 x_position = GET_X_LPARAM(l_param);
            //i32 y_position = GET_Y_LPARAM(l_param);
            // TODO: input processing.
        } break;
        case WM_MOUSEWHEEL: {
            // i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            // if (z_delta != 0) {
            //     // Flatten the input to an OS-independent (-1, 1)
            //     z_delta = (z_delta < 0) ? -1 : 1;
            //     // TODO: input processing.
            // }
        } break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            //b8 pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            // TODO: input processing.
        } break;
        }

        return DefWindowProcA(hwnd, msg, w_param, l_param);
    }
}
#endif