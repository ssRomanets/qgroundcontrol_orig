if(QGC_AIRLINK_DISABLED)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE QGC_AIRLINK_DISABLED)
    return()
endif()


target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})