#include "config_manager.hpp"

namespace MemStand {

std::wstring ConfigManager::GetConfigPath() {
    return GetExecutableDir() + L"\\config.json";
}

void ConfigManager::ApplyOptimalDefaults() {
    MEMORYSTATUSEX mem{};
    mem.dwLength = sizeof(mem);
    if (GlobalMemoryStatusEx(&mem)) {
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

        m_config.standby_threshold_mb = (standardGb * 1024) / 2;
        m_config.free_mem_threshold_mb = std::max<uint64_t>(1024, m_config.standby_threshold_mb / 4);
        m_config.high_ram_threshold_percent = (standardGb <= 8) ? 80 : (standardGb <= 32 ? 85 : 90);
        m_config.polling_rate_ms = 1000;
    }
}

bool ConfigManager::Load() {
    std::wstring path = GetConfigPath();
    std::ifstream file(path);
    if (!file.is_open()) {
        // Defaults: auto-detect system language and compute optimal RAM thresholds
        LANGID langId = GetUserDefaultUILanguage();
        if ((langId & 0xFF) == LANG_TURKISH) {
            m_config.language = "turkish";
        } else {
            m_config.language = "english";
        }
        ApplyOptimalDefaults();
        Save();
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    auto GetStringValue = [&](const std::string& key, const std::string& defVal) -> std::string {
        size_t pos = content.find("\"" + key + "\"");
        if (pos == std::string::npos) return defVal;
        pos = content.find(':', pos);
        if (pos == std::string::npos) return defVal;
        size_t start = content.find('\"', pos);
        if (start == std::string::npos) return defVal;
        size_t end = content.find('\"', start + 1);
        if (end == std::string::npos) return defVal;
        return content.substr(start + 1, end - start - 1);
    };

    auto GetBoolValue = [&](const std::string& key, bool defVal) -> bool {
        size_t pos = content.find("\"" + key + "\"");
        if (pos == std::string::npos) return defVal;
        pos = content.find(':', pos);
        if (pos == std::string::npos) return defVal;
        size_t t = content.find("true", pos);
        size_t f = content.find("false", pos);
        size_t nextComma = content.find_first_of(",}\n", pos);
        if (t != std::string::npos && t < nextComma) return true;
        if (f != std::string::npos && f < nextComma) return false;
        return defVal;
    };

    auto GetIntValue = [&](const std::string& key, int64_t defVal) -> int64_t {
        size_t pos = content.find("\"" + key + "\"");
        if (pos == std::string::npos) return defVal;
        pos = content.find(':', pos);
        if (pos == std::string::npos) return defVal;
        while (pos < content.size() && (content[pos] == ':' || content[pos] == ' ' || content[pos] == '\t')) pos++;
        try {
            return std::stoll(content.substr(pos));
        } catch (...) {
            return defVal;
        }
    };

    auto GetDoubleValue = [&](const std::string& key, double defVal) -> double {
        size_t pos = content.find("\"" + key + "\"");
        if (pos == std::string::npos) return defVal;
        pos = content.find(':', pos);
        if (pos == std::string::npos) return defVal;
        while (pos < content.size() && (content[pos] == ':' || content[pos] == ' ' || content[pos] == '\t')) pos++;
        try {
            return std::stod(content.substr(pos));
        } catch (...) {
            return defVal;
        }
    };

    m_config.language = GetStringValue("language", m_config.language);
    m_config.standby_threshold_mb = (uint64_t)GetIntValue("standby_threshold_mb", (int64_t)m_config.standby_threshold_mb);
    m_config.free_mem_threshold_mb = (uint64_t)GetIntValue("free_mem_threshold_mb", (int64_t)m_config.free_mem_threshold_mb);
    m_config.high_ram_threshold_percent = (int)GetIntValue("high_ram_threshold_percent", m_config.high_ram_threshold_percent);
    m_config.polling_rate_ms = (int)GetIntValue("polling_rate_ms", m_config.polling_rate_ms);

    m_config.auto_clean_standby = GetBoolValue("auto_clean_standby", m_config.auto_clean_standby);
    m_config.auto_clean_high_ram = GetBoolValue("auto_clean_high_ram", m_config.auto_clean_high_ram);

    m_config.timer_resolution_enabled = GetBoolValue("timer_resolution_enabled", m_config.timer_resolution_enabled);
    m_config.desired_timer_resolution_ms = GetDoubleValue("desired_timer_resolution_ms", m_config.desired_timer_resolution_ms);

    m_config.auto_start_monitoring = GetBoolValue("auto_start_monitoring", m_config.auto_start_monitoring);
    m_config.start_with_windows = GetBoolValue("start_with_windows", m_config.start_with_windows);
    m_config.start_minimized = GetBoolValue("start_minimized", m_config.start_minimized);
    m_config.minimize_to_tray_on_close = GetBoolValue("minimize_to_tray_on_close", m_config.minimize_to_tray_on_close);

    m_config.hotkey_enabled = GetBoolValue("hotkey_enabled", m_config.hotkey_enabled);
    m_config.show_notifications = GetBoolValue("show_notifications", m_config.show_notifications);

    // If config has legacy 1024/1024 defaults, upgrade them automatically to system-optimal values
    if (m_config.standby_threshold_mb == 1024 && m_config.free_mem_threshold_mb == 1024) {
        ApplyOptimalDefaults();
        Save();
    }

    return true;
}

bool ConfigManager::Save() {
    std::wstring path = GetConfigPath();
    std::ofstream file(path);
    if (!file.is_open()) return false;

    file << "{\n";
    file << "  \"language\": \"" << m_config.language << "\",\n";
    file << "  \"standby_threshold_mb\": " << m_config.standby_threshold_mb << ",\n";
    file << "  \"free_mem_threshold_mb\": " << m_config.free_mem_threshold_mb << ",\n";
    file << "  \"high_ram_threshold_percent\": " << m_config.high_ram_threshold_percent << ",\n";
    file << "  \"auto_clean_standby\": " << (m_config.auto_clean_standby ? "true" : "false") << ",\n";
    file << "  \"auto_clean_high_ram\": " << (m_config.auto_clean_high_ram ? "true" : "false") << ",\n";
    file << "  \"polling_rate_ms\": " << m_config.polling_rate_ms << ",\n";
    file << "  \"timer_resolution_enabled\": " << (m_config.timer_resolution_enabled ? "true" : "false") << ",\n";
    file << std::fixed << std::setprecision(2);
    file << "  \"desired_timer_resolution_ms\": " << m_config.desired_timer_resolution_ms << ",\n";
    file << "  \"auto_start_monitoring\": " << (m_config.auto_start_monitoring ? "true" : "false") << ",\n";
    file << "  \"start_with_windows\": " << (m_config.start_with_windows ? "true" : "false") << ",\n";
    file << "  \"start_minimized\": " << (m_config.start_minimized ? "true" : "false") << ",\n";
    file << "  \"minimize_to_tray_on_close\": " << (m_config.minimize_to_tray_on_close ? "true" : "false") << ",\n";
    file << "  \"hotkey_enabled\": " << (m_config.hotkey_enabled ? "true" : "false") << ",\n";
    file << "  \"show_notifications\": " << (m_config.show_notifications ? "true" : "false") << "\n";
    file << "}\n";

    return true;
}


} // namespace MemStand
