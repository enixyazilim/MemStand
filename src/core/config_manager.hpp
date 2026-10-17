#pragma once

#include "../common.hpp"

namespace MemStand {

struct AppConfig {
    std::string language = "turkish";
    uint64_t standby_threshold_mb = 1024;
    uint64_t free_mem_threshold_mb = 1024;
    int high_ram_threshold_percent = 85;
    bool auto_clean_standby = true;
    bool auto_clean_high_ram = true;
    int polling_rate_ms = 1000;

    bool auto_start_monitoring = false;
    bool timer_resolution_enabled = true;
    double desired_timer_resolution_ms = 0.50;

    bool start_with_windows = false;
    bool start_minimized = false;
    bool minimize_to_tray_on_close = true;

    bool hotkey_enabled = true;
    UINT hotkey_modifiers = MOD_CONTROL | MOD_SHIFT;
    UINT hotkey_key = 'M';

    bool show_notifications = true;
};

class ConfigManager {
public:
    static ConfigManager& Instance() {
        static ConfigManager instance;
        return instance;
    }

    bool Load();
    bool Save();
    void ApplyOptimalDefaults();

    AppConfig& Get() { return m_config; }
    const AppConfig& Get() const { return m_config; }

private:
    ConfigManager() = default;
    std::wstring GetConfigPath();

    AppConfig m_config;
};

} // namespace MemStand
