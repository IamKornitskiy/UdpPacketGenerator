#ifndef THEMESETTINGSMANAGER_H
#define THEMESETTINGSMANAGER_H

#include "abstract_settings_manager.h"
#include <QString>

class ThemeSettingsManager : public AbstractSettingsManager
{
    Q_OBJECT

public:
    explicit ThemeSettingsManager(QObject *parent = nullptr);

    QString currentTheme() const { return m_currentTheme; }
    void setCurrentTheme(const QString &themeName);

signals:
    void themeChanged(const QString &themeName);

protected:
    void setParams(QSettings &settings) override;
    void getParams(QSettings &settings) override;
    void defaultSettings() override;

private:
    QString m_currentTheme;
};

#endif // THEMESETTINGSMANAGER_H
