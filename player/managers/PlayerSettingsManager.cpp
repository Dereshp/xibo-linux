#include "PlayerSettingsManager.hpp"

PlayerSettingsManager::PlayerSettingsManager(const FilePath& settingsFile) :
    SettingsManager{settingsFile}
{
}

PlayerSettings PlayerSettingsManager::loadImpl()
{
    PlayerSettings settings;

    loadHelper(settings.dimensions.width,
               settings.dimensions.height,
               settings.dimensions.x,
               settings.dimensions.y,
               settings.logLevel,
               settings.displayName,
               settings.preventSleep,
               settings.statsEnabled,
               settings.screenshotSize,
               settings.collectInterval,
               settings.downloadEndWindow,
               settings.xmrNetworkAddress,
               settings.embeddedServerPort,
               settings.maxLogFilesUploads,
               settings.screenshotInterval,
               settings.statusLayoutUpdate,
               settings.downloadStartWindow,
               settings.screenshotRequested,
               settings.shellCommandsEnabled,
               settings.maxConcurrentDownloads,
               settings.modifiedLayoutsEnabled);

    return settings;
}

void PlayerSettingsManager::updateImpl(const PlayerSettings& settings)
{
    updateHelper(settings.dimensions.width,
                 settings.dimensions.height,
                 settings.dimensions.x,
                 settings.dimensions.y,
                 settings.logLevel,
                 settings.displayName,
                 settings.preventSleep,
                 settings.statsEnabled,
                 settings.screenshotSize,
                 settings.collectInterval,
                 settings.downloadEndWindow,
                 settings.xmrNetworkAddress,
                 settings.embeddedServerPort,
                 settings.maxLogFilesUploads,
                 settings.screenshotInterval,
                 settings.statusLayoutUpdate,
                 settings.downloadStartWindow,
                 settings.screenshotRequested,
                 settings.shellCommandsEnabled,
                 settings.maxConcurrentDownloads,
                 settings.modifiedLayoutsEnabled);
}
