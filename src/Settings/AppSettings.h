
#pragma once

#include "QUAVApplication.h"
#include "SettingsGroup.h"

/// Application Settings
class AppSettings : public SettingsGroup
{
    Q_OBJECT

public:
    AppSettings(QObject* parent = nullptr);

    DEFINE_SETTING_NAME_GROUP()

    DEFINE_SETTINGFACT(appFontPointSize)

    DEFINE_SETTINGFACT(indoorPalette)
    DEFINE_SETTINGFACT(savePath)
    DEFINE_SETTINGFACT(enableMultiVehiclePanel)
    DEFINE_SETTINGFACT(mapboxToken)
    DEFINE_SETTINGFACT(mapboxAccount)
    DEFINE_SETTINGFACT(mapboxStyle)
    DEFINE_SETTINGFACT(esriToken)
    DEFINE_SETTINGFACT(customURL)
    DEFINE_SETTINGFACT(vworldToken)
    DEFINE_SETTINGFACT(disableAllPersistence)

    DEFINE_SETTINGFACT(qLocaleLanguage)

    Q_PROPERTY(QString videoSavePath READ videoSavePath NOTIFY savePathsChanged)
    Q_PROPERTY(QString photoSavePath READ photoSavePath NOTIFY savePathsChanged)

    QString videoSavePath();
    QString photoSavePath();

    static constexpr const char* videoDirectory = QT_TRANSLATE_NOOP("AppSettings", "Video");
    static constexpr const char* photoDirectory = QT_TRANSLATE_NOOP("AppSettings", "Photo");

signals:
    void savePathsChanged();

private slots:
    void m_indoorPaletteChanged();
    void m_qLocaleLanguageChanged();

private:
    static QLocale::Language m_qLocaleLanguageEarlyAccess(void);

    static QList<QLocale::Language> m_rgReleaseLanguages;
    static QList<QLocale::Language> m_rgPartialLanguages;

    typedef struct {
        QLocale::Language   languageId;
        const char*         languageName;
    } LanguageInfo_t;
    static LanguageInfo_t m_rgLanguageInfo[];
    
    friend class QUAVApplication;
};
