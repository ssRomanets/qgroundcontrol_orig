target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        AppSettings.cc
        AppSettings.h
        AutoConnectSettings.cc
        AutoConnectSettings.h
        BrandImageSettings.cc
        BrandImageSettings.h
        FlightMapSettings.cc
        FlightMapSettings.h
        FlyViewSettings.cc
        FlyViewSettings.h
        MavlinkSettings.cc
        MavlinkSettings.h
        SettingsGroup.cc
        SettingsGroup.h
        SettingsManager.cc
        SettingsManager.h
        UnitsSettings.cc
        UnitsSettings.h
        MapsSettings.cc
        MapsSettings.h
        VideoSettings.cc
        VideoSettings.h
)

if(NOT QGC_DISABLE_APM_MAVLINK)
endif()

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
