#include "tray_icon.hpp"
#include "main_window.hpp"
#include "../i18n/localization.hpp"
#include "../core/timer_resolution.hpp"
#include "../core/config_manager.hpp"

namespace MemStand {

bool TrayIcon::Initialize(HWND hwnd) {
    m_hwnd = hwnd;
    ZeroMemory(&m_nid, sizeof(m_nid));
    m_nid.cbSize = sizeof(NOTIFYICONDATAW);
    m_nid.hWnd = hwnd;
    m_nid.uID = 1;
    m_nid.uFlags = NIF_MESSAGE | NIF_TIP | NIF_ICON;
    m_nid.uCallbackMessage = WM_MEMSTAND_TRAY;
    wcscpy_s(m_nid.szTip, L"MemStand");

    m_currentIcon = CreateDynamicIcon(0);
    m_nid.hIcon = m_currentIcon;

    m_added = Shell_NotifyIconW(NIM_ADD, &m_nid);
    if (m_added) {
        m_nid.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &m_nid);
    }
    return m_added;
}

HICON TrayIcon::CreateDynamicIcon(int percent) {
    int cx = GetSystemMetrics(SM_CXSMICON);
    int cy = GetSystemMetrics(SM_CYSMICON);
    if (cx <= 0) cx = 16;
    if (cy <= 0) cy = 16;

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HDC hdcMask = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cx;
    bmi.bmiHeader.biHeight = cy;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;

    void* pBits = nullptr;
    HBITMAP hbmColor = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    HBITMAP hbmMask = CreateBitmap(cx, cy, 1, 1, nullptr);

    HBITMAP hOldColor = (HBITMAP)SelectObject(hdcMem, hbmColor);
    HBITMAP hOldMask = (HBITMAP)SelectObject(hdcMask, hbmMask);

    // Color based on memory usage
    COLORREF bgColor;
    if (percent < 70) {
        bgColor = RGB(16, 185, 129); // Emerald Green
    } else if (percent < 85) {
        bgColor = RGB(245, 158, 11);  // Amber Orange
    } else {
        bgColor = RGB(239, 68, 68);   // Crimson Red
    }

    HBRUSH hbrBg = CreateSolidBrush(bgColor);
    RECT rc{ 0, 0, cx, cy };
    FillRect(hdcMem, &rc, hbrBg);
    DeleteObject(hbrBg);

    // Mask bitmap is 0 for opaque pixels
    RECT rcMask{ 0, 0, cx, cy };
    HBRUSH hbrBlack = (HBRUSH)GetStockObject(BLACK_BRUSH);
    FillRect(hdcMask, &rcMask, hbrBlack);

    // Draw text: e.g. "42"
    wchar_t txt[16];
    swprintf_s(txt, L"%d", percent);

    HFONT hFont = CreateFontW(
        -(cy - 4), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial"
    );

    HFONT hOldFont = (HFONT)SelectObject(hdcMem, hFont);
    SetBkMode(hdcMem, TRANSPARENT);
    SetTextColor(hdcMem, RGB(255, 255, 255));
    DrawTextW(hdcMem, txt, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdcMem, hOldFont);
    DeleteObject(hFont);

    SelectObject(hdcMem, hOldColor);
    SelectObject(hdcMask, hOldMask);
    DeleteDC(hdcMem);
    DeleteDC(hdcMask);
    ReleaseDC(nullptr, hdcScreen);

    ICONINFO ii{};
    ii.fIcon = TRUE;
    ii.hbmMask = hbmMask;
    ii.hbmColor = hbmColor;

    HICON hIcon = CreateIconIndirect(&ii);

    DeleteObject(hbmColor);
    DeleteObject(hbmMask);

    return hIcon;
}

void TrayIcon::Update(int memoryUsagePercent, const std::wstring& tooltip) {
    if (!m_added) return;

    bool iconChanged = (memoryUsagePercent != m_lastPercent || !m_currentIcon);
    bool tooltipChanged = (tooltip != m_lastTooltip);

    if (!iconChanged && !tooltipChanged) {
        return; // Avoid bombarding Explorer with 10,000+ redundant NIM_MODIFY calls over long sessions
    }

    m_nid.uFlags = NIF_MESSAGE;

    if (iconChanged) {
        m_lastPercent = memoryUsagePercent;
        HICON newIcon = CreateDynamicIcon(memoryUsagePercent);

        if (m_currentIcon) {
            DestroyIcon(m_currentIcon);
        }
        m_currentIcon = newIcon;
        m_nid.hIcon = m_currentIcon;
        m_nid.uFlags |= NIF_ICON;
    }

    if (tooltipChanged) {
        m_lastTooltip = tooltip;
        wcsncpy_s(m_nid.szTip, tooltip.c_str(), _TRUNCATE);
        m_nid.uFlags |= NIF_TIP;
    }

    m_nid.szInfo[0] = L'\0';
    m_nid.szInfoTitle[0] = L'\0';

    Shell_NotifyIconW(NIM_MODIFY, &m_nid);
}

void TrayIcon::ShowBalloon(const std::wstring& title, const std::wstring& message) {
    if (!m_added) return;
    if (!ConfigManager::Instance().Get().show_notifications) return;

    NOTIFYICONDATAW nid = m_nid;
    nid.uFlags = NIF_INFO;
    nid.dwInfoFlags = NIIF_INFO;
    nid.uTimeout = 3000;
    wcsncpy_s(nid.szInfoTitle, title.c_str(), _TRUNCATE);
    wcsncpy_s(nid.szInfo, message.c_str(), _TRUNCATE);
    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

void TrayIcon::ShowContextMenu(HWND hwnd) {
    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) return;

    AppendMenuW(hMenu, MF_STRING, IDM_TRAY_SHOW, L("tray_open").c_str());
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, IDM_TRAY_PURGE_STANDBY, L("tray_clean_standby").c_str());
    AppendMenuW(hMenu, MF_STRING, IDM_TRAY_CLEAN_ALL, L("tray_clean_all").c_str());

    bool isMonitoring = MainWindow::Instance().IsMonitoringActive();
    std::wstring toggleText = isMonitoring ? L("btn_stop") : L("btn_start");
    AppendMenuW(hMenu, MF_STRING, IDM_TRAY_TOGGLE_TIMER, toggleText.c_str());

    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, IDM_TRAY_CHECK_UPDATES, L("tray_check_updates").c_str());
    AppendMenuW(hMenu, MF_STRING, IDM_TRAY_EXIT, L("tray_exit").c_str());

    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hwnd);
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hwnd, nullptr);
    PostMessageW(hwnd, WM_NULL, 0, 0);

    DestroyMenu(hMenu);
}

void TrayIcon::Remove() {
    if (m_added) {
        Shell_NotifyIconW(NIM_DELETE, &m_nid);
        m_added = false;
    }
    if (m_currentIcon) {
        DestroyIcon(m_currentIcon);
        m_currentIcon = nullptr;
    }
}

} // namespace MemStand
