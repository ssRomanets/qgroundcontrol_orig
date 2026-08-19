import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Window

import QUAV
import QUAV.Palette
import QUAV.Controls
import QUAV.FactControls
import QUAV.ScreenTools
import QUAV.FlightDisplay

/// @brief Native QML top level window
/// All properties defined here are visible to all QML pages.
ApplicationWindow {
    id:             mainWindow
    visible:        true

    Component.onCompleted: {    }

    /// Saves main window position and size and re-opens it in the same position and size next time
    MainWindowSavedState { window: mainWindow }

    //-------------------------------------------------------------------------
    //-- Global Scope Variables

    QtObject {
        id: globals

        readonly property var activeVehicle: QUAV.multiVehicleManager.activeVehicle

        // Number of QGCTextField's with validation errors. Used to prevent closing panels with validation errors.
        property int                validationErrorCount:           0

        // Property to manage RemoteID quick access to settings page
        property bool commingFromRIDIndicator: false

        property bool   videoOnWindow:  false
        property bool   telemOnVideo:   true
        property string videoFileName:  ""

        property bool closeWindowItem : false

        property bool cameraStreamAct: true
        property bool toggleVideoRecordingAct: false
        property bool togglePhotoCaptureAct: false

        property var  videoUVCCamera :    null
        property int fixVideoPosition: -1
    }

    /// Default color palette used throughout the UI
    QUAVPalette { id: quavPal; colorGroupEnabled: true }

    //-- Global Scope Functions
    // This function is used to prevent view switching if there are validation errors
    function allowViewSwitch(previousValidationErrorCount = 0) {
        // Run validation on active focus control to ensure it is valid before switching views
        if (mainWindow.activeFocusControl instanceof FactTextField) {
            mainWindow.activeFocusControl.m_onEditingFinished()
        }
        return globals.validationErrorCount <= previousValidationErrorCount
    }

    function showPlanView() {
        flyView.visible = false
        planView.visible = true
    }

    function showFlyView() {
        flyView.visible = true
        planView.visible = false
    }

    function showTool(toolTitle, toolSource, toolIcon) {
        toolDrawer.backIcon     = flyView.visible ? "/qmlimages/PaperPlane.svg" : "/qmlimages/Plan.svg"
        toolDrawer.toolTitle    = toolTitle
        toolDrawer.toolSource   = toolSource
        toolDrawer.toolIcon     = toolIcon
        toolDrawer.visible      = true
    }

    function showAnalyzeTool() {
        showTool(qsTr("Analyze Tools"), "qrc:/qml/QUAV/AnalyzeView/AnalyzeView.qml", "/qmlimages/Analyze.svg")
    }

    function showVehicleConfig() {
        showTool(qsTr("Vehicle Configuration"), "qrc:/qml/QUAV/VehicleSetup/SetupView.qml", "/qmlimages/Gears.svg")
    }

    function showSettingsTool() {
        showTool(qsTr("Application Settings"), "qrc:/qml/QUAV/Controls/AppSettings.qml", "/res/QGCLogoWhite")
    }

    // This variant is only meant to be called by QUAVApplication
    function showMessageDialog(dialogTitle, dialogText, buttons = Dialog.Ok, acceptFunction = null, closeFunction = null) {
        simpleMessageDialogComponent.createObject(
            mainWindow,
            {
                title: dialogTitle, text: dialogText, buttons: buttons,
                acceptFunction: acceptFunction, closeFunction: closeFunction
            }
        ).open()
    }

    // This variant is only meant to be called by QGCApplication
    function m_showMessageDialog(dialogTitle, dialogText) { showMessageDialog(dialogTitle, dialogText) }

    Component {
        id: simpleMessageDialogComponent

        QUAVSimpleMessageDialog{}
    }

    property bool         m_forceClose: false

    function finishCloseProcess() {
        m_forceClose = true
        QUAV.linkManager.shutdown();
        mainWindow.close()
    }

    // Check for things which should prevent the app from closing
    //  Returns true if it is OK to close
    readonly property int m_skipActiveConnectionsCheckMask: 0x04
    property int m_closeChecksToSkip: 0
    function performCloseChecks() {
        if (!(m_closeChecksToSkip & m_skipActiveConnectionsCheckMask) && !checkForActiveConnections()) { return false }
        finishCloseProcess()
        return true
    }

    property string closeDialogTitle: qsTr("Close %1").arg(QUAV.appName)

    function checkForActiveConnections() {
        if (QUAV.multiVehicleManager.activeVehicle) {
            mainWindow.showMessageDialog(
                closeDialogTitle,
                qsTr("There are still active connections to vehicles. Are you sure you want to exit?"),
                Dialog.Yes | Dialog.No,
                function() {
                    m_closeChecksToSkip |= m_skipActiveConnectionsCheckMask;
                    performCloseChecks()
                }
            )
            return false
        }
        else { return true }
    }

    onClosing: (close) => {
        if (!m_forceClose) {
            m_closeChecksToSkip = 0
            close.accepted      = performCloseChecks()
        }
    }

    background: Rectangle {
        anchors.fill:   parent
        color:          QUAV.globalPalette.window
    }

    FlyView { 
        id:             flyView
        anchors.fill:   parent
        visible:        true
    }

    PlanView {
        id:             planView
        anchors.fill:   parent
        visible:        false
    }

    function showToolSelectDialog() {
        if (mainWindow.allowViewSwitch()) {
            mainWindow.showIndicatorDrawer(toolSelectComponent)
        }
    }

    function closeIndicatorDrawer() {
        indicatorDrawer.close()
    }

    Component {
        id: toolSelectComponent

        ToolIndicatorPage {
            id: toolSelectDialog

            property real m_toolButtonHeight: ScreenTools.defaultFontPixelHeight*3
            property real m_margins:          ScreenTools.defaultFontPixelWidth

            contentComponent: Component {
                ColumnLayout {
                    width:  innerLayout.width + (toolSelectDialog.m_margins * 2)
                    height: innerLayout.height + (toolSelectDialog.m_margins * 2)

                    ColumnLayout {
                        id:             innerLayout
                        Layout.margins: toolSelectDialog.m_margins
                        spacing:        ScreenTools.defaultFontPixelWidth

                        SubMenuButton {
                            height:             toolSelectDialog.m_toolButtonHeight
                            Layout.fillWidth:   true
                            text:               qsTr("Plan Flight")
                            imageResource:      "/qmlimages/Plan.svg"
                            onClicked: {
                                if (mainWindow.allowViewSwitch()) {
                                    mainWindow.closeIndicatorDrawer()
                                    mainWindow.showPlanView()
                                }
                            }
                        }

                        SubMenuButton {
                            id:                 analyzeButton
                            height:             toolSelectDialog.m_toolButtonHeight
                            Layout.fillWidth:   true
                            text:               qsTr("Analyze Tools")
                            imageResource:      "/qmlimages/Analyze.svg"
                            visible:            QUAV.corePlugin.showAdvancedUI
                            onClicked: {
                                if (mainWindow.allowViewSwitch()) {
                                    mainWindow.closeIndicatorDrawer()
                                    mainWindow.showAnalyzeTool()
                                }
                            }
                        }

                        SubMenuButton {
                            id:                 setupButton
                            height:             toolSelectDialog.m_toolButtonHeight
                            Layout.fillWidth:   true
                            text:               qsTr("Vehicle Configuration")
                            imageResource:      "/qmlimages/Gears.svg"
                            onClicked: {
                                if (mainWindow.allowViewSwitch()) {
                                    mainWindow.closeIndicatorDrawer()
                                    mainWindow.showVehicleConfig()
                                }
                            }
                        }

                        SubMenuButton {
                            id:                 settingsButton
                            height:             toolSelectDialog.m_toolButtonHeight
                            Layout.fillWidth:   true
                            text:               qsTr("Application Settings")
                            imageResource:      "/res/QGCLogoFull.svg"
                            imageColor:         "transparent"
                            visible:            !QUAV.corePlugin.options.combineSettingsAndSetup
                            onClicked: {
                                if (mainWindow.allowViewSwitch()) {
                                    mainWindow.closeIndicatorDrawer()
                                    mainWindow.showSettingsTool()
                                }
                            }
                        }

                        SubMenuButton {
                            id: closeButton
                            height: toolSelectDialog.m_toolButtonHeight
                            Layout.fillWidth: true
                            text: qsTr("Close %1").arg(QUAV.appName)
                            imageResource: "/res/cancel.svg"
                            visible: mainWindow.visibility === Window.FullScreen
                            onClicked: {
                                if (mainWindow.allowViewSwitch()) {
                                    mainWindow.finishCloseProcess()
                                }
                            }
                        }

                        ColumnLayout {
                            width:     innerLayout.width
                            spacing:   0
                            Layout.alignment: Qt.AlignHCenter

                            QUAVLabel {
                                id: versionLabel
                                text: qsTr("%1 Version").arg(QUAV.appName)
                                font.pointSize: ScreenTools.smallFontPointSize
                                wrapMode: QUAVLabel.WordWrap
                                Layout.maximumWidth: parent.width
                                Layout.alignment: Qt.AlignHCenter
                            }

                            QUAVLabel {
                                text:                QUAV.quavVersion
                                font.pointSize:      ScreenTools.smallFontPointSize
                                wrapMode:            QUAVLabel.WrapAnywhere
                                Layout.maximumWidth: parent.width
                                Layout.alignment:    Qt.AlignHCenter
                            }
                        }

                    }
                }
            }
        }
    }

    Rectangle {
        id:             toolDrawer
        anchors.fill:   parent
        visible:        false
        color:          quavPal.window
        property var backIcon
        property string toolTitle
        property alias toolSource:  toolDrawerLoader.source
        property var toolIcon

        onVisibleChanged: {
            if (!toolDrawer.visible) {
                toolDrawerLoader.source = ""
            }
        }

        // This need to block click event leakage to underlying map.
        DeadMouseArea {
            anchors.fill: parent
        }

        Rectangle {
            id:             toolDrawerToolbar
            anchors.left:   parent.left
            anchors.right:  parent.right
            anchors.top:    parent.top
            height:         ScreenTools.toolbarHeight
            color:          quavPal.toolbarBackground

            RowLayout {
                id:                 toolDrawerToolbarLayout
                anchors.leftMargin: ScreenTools.defaultFontPixelWidth
                anchors.left:       parent.left
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                spacing:            ScreenTools.defaultFontPixelWidth

                QUAVLabel {
                    font.pointSize: ScreenTools.largeFontPointSize
                    text:           "<"
                }

                QUAVLabel {
                    id:             toolbarDrawerText
                    text:           qsTr("Exit") + " " + toolDrawer.toolTitle
                    font.pointSize: ScreenTools.largeFontPointSize
                }
            }

            QUAVMouseArea {
                anchors.fill: toolDrawerToolbarLayout
                onClicked: {
                    if (mainWindow.allowViewSwitch()) {
                        toolDrawer.visible = false
                    }
                }
            }
        }

        Loader {
            id:             toolDrawerLoader
            anchors.left:   parent.left
            anchors.right:  parent.right
            anchors.top:    toolDrawerToolbar.bottom
            anchors.bottom: parent.bottom

            Connections {
                target:                 toolDrawerLoader.item
            }
        }
    }

    //-- Indicator Drawer
    function showIndicatorDrawer(drawerComponent) {
       indicatorDrawer.sourceComponent = drawerComponent
       indicatorDrawer.open()
    }

    Popup {
        id: indicatorDrawer
        x:              calcXPosition()
        y:              ScreenTools.toolbarHeight + m_margins
        leftInset:      0
        rightInset:     0
        topInset:       0
        bottomInset:    0
        padding:        m_margins * 2
        visible:        false
        modal:          true
        focus:          true
        closePolicy:    Popup.CloseOnEscape | Popup.CloseOnPressOutside

        property var sourceComponent

        property real m_margins:     ScreenTools.defaultFontPixelHeight / 4

        function calcXPosition() {
            return m_margins
        }

        onOpened: {
            indicatorDrawerLoader.sourceComponent   = indicatorDrawer.sourceComponent
        }

        onClosed: {
            indicatorDrawerLoader.sourceComponent   = undefined
        }

        background: Item {
            Rectangle {
                id: backgroundRect
                anchors.fill: parent
                color: QUAV.globalPalette.window
                radius: indicatorDrawer.m_margins
                opacity: 0.85
            }
        }

        contentItem: QUAVFlickable {
            id:             indicatorDrawerLoaderFlickable
            implicitWidth:  Math.min(mainWindow.contentItem.width - (2 * indicatorDrawer.m_margins) - (indicatorDrawer.padding * 2), indicatorDrawerLoader.width)
            implicitHeight: Math.min(mainWindow.contentItem.height - ScreenTools.toolbarHeight - (2 * indicatorDrawer.m_margins) - (indicatorDrawer.padding * 2), indicatorDrawerLoader.height)
            contentWidth:   indicatorDrawerLoader.width
            contentHeight:  indicatorDrawerLoader.height

            Loader {
                id: indicatorDrawerLoader
            }
        }
    }

    function createrWindowedAnalyzePage(title, source) {
        var windowedPage = windowedAnalyzePage.createObject(mainWindow)
        windowedPage.title = title
        windowedPage.source = source
    }

    Component {
        id: windowedAnalyzePage

        Window {
            width:      ScreenTools.defaultFontPixelWidth  * 100
            height:     ScreenTools.defaultFontPixelHeight * 40
            visible:    true

            property alias source: loader.source

            Rectangle {
                color:          QUAV.globalPalette.window
                anchors.fill:   parent

                Loader {
                    id:             loader
                    anchors.fill:   parent
                    onLoaded:       item.popped = true
                }
            }

            onClosing: {
                visible = false
                source = ""
            }
        }
    }
}