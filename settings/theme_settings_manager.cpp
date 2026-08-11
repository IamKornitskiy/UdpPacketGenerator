#include "theme_settings_manager.h"

ThemeSettingsManager::ThemeSettingsManager(QObject *parent)
    : AbstractSettingsManager("Theme", parent)
{
    loadSettings();
}

void ThemeSettingsManager::setCurrentTheme(const QString &themeName)
{
    if (m_currentTheme != themeName) {
        m_currentTheme = themeName;
        saveSettings();
        emit themeChanged(themeName);
        emit settingsChanged();
    }
}

void ThemeSettingsManager::setParams(QSettings &settings)
{
    settings.setValue("currentTheme", m_currentTheme);
}

void ThemeSettingsManager::getParams(QSettings &settings)
{
    m_currentTheme = settings.value("currentTheme", "dark").toString();
}

void ThemeSettingsManager::defaultSettings()
{
    m_currentTheme = "dark";
}
