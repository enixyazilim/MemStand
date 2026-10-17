#include "automation_engine.hpp"
#include "timer_resolution.hpp"
#include "../i18n/localization.hpp"

namespace MemStand {

void AutomationEngine::Start() {
    if (m_running) return;
    m_running = true;
    m_workerThread = std::thread(&AutomationEngine::WorkerLoop, this);
}

void AutomationEngine::Stop() {
    if (!m_running) return;
    m_running = false;
    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }
}

void AutomationEngine::WorkerLoop() {
    while (m_running) {
        const auto& config = ConfigManager::Instance().Get();
        int sleepMs = config.polling_rate_ms;
        if (sleepMs < 250) sleepMs = 250;
        if (sleepMs > 10000) sleepMs = 10000;

        // Sleep in chunks so Stop() is responsive
        int chunks = sleepMs / 100;
        for (int i = 0; i < chunks && m_running; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        if (!m_running) break;

        auto now = std::chrono::steady_clock::now();
        auto snapshot = MemoryManager::Instance().GetSnapshot();

        // Re-enforce timer resolution to prevent Windows / other apps from resetting it
        if (config.timer_resolution_enabled) {
            TimerResolutionManager::Instance().SetResolution(config.desired_timer_resolution_ms);
        }

        // 1. Standby list threshold check
        if (config.auto_clean_standby) {
            uint64_t standbyThreshBytes = config.standby_threshold_mb * 1024ULL * 1024ULL;
            uint64_t freeThreshBytes = config.free_mem_threshold_mb * 1024ULL * 1024ULL;

            auto standbyCooldown = std::chrono::duration_cast<std::chrono::seconds>(now - m_lastStandbyClean).count();

            if (snapshot.standby_bytes >= standbyThreshBytes &&
                snapshot.free_ram_bytes <= freeThreshBytes &&
                standbyCooldown >= 10) {

                uint64_t freed = MemoryManager::Instance().PurgeStandbyList();
                m_lastStandbyClean = now;

                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_callback) {
                    m_callback(L("reason_auto_standby"), freed);
                }
            }
        }

        // 2. High RAM pressure check
        if (config.auto_clean_high_ram) {
            auto ramCooldown = std::chrono::duration_cast<std::chrono::seconds>(now - m_lastHighRamClean).count();

            if (snapshot.usage_percentage >= config.high_ram_threshold_percent && ramCooldown >= 30) {
                uint64_t freed = 0;
                freed += MemoryManager::Instance().EmptyWorkingSets();
                freed += MemoryManager::Instance().FlushSystemFileCache();
                freed += MemoryManager::Instance().PurgeStandbyList();
                m_lastHighRamClean = now;

                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_callback) {
                    m_callback(L("reason_auto_high_ram"), freed);
                }
            }
        }
    }
}

} // namespace MemStand
