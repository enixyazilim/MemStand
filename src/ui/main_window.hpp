#pragma once

#include "../common.hpp"
#include "../../resources/resource.h"
#include "tray_icon.hpp"
#include "../core/memory_manager.hpp"
#include "../core/timer_resolution.hpp"
#include "../core/automation_engine.hpp"
#include "../core/config_manager.hpp"
#include "../i18n/localization.hpp"

namespace MemStand {

class MainWindow {
public:
    static MainWindow& Instance() {
        static MainWindow instance;
        return instance;
    }

    bool Create();
    void Show(int cmdShow = SW_SHOW);
    HWND GetHwnd() const { return m_hwnd; }
    bool IsMonitoringActive() const { return m_isMonitoringActive; }

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    MainWindow() = default;

    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void CreateControls(HWND hwnd);
    void UpdateMetrics();
    void RefreshLanguage();
    void ApplySettingsFromUI();
    void PopulateSettingsToUI();
    void RegisterGlobalHotkey();
    void UnregisterGlobalHotkey();
    void SwitchTab(int tabIndex);
    void ToggleMonitoring();
    void AutoDetectOptimalSettings();
    void CheckForUpdates(bool silentIfLatest = false);

    void DrawDashboard(HDC hdc, const RECT& rcClient);
    void DrawSettings(HDC hdc, const RECT& rcClient);

    HWND m_hwnd = nullptr;
    int m_currentTab = 0; // 0 = Dashboard, 1 = Settings

    // Navigation Tabs
    HWND m_hTabDashboard = nullptr;
    HWND m_hTabSettings = nullptr;

    // Dashboard Controls
    HWND m_hBtnStartStop = nullptr;
    HWND m_hBtnPurgeStandby = nullptr;
    HWND m_hBtnCleanAll = nullptr;

    // Settings Controls
    HWND m_hChkAutoStandby = nullptr;
    HWND m_hEditStandbyThresh = nullptr;
    HWND m_hEditFreeMemThresh = nullptr;
    HWND m_hChkAutoRam = nullptr;
    HWND m_hEditRamThresh = nullptr;
    HWND m_hEditPolling = nullptr;

    HWND m_hComboTimer = nullptr;
    HWND m_hChkAutoStartMonitoring = nullptr;
    HWND m_hChkStartWindows = nullptr;
    HWND m_hChkStartMinimized = nullptr;
    HWND m_hChkMinOnClose = nullptr;
    HWND m_hChkHotkey = nullptr;
    HWND m_hChkShowNotifications = nullptr;
    HWND m_hComboLanguage = nullptr;
    HWND m_hBtnSaveSettings = nullptr;
    HWND m_hBtnAutoDetect = nullptr;
    HWND m_hBtnCheckUpdates = nullptr;

    TrayIcon m_trayIcon;
    MemorySnapshot m_lastSnapshot{};
    std::wstring m_statusText;
    UINT_PTR m_timerId = 1;
    bool m_hotkeyRegistered = false;
    bool m_isMonitoringActive = false;
};

} // namespace MemStand
