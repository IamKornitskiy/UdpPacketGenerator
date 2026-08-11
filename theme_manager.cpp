#include "theme_manager.h"
#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QDebug>

namespace {
const QMap<QString, QString> kThemeList = {{"dark", ":/style/dark.qss"},
                                           {"light", ":/style/light.qss"},
                                           {"breeze_dark", ":/style/breeze_dark.qss"},
                                           {"breeze_light", ":/style/breeze_light.qss"}};
}

ThemeManager::ThemeManager(QApplication *app, QObject *parent)
    : QObject(parent)
    , m_app(app)
    , m_settings(new ThemeSettingsManager(this))
{
    loadFromSettings();
    applyTheme(m_currentThemeKey);
}

QList<ThemeManager::ThemeInfo> ThemeManager::availableThemes() const
{
    QList<ThemeInfo> themes;

    for (const QString &style : systemStyles()) {
        themes.append({style, style, ThemeType::System});
    }

    for (const QString &key : kThemeList.keys()) {
        if (!systemStyles().contains(key)) {
            themes.append({key, key, ThemeType::Custom});
        }
    }

    return themes;
}

QPair<QStringList, QStringList> ThemeManager::themeNamesAndKeys() const
{
    QStringList names, keys;
    for (const ThemeInfo &theme : availableThemes()) {
        names.append(theme.name);
        keys.append(theme.key);
    }
    return {names, keys};
}

QStringList ThemeManager::systemStyles() const
{
    return QStyleFactory::keys();
}

bool ThemeManager::applyTheme(const QString &key)
{
    bool success = false;
    ThemeType type = ThemeType::Custom;

    if (kThemeList.contains(key)) {
        success = applyCustomTheme(key);
        type = ThemeType::Custom;
    } else {
        success = applySystemStyle(key);
        type = ThemeType::System;
    }

    if (success) {
        m_currentThemeKey = key;
        m_currentThemeType = type;
        saveToSettings();
        emit themeChanged(key);
        return true;
    }

    return false;
}

bool ThemeManager::applySystemStyle(const QString &key)
{
    if (!m_app) return false;

    m_app->setStyleSheet(QString());

    QStyle *style = QStyleFactory::create(key);
    if (style) {
        m_app->setStyle(style);
        return true;
    }
    return false;
}

bool ThemeManager::applyCustomTheme(const QString &key)
{
    if (!m_app) return false;

    QString path = kThemeList.value(key);
    QFile file(path);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        m_app->setStyleSheet(styleSheet);
        return true;
    }

    qDebug() << "Failed to load theme:" << path;
    return false;
}

void ThemeManager::loadFromSettings()
{
    QString themeName = m_settings->currentTheme();
    
    if (systemStyles().contains(themeName)) {
        m_currentThemeKey = themeName;
        m_currentThemeType = ThemeType::System;
    } else if (kThemeList.contains(themeName)) {
        m_currentThemeKey = themeName;
        m_currentThemeType = ThemeType::Custom;
    } else {
        m_currentThemeKey = "dark";
        m_currentThemeType = ThemeType::Custom;
    }
}

void ThemeManager::saveToSettings()
{
    m_settings->setCurrentTheme(m_currentThemeKey);
}

QString ThemeManager::currentThemeKey() const
{
    return m_currentThemeKey;
}

ThemeManager::ThemeType ThemeManager::currentThemeType() const
{
    return m_currentThemeType;
}
