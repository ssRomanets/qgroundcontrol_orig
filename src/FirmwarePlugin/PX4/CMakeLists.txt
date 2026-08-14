if(NOT QGC_DISABLE_PX4_PLUGIN_FACTORY)
    target_sources(${CMAKE_PROJECT_NAME}
        PRIVATE
            PX4FirmwarePluginFactory.cc
            PX4FirmwarePluginFactory.h
    )
endif()


target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

