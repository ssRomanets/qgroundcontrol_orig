/// @file
/// @brief Branding settings

#pragma once

#include "SettingsGroup.h"

/// Simple branding. Allows to define icon to use on main toolbar.
class BrandImageSettings : public SettingsGroup
{
    Q_OBJECT
   public:
    BrandImageSettings(QObject* parent = nullptr);
    DEFINE_SETTING_NAME_GROUP()
    DEFINE_SETTINGFACT(userBrandImageIndoor)
    DEFINE_SETTINGFACT(userBrandImageOutdoor)
};
