#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>

class VersionChecker : public QObject
{
    Q_OBJECT

    // Allow test class to access private methods
    friend class TestVersionChecker;

public:
    explicit VersionChecker(QObject *parent = nullptr);

    // Check for newer version on GitHub
    void checkForUpdates();

    // Returns true if check is currently in progress
    bool isChecking() const { return m_isChecking; }

    // Returns the latest version string (empty if not known)
    QString latestVersion() const { return m_latestVersion; }

    // Returns true if a newer version is available
    bool hasNewVersion() const { return m_hasNewVersion; }

signals:
    // Emitted when check is complete
    // newerAvailable: true if a newer version exists
    // latestVersion: the latest version string (e.g., "v2.1.0")
    // error: error message if any (empty if success)
    void checkComplete(bool newerAvailable, const QString &latestVersion, const QString &error);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_latestVersion;
    bool m_hasNewVersion = false;
    bool m_isChecking = false;

    // Compare semantic versions (e.g., "v2.0.1" vs "v2.1.0")
    bool isNewerVersion(const QString &current, const QString &latest);

    // Parse GitHub releases JSON and return the latest stable version tag
    // Returns empty string if no stable release found or JSON is invalid
    QString parseLatestVersionFromJson(const QByteArray &json);
};
