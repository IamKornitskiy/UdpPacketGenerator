#ifndef ABSTRACTSETTINGSMANAGER_H
#define ABSTRACTSETTINGSMANAGER_H

#include <QObject>
#include <QSettings>

class AbstractSettingsManager : public QObject
{
    Q_OBJECT
public:
    explicit AbstractSettingsManager(const QString &groupName, QObject *parent = nullptr);
    virtual ~AbstractSettingsManager() = default;

    void saveSettings();
    void loadSettings();

protected:
    virtual void setParams(QSettings &settings) = 0;
    virtual void getParams(QSettings &settings) = 0;
    virtual void defaultSettings() = 0;

    QString m_GroupName;

signals:
    void settingsChanged();
};

#endif // ABSTRACTSETTINGSMANAGER_H
