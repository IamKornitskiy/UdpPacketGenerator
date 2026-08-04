#include "version_checker.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>

// GitHub API URL for releases of this repo
static const QString k_GITHUB_API_URL
    = "https://api.github.com/repos/IamKornitskiy/UdpPacketGenerator/releases";

VersionChecker::VersionChecker(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &VersionChecker::onReplyFinished);
}

void VersionChecker::checkForUpdates()
{
    if (m_isChecking)
        return;

    m_isChecking = true;
    m_hasNewVersion = false;
    m_latestVersion.clear();

    QNetworkRequest request((QUrl(k_GITHUB_API_URL)));
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      QString("UdpPacketGenerator/%1").arg(APP_VERSION));
    m_networkManager->get(request);
}

void VersionChecker::onReplyFinished(QNetworkReply *reply)
{
    m_isChecking = false;

    QString error;
    bool newerAvailable = false;
    QString latestVersion;

    if (reply->error() != QNetworkReply::NoError) {
        error = QString("Network error: %1").arg(reply->errorString());
    } else {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (doc.isNull() || !doc.isArray()) {
            error = "Invalid JSON response from GitHub";
        } else {
            QJsonArray releases = doc.array();

            // Find the first non-prerelease, non-draft release
            for (const QJsonValue &val : releases) {
                QJsonObject release = val.toObject();
                bool isPrerelease = release["prerelease"].toBool(false);
                bool isDraft = release["draft"].toBool(false);

                if (!isPrerelease && !isDraft) {
                    latestVersion = release["tag_name"].toString();
                    break;
                }
            }

            if (latestVersion.isEmpty()) {
                error = "No stable releases found";
            } else {
                m_latestVersion = latestVersion;
                m_hasNewVersion = isNewerVersion(APP_VERSION, latestVersion);
                newerAvailable = m_hasNewVersion;
            }
        }
    }

    emit checkComplete(newerAvailable, m_latestVersion, error);
    reply->deleteLater();
}

bool VersionChecker::isNewerVersion(const QString &current, const QString &latest)
{
    // Remove leading 'v' or 'V' if present
    QString cur = current;
    QString lat = latest;

    if (cur.startsWith('v', Qt::CaseInsensitive))
        cur = cur.mid(1);
    if (lat.startsWith('v', Qt::CaseInsensitive))
        lat = lat.mid(1);

    // Split into components (e.g., "2.0.1" -> [2,0,1])
    QStringList curParts = cur.split('.', Qt::SkipEmptyParts);
    QStringList latParts = lat.split('.', Qt::SkipEmptyParts);

    int maxLen = qMax(curParts.size(), latParts.size());
    for (int i = 0; i < maxLen; ++i) {
        int curVal = (i < curParts.size()) ? curParts[i].toInt() : 0;
        int latVal = (i < latParts.size()) ? latParts[i].toInt() : 0;

        if (latVal > curVal)
            return true;
        if (latVal < curVal)
            return false;
    }

    return false; // Equal versions
}
