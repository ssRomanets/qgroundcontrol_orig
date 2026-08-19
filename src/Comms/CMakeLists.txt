target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        LinkConfiguration.cc
        LinkConfiguration.h
        LinkInterface.cc
        LinkInterface.h
        LinkManager.cc
        LinkManager.h
        MAVLinkProtocol.cc
        MAVLinkProtocol.h
        QUAVSerialPortInfo.h
        QUAVSerialPortInfo.cc
        SerialLink.cc
        SerialLink.h
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE QABSTRACTSOCKET_DEBUG)

#===========================================================================#

if(QGC_NO_SERIAL_LINK)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE QGC_NO_SERIAL_LINK)
else()

    if(NOT ANDROID AND NOT IOS)
        target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::SerialPort)
    endif()
endif()

#===========================================================================#

if(QGC_ENABLE_BLUETOOTH)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Bluetooth)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE QGC_ENABLE_BLUETOOTH)
endif()

#===========================================================================#

if(QGC_ZEROCONF_ENABLED)
    CPMAddPackage(
        NAME qmdnsengine
        GITHUB_REPOSITORY nitroshare/qmdnsengine
        GIT_TAG master
        OPTIONS
            "BUILD_DOC OFF"
            "BUILD_EXAMPLES OFF"
            "BUILD_TESTS OFF"
    )

    if(TARGET qmdnsengine)
        target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE qmdnsengine)
        target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE QGC_ZEROCONF_ENABLED)
    endif()
endif()

#===========================================================================#

