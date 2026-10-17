#include "main_window.hpp"
#include "dark_theme.hpp"
#include "../core/task_scheduler.hpp"
#include "../core/update_checker.hpp"
#include "../version.hpp"

namespace MemStand {

constexpr int HOTKEY_ID = 9001;

// Subclass helper for Custom Dark Checkboxes (solves the black text issue permanently)
static LRESULT CALLBACK CheckboxSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    switch (uMsg) {
    case WM_MOUSEMOVE: {
        if (!GetPropW(hwnd, L"HOVER")) {
            SetPropW(hwnd, L"HOVER", (HANDLE)1);
            TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        break;
    }
    case WM_MOUSELEAVE: {
        RemovePropW(hwnd, L"HOVER");
        InvalidateRect(hwnd, nullptr, FALSE);
        break;
    }
    case WM_NCDESTROY: {
        RemovePropW(hwnd, L"HOVER");
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rc;
        GetClientRect(hwnd, &rc);

        // Fill background matching Card
        HBRUSH hbrBg = DarkTheme::GetCardBrush();
        FillRect(hdc, &rc, hbrBg);

        bool checked = (SendMessageW(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED);
        bool isHovered = (GetPropW(hwnd, L"HOVER") != nullptr);

        // Box dimensions
        int boxSize = 18;
        int boxY = (rc.bottom - boxSize) / 2;
        RECT rcBox{ 2, boxY, 2 + boxSize, boxY + boxSize };

        COLORREF boxBgCol;
        COLORREF boxBorderCol;

        if (checked) {
            boxBgCol = isHovered ? RGB(59, 130, 246) : RGB(37, 99, 235);
            boxBorderCol = RGB(96, 165, 250);
        } else {
            boxBgCol = isHovered ? RGB(51, 65, 85) : RGB(15, 23, 42);
            boxBorderCol = isHovered ? RGB(148, 163, 184) : RGB(71, 85, 105);
        }

        HBRUSH hbrBox = CreateSolidBrush(boxBgCol);
        HPEN hPenBox = CreatePen(PS_SOLID, 1, boxBorderCol);
        HBRUSH hOldBr = (HBRUSH)SelectObject(hdc, hbrBox);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPenBox);
        RoundRect(hdc, rcBox.left, rcBox.top, rcBox.right, rcBox.bottom, 4, 4);

        if (checked) {
            // Crisp White Checkmark
            HPEN hPenCheck = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
            SelectObject(hdc, hPenCheck);
            MoveToEx(hdc, rcBox.left + 4, rcBox.top + 9, nullptr);
            LineTo(hdc, rcBox.left + 7, rcBox.top + 13);
            LineTo(hdc, rcBox.left + 14, rcBox.top + 5);
            DeleteObject(hPenCheck);
        }

        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBr);
        DeleteObject(hbrBox);
        DeleteObject(hPenBox);

        // Draw Text with high contrast
        wchar_t text[256];
        GetWindowTextW(hwnd, text, 256);
        RECT rcText = rc;
        rcText.left = rcBox.right + 10;
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, isHovered ? RGB(255, 255, 255) : Theme::TextPrimary);
        HFONT hFont = DarkTheme::GetMainFont(9, dwRefData == 1);
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        DrawTextW(hdc, text, -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, hOldFont);

        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

// Subclass helper for Custom Modern Push Buttons
static LRESULT CALLBACK ActionButtonSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    switch (uMsg) {
    case WM_MOUSEMOVE: {
        if (!GetPropW(hwnd, L"HOVER")) {
            SetPropW(hwnd, L"HOVER", (HANDLE)1);
            TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        break;
    }
    case WM_MOUSELEAVE: {
        RemovePropW(hwnd, L"HOVER");
        InvalidateRect(hwnd, nullptr, FALSE);
        break;
    }
    case WM_NCDESTROY: {
        RemovePropW(hwnd, L"HOVER");
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rc;
        GetClientRect(hwnd, &rc);

        bool isPressed = (SendMessageW(hwnd, BM_GETSTATE, 0, 0) & BST_PUSHED) != 0;
        bool isHovered = (GetPropW(hwnd, L"HOVER") != nullptr);

        // Button type based on dwRefData:
        // 1 = Master Start (Emerald Green)
        // 2 = Master Stop (Crimson Red)
        // 3 = Standby Purge (Slate with Blue accent)
        // 4 = Clean All (Slate with Amber accent)
        // 5 = Save Settings (Royal Blue)
        COLORREF bgCol;
        COLORREF borderCol;

        if (dwRefData == 1) { // Master START
            if (isPressed) {
                bgCol = RGB(4, 120, 87);
                borderCol = RGB(6, 95, 70);
            } else if (isHovered) {
                bgCol = RGB(5, 150, 105);
                borderCol = RGB(16, 185, 129);
            } else {
                bgCol = RGB(16, 185, 129);
                borderCol = RGB(5, 150, 105);
            }
        } else if (dwRefData == 2) { // Master STOP
            if (isPressed) {
                bgCol = RGB(185, 28, 28);
                borderCol = RGB(153, 27, 27);
            } else if (isHovered) {
                bgCol = RGB(220, 38, 38);
                borderCol = RGB(248, 113, 113);
            } else {
                bgCol = RGB(239, 68, 68);
                borderCol = RGB(220, 38, 38);
            }
        } else if (dwRefData == 5) { // Save
            if (isPressed) {
                bgCol = RGB(29, 78, 216);
                borderCol = RGB(30, 64, 175);
            } else if (isHovered) {
                bgCol = RGB(37, 99, 235);
                borderCol = RGB(96, 165, 250);
            } else {
                bgCol = RGB(59, 130, 246);
                borderCol = RGB(37, 99, 235);
            }
        } else if (dwRefData == 3) { // Standby
            if (isPressed) {
                bgCol = RGB(15, 23, 42);
                borderCol = RGB(37, 99, 235);
            } else if (isHovered) {
                bgCol = RGB(51, 65, 85);
                borderCol = RGB(96, 165, 250);
            } else {
                bgCol = RGB(30, 41, 59);
                borderCol = RGB(59, 130, 246);
            }
        } else if (dwRefData == 6) { // Auto-detect / Recommended pill button
            if (isPressed) {
                bgCol = RGB(15, 23, 42);      // Slate 900
                borderCol = RGB(37, 99, 235); // Blue 600
            } else if (isHovered) {
                bgCol = RGB(51, 65, 85);      // Slate 700
                borderCol = RGB(96, 165, 250); // Soft Blue 400
            } else {
                bgCol = RGB(30, 41, 59);      // Slate 800
                borderCol = RGB(51, 65, 85);  // Slate 700
            }
        } else if (dwRefData == 7) { // Check Updates
            if (isPressed) {
                bgCol = RGB(15, 23, 42);
                borderCol = RGB(14, 165, 233);
            } else if (isHovered) {
                bgCol = RGB(51, 65, 85);
                borderCol = RGB(56, 189, 248);
            } else {
                bgCol = RGB(30, 41, 59);
                borderCol = RGB(71, 85, 105);
            }
        } else { // Clean All
            if (isPressed) {
                bgCol = RGB(15, 23, 42);
                borderCol = RGB(217, 119, 6);
            } else if (isHovered) {
                bgCol = RGB(51, 65, 85);
                borderCol = RGB(251, 191, 36);
            } else {
                bgCol = RGB(30, 41, 59);
                borderCol = RGB(245, 158, 11);
            }
        }

        HBRUSH hbrBg = CreateSolidBrush(bgCol);
        HPEN hPenBorder = CreatePen(PS_SOLID, 1, borderCol);
        HBRUSH hOldBr = (HBRUSH)SelectObject(hdc, hbrBg);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPenBorder);
        int radius = (dwRefData == 6 ? 6 : 8);
        RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, radius, radius);
        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBr);
        DeleteObject(hbrBg);
        DeleteObject(hPenBorder);

        // Draw centered button text
        wchar_t text[256];
        GetWindowTextW(hwnd, text, 256);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, (dwRefData == 6) ? RGB(147, 197, 253) : RGB(255, 255, 255));
        HFONT hFont = DarkTheme::GetMainFont(dwRefData <= 2 ? 10 : (dwRefData == 6 ? 8 : 9), true);
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        DrawTextW(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, hOldFont);

        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

// Subclass helper for Custom Segmented Navigation Tabs
static LRESULT CALLBACK TabButtonSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    switch (uMsg) {
    case WM_MOUSEMOVE: {
        if (!GetPropW(hwnd, L"HOVER")) {
            SetPropW(hwnd, L"HOVER", (HANDLE)1);
            TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        break;
    }
    case WM_MOUSELEAVE: {
        RemovePropW(hwnd, L"HOVER");
        InvalidateRect(hwnd, nullptr, FALSE);
        break;
    }
    case WM_NCDESTROY: {
        RemovePropW(hwnd, L"HOVER");
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rc;
        GetClientRect(hwnd, &rc);

        bool isSelected = (dwRefData == 1);
        bool isHovered = (GetPropW(hwnd, L"HOVER") != nullptr);

        COLORREF bgCol;
        COLORREF borderCol;
        COLORREF textCol;

        if (isSelected) {
            bgCol = RGB(37, 99, 235);       // Vibrant Blue 600
            borderCol = RGB(96, 165, 250);  // Soft Blue 400
            textCol = RGB(255, 255, 255);
        } else if (isHovered) {
            bgCol = RGB(51, 65, 85);        // Slate 700 hover
            borderCol = RGB(100, 116, 139); // Slate 500
            textCol = RGB(248, 250, 252);   // Slate 50
        } else {
            bgCol = RGB(30, 41, 59);        // Slate 800
            borderCol = RGB(51, 65, 85);    // Slate 700
            textCol = RGB(148, 163, 184);   // Slate 400
        }

        HBRUSH hbrBg = CreateSolidBrush(bgCol);
        HPEN hPenBorder = CreatePen(PS_SOLID, 1, borderCol);
        HBRUSH hOldBr = (HBRUSH)SelectObject(hdc, hbrBg);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPenBorder);
        RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 6, 6);
        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBr);
        DeleteObject(hbrBg);
        DeleteObject(hPenBorder);

        wchar_t text[128];
        GetWindowTextW(hwnd, text, 128);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, textCol);
        HFONT hFont = DarkTheme::GetMainFont(9, isSelected);
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        DrawTextW(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, hOldFont);

        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

bool MainWindow::Create() {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWindow::WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hIcon = LoadIconW(wc.hInstance, MAKEINTRESOURCEW(IDI_APP_ICON));
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = DarkTheme::GetBgBrush();
    wc.lpszClassName = L"MemStandMainWindowClass";

    RegisterClassExW(&wc);

    DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT rcClient{ 0, 0, 500, 525 };
    AdjustWindowRectEx(&rcClient, dwStyle, FALSE, 0);
    int winWidth = rcClient.right - rcClient.left;
    int winHeight = rcClient.bottom - rcClient.top;
    int screenX = (GetSystemMetrics(SM_CXSCREEN) - winWidth) / 2;
    int screenY = (GetSystemMetrics(SM_CYSCREEN) - winHeight) / 2;

    m_hwnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        L("app_title").c_str(),
        dwStyle,
        screenX, screenY, winWidth, winHeight,
        nullptr, nullptr, wc.hInstance, this
    );

    return (m_hwnd != nullptr);
}

void MainWindow::Show(int cmdShow) {
    if (m_hwnd) {
        ShowWindow(m_hwnd, cmdShow);
        UpdateWindow(m_hwnd);
    }
}

LRESULT CALLBACK MainWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    MainWindow* pThis = nullptr;
    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        pThis = reinterpret_cast<MainWindow*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        pThis->m_hwnd = hwnd;
    } else {
        pThis = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (pThis) {
        return pThis->HandleMessage(hwnd, msg, wParam, lParam);
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void MainWindow::CreateControls(HWND hwnd) {
    HINSTANCE hInst = GetModuleHandleW(nullptr);
    HFONT hFont = DarkTheme::GetMainFont(9);
    HFONT hFontBold = DarkTheme::GetMainFont(9, true);

    // 1. Navigation Segmented Tabs (Replaces ugly white TabControl!)
    m_hTabDashboard = CreateWindowExW(
        0, L"BUTTON", L("tab_dashboard").c_str(),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        16, 12, 228, 30,
        hwnd, (HMENU)IDC_TAB_BTN_DASHBOARD, hInst, nullptr
    );
    SetWindowSubclass(m_hTabDashboard, TabButtonSubclassProc, 0, 1); // 1 = selected

    m_hTabSettings = CreateWindowExW(
        0, L"BUTTON", L("tab_settings").c_str(),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        256, 12, 228, 30,
        hwnd, (HMENU)IDC_TAB_BTN_SETTINGS, hInst, nullptr
    );
    SetWindowSubclass(m_hTabSettings, TabButtonSubclassProc, 0, 0); // 0 = unselected

    // --- TAB 0: DASHBOARD CONTROLS ---

    // Master START / STOP Action Button
    m_hBtnStartStop = CreateWindowExW(
        0, L"BUTTON", L("btn_start").c_str(),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        16, 368, 468, 40,
        hwnd, (HMENU)IDC_BTN_START_STOP, hInst, nullptr
    );
    SetWindowSubclass(m_hBtnStartStop, ActionButtonSubclassProc, 0, 1); // 1 = start

    // Standby & Clean All Quick Buttons
    m_hBtnPurgeStandby = CreateWindowExW(
        0, L"BUTTON", L("btn_purge_standby").c_str(),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        16, 414, 228, 36,
        hwnd, (HMENU)IDC_BTN_PURGE_STANDBY, hInst, nullptr
    );
    SetWindowSubclass(m_hBtnPurgeStandby, ActionButtonSubclassProc, 0, 3); // 3 = standby

    m_hBtnCleanAll = CreateWindowExW(
        0, L"BUTTON", L("btn_clean_all").c_str(),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        256, 414, 228, 36,
        hwnd, (HMENU)IDC_BTN_CLEAN_ALL, hInst, nullptr
    );
    SetWindowSubclass(m_hBtnCleanAll, ActionButtonSubclassProc, 0, 4); // 4 = clean all

    // --- TAB 1: SETTINGS CONTROLS ---

    // Card 1: Automation Thresholds
    m_hBtnAutoDetect = CreateWindowExW(
        0, L"BUTTON", L("btn_auto_detect").c_str(),
        WS_CHILD | BS_PUSHBUTTON,
        310, 52, 158, 22,
        hwnd, (HMENU)IDC_BTN_AUTO_DETECT, hInst, nullptr
    );
    SetWindowSubclass(m_hBtnAutoDetect, ActionButtonSubclassProc, 0, 6); // 6 = auto-detect

    m_hChkAutoStandby = CreateWindowExW(
        0, L"BUTTON", L("auto_standby_clean").c_str(),
        WS_CHILD | BS_AUTOCHECKBOX,
        30, 78, 440, 20,
        hwnd, (HMENU)IDC_CHK_AUTO_STANDBY, hInst, nullptr
    );
    SetWindowSubclass(m_hChkAutoStandby, CheckboxSubclassProc, 0, 1);

    m_hEditStandbyThresh = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", L"1024",
        WS_CHILD | ES_NUMBER | ES_AUTOHSCROLL,
        182, 98, 52, 22,
        hwnd, (HMENU)IDC_EDIT_STANDBY_THRESH, hInst, nullptr
    );
    SendMessageW(m_hEditStandbyThresh, WM_SETFONT, (WPARAM)hFont, TRUE);

    m_hEditFreeMemThresh = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", L"1024",
        WS_CHILD | ES_NUMBER | ES_AUTOHSCROLL,
        402, 98, 52, 22,
        hwnd, (HMENU)IDC_EDIT_FREEMEM_THRESH, hInst, nullptr
    );
    SendMessageW(m_hEditFreeMemThresh, WM_SETFONT, (WPARAM)hFont, TRUE);

    m_hChkAutoRam = CreateWindowExW(
        0, L"BUTTON", L("auto_ram_clean").c_str(),
        WS_CHILD | BS_AUTOCHECKBOX,
        30, 126, 440, 20,
        hwnd, (HMENU)IDC_CHK_AUTO_RAM, hInst, nullptr
    );
    SetWindowSubclass(m_hChkAutoRam, CheckboxSubclassProc, 0, 1);

    m_hEditRamThresh = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", L"85",
        WS_CHILD | ES_NUMBER | ES_AUTOHSCROLL,
        182, 150, 52, 22,
        hwnd, (HMENU)IDC_EDIT_RAM_THRESH, hInst, nullptr
    );
    SendMessageW(m_hEditRamThresh, WM_SETFONT, (WPARAM)hFont, TRUE);

    m_hEditPolling = CreateWindowExW(
        WS_EX_CLIENTEDGE, L"EDIT", L"1000",
        WS_CHILD | ES_NUMBER | ES_AUTOHSCROLL,
        402, 150, 52, 22,
        hwnd, (HMENU)IDC_EDIT_POLLING_RATE, hInst, nullptr
    );
    SendMessageW(m_hEditPolling, WM_SETFONT, (WPARAM)hFont, TRUE);

    // Card 2: System & Timer Preferences
    m_hComboTimer = CreateWindowExW(
        0, L"COMBOBOX", L"",
        WS_CHILD | CBS_DROPDOWN | WS_VSCROLL,
        140, 215, 85, 120,
        hwnd, (HMENU)IDC_COMBO_TARGET_TIMER, hInst, nullptr
    );
    SendMessageW(m_hComboTimer, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(m_hComboTimer, CB_ADDSTRING, 0, (LPARAM)L"0.50 ms");
    SendMessageW(m_hComboTimer, CB_ADDSTRING, 0, (LPARAM)L"0.75 ms");
    SendMessageW(m_hComboTimer, CB_ADDSTRING, 0, (LPARAM)L"1.00 ms");
    SendMessageW(m_hComboTimer, CB_ADDSTRING, 0, (LPARAM)L"15.62 ms");
    SendMessageW(m_hComboTimer, CB_SETCURSEL, 0, 0);

    m_hComboLanguage = CreateWindowExW(
        0, L"COMBOBOX", L"",
        WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        340, 215, 125, 120,
        hwnd, (HMENU)IDC_COMBO_LANGUAGE, hInst, nullptr
    );
    SendMessageW(m_hComboLanguage, WM_SETFONT, (WPARAM)hFont, TRUE);

    m_hChkAutoStartMonitoring = CreateWindowExW(
        0, L"BUTTON", L("auto_start_monitoring").c_str(),
        WS_CHILD | BS_AUTOCHECKBOX,
        30, 246, 440, 20,
        hwnd, (HMENU)IDC_CHK_AUTO_START_MONITORING, hInst, nullptr
    );
    SetWindowSubclass(m_hChkAutoStartMonitoring, CheckboxSubclassProc, 0, 1);

    m_hChkStartWindows = CreateWindowExW(
        0, L"BUTTON", L("start_with_windows").c_str(),
        WS_CHILD | BS_AUTOCHECKBOX,
        30, 270, 440, 20,
        hwnd, (HMENU)IDC_CHK_START_WINDOWS, hInst, nullptr
    );
    SetWindowSubclass(m_hChkStartWindows, CheckboxSubclassProc, 0, 1);

    m_hChkStartMinimized = CreateWindowExW(
        0, L"BUTTON", L("start_minimized").c_str(),
        WS_CHILD | BS_AUTOCHECKBOX,
        30, 294, 440, 20,
        hwnd, (HMENU)IDC_CHK_START_MINIMIZED, hInst, nullptr
    );
    SetWindowSubclass(m_hChkStartMinimized, CheckboxSubclassProc, 0, 1);

    m_hChkMinOnClose = CreateWindowExW(
        0, L"BUTTON", L("minimize_to_tray_on_close").c_str(),
        WS_CHILD | BS_AUTOCHECKBOX,
        30, 318, 440, 20,
        hwnd, (HMENU)IDC_CHK_MIN_ON_CLOSE, hInst, nullptr
    );
    SetWindowSubclass(m_hChkMinOnClose, CheckboxSubclassProc, 0, 1);

    m_hChkHotkey = CreateWindowExW(
        0, L"BUTTON", L("hotkey_enable").c_str(),
        WS_CHILD | BS_AUTOCHECKBOX,
        30, 342, 440, 20,
        hwnd, (HMENU)IDC_CHK_HOTKEY_ENABLE, hInst, nullptr
    );
    SetWindowSubclass(m_hChkHotkey, CheckboxSubclassProc, 0, 1);

    m_hChkShowNotifications = CreateWindowExW(
        0, L"BUTTON", L("show_notifications").c_str(),
        WS_CHILD | BS_AUTOCHECKBOX,
        30, 366, 440, 20,
        hwnd, (HMENU)IDC_CHK_SHOW_NOTIFICATIONS, hInst, nullptr
    );
    SetWindowSubclass(m_hChkShowNotifications, CheckboxSubclassProc, 0, 1);

    m_hBtnSaveSettings = CreateWindowExW(
        0, L"BUTTON", L("btn_save_settings").c_str(),
        WS_CHILD | BS_PUSHBUTTON,
        16, 412, 310, 38,
        hwnd, (HMENU)IDC_BTN_SAVE_SETTINGS, hInst, nullptr
    );
    SetWindowSubclass(m_hBtnSaveSettings, ActionButtonSubclassProc, 0, 5); // 5 = save

    m_hBtnCheckUpdates = CreateWindowExW(
        0, L"BUTTON", L("btn_check_updates").c_str(),
        WS_CHILD | BS_PUSHBUTTON,
        336, 412, 148, 38,
        hwnd, (HMENU)IDC_BTN_CHECK_UPDATES, hInst, nullptr
    );
    SetWindowSubclass(m_hBtnCheckUpdates, ActionButtonSubclassProc, 0, 7); // 7 = check updates

    PopulateSettingsToUI();
    SwitchTab(0);
}

void MainWindow::PopulateSettingsToUI() {
    const auto& config = ConfigManager::Instance().Get();

    // Check if monitoring was auto-started
    m_isMonitoringActive = config.auto_start_monitoring;

    // Language list
    SendMessageW(m_hComboLanguage, CB_RESETCONTENT, 0, 0);
    auto langs = Localization::Instance().GetAvailableLanguages();
    int selIdx = 0;
    for (size_t i = 0; i < langs.size(); ++i) {
        SendMessageW(m_hComboLanguage, CB_ADDSTRING, 0, (LPARAM)langs[i].displayName.c_str());
        if (langs[i].id == config.language) {
            selIdx = (int)i;
        }
    }
    SendMessageW(m_hComboLanguage, CB_SETCURSEL, selIdx, 0);

    // Target timer
    wchar_t timerStr[32];
    swprintf_s(timerStr, L"%.2f ms", config.desired_timer_resolution_ms);
    SetWindowTextW(m_hComboTimer, timerStr);

    // Settings checkboxes
    SendMessageW(m_hChkAutoStandby, BM_SETCHECK, config.auto_clean_standby ? BST_CHECKED : BST_UNCHECKED, 0);
    SetWindowTextW(m_hEditStandbyThresh, std::to_wstring(config.standby_threshold_mb).c_str());
    SetWindowTextW(m_hEditFreeMemThresh, std::to_wstring(config.free_mem_threshold_mb).c_str());

    SendMessageW(m_hChkAutoRam, BM_SETCHECK, config.auto_clean_high_ram ? BST_CHECKED : BST_UNCHECKED, 0);
    SetWindowTextW(m_hEditRamThresh, std::to_wstring(config.high_ram_threshold_percent).c_str());
    SetWindowTextW(m_hEditPolling, std::to_wstring(config.polling_rate_ms).c_str());

    SendMessageW(m_hChkAutoStartMonitoring, BM_SETCHECK, config.auto_start_monitoring ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessageW(m_hChkStartWindows, BM_SETCHECK, config.start_with_windows ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessageW(m_hChkStartMinimized, BM_SETCHECK, config.start_minimized ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessageW(m_hChkMinOnClose, BM_SETCHECK, config.minimize_to_tray_on_close ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessageW(m_hChkHotkey, BM_SETCHECK, config.hotkey_enabled ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessageW(m_hChkShowNotifications, BM_SETCHECK, config.show_notifications ? BST_CHECKED : BST_UNCHECKED, 0);

    // Update Start/Stop button appearance
    SetWindowTextW(m_hBtnStartStop, m_isMonitoringActive ? L("btn_stop").c_str() : L("btn_start").c_str());
    SetWindowSubclass(m_hBtnStartStop, ActionButtonSubclassProc, 0, m_isMonitoringActive ? 2 : 1);
}

void MainWindow::ToggleMonitoring() {
    auto& config = ConfigManager::Instance().Get();
    m_isMonitoringActive = !m_isMonitoringActive;

    if (m_isMonitoringActive) {
        // Start monitoring & apply timer resolution immediately!
        AutomationEngine::Instance().Start();
        if (config.timer_resolution_enabled) {
            TimerResolutionManager::Instance().SetResolution(config.desired_timer_resolution_ms);
        }
        m_statusText = L("status_running");
        SetWindowTextW(m_hBtnStartStop, L("btn_stop").c_str());
        SetWindowSubclass(m_hBtnStartStop, ActionButtonSubclassProc, 0, 2); // 2 = stop (red)
    } else {
        // Stop monitoring & restore system default timer!
        AutomationEngine::Instance().Stop();
        TimerResolutionManager::Instance().RestoreDefault();
        m_statusText = L("status_stopped");
        SetWindowTextW(m_hBtnStartStop, L("btn_start").c_str());
        SetWindowSubclass(m_hBtnStartStop, ActionButtonSubclassProc, 0, 1); // 1 = start (green)
    }

    UpdateMetrics();
    InvalidateRect(m_hwnd, nullptr, TRUE);
}

void MainWindow::SwitchTab(int tabIndex) {
    m_currentTab = tabIndex;

    SetWindowSubclass(m_hTabDashboard, TabButtonSubclassProc, 0, (m_currentTab == 0 ? 1 : 0));
    SetWindowSubclass(m_hTabSettings, TabButtonSubclassProc, 0, (m_currentTab == 1 ? 1 : 0));
    InvalidateRect(m_hTabDashboard, nullptr, TRUE);
    InvalidateRect(m_hTabSettings, nullptr, TRUE);

    BOOL showDash = (m_currentTab == 0) ? SW_SHOW : SW_HIDE;
    BOOL showSettings = (m_currentTab == 1) ? SW_SHOW : SW_HIDE;

    ShowWindow(m_hBtnStartStop, showDash);
    ShowWindow(m_hBtnPurgeStandby, showDash);
    ShowWindow(m_hBtnCleanAll, showDash);

    ShowWindow(m_hBtnAutoDetect, showSettings);
    ShowWindow(m_hChkAutoStandby, showSettings);
    ShowWindow(m_hEditStandbyThresh, showSettings);
    ShowWindow(m_hEditFreeMemThresh, showSettings);
    ShowWindow(m_hChkAutoRam, showSettings);
    ShowWindow(m_hEditRamThresh, showSettings);
    ShowWindow(m_hEditPolling, showSettings);

    ShowWindow(m_hComboTimer, showSettings);
    ShowWindow(m_hChkAutoStartMonitoring, showSettings);
    ShowWindow(m_hChkStartWindows, showSettings);
    ShowWindow(m_hChkStartMinimized, showSettings);
    ShowWindow(m_hChkMinOnClose, showSettings);
    ShowWindow(m_hChkHotkey, showSettings);
    ShowWindow(m_hChkShowNotifications, showSettings);
    ShowWindow(m_hComboLanguage, showSettings);
    ShowWindow(m_hBtnSaveSettings, showSettings);
    ShowWindow(m_hBtnCheckUpdates, showSettings);

    InvalidateRect(m_hwnd, nullptr, TRUE);
}

void MainWindow::ApplySettingsFromUI() {
    auto& config = ConfigManager::Instance().Get();

    // Language
    int sel = (int)SendMessageW(m_hComboLanguage, CB_GETCURSEL, 0, 0);
    auto langs = Localization::Instance().GetAvailableLanguages();
    if (sel >= 0 && sel < (int)langs.size()) {
        config.language = langs[sel].id;
        Localization::Instance().LoadLanguage(config.language);
    }

    // Target Timer Resolution
    wchar_t timerBuf[32]{};
    GetWindowTextW(m_hComboTimer, timerBuf, 32);
    try {
        config.desired_timer_resolution_ms = std::stod(timerBuf);
    } catch (...) {}

    // If currently running, re-apply the updated timer immediately!
    if (m_isMonitoringActive && config.timer_resolution_enabled) {
        TimerResolutionManager::Instance().SetResolution(config.desired_timer_resolution_ms);
    }

    // Auto Clean
    config.auto_clean_standby = (SendMessageW(m_hChkAutoStandby, BM_GETCHECK, 0, 0) == BST_CHECKED);
    config.auto_clean_high_ram = (SendMessageW(m_hChkAutoRam, BM_GETCHECK, 0, 0) == BST_CHECKED);

    wchar_t buf[32]{};
    GetWindowTextW(m_hEditStandbyThresh, buf, 32);
    try { config.standby_threshold_mb = std::stoull(buf); } catch (...) {}

    GetWindowTextW(m_hEditFreeMemThresh, buf, 32);
    try { config.free_mem_threshold_mb = std::stoull(buf); } catch (...) {}

    GetWindowTextW(m_hEditRamThresh, buf, 32);
    try { config.high_ram_threshold_percent = std::stoi(buf); } catch (...) {}

    GetWindowTextW(m_hEditPolling, buf, 32);
    try { config.polling_rate_ms = std::stoi(buf); } catch (...) {}

    config.auto_start_monitoring = (SendMessageW(m_hChkAutoStartMonitoring, BM_GETCHECK, 0, 0) == BST_CHECKED);

    // Startup
    bool startWindows = (SendMessageW(m_hChkStartWindows, BM_GETCHECK, 0, 0) == BST_CHECKED);
    if (startWindows != config.start_with_windows) {
        config.start_with_windows = startWindows;
        if (startWindows) {
            TaskSchedulerHelper::RegisterStartupTask(config.start_minimized);
        } else {
            TaskSchedulerHelper::UnregisterStartupTask();
        }
    }

    config.start_minimized = (SendMessageW(m_hChkStartMinimized, BM_GETCHECK, 0, 0) == BST_CHECKED);
    config.minimize_to_tray_on_close = (SendMessageW(m_hChkMinOnClose, BM_GETCHECK, 0, 0) == BST_CHECKED);

    // Hotkey
    config.hotkey_enabled = (SendMessageW(m_hChkHotkey, BM_GETCHECK, 0, 0) == BST_CHECKED);
    if (config.hotkey_enabled) {
        RegisterGlobalHotkey();
    } else {
        UnregisterGlobalHotkey();
    }

    // Notifications
    config.show_notifications = (SendMessageW(m_hChkShowNotifications, BM_GETCHECK, 0, 0) == BST_CHECKED);

    ConfigManager::Instance().Save();
    RefreshLanguage();

    m_statusText = L("status_settings_saved");
    InvalidateRect(m_hwnd, nullptr, TRUE);
}

void MainWindow::AutoDetectOptimalSettings() {
    MEMORYSTATUSEX mem{};
    mem.dwLength = sizeof(mem);
    if (!GlobalMemoryStatusEx(&mem)) return;

    uint64_t physMb = mem.ullTotalPhys / (1024 * 1024);
    uint64_t standardGb = 8;
    if (physMb <= 4600) standardGb = 4;
    else if (physMb <= 9200) standardGb = 8;
    else if (physMb <= 13800) standardGb = 12;
    else if (physMb <= 18400) standardGb = 16;
    else if (physMb <= 27600) standardGb = 24;
    else if (physMb <= 36800) standardGb = 32;
    else if (physMb <= 55200) standardGb = 48;
    else if (physMb <= 73600) standardGb = 64;
    else standardGb = (physMb + 1023) / 1024;

    uint64_t standbyMb = (standardGb * 1024) / 2;
    uint64_t freeMb = std::max<uint64_t>(1024, standbyMb / 4);
    int ramPct = (standardGb <= 8) ? 80 : (standardGb <= 32 ? 85 : 90);
    int pollMs = 1000;

    SetWindowTextW(m_hEditStandbyThresh, std::to_wstring(standbyMb).c_str());
    SetWindowTextW(m_hEditFreeMemThresh, std::to_wstring(freeMb).c_str());
    SetWindowTextW(m_hEditRamThresh, std::to_wstring(ramPct).c_str());
    SetWindowTextW(m_hEditPolling, std::to_wstring(pollMs).c_str());

    wchar_t buf[128];
    swprintf_s(buf, L("status_auto_detected").c_str(), standardGb);
    m_statusText = buf;
    m_trayIcon.ShowBalloon(L"MemStand", m_statusText);
    InvalidateRect(m_hwnd, nullptr, TRUE);
}

void MainWindow::RefreshLanguage() {
    SetWindowTextW(m_hwnd, L("app_title").c_str());

    SetWindowTextW(m_hTabDashboard, L("tab_dashboard").c_str());
    SetWindowTextW(m_hTabSettings, L("tab_settings").c_str());

    SetWindowTextW(m_hBtnStartStop, m_isMonitoringActive ? L("btn_stop").c_str() : L("btn_start").c_str());
    SetWindowTextW(m_hBtnPurgeStandby, L("btn_purge_standby").c_str());
    SetWindowTextW(m_hBtnCleanAll, L("btn_clean_all").c_str());

    SetWindowTextW(m_hChkAutoStandby, L("auto_standby_clean").c_str());
    SetWindowTextW(m_hChkAutoRam, L("auto_ram_clean").c_str());
    SetWindowTextW(m_hChkAutoStartMonitoring, L("auto_start_monitoring").c_str());
    SetWindowTextW(m_hChkStartWindows, L("start_with_windows").c_str());
    SetWindowTextW(m_hChkStartMinimized, L("start_minimized").c_str());
    SetWindowTextW(m_hChkMinOnClose, L("minimize_to_tray_on_close").c_str());
    SetWindowTextW(m_hChkHotkey, L("hotkey_enable").c_str());
    SetWindowTextW(m_hChkShowNotifications, L("show_notifications").c_str());
    SetWindowTextW(m_hBtnSaveSettings, L("btn_save_settings").c_str());
    SetWindowTextW(m_hBtnAutoDetect, L("btn_auto_detect").c_str());
    SetWindowTextW(m_hBtnCheckUpdates, L("btn_check_updates").c_str());


    InvalidateRect(m_hwnd, nullptr, TRUE);
}

void MainWindow::RegisterGlobalHotkey() {
    UnregisterGlobalHotkey();
    const auto& config = ConfigManager::Instance().Get();
    if (config.hotkey_enabled) {
        m_hotkeyRegistered = RegisterHotKey(m_hwnd, HOTKEY_ID, config.hotkey_modifiers, config.hotkey_key);
    }
}

void MainWindow::UnregisterGlobalHotkey() {
    if (m_hotkeyRegistered) {
        UnregisterHotKey(m_hwnd, HOTKEY_ID);
        m_hotkeyRegistered = false;
    }
}

void MainWindow::UpdateMetrics() {
    m_lastSnapshot = MemoryManager::Instance().GetSnapshot();

    // If monitoring is active, continuously re-assert desired timer resolution
    const auto& config = ConfigManager::Instance().Get();
    if (m_isMonitoringActive && config.timer_resolution_enabled) {
        TimerResolutionManager::Instance().SetResolution(config.desired_timer_resolution_ms);
    }

    wchar_t tooltip[128];
    swprintf_s(tooltip, L"MemStand: %d%% RAM\nStandby: %s\nFree: %s",
        m_lastSnapshot.usage_percentage,
        FormatMemoryBytes(m_lastSnapshot.standby_bytes).c_str(),
        FormatMemoryBytes(m_lastSnapshot.free_ram_bytes).c_str()
    );

    m_trayIcon.Update(m_lastSnapshot.usage_percentage, tooltip);

    if (m_currentTab == 0) {
        RECT rc{ 16, 48, 484, 482 };
        InvalidateRect(m_hwnd, &rc, FALSE);
    }
}

void MainWindow::CheckForUpdates(bool silentIfLatest) {
    m_statusText = L("status_update_checking");
    InvalidateRect(m_hwnd, nullptr, TRUE);

    UpdateChecker::Instance().CheckForUpdatesAsync([this, silentIfLatest](const UpdateInfo& info) {
        if (info.updateAvailable) {
            wchar_t msg[256];
            swprintf_s(msg, L("update_available_msg").c_str(), info.latestVersion.c_str());
            m_statusText = msg;
            m_trayIcon.ShowBalloon(L("update_available_title"), msg);

            if (!silentIfLatest) {
                std::wstring dialogMsg = std::wstring(msg) + L"\n\n" + MEMSTAND_RELEASES_URL;
                int ans = MessageBoxW(m_hwnd, dialogMsg.c_str(), L("update_available_title").c_str(), MB_YESNO | MB_ICONINFORMATION);
                if (ans == IDYES) {
                    ShellExecuteW(nullptr, L"open", info.releaseUrl.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                }
            }
        } else if (!info.errorMessage.empty()) {
            if (!silentIfLatest) {
                wchar_t err[256];
                swprintf_s(err, L("update_error_msg").c_str(), info.errorMessage.c_str());
                m_statusText = err;
                m_trayIcon.ShowBalloon(L"MemStand", err);
            }
        } else {
            if (!silentIfLatest) {
                wchar_t upToDate[256];
                swprintf_s(upToDate, L("update_latest_msg").c_str(), info.currentVersion.c_str());
                m_statusText = upToDate;
                m_trayIcon.ShowBalloon(L"MemStand", upToDate);
            }
        }
        PostMessageW(m_hwnd, WM_TIMER, 1, 0);
    });
}

void MainWindow::DrawDashboard(HDC hdc, const RECT& rcClient) {

    SetBkMode(hdc, TRANSPARENT);
    HFONT hFontMain = DarkTheme::GetMainFont(9);
    HFONT hFontBold = DarkTheme::GetMainFont(9, true);
    HFONT hFontLarge = DarkTheme::GetMainFont(18, true);

    HBRUSH hbrCard = DarkTheme::GetCardBrush();
    HPEN hPenBorder = CreatePen(PS_SOLID, 1, Theme::CardBorder);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPenBorder);
    HBRUSH hOldBr = (HBRUSH)SelectObject(hdc, hbrCard);

    // 1. RAM Usage Card
    RECT rcCard1{ 16, 48, 484, 140 };
    RoundRect(hdc, rcCard1.left, rcCard1.top, rcCard1.right, rcCard1.bottom, 10, 10);

    SelectObject(hdc, hFontBold);
    SetTextColor(hdc, Theme::TextPrimary);
    RECT rcTitle{ rcCard1.left + 14, rcCard1.top + 10, rcCard1.right - 14, rcCard1.top + 28 };
    DrawTextW(hdc, L("ram_usage").c_str(), -1, &rcTitle, DT_LEFT | DT_TOP);

    SelectObject(hdc, hFontLarge);
    wchar_t pctTxt[32];
    swprintf_s(pctTxt, L"%d%%", m_lastSnapshot.usage_percentage);
    RECT rcPct{ rcCard1.left + 14, rcCard1.top + 30, rcCard1.left + 120, rcCard1.top + 60 };
    DrawTextW(hdc, pctTxt, -1, &rcPct, DT_LEFT | DT_TOP);

    SelectObject(hdc, hFontMain);
    SetTextColor(hdc, Theme::TextMuted);
    wchar_t ramBytesTxt[64];
    swprintf_s(ramBytesTxt, L"%s / %s",
        FormatMemoryBytes(m_lastSnapshot.used_ram_bytes).c_str(),
        FormatMemoryBytes(m_lastSnapshot.total_ram_bytes).c_str()
    );
    RECT rcBytes{ rcCard1.left + 120, rcCard1.top + 36, rcCard1.right - 14, rcCard1.top + 60 };
    DrawTextW(hdc, ramBytesTxt, -1, &rcBytes, DT_RIGHT | DT_TOP);

    // Progress Bar Track & Fill
    RECT rcProgBg{ rcCard1.left + 14, rcCard1.top + 66, rcCard1.right - 14, rcCard1.top + 80 };
    HBRUSH hbrProgBg = CreateSolidBrush(RGB(15, 23, 42)); // Deep Slate track
    HPEN hPenProgBg = CreatePen(PS_SOLID, 1, RGB(51, 65, 85)); // 1px subtle track border
    HPEN hOldPenPB = (HPEN)SelectObject(hdc, hPenProgBg);
    HBRUSH hOldBrPB = (HBRUSH)SelectObject(hdc, hbrProgBg);
    RoundRect(hdc, rcProgBg.left, rcProgBg.top, rcProgBg.right, rcProgBg.bottom, 6, 6);
    SelectObject(hdc, hOldPenPB);
    SelectObject(hdc, hOldBrPB);
    DeleteObject(hbrProgBg);
    DeleteObject(hPenProgBg);

    int totalTrackWidth = (rcProgBg.right - 2) - (rcProgBg.left + 2);
    int fillWidth = (totalTrackWidth * m_lastSnapshot.usage_percentage) / 100;
    if (fillWidth > 2) {
        RECT rcProgFill{ rcProgBg.left + 2, rcProgBg.top + 2, rcProgBg.left + 2 + fillWidth, rcProgBg.bottom - 2 };

        COLORREF barColor = Theme::AccentGreen;
        if (m_lastSnapshot.usage_percentage >= 85) barColor = Theme::AccentRed;
        else if (m_lastSnapshot.usage_percentage >= 70) barColor = Theme::AccentYellow;

        HBRUSH hbrProgFill = CreateSolidBrush(barColor);
        HPEN hPenProgFill = CreatePen(PS_SOLID, 1, barColor);
        HPEN hOldPenPF = (HPEN)SelectObject(hdc, hPenProgFill);
        HBRUSH hOldBrPF = (HBRUSH)SelectObject(hdc, hbrProgFill);
        RoundRect(hdc, rcProgFill.left, rcProgFill.top, rcProgFill.right, rcProgFill.bottom, 4, 4);
        SelectObject(hdc, hOldPenPF);
        SelectObject(hdc, hOldBrPF);
        DeleteObject(hbrProgFill);
        DeleteObject(hPenProgFill);
    }

    // 2. Memory Details Card
    RECT rcCard2{ 16, 146, 484, 266 };
    RoundRect(hdc, rcCard2.left, rcCard2.top, rcCard2.right, rcCard2.bottom, 10, 10);

    auto DrawMetricRow = [&](int y, const std::wstring& label, const std::wstring& value, COLORREF valColor = Theme::TextPrimary) {
        SelectObject(hdc, hFontMain);
        SetTextColor(hdc, Theme::TextMuted);
        RECT rcL{ rcCard2.left + 14, y, rcCard2.left + 240, y + 20 };
        DrawTextW(hdc, label.c_str(), -1, &rcL, DT_LEFT | DT_TOP);

        SelectObject(hdc, hFontBold);
        SetTextColor(hdc, valColor);
        RECT rcR{ rcCard2.left + 250, y, rcCard2.right - 14, y + 20 };
        DrawTextW(hdc, value.c_str(), -1, &rcR, DT_RIGHT | DT_TOP);
    };

    DrawMetricRow(rcCard2.top + 12, L("standby_list"), FormatMemoryBytes(m_lastSnapshot.standby_bytes), Theme::AccentBlue);
    DrawMetricRow(rcCard2.top + 38, L("free_memory"), FormatMemoryBytes(m_lastSnapshot.free_ram_bytes), Theme::AccentGreen);
    DrawMetricRow(rcCard2.top + 64, L("modified_list"), FormatMemoryBytes(m_lastSnapshot.modified_bytes));
    DrawMetricRow(rcCard2.top + 90, L("system_cache"), FormatMemoryBytes(m_lastSnapshot.file_cache_bytes));

    // 3. Timer & Engine Status Card
    RECT rcCard3{ 16, 272, 484, 360 };
    RoundRect(hdc, rcCard3.left, rcCard3.top, rcCard3.right, rcCard3.bottom, 10, 10);

    SelectObject(hdc, hFontBold);
    SetTextColor(hdc, Theme::TextPrimary);
    RECT rcTimerTitle{ rcCard3.left + 14, rcCard3.top + 10, rcCard3.left + 240, rcCard3.top + 28 };
    DrawTextW(hdc, L("timer_resolution_title").c_str(), -1, &rcTimerTitle, DT_LEFT | DT_TOP);

    // Status Badge Pill (Running vs Stopped)
    std::wstring badgeText = m_isMonitoringActive ? L("status_running") : L("status_stopped");
    COLORREF badgeBgColor = m_isMonitoringActive ? RGB(6, 78, 59) : RGB(15, 23, 42);
    COLORREF badgeBorderColor = m_isMonitoringActive ? RGB(16, 185, 129) : RGB(51, 65, 85);
    COLORREF badgeTextColor = m_isMonitoringActive ? RGB(209, 250, 229) : RGB(148, 163, 184);

    SIZE szBadge;
    SelectObject(hdc, hFontBold);
    GetTextExtentPoint32W(hdc, badgeText.c_str(), (int)badgeText.length(), &szBadge);
    int pillW = szBadge.cx + 20;
    int pillH = szBadge.cy + 6;
    RECT rcPill{ rcCard3.right - 14 - pillW, rcCard3.top + 8, rcCard3.right - 14, rcCard3.top + 8 + pillH };

    HBRUSH hbrPill = CreateSolidBrush(badgeBgColor);
    HPEN hPenPill = CreatePen(PS_SOLID, 1, badgeBorderColor);
    HPEN hOldPenPill = (HPEN)SelectObject(hdc, hPenPill);
    HBRUSH hOldBrPill = (HBRUSH)SelectObject(hdc, hbrPill);
    RoundRect(hdc, rcPill.left, rcPill.top, rcPill.right, rcPill.bottom, 8, 8);
    SelectObject(hdc, hOldPenPill);
    SelectObject(hdc, hOldBrPill);
    DeleteObject(hbrPill);
    DeleteObject(hPenPill);

    SetTextColor(hdc, badgeTextColor);
    DrawTextW(hdc, badgeText.c_str(), -1, &rcPill, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Live Current Timer Resolution
    double curMs = TimerResolutionManager::Instance().GetCurrentMs();
    wchar_t curTimerTxt[64];
    swprintf_s(curTimerTxt, L"%.3f ms", curMs);

    SelectObject(hdc, hFontMain);
    SetTextColor(hdc, Theme::TextMuted);
    RECT rcCurLabel{ rcCard3.left + 14, rcCard3.top + 35, rcCard3.left + 240, rcCard3.top + 53 };
    DrawTextW(hdc, L("current_timer").c_str(), -1, &rcCurLabel, DT_LEFT | DT_TOP);

    SelectObject(hdc, hFontBold);
    bool isLowLatency = (curMs <= 1.05);
    SetTextColor(hdc, isLowLatency ? Theme::AccentGreen : Theme::AccentYellow);
    RECT rcCurVal{ rcCard3.left + 250, rcCard3.top + 35, rcCard3.right - 14, rcCard3.top + 53 };
    DrawTextW(hdc, curTimerTxt, -1, &rcCurVal, DT_RIGHT | DT_TOP);

    // Target Timer Resolution
    SelectObject(hdc, hFontMain);
    SetTextColor(hdc, Theme::TextMuted);
    RECT rcTgtLabel{ rcCard3.left + 14, rcCard3.top + 60, rcCard3.left + 240, rcCard3.top + 78 };
    DrawTextW(hdc, L("target_timer").c_str(), -1, &rcTgtLabel, DT_LEFT | DT_TOP);

    SelectObject(hdc, hFontBold);
    SetTextColor(hdc, Theme::TextPrimary);
    wchar_t tgtTimerTxt[64];
    swprintf_s(tgtTimerTxt, L"%.2f ms", ConfigManager::Instance().Get().desired_timer_resolution_ms);
    RECT rcTgtVal{ rcCard3.left + 250, rcCard3.top + 60, rcCard3.right - 14, rcCard3.top + 78 };
    DrawTextW(hdc, tgtTimerTxt, -1, &rcTgtVal, DT_RIGHT | DT_TOP);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBr);
    DeleteObject(hPenBorder);

    // Status & Footer (Two separate rows to guarantee zero overlap)
    SelectObject(hdc, hFontMain);
    SetTextColor(hdc, Theme::TextMuted);

    RECT rcStatus{ 16, 462, 484, 482 };
    std::wstring statusDisplay = m_statusText.empty() ? L("status_ready") : m_statusText;
    DrawTextW(hdc, statusDisplay.c_str(), -1, &rcStatus, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    // Subtle divider line
    HPEN hPenDiv = CreatePen(PS_SOLID, 1, Theme::CardBorder);
    HPEN hOldPenDiv = (HPEN)SelectObject(hdc, hPenDiv);
    MoveToEx(hdc, 16, 488, nullptr);
    LineTo(hdc, 484, 488);
    SelectObject(hdc, hOldPenDiv);
    DeleteObject(hPenDiv);

    // Brand / Version Footer
    RECT rcBrand{ 16, 494, 484, 514 };
    DrawTextW(hdc, MEMSTAND_BRAND_FOOTER, -1, &rcBrand, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
}

void MainWindow::DrawSettings(HDC hdc, const RECT& rcClient) {
    SetBkMode(hdc, TRANSPARENT);
    HFONT hFontMain = DarkTheme::GetMainFont(9);
    HFONT hFontBold = DarkTheme::GetMainFont(9, true);

    HBRUSH hbrCard = DarkTheme::GetCardBrush();
    HPEN hPenBorder = CreatePen(PS_SOLID, 1, Theme::CardBorder);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPenBorder);
    HBRUSH hOldBr = (HBRUSH)SelectObject(hdc, hbrCard);

    // Card 1: Automation Thresholds
    RECT rcCard1{ 16, 48, 484, 180 };
    RoundRect(hdc, rcCard1.left, rcCard1.top, rcCard1.right, rcCard1.bottom, 10, 10);

    SelectObject(hdc, hFontBold);
    SetTextColor(hdc, Theme::TextPrimary);
    RECT rcT1{ rcCard1.left + 14, rcCard1.top + 8, rcCard1.right - 14, rcCard1.top + 26 };
    DrawTextW(hdc, L("settings_cat_automation").c_str(), -1, &rcT1, DT_LEFT | DT_TOP);

    SelectObject(hdc, hFontMain);
    SetTextColor(hdc, Theme::TextMuted);

    RECT rcL1{ 48, 98, 178, 120 };
    DrawTextW(hdc, L("standby_threshold").c_str(), -1, &rcL1, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT rcL2{ 248, 98, 398, 120 };
    DrawTextW(hdc, L("free_memory_threshold").c_str(), -1, &rcL2, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT rcL3{ 48, 150, 178, 172 };
    DrawTextW(hdc, L("ram_threshold_percent").c_str(), -1, &rcL3, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT rcL4{ 248, 150, 398, 172 };
    DrawTextW(hdc, L("polling_rate").c_str(), -1, &rcL4, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    // Card 2: System & Timer Preferences
    RECT rcCard2{ 16, 186, 484, 400 };
    RoundRect(hdc, rcCard2.left, rcCard2.top, rcCard2.right, rcCard2.bottom, 10, 10);

    SelectObject(hdc, hFontBold);
    SetTextColor(hdc, Theme::TextPrimary);
    RECT rcT2{ rcCard2.left + 14, rcCard2.top + 8, rcCard2.right - 14, rcCard2.top + 26 };
    DrawTextW(hdc, L("settings_cat_general").c_str(), -1, &rcT2, DT_LEFT | DT_TOP);

    SelectObject(hdc, hFontMain);
    SetTextColor(hdc, Theme::TextMuted);

    RECT rcTgt{ 30, 215, 136, 237 };
    DrawTextW(hdc, L("target_timer").c_str(), -1, &rcTgt, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT rcLang{ 240, 215, 332, 237 };
    DrawTextW(hdc, L("language_label").c_str(), -1, &rcLang, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBr);
    DeleteObject(hPenBorder);

    // Status & Footer (Two separate rows to guarantee zero overlap)
    SelectObject(hdc, hFontMain);
    SetTextColor(hdc, Theme::TextMuted);

    RECT rcStatus{ 16, 462, 484, 482 };
    std::wstring statusDisplay = m_statusText.empty() ? L("status_ready") : m_statusText;
    DrawTextW(hdc, statusDisplay.c_str(), -1, &rcStatus, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    // Subtle divider line
    HPEN hPenDiv = CreatePen(PS_SOLID, 1, Theme::CardBorder);
    HPEN hOldPenDiv = (HPEN)SelectObject(hdc, hPenDiv);
    MoveToEx(hdc, 16, 488, nullptr);
    LineTo(hdc, 484, 488);
    SelectObject(hdc, hOldPenDiv);
    DeleteObject(hPenDiv);

    // Brand / Version Footer
    RECT rcBrand{ 16, 494, 484, 514 };
    DrawTextW(hdc, MEMSTAND_BRAND_FOOTER, -1, &rcBrand, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
}


LRESULT MainWindow::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static const UINT s_uTaskbarRestart = RegisterWindowMessageW(L"TaskbarCreated");
    if (msg == s_uTaskbarRestart) {
        m_trayIcon.Initialize(hwnd);
        UpdateMetrics();
        return 0;
    }

    switch (msg) {
    case WM_CREATE: {
        DarkTheme::EnableImmersiveDarkMode(hwnd);
        CreateControls(hwnd);
        m_trayIcon.Initialize(hwnd);

        // Automation callback
        AutomationEngine::Instance().SetCallback([this](const std::wstring& reason, uint64_t freedBytes) {
            wchar_t buf[128];
            swprintf_s(buf, L("status_cleaned").c_str(), FormatMemoryBytes(freedBytes).c_str());
            m_statusText = std::wstring(reason) + L": " + buf;
            m_trayIcon.ShowBalloon(L"MemStand", m_statusText);
            PostMessageW(m_hwnd, WM_TIMER, 1, 0);
        });

        const auto& config = ConfigManager::Instance().Get();
        if (config.hotkey_enabled) {
            RegisterGlobalHotkey();
        }

        // Timer for 1-second refresh
        SetTimer(hwnd, m_timerId, 1000, nullptr);
        UpdateMetrics();
        return 0;
    }

    case WM_TIMER: {
        UpdateMetrics();
        return 0;
    }

    case WM_COMMAND: {
        int id = LOWORD(wParam);
        int code = HIWORD(wParam);

        // If message comes from a child control (lParam != 0), only accept BN_CLICKED for button actions.
        // This stops non-click notifications (BN_KILLFOCUS, BN_SETFOCUS, EN_CHANGE, etc.) from triggering commands.
        if (lParam != 0 && code != BN_CLICKED) {
            break;
        }

        if (id == IDC_TAB_BTN_DASHBOARD) {
            SwitchTab(0);
            return 0;
        }

        if (id == IDC_TAB_BTN_SETTINGS) {
            SwitchTab(1);
            return 0;
        }

        if (id == IDC_BTN_START_STOP || id == IDM_TRAY_TOGGLE_TIMER) {
            ToggleMonitoring();
            return 0;
        }

        if (id == IDC_BTN_PURGE_STANDBY || id == IDM_TRAY_PURGE_STANDBY) {
            uint64_t freed = MemoryManager::Instance().PurgeStandbyList();
            wchar_t buf[128];
            swprintf_s(buf, L("status_cleaned").c_str(), FormatMemoryBytes(freed).c_str());
            m_statusText = buf;
            m_trayIcon.ShowBalloon(L"MemStand", m_statusText);
            UpdateMetrics();
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        }

        if (id == IDC_BTN_CLEAN_ALL || id == IDM_TRAY_CLEAN_ALL) {
            uint64_t freed = MemoryManager::Instance().PurgeAll();
            wchar_t buf[128];
            swprintf_s(buf, L("status_cleaned").c_str(), FormatMemoryBytes(freed).c_str());
            m_statusText = buf;
            m_trayIcon.ShowBalloon(L"MemStand", m_statusText);
            UpdateMetrics();
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        }

        if (id == IDC_BTN_SAVE_SETTINGS) {
            ApplySettingsFromUI();
            return 0;
        }

        if (id == IDC_BTN_AUTO_DETECT) {
            AutoDetectOptimalSettings();
            return 0;
        }

        if (id == IDC_BTN_CHECK_UPDATES || id == IDM_TRAY_CHECK_UPDATES) {
            CheckForUpdates(false);
            return 0;
        }

        if (id == IDM_TRAY_SHOW) {
            if (IsIconic(hwnd)) {
                ShowWindow(hwnd, SW_RESTORE);
            } else {
                ShowWindow(hwnd, SW_SHOW);
            }
            SetForegroundWindow(hwnd);
            return 0;
        }

        if (id == IDM_TRAY_EXIT) {
            m_trayIcon.Remove();
            DestroyWindow(hwnd);
            return 0;
        }
        break;
    }

    case WM_MEMSTAND_TRAY: {
        UINT msgType = LOWORD(lParam);
        if (msgType == WM_LBUTTONUP || msgType == WM_LBUTTONDBLCLK || msgType == NIN_SELECT || msgType == NIN_BALLOONUSERCLICK) {
            if (IsIconic(hwnd)) {
                ShowWindow(hwnd, SW_RESTORE);
            } else {
                ShowWindow(hwnd, SW_SHOW);
            }
            SetForegroundWindow(hwnd);
        } else if (msgType == WM_RBUTTONUP || msgType == WM_CONTEXTMENU) {
            m_trayIcon.ShowContextMenu(hwnd);
        }
        return 0;
    }

    case WM_HOTKEY: {
        if (wParam == HOTKEY_ID) {
            uint64_t freed = MemoryManager::Instance().PurgeStandbyList();
            wchar_t buf[128];
            swprintf_s(buf, L("status_cleaned").c_str(), FormatMemoryBytes(freed).c_str());
            m_statusText = L("reason_hotkey") + L": " + buf;
            m_trayIcon.ShowBalloon(L"MemStand", m_statusText);
            UpdateMetrics();
        }
        return 0;
    }

    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX: {
        HDC hdcEdit = (HDC)wParam;
        SetTextColor(hdcEdit, Theme::TextPrimary);
        SetBkColor(hdcEdit, Theme::InputBg);
        return (LRESULT)DarkTheme::GetInputBrush();
    }

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN: {
        HDC hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, Theme::TextPrimary);
        SetBkColor(hdcStatic, Theme::BgColor);
        return (LRESULT)DarkTheme::GetBgBrush();
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rcClient;
        GetClientRect(hwnd, &rcClient);

        // Double buffering
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
        HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hbmMem);

        FillRect(hdcMem, &rcClient, DarkTheme::GetBgBrush());

        if (m_currentTab == 0) {
            DrawDashboard(hdcMem, rcClient);
        } else {
            DrawSettings(hdcMem, rcClient);
        }

        BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hdcMem, 0, 0, SRCCOPY);

        SelectObject(hdcMem, hOldBmp);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_CLOSE: {
        if (ConfigManager::Instance().Get().minimize_to_tray_on_close) {
            ShowWindow(hwnd, SW_HIDE);
        } else {
            m_trayIcon.Remove();
            DestroyWindow(hwnd);
        }
        return 0;
    }

    case WM_DESTROY: {
        KillTimer(hwnd, m_timerId);
        UnregisterGlobalHotkey();
        m_trayIcon.Remove();
        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

} // namespace MemStand
