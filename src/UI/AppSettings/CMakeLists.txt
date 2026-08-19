qt_add_library(AppSettingsModule STATIC)

qt_add_qml_module(AppSettingsModule
    URI QUAV.AppSettings
    VERSION 1.0
    RESOURCE_PREFIX /qml
    QML_FILES
        MapSettings.qml
        SettingsPage.qml
        SettingsPagesModel.qml
        VideoSettings.qml
    NO_PLUGIN
)
