// Temporarily allow access to private members for testing
#define private public
#include "../version_checker.h"
#undef private

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
        QVERIFY(checker.isNewerVersion("", "v2.0.0"));
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
};

QTEST_MAIN(TestVersionChecker)
#include "tst_version_checker.moc"
