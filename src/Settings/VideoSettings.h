#pragma once

#include "SettingsGroup.h"

class VideoSettings : public SettingsGroup
{
    Q_OBJECT

public:
    VideoSettings(QObject* parent = nullptr);
    DEFINE_SETTING_NAME_GROUP()

    DEFINE_SETTINGFACT(videoSource)
    DEFINE_SETTINGFACT(videoFit)
    DEFINE_SETTINGFACT(gridLines)
    DEFINE_SETTINGFACT(recordingFormat)
    DEFINE_SETTINGFACT(streamEnabled)

    DEFINE_SETTINGFACT(maxVideoSize)
    DEFINE_SETTINGFACT(enableStorageLimit)

    Q_PROPERTY(bool streamConfigured READ streamConfigured NOTIFY streamConfiguredChanged)

    bool streamConfigured ();

    static constexpr const char* videoSourceNoVideo = QT_TRANSLATE_NOOP("VideoSettings", "No Video Available");
    static constexpr const char* videoDisabled      = QT_TRANSLATE_NOOP("VideoSettings", "Video Stream Disabled");

signals:
    void streamConfiguredChanged (bool configured);

private slots:
    void m_configChanged (QVariant value);

private:
    void m_setDefaults();

private:
    bool m_noVideo = false;
};
