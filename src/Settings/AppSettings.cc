#include "AppSettings.h"
#include "QUAVPalette.h"
#include "QUAVApplication.h"

#include <QtQml/QQmlEngine>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QSettings>

// Release languages are 90%+ complete
QList<QLocale::Language> AppSettings::m_rgReleaseLanguages = {
    QLocale::English,
    QLocale::Azerbaijani,    
    QLocale::Chinese,
    QLocale::Japanese,
    QLocale::Korean,
    QLocale::Portuguese,
    QLocale::Russian,
};

// Partial languages are 40%+ complete
QList<QLocale::Language> AppSettings::m_rgPartialLanguages = {
    QLocale::Ukrainian,
};

AppSettings::LanguageInfo_t AppSettings::m_rgLanguageInfo[] = {
    { QLocale::AnyLanguage,     "System" },                     // Must be first
    { QLocale::Azerbaijani,     "Azerbaijani (Azerbaijani)" },
    { QLocale::Bulgarian,       "български (Bulgarian)" },
    { QLocale::Chinese,         "中文 (Chinese)" },
    { QLocale::Dutch,           "Nederlands (Dutch)" },
    { QLocale::English,         "English" },
    { QLocale::Finnish,         "Suomi (Finnish)" },
    { QLocale::French,          "Français (French)" },
    { QLocale::German,          "Deutsche (German)" },
    { QLocale::Greek,           "Ελληνικά (Greek)" },
    { QLocale::Hebrew,          "עברית (Hebrew)" },
    { QLocale::Italian,         "Italiano (Italian)" },
    { QLocale::Japanese,        "日本語 (Japanese)" },
    { QLocale::Korean,          "한국어 (Korean)" },
    { QLocale::NorwegianBokmal, "Norsk (Norwegian)" },
    { QLocale::Polish,          "Polskie (Polish)" },
    { QLocale::Portuguese,      "Português (Portuguese)" },
    { QLocale::Russian,         "Pусский (Russian)" },
    { QLocale::Spanish,         "Español (Spanish)" },
    { QLocale::Swedish,         "Svenska (Swedish)" },
    { QLocale::Turkish,         "Türk (Turkish)" }
};

DECLARE_SETTINGGROUP(App, "") {}

DECLARE_SETTINGSFACT(AppSettings, appFontPointSize)
DECLARE_SETTINGSFACT(AppSettings, savePath)
DECLARE_SETTINGSFACT(AppSettings, enableMultiVehiclePanel)
DECLARE_SETTINGSFACT(AppSettings, mapboxToken)
DECLARE_SETTINGSFACT(AppSettings, mapboxAccount)
DECLARE_SETTINGSFACT(AppSettings, mapboxStyle)
DECLARE_SETTINGSFACT(AppSettings, esriToken)
DECLARE_SETTINGSFACT(AppSettings, customURL)
DECLARE_SETTINGSFACT(AppSettings, vworldToken)
DECLARE_SETTINGSFACT(AppSettings, disableAllPersistence)

DECLARE_SETTINGSFACT_NO_FUNC(AppSettings, indoorPalette)
{
    if (!m_indoorPaletteFact) {
        m_indoorPaletteFact = m_createSettingsFact(indoorPaletteName);
        connect(m_indoorPaletteFact, &Fact::rawValueChanged, this, &AppSettings::m_indoorPaletteChanged);
    }
    return m_indoorPaletteFact;
}

DECLARE_SETTINGSFACT_NO_FUNC(AppSettings, qLocaleLanguage)
{
    if (!m_qLocaleLanguageFact) {
        m_qLocaleLanguageFact = m_createSettingsFact(qLocaleLanguageName);
        connect(m_qLocaleLanguageFact, &Fact::rawValueChanged, this, &AppSettings::m_qLocaleLanguageChanged);

        FactMetaData*   metaData            = m_qLocaleLanguageFact->metaData();
        QStringList     rgEnumStrings;
        QVariantList    rgEnumValues;

        // System is always an available selection
        rgEnumStrings.append(m_rgLanguageInfo[0].languageName);
        rgEnumValues.append(m_rgLanguageInfo[0].languageId);

        for (const auto& languageInfo: m_rgLanguageInfo) {
            if (m_rgReleaseLanguages.contains(languageInfo.languageId)) {
                rgEnumStrings.append(languageInfo.languageName);
                rgEnumValues.append(languageInfo.languageId);
            }
        }
        for (const auto& languageInfo: m_rgLanguageInfo) {
            if (m_rgPartialLanguages.contains(languageInfo.languageId)) {
                rgEnumStrings.append(QString(languageInfo.languageName) + AppSettings::tr(" (Partial)"));
                rgEnumValues.append(languageInfo.languageId);
            }
        }
#ifdef QGC_DAILY_BUILD
        // Only daily builds include full set of languages for testing purposes
        for (const auto& languageInfo: m_rgLanguageInfo) {
            if (!m_rgReleaseLanguages.contains(languageInfo.languageId) && !m_rgPartialLanguages.contains(languageInfo.languageId)) {
                rgEnumStrings.append(QString(languageInfo.languageName) + AppSettings::tr(" (Test Only)"));
                rgEnumValues.append(languageInfo.languageId);
            }
        }
#endif
        metaData->setEnumInfo(rgEnumStrings, rgEnumValues);

        if (m_qLocaleLanguageFact->enumIndex() == -1) {
            m_qLocaleLanguageFact->setRawValue(QLocale::AnyLanguage);
        }
    }
    return m_qLocaleLanguageFact;
}

void AppSettings::m_qLocaleLanguageChanged()
{
    quavApp()->setLanguage();
}

QString AppSettings::videoSavePath(void)
{
    QString path = savePath()->rawValue().toString();
    if (!path.isEmpty() && QDir(path).exists()) {
        QDir dir(path);
        return dir.filePath(videoDirectory);
    }
    return QString();
}

QString AppSettings::photoSavePath(void)
{
    QString path = savePath()->rawValue().toString();
    if (!path.isEmpty() && QDir(path).exists()) {
        QDir dir(path);
        return dir.filePath(photoDirectory);
    }
    return QString();
}

void AppSettings::m_indoorPaletteChanged(void)
{
    QUAVPalette::setGlobalTheme(indoorPalette()->rawValue().toBool() ? QUAVPalette::Dark : QUAVPalette::Light);
}


/// Returns the current qLocaleLanguage setting bypassing the standard SettingsGroup path. It also validates
/// that the value is a supported language. This should only be used by QUAVApplication::setLanguage to query
/// the language setting as early in the boot process as possible. Specfically prior to any JSON files being 
/// loaded such that JSON file can be translated. Also since this is a one-off mechanism custom build overrides 
/// for language are not currently supported.
QLocale::Language AppSettings::m_qLocaleLanguageEarlyAccess(void)
{
    QSettings settings;

    // Note that the AppSettings group has no group name
    QLocale::Language localeLanguage = static_cast<QLocale::Language>(settings.value(qLocaleLanguageName).toInt());
    for (auto& languageInfo: m_rgLanguageInfo) {
        if (languageInfo.languageId == localeLanguage) {
            return localeLanguage;
        }
    }

    localeLanguage = QLocale::AnyLanguage;
    settings.setValue(qLocaleLanguageName, localeLanguage);

    return localeLanguage;
}