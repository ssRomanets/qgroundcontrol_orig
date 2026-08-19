/// @file
/// @author Gus Grubba <gus@auterion.com>

#include "ScreenToolsController.h"
#include "QUAVApplication.h"
#include "SettingsManager.h"
#include "AppSettings.h"

ScreenToolsController::ScreenToolsController(QObject *parent)
    : QObject(parent)
{

}

ScreenToolsController::~ScreenToolsController()
{

}

bool ScreenToolsController::fakeMobile()
{
    return quavApp()->fakeMobile();
}


QString ScreenToolsController::normalFontFamily()
{
    //-- See App.SettinsGroup.json for index
    const int langID = SettingsManager::instance()->appSettings()->qLocaleLanguage()->rawValue().toInt();
    if (langID == QLocale::Korean) {
        return QStringLiteral("NanumGothic");
    }

    return QStringLiteral("Open Sans");
}


