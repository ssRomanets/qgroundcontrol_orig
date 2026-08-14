target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        QUAVPalette.cc
        QUAVPalette.h
        QUAVMapPalette.cc
        QUAVMapPalette.h
        QUAVQmlGlobal.cc
        QUAVQmlGlobal.h
        QmlObjectListModel.cc
        QmlObjectListModel.h
        ScreenToolsController.cc
        ScreenToolsController.h
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

qt_add_library(QUAVControlsModule STATIC)

qt_add_qml_module(QUAVControlsModule
    URI QUAV.Controls
    VERSION 1.0
    RESOURCE_PREFIX /qml
    QML_FILES
        AnalyzePage.qml
        AppSettings.qml
        DeadMouseArea.qml
        FlyViewToolBar.qml
        LabelledComboBox.qml
        MainStatusIndicator.qml
        MainWindowSavedState.qml
        MAVLinkMessageButton.qml
        PipState.qml
        PipView.qml
        PlanView.qml
        PlanViewToolBar.qml
        QUAVButton.qml
        QUAVCheckBox.qml
        QUAVCheckBoxSlider.qml
        QUAVColoredImage.qml
        QUAVComboBox.qml
        QUAVFlickable.qml
        QUAVFlickableScrollIndicator.qml
        QUAVLabel.qml
        QUAVMapLabel.qml
        QUAVMouseArea.qml
        QUAVPopupDialog.qml
        QUAVSimpleMessageDialog.qml
        QUAVSwitch.qml
        QUAVTextField.qml
        QUAVToolBarButton.qml
        SelectableControl.qml
        SettingsButton.qml
        SettingsGroupLayout.qml
        SubMenuButton.qml
        ToolIndicatorPage.qml
    NO_PLUGIN
)

qt_add_library(ScreenToolsModule STATIC)

set_source_files_properties(ScreenTools.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(ScreenToolsModule
    URI QUAV.ScreenTools
    VERSION 1.0
    RESOURCE_PREFIX /qml
    QML_FILES
        ScreenTools.qml
    NO_PLUGIN # Required by Qt 6.6.3. TODO: Remove when no longer supported.
)
