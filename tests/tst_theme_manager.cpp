#include <QApplication>
#include <QSignalSpy>
#include <QStyleFactory>
#include <QtTest>
#include "../theme_manager.h"

class TestThemeManager : public QObject
{
    Q_OBJECT

private:
    QApplication *m_app = nullptr;
    int m_argc = 1;
    char m_argv0[16] = "test";
    char *m_argv[2] = {m_argv0, nullptr};

public:
    TestThemeManager() = default;

private slots:
    void initTestCase()
    {
        // QApplication is already created by QTEST_MAIN
        // We just need to get the instance
        m_app = qobject_cast<QApplication *>(QCoreApplication::instance());
        if (!m_app) {
            m_app = new QApplication(m_argc, m_argv);
        }
    }

    void cleanupTestCase()
    {
        // Do not delete QApplication - it's managed by QTest
        m_app = nullptr;
    }

    void testConstructorLoadsThemeFromSettings()
    {
        // Use a unique org/app to avoid conflicts
        QCoreApplication::setOrganizationName("ThemeManagerTest");
        QCoreApplication::setApplicationName("TestApp");

        // Set a theme in settings
        {
            QSettings settings;
            settings.setValue("Theme/currentTheme", "Light");
        }

        ThemeManager manager(m_app);
        QCOMPARE(manager.currentThemeKey(), QString("Light"));
        QCOMPARE(manager.currentThemeType(), ThemeManager::ThemeType::Custom);
    }

    void testAvailableThemes()
    {
        ThemeManager manager(m_app);
        QList<ThemeManager::ThemeInfo> themes = manager.availableThemes();

        // Should contain at least custom themes
        QVERIFY(themes.size() > 0);

        bool hasCustomTheme = false;
        for (const auto &theme : themes) {
            if (theme.type == ThemeManager::ThemeType::Custom) {
                hasCustomTheme = true;
                break;
            }
        }
        QVERIFY(hasCustomTheme);
    }

    void testThemeNamesAndKeys()
    {
        ThemeManager manager(m_app);
        auto [names, keys] = manager.themeNamesAndKeys();

        QCOMPARE(names.size(), keys.size());
        QVERIFY(names.size() > 0);
        QVERIFY(keys.size() > 0);
    }

    void testApplyCustomTheme()
    {
        ThemeManager manager(m_app);
        QSignalSpy spyThemeChanged(&manager, &ThemeManager::themeChanged);

        bool result = manager.applyTheme("Dark");
        QVERIFY(result);

        QCOMPARE(manager.currentThemeKey(), QString("Dark"));
        QCOMPARE(manager.currentThemeType(), ThemeManager::ThemeType::Custom);
        QCOMPARE(spyThemeChanged.count(), 1);

        QList<QVariant> arguments = spyThemeChanged.takeFirst();
        QCOMPARE(arguments.at(0).toString(), QString("Dark"));
    }

    void testApplyInvalidThemeReturnsFalse()
    {
        ThemeManager manager(m_app);
        bool result = manager.applyTheme("NonExistentTheme");
        QVERIFY(!result);

        // Should not change current theme
        QVERIFY(!manager.currentThemeKey().isEmpty());
    }

    void testApplyThemeSavesToSettings()
    {
        QCoreApplication::setOrganizationName("ThemeManagerSaveTest");
        QCoreApplication::setApplicationName("TestApp");

        {
            ThemeManager manager(m_app);
            manager.applyTheme("Dark blue");
            QCOMPARE(manager.currentThemeKey(), QString("Dark blue"));
        }

        {
            ThemeManager manager(m_app);
            QCOMPARE(manager.currentThemeKey(), QString("Dark blue"));
        }
    }

    void testLoadFromSettingsWithSystemStyle()
    {
        QCoreApplication::setOrganizationName("ThemeManagerSystemTest");
        QCoreApplication::setApplicationName("TestApp");

        // Get first available system style (excluding windows)
        QStringList styles = QStyleFactory::keys();
        QString systemStyle;
        for (const QString &style : styles) {
            if (!style.contains("windows", Qt::CaseInsensitive)) {
                systemStyle = style;
                break;
            }
        }

        if (systemStyle.isEmpty()) {
            QSKIP("No non-Windows system styles available");
        }

        // Save system style to settings
        {
            QSettings settings;
            settings.setValue("Theme/currentTheme", systemStyle);
        }

        ThemeManager manager(m_app);
        QCOMPARE(manager.currentThemeKey(), systemStyle);
        QCOMPARE(manager.currentThemeType(), ThemeManager::ThemeType::System);
    }

    void testLoadFromSettingsWithUnknownThemeUsesDefault()
    {
        QCoreApplication::setOrganizationName("ThemeManagerUnknownTest");
        QCoreApplication::setApplicationName("TestApp");

        // Save unknown theme to settings
        {
            QSettings settings;
            settings.setValue("Theme/currentTheme", "unknown_theme");
        }

        ThemeManager manager(m_app);
        QCOMPARE(manager.currentThemeKey(), QString("dark"));
        QCOMPARE(manager.currentThemeType(), ThemeManager::ThemeType::Custom);
    }

    void testApplySystemStyle()
    {
        ThemeManager manager(m_app);

        // Find a non-Windows system style
        QStringList styles = QStyleFactory::keys();
        QString systemStyle;
        for (const QString &style : styles) {
            if (!style.contains("windows", Qt::CaseInsensitive)) {
                systemStyle = style;
                break;
            }
        }

        if (systemStyle.isEmpty()) {
            QSKIP("No non-Windows system styles available");
        }

        QSignalSpy spyThemeChanged(&manager, &ThemeManager::themeChanged);

        bool result = manager.applyTheme(systemStyle);
        QVERIFY(result);

        QCOMPARE(manager.currentThemeKey(), systemStyle);
        QCOMPARE(manager.currentThemeType(), ThemeManager::ThemeType::System);
        QCOMPARE(spyThemeChanged.count(), 1);

        QList<QVariant> arguments = spyThemeChanged.takeFirst();
        QCOMPARE(arguments.at(0).toString(), systemStyle);
    }

    void testApplySystemStyleClearsStylesheet()
    {
        ThemeManager manager(m_app);

        // First apply a custom theme (sets stylesheet)
        manager.applyTheme("Dark");
        QVERIFY(!m_app->styleSheet().isEmpty());

        // Find a system style
        QStringList styles = QStyleFactory::keys();
        QString systemStyle;
        for (const QString &style : styles) {
            if (!style.contains("windows", Qt::CaseInsensitive)) {
                systemStyle = style;
                break;
            }
        }

        if (systemStyle.isEmpty()) {
            QSKIP("No non-Windows system styles available");
        }

        // Apply system style - should clear stylesheet
        manager.applyTheme(systemStyle);
        QVERIFY(m_app->styleSheet().isEmpty());
    }
};

QTEST_MAIN(TestThemeManager)
#include "tst_theme_manager.moc"
