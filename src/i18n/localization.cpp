#include "localization.hpp"

namespace MemStand {

Localization::Localization() {
    LoadBuiltinFallbacks();
}

void Localization::LoadBuiltinFallbacks() {
    // Built-in English
    auto& en = m_builtinLanguages["english"];
    en["app_title"] = L"MemStand - Smart Memory & Timer Manager";
    en["tab_dashboard"] = L"Dashboard";
    en["tab_settings"] = L"Settings";
    en["btn_start"] = L"▶ START MONITORING";
    en["btn_stop"] = L"⏹ STOP MONITORING";
    en["status_running"] = L"Running (Timer & Automation Active)";
    en["status_stopped"] = L"Stopped (Standby)";
    en["auto_start_monitoring"] = L"Auto-start Monitoring on Launch";
    en["settings_cat_automation"] = L"Smart Memory Automation";
    en["settings_cat_general"] = L"System & Startup Preferences";
    en["ram_usage"] = L"RAM Usage";
    en["standby_list"] = L"Standby List";
    en["free_memory"] = L"Free Memory";
    en["modified_list"] = L"Modified List";
    en["system_cache"] = L"System File Cache";
    en["btn_purge_standby"] = L"Purge Standby";
    en["btn_clean_all"] = L"Clean All";
    en["btn_save_settings"] = L"Save Settings";
    en["btn_auto_detect"] = L"⚡ Auto-Calculate";
    en["status_auto_detected"] = L"Optimal values calculated for your system (%llu GB RAM).";
    en["timer_resolution_title"] = L"High-Precision Timer";
    en["current_timer"] = L"Current System Timer";
    en["target_timer"] = L"Target Timer";
    en["timer_status_active"] = L"Active (Low Latency)";
    en["timer_status_inactive"] = L"System Default";
    en["timer_enable"] = L"Enable Timer Resolution";
    en["automation_title"] = L"Smart Automation Settings";
    en["auto_standby_clean"] = L"Auto Purge Standby List";
    en["standby_threshold"] = L"Standby Threshold (MB):";
    en["free_memory_threshold"] = L"Free RAM Threshold (MB):";
    en["auto_ram_clean"] = L"Auto Clean On High Memory Pressure";
    en["ram_threshold_percent"] = L"RAM Usage Limit (%):";
    en["polling_rate"] = L"Polling Rate (ms):";
    en["general_settings_title"] = L"General Preferences";
    en["language_label"] = L"Interface Language";
    en["start_with_windows"] = L"Start with Windows (UAC-less)";
    en["start_minimized"] = L"Start Minimized to System Tray";
    en["minimize_to_tray_on_close"] = L"Minimize to Tray on Close";
    en["hotkey_enable"] = L"Global Purge Hotkey";
    en["hotkey_label"] = L"Hotkey (Ctrl+Shift+M)";
    en["show_notifications"] = L"Show Balloon Notifications";
    en["tray_open"] = L"Show MemStand";
    en["tray_clean_standby"] = L"Purge Standby List";
    en["tray_clean_all"] = L"Clean All Memory Regions";
    en["tray_toggle_timer"] = L"Toggle 0.50ms Timer";
    en["tray_check_updates"] = L"Check for Updates";
    en["tray_exit"] = L"Exit";
    en["btn_check_updates"] = L"Check Updates";
    en["status_ready"] = L"Ready";
    en["status_cleaned"] = L"Cleaned! Memory Freed: %s";
    en["status_error_privilege"] = L"Error: Administrator privileges required!";
    en["status_timer_applied"] = L"Timer resolution applied: %.2f ms";
    en["status_timer_reverted"] = L"Timer reverted to system default.";
    en["status_settings_saved"] = L"Settings saved successfully.";
    en["status_update_checking"] = L"Checking for updates...";
    en["update_available_title"] = L"MemStand Update Available";
    en["update_available_msg"] = L"A new version (%s) is available! Click to open the release page.";
    en["update_latest_msg"] = L"You are using the latest version of MemStand (%s).";
    en["update_error_msg"] = L"Update check failed: %s";
    en["reason_auto_standby"] = L"Auto Standby";
    en["reason_auto_high_ram"] = L"Auto High RAM";
    en["reason_hotkey"] = L"Hotkey";

    // Built-in Turkish
    auto& tr = m_builtinLanguages["turkish"];
    tr["app_title"] = L"MemStand - Akıllı Bellek & Zamanlayıcı Yöneticisi";
    tr["tab_dashboard"] = L"Kontrol Paneli";
    tr["tab_settings"] = L"Ayarlar";
    tr["btn_start"] = L"▶ İZLEMEYİ BAŞLAT";
    tr["btn_stop"] = L"⏹ İZLEMEYİ DURDUR";
    tr["status_running"] = L"Çalışıyor (Zamanlayıcı & Otomasyon Aktif)";
    tr["status_stopped"] = L"Durduruldu (Beklemede)";
    tr["auto_start_monitoring"] = L"Uygulama Açıldığında İzlemeyi Otomatik Başlat";
    tr["settings_cat_automation"] = L"Akıllı Bellek Otomasyonu";
    tr["settings_cat_general"] = L"Sistem ve Başlangıç Tercihleri";
    tr["ram_usage"] = L"RAM Kullanımı";
    tr["standby_list"] = L"Bekleme Listesi (Standby)";
    tr["free_memory"] = L"Boş Bellek";
    tr["modified_list"] = L"Değiştirilmiş Liste";
    tr["system_cache"] = L"Sistem Önbelleği";
    tr["btn_purge_standby"] = L"Standby Temizle";
    tr["btn_clean_all"] = L"Tümünü Temizle";
    tr["btn_save_settings"] = L"Ayarları Kaydet";
    tr["btn_auto_detect"] = L"⚡ Önerilen Değerler";
    tr["status_auto_detected"] = L"Sisteminiz (%llu GB RAM) için önerilen değerler uygulandı.";
    tr["timer_resolution_title"] = L"Yüksek Hassasiyetli Zamanlayıcı";
    tr["current_timer"] = L"Mevcut Sistem Zamanlayıcısı";
    tr["target_timer"] = L"Hedef Zamanlayıcı";
    tr["timer_status_active"] = L"Aktif (Düşük Gecikme)";
    tr["timer_status_inactive"] = L"Sistem Varsayılanı";
    tr["timer_enable"] = L"Zamanlayıcıyı Etkinleştir";
    tr["auto_standby_clean"] = L"Standby Listesini Otomatik Temizle";
    tr["standby_threshold"] = L"Standby Eşiği (MB):";
    tr["free_memory_threshold"] = L"Boş RAM Eşiği (MB):";
    tr["auto_ram_clean"] = L"Yüksek RAM Baskısında Temizle";
    tr["ram_threshold_percent"] = L"RAM Eşik Sınırı (%):";
    tr["polling_rate"] = L"Kontrol Sıklığı (ms):";
    tr["language_label"] = L"Arayüz Dili";
    tr["start_with_windows"] = L"Windows Açılışında Başlat (UAC'siz)";
    tr["start_minimized"] = L"Sistem Tepsisine Küçültülmüş Başlat";
    tr["minimize_to_tray_on_close"] = L"Kapatıldığında Tepsiye Küçült";
    tr["hotkey_enable"] = L"Global Temizleme Kısayolu (Ctrl+Shift+M)";
    tr["show_notifications"] = L"Masaüstü Bildirimlerini Göster";
    tr["tray_open"] = L"MemStand'i Göster";
    tr["tray_clean_standby"] = L"Standby Listesini Temizle";
    tr["tray_clean_all"] = L"Tüm Bellek Bölgelerini Temizle";
    tr["tray_check_updates"] = L"Güncellemeleri Denetle";
    tr["tray_exit"] = L"Çıkış";
    tr["btn_check_updates"] = L"Güncellemeleri Denetle";
    tr["status_ready"] = L"Hazır";
    tr["status_cleaned"] = L"Temizlendi! Kazanılan Alan: %s";
    tr["status_error_privilege"] = L"Hata: Yönetici hakları gerekiyor!";
    tr["status_timer_applied"] = L"Zamanlayıcı çözünürlüğü uygulandı: %.2f ms";
    tr["status_timer_reverted"] = L"Zamanlayıcı sistem varsayılanına döndürüldü.";
    tr["status_settings_saved"] = L"Ayarlar başarıyla kaydedildi.";
    tr["status_update_checking"] = L"Güncellemeler denetleniyor...";
    tr["update_available_title"] = L"MemStand Güncellemesi Mevcut";
    tr["update_available_msg"] = L"Yeni bir sürüm (%s) yayınlandı! İndirme sayfasını açmak için tıklayın.";
    tr["update_latest_msg"] = L"En güncel MemStand sürümünü (%s) kullanıyorsunuz.";
    tr["update_error_msg"] = L"Güncelleme denetimi başarısız: %s";
    tr["reason_auto_standby"] = L"Otomatik Standby";
    tr["reason_auto_high_ram"] = L"Otomatik Yüksek RAM";
    tr["reason_hotkey"] = L"Kısayol Tuşu";
}


bool Localization::ParseLanguageJson(const std::string& filePath, LanguageInfo& info, std::map<std::string, std::wstring>& strings) {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    auto ExtractString = [&](const std::string& key) -> std::string {
        size_t pos = content.find("\"" + key + "\"");
        if (pos == std::string::npos) return "";
        pos = content.find(':', pos);
        if (pos == std::string::npos) return "";
        size_t start = content.find('\"', pos);
        if (start == std::string::npos) return "";
        size_t end = content.find('\"', start + 1);
        if (end == std::string::npos) return "";
        return content.substr(start + 1, end - start - 1);
    };

    std::string langName = ExtractString("language_name");
    std::string langCode = ExtractString("language_code");
    if (langName.empty()) {
        std::filesystem::path p(filePath);
        langName = p.stem().string();
    }

    std::filesystem::path p(filePath);
    info.id = p.stem().string();
    info.displayName = Utf8ToWide(langName);
    info.filePath = filePath;

    // Parse "strings" object
    size_t stringsPos = content.find("\"strings\"");
    if (stringsPos != std::string::npos) {
        size_t blockStart = content.find('{', stringsPos);
        if (blockStart != std::string::npos) {
            size_t blockEnd = content.find('}', blockStart);
            if (blockEnd != std::string::npos) {
                std::string block = content.substr(blockStart, blockEnd - blockStart + 1);
                
                size_t cursor = 0;
                while (cursor < block.size()) {
                    size_t kStart = block.find('\"', cursor);
                    if (kStart == std::string::npos) break;
                    size_t kEnd = block.find('\"', kStart + 1);
                    if (kEnd == std::string::npos) break;
                    std::string key = block.substr(kStart + 1, kEnd - kStart - 1);

                    size_t colon = block.find(':', kEnd);
                    if (colon == std::string::npos) break;

                    size_t vStart = block.find('\"', colon);
                    if (vStart == std::string::npos || vStart > blockEnd) break;
                    size_t vEnd = block.find('\"', vStart + 1);
                    if (vEnd == std::string::npos) break;
                    std::string val = block.substr(vStart + 1, vEnd - vStart - 1);

                    strings[key] = Utf8ToWide(val);
                    cursor = vEnd + 1;
                }
            }
        }
    }

    return true;
}

void Localization::ScanLanguagesFolder() {
    m_availableLanguages.clear();

    // Default registered built-ins
    m_availableLanguages.push_back({ "turkish", L"Türkçe", "" });
    m_availableLanguages.push_back({ "english", L"English", "" });

    std::wstring langDir = GetExecutableDir() + L"\\languages";
    try {
        if (std::filesystem::exists(langDir)) {
            for (const auto& entry : std::filesystem::directory_iterator(langDir)) {
                if (entry.is_regular_file() && entry.path().extension() == ".json") {
                    std::string stem = entry.path().stem().string();
                    if (stem == "template") continue; // skip template

                    LanguageInfo info;
                    std::map<std::string, std::wstring> dummy;
                    if (ParseLanguageJson(entry.path().string(), info, dummy)) {
                        bool exists = false;
                        for (auto& item : m_availableLanguages) {
                            if (item.id == info.id) {
                                item.displayName = info.displayName;
                                item.filePath = info.filePath;
                                exists = true;
                                break;
                            }
                        }
                        if (!exists) {
                            m_availableLanguages.push_back(info);
                        }
                    }
                }
            }
        }
    } catch (...) {}
}

void Localization::Initialize() {
    ScanLanguagesFolder();
}

bool Localization::LoadLanguage(const std::string& langId) {
    m_currentStrings.clear();
    m_currentLangId = langId;

    // First load fallback built-in so missing keys in custom JSON will have fallback values
    auto itBuiltin = m_builtinLanguages.find(langId);
    if (itBuiltin != m_builtinLanguages.end()) {
        m_currentStrings = itBuiltin->second;
    } else {
        // default fallback English
        m_currentStrings = m_builtinLanguages["english"];
    }

    // Next check if a JSON file exists in languages/
    for (const auto& lang : m_availableLanguages) {
        if (lang.id == langId && !lang.filePath.empty()) {
            LanguageInfo info;
            std::map<std::string, std::wstring> fileStrings;
            if (ParseLanguageJson(lang.filePath, info, fileStrings)) {
                for (const auto& kv : fileStrings) {
                    m_currentStrings[kv.first] = kv.second;
                }
            }
            break;
        }
    }

    return true;
}

std::wstring Localization::Get(const std::string& key) const {
    auto it = m_currentStrings.find(key);
    if (it != m_currentStrings.end()) {
        return it->second;
    }
    return Utf8ToWide(key);
}

std::vector<LanguageInfo> Localization::GetAvailableLanguages() const {
    return m_availableLanguages;
}

} // namespace MemStand
