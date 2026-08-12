#include <QSignalSpy>
#include <QtTest>
#include "../settings/theme_settings_manager.h"

class TestThemeSettingsManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        // Setup for all tests
    }

    void cleanupTestCase()
    {
        // Cleanup after all tests
    }

    void testDefaultTheme()
    {
        ThemeSettingsManager manager;
        QCOMPARE(manager.currentTheme(), QString("dark"));
    }

    void testSetCurrentTheme()
    {
        ThemeSettingsManager manager;

        QSignalSpy spyThemeChanged(&manager, &ThemeSettingsManager::themeChanged);
        QSignalSpy spySettingsChanged(&manager, &ThemeSettingsManager::settingsChanged);

        manager.setCurrentTheme("light");
        QCOMPARE(manager.currentTheme(), QString("light"));
        QCOMPARE(spyThemeChanged.count(), 1);
        QCOMPARE(spySettingsChanged.count(), 1);

        QList<QVariant> arguments = spyThemeChanged.takeFirst();
        QCOMPARE(arguments.at(0).toString(), QString("light"));
    }

    void testSetSameThemeDoesNotEmitSignals()
    {
        ThemeSettingsManager manager;
        manager.setCurrentTheme("dark"); // default is "dark"

        QSignalSpy spyThemeChanged(&manager, &ThemeSettingsManager::themeChanged);
        QSignalSpy spySettingsChanged(&manager, &ThemeSettingsManager::settingsChanged);

        // Set same theme again
        manager.setCurrentTheme("dark");
        QCOMPARE(manager.currentTheme(), QString("dark"));
        QCOMPARE(spyThemeChanged.count(), 0);
        QCOMPARE(spySettingsChanged.count(), 0);
    }

    void testSaveAndLoadSettings()
    {
        // Use a unique organization/application name to avoid conflicts
        QCoreApplication::setOrganizationName("TestOrg");
        QCoreApplication::setApplicationName("TestApp");

        {
            ThemeSettingsManager manager;
            manager.setCurrentTheme("dark_blue");
            QCOMPARE(manager.currentTheme(), QString("dark_blue"));
        }

        {
            ThemeSettingsManager manager;
            // Should load saved theme from QSettings
            QCOMPARE(manager.currentTheme(), QString("dark_blue"));
        }
    }

    void testLoadDefaultIfSettingMissing()
    {
        // Use a different org/app to ensure clean state
        QCoreApplication::setOrganizationName("TestOrgMissing");
        QCoreApplication::setApplicationName("TestAppMissing");

        // Clear settings
        QSettings settings;
        settings.remove("Theme/currentTheme");

        ThemeSettingsManager manager;
        QCOMPARE(manager.currentTheme(), QString("dark"));
    }
};

QTEST_MAIN(TestThemeSettingsManager)
#include "tst_theme_settings_manager.moc"
