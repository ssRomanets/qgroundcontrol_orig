target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        QUAVCorePlugin.cc
        QUAVCorePlugin.h
        QUAVOptions.cc
        QUAVOptions.h
        QmlComponentInfo.cc
        QmlComponentInfo.h
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
