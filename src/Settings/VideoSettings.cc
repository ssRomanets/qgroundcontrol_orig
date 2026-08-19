#include "VideoSettings.h"
#include "UVCReceiver.h"

#include <QtCore/QVariantList>

DECLARE_SETTINGGROUP(Video, "Video")
{

}

DECLARE_SETTINGSFACT(VideoSettings, recordingFormat)
DECLARE_SETTINGSFACT(VideoSettings, maxVideoSize)
DECLARE_SETTINGSFACT(VideoSettings, enableStorageLimit)
DECLARE_SETTINGSFACT(VideoSettings, streamEnabled)

DECLARE_SETTINGSFACT_NO_FUNC(VideoSettings, videoSource)
{
    m_noVideo = false;
    
    // Setup enum values for videoSource settings into meta data
    QVariantList videoSourceList;
    
    videoSourceList.append(UVCReceiver::getDeviceNameList());

    if (videoSourceList.count() == 0) {
        m_noVideo = true;
        videoSourceList.append(videoSourceNoVideo);
        setVisible(false);
    } else {
        videoSourceList.insert(0, videoDisabled);
    }
    
    // make translated strings
    QStringList videoSourceCookedList;
    for (const QVariant& videoSource: videoSourceList) {
        videoSourceCookedList.append(VideoSettings::tr(videoSource.toString().toStdString().c_str()));
    }
    
    m_nameToMetaDataMap[videoSourceName]->setEnumInfo(videoSourceCookedList, videoSourceList);
    
    // Set defaults value for videoSource
    m_setDefaults();
    
    if (!m_videoSourceFact) {
        m_videoSourceFact = m_createSettingsFact(videoSourceName);
        //-- Check for sources no longer available
        if (!m_videoSourceFact->enumValues().contains(m_videoSourceFact->rawValue().toString())) {
            if (m_noVideo) {
                m_videoSourceFact->setRawValue(videoSourceNoVideo);
            } else {
                m_videoSourceFact->setRawValue(videoDisabled);
            }
        }
    }
    return m_videoSourceFact;
}

bool VideoSettings::streamConfigured(void)
{
    //-- Check if it's disabled
    QString vSource = videoSource()->rawValue().toString();

    if (UVCReceiver::enabled() && UVCReceiver::deviceExists(vSource)) {
        return true;
    }
    
    return false;
}

void VideoSettings::m_configChanged(QVariant value)
{
    emit streamConfiguredChanged(streamConfigured());
}

void VideoSettings::m_setDefaults()
{
    if (m_noVideo) {
        m_nameToMetaDataMap[videoSourceName]->setRawDefaultValue(videoSourceNoVideo);
    } else {
        m_nameToMetaDataMap[videoSourceName]->setRawDefaultValue(videoDisabled);
    }
}

DECLARE_SETTINGSFACT(VideoSettings, videoFit)
DECLARE_SETTINGSFACT(VideoSettings, gridLines)