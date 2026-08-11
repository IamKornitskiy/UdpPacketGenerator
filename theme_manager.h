#pragma once

#include <QObject>
#include <QString>
#include <QMap>
#include <QStringList>
#include "theme_settings_manager.h"

class QApplication;

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum class ThemeType { System, Custom };

    struct ThemeInfo {
        QString name;
        QString key;
        ThemeType type;
    };

    explicit ThemeManager(QApplication *app, QObject *parent = nullptr);

    QList<ThemeInfo> availableThemes() const;

    // Returns pair of (theme names, theme keys) for UI use
    QPair<QStringList, QStringList> themeNamesAndKeys() const;

    bool applyTheme(const QString &key);

    QString currentThemeKey() const;
    ThemeType currentThemeType() const;

signals:
    void themeChanged(const QString &key);

private:
    QApplication *m_app;
    ThemeSettingsManager *m_settings;
    QString m_currentThemeKey;
    ThemeType m_currentThemeType;

    QStringList systemStyles() const;

    bool applySystemStyle(const QString &key);
    bool applyCustomTheme(const QString &key);

    void loadFromSettings();
    void saveToSettings();
};
