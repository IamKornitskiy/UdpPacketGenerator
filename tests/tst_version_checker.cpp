#include "../version_checker.h"
#include <QSignalSpy>
#include <QtTest>
#include <QNetworkReply>

class TestVersionChecker : public QObject
{
    Q_OBJECT

private slots:
    void testConstructor()
    {
        VersionChecker checker;
        QVERIFY(!checker.isChecking());
        QVERIFY(checker.latestVersion().isEmpty());
        QVERIFY(!checker.hasNewVersion());
    }

    void testIsNewerVersion()
    {
        VersionChecker checker;
        
        // Same version
        QVERIFY(!checker.isNewerVersion("v2.0.0", "v2.0.0"));
        QVERIFY(!checker.isNewerVersion("2.0.0", "2.0.0"));

        // Newer version
        QVERIFY(checker.isNewerVersion("v2.0.0", "v2.0.1"));
        QVERIFY(checker.isNewerVersion("2.0.0", "2.0.1"));
        QVERIFY(checker.isNewerVersion("v2.0.0", "v2.1.0"));
        QVERIFY(checker.isNewerVersion("v2.0.0", "v3.0.0"));

        // Older version
        QVERIFY(!checker.isNewerVersion("v2.0.1", "v2.0.0"));
        QVERIFY(!checker.isNewerVersion("v3.0.0", "v2.0.0"));

        // Different lengths
        QVERIFY(checker.isNewerVersion("v2.0.0", "v2.0.0.1"));
        QVERIFY(!checker.isNewerVersion("v2.0.0.1", "v2.0.0"));

        // With 'v' prefix variations
        QVERIFY(checker.isNewerVersion("v2.0.0", "2.0.1"));
        QVERIFY(checker.isNewerVersion("2.0.0", "v2.0.1"));

        // Empty / invalid
        QVERIFY(!checker.isNewerVersion("", ""));
        QVERIFY(!checker.isNewerVersion("v2.0.0", ""));
        QVERIFY(checker.isNewerVersion("", "v2.0.0")); // empty considered old
    }

    void testCheckForUpdates()
    {
        VersionChecker checker;
        QSignalSpy spyComplete(&checker, &VersionChecker::checkComplete);

        checker.checkForUpdates();
        QVERIFY(checker.isChecking());

        // Wait for network reply (up to 5 seconds)
        QTRY_VERIFY_WITH_TIMEOUT(spyComplete.count() == 1, 5000);

        QList<QVariant> args = spyComplete.takeFirst();
        bool newerAvailable = args[0].toBool();
        QString latestVersion = args[1].toString();
        QString error = args[2].toString();

        // Either we get a success or an error (network issue in CI)
        if (!error.isEmpty()) {
            qDebug() << "Version check error:" << error;
        }
        QVERIFY(!checker.isChecking());
    }

    void testCheckForUpdatesTwice()
    {
        VersionChecker checker;
        QSignalSpy spyComplete(&checker, &VersionChecker::checkComplete);

        checker.checkForUpdates();
        QVERIFY(checker.isChecking());

        // Second call should be ignored while first is in progress
        checker.checkForUpdates();
        QVERIFY(checker.isChecking());

        QTRY_VERIFY_WITH_TIMEOUT(spyComplete.count() == 1, 5000);
        QVERIFY(!checker.isChecking());
    }

    void testParseLatestVersionFromJson()
    {
        VersionChecker checker;
        
        // Valid JSON with stable release
        QByteArray validJson = R"([
            {"tag_name": "v1.0.0", "prerelease": false, "draft": false},
            {"tag_name": "v2.0.0", "prerelease": false, "draft": false}
        ])";
        QCOMPARE(checker.parseLatestVersionFromJson(validJson), QString("v1.0.0"));

        // Prerelease should be skipped
        QByteArray prereleaseJson = R"([
            {"tag_name": "v1.0.0-rc1", "prerelease": true, "draft": false},
            {"tag_name": "v2.0.0", "prerelease": false, "draft": false}
        ])";
        QCOMPARE(checker.parseLatestVersionFromJson(prereleaseJson), QString("v2.0.0"));

        // Draft should be skipped
        QByteArray draftJson = R"([
            {"tag_name": "v1.0.0-draft", "prerelease": false, "draft": true},
            {"tag_name": "v2.0.0", "prerelease": false, "draft": false}
        ])";
        QCOMPARE(checker.parseLatestVersionFromJson(draftJson), QString("v2.0.0"));

        // All prerelease/draft -> empty
        QByteArray allPrereleaseJson = R"([
            {"tag_name": "v1.0.0-rc1", "prerelease": true, "draft": false},
            {"tag_name": "v2.0.0-rc2", "prerelease": true, "draft": false}
        ])";
        QVERIFY(checker.parseLatestVersionFromJson(allPrereleaseJson).isEmpty());

        // Empty array
        QByteArray emptyJson = R"([])";
        QVERIFY(checker.parseLatestVersionFromJson(emptyJson).isEmpty());

        // Invalid JSON
        QByteArray invalidJson = "{invalid json}";
        QVERIFY(checker.parseLatestVersionFromJson(invalidJson).isEmpty());

        // Not an array (object)
        QByteArray objectJson = R"({"tag_name": "v1.0.0"})";
        QVERIFY(checker.parseLatestVersionFromJson(objectJson).isEmpty());

        // Missing tag_name
        QByteArray missingTagJson = R"([
            {"prerelease": false, "draft": false}
        ])";
        QVERIFY(checker.parseLatestVersionFromJson(missingTagJson).isEmpty());

        // Empty tag_name
        QByteArray emptyTagJson = R"([
            {"tag_name": "", "prerelease": false, "draft": false}
        ])";
        QVERIFY(checker.parseLatestVersionFromJson(emptyTagJson).isEmpty());
    }
};

QTEST_MAIN(TestVersionChecker)
#include "tst_version_checker.moc"
