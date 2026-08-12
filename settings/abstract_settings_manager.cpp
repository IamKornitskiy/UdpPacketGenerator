#include "abstract_settings_manager.h"

AbstractSettingsManager::AbstractSettingsManager(const QString &groupName, QObject *parent)
    : QObject{parent}
    , m_GroupName(groupName)
{}

void AbstractSettingsManager::saveSettings()
{
    QSettings settings;

    settings.beginGroup(m_GroupName);
    setParams(settings);
    settings.endGroup();
}

void AbstractSettingsManager::loadSettings()
{
    QSettings settings;

    settings.beginGroup(m_GroupName);
    getParams(settings);
    settings.endGroup();
}
