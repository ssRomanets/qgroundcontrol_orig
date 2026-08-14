import QtQuick
import QtQuick.Window

import QUAV
import QUAV.ScreenTools
import QUAV.Controls
import QUAV.Palette

Item {
    id: m_root
    width: m_pipSize
    height: m_pipSize*(9/16)
    visible: item2 && item2.pipState !== item2.pipState.window && show

    property var    item1:                  null
    property var    item2:                  null
    property string item1IsFullSettingsKey
    property bool   show:                   true

    readonly property string m_pipExpandedSettingsKey: "IsPIPVisible"

    property var    m_pipOrWindowItem
    property alias  m_windowContentItem:    window.contentItem
    property alias  m_pipContentItem:       pipContent
    property bool   m_isExpanded:           true
    property real   m_pipSize:              parent.width*0.2
    property real   m_maxSize:              0.75
    property real   m_minSize:              0.10
    property bool   m_componentComplete:    false

    property var m_videoManager: QUAV.videoManager

    Component.onCompleted: {
        m_initForItems()
        m_componentComplete = true
        timer.start()
    }

    onItem2Changed: {
        m_initForItems()
    }

    function showWindow() {
        window.width = m_root.width
        window.height = m_root.height
        window.show()
    }

    function m_initForItems() {  
        var item1IsFull = QUAV.loadBoolGlobalSetting(item1IsFullSettingsKey, true)
        if (item1 && item2) {
            item2.pipState.state = item1IsFull ? item2.pipState.pipState : item2.pipState.fullState
            globals.videoOnWindow = item1IsFull ? false : true

            item1.pipState.state = item1IsFull ? item1.pipState.fullState : item1.pipState.pipState

            m_pipOrWindowItem = item1IsFull ? item2: item1
        } else {
            item1.pipState.state = item1.pipState.fullState
            globals.videoOnWindow = false
            m_pipOrWindowItem = null
        }
        m_setPipIsExpanded(QUAV.loadBoolGlobalSetting(m_pipExpandedSettingsKey, true))
    }

    function m_swapPip() {
        var item1IsFull = false
        if (item1.pipState.state === item1.pipState.fullState) {
            item1.pipState.state = item1.pipState.pipState

            item2.pipState.state = item2.pipState.fullState
            globals.videoOnWindow = true

            m_pipOrWindowItem = item1
            item1IsFull = false
        } else {
            item1.pipState.state = item1.pipState.fullState

            item2.pipState.state = item2.pipState.pipState
            globals.videoOnWindow = false

            m_pipOrWindowItem = item2
            item1IsFull = true
        }
        QUAV.saveBoolGlobalSetting(item1IsFullSettingsKey,item1IsFull)
    }

    function m_setPipIsExpanded(isExpanded) {
        QUAV.saveBoolGlobalSetting(m_pipExpandedSettingsKey, isExpanded)
        m_isExpanded = isExpanded
    }

    Window {
        id: window
        visible:  false
        onClosing: {
            var item = contentItem.children[0]
            if (item) {
                item.pipState.state = item.pipState.pipState
            }
        }
    }

    Item {
        id: pipContent
        anchors.fill: parent
        visible: m_isExpanded
        clip: true
    }

    MouseArea {
        id: pipMouseArea
        anchors.fill: parent
        enabled: m_isExpanded
        preventStealing: true
        hoverEnabled: true
        onClicked: m_swapPip()
    }

    // MouseArea to drag in order to resize the PiP area
    MouseArea {
        id: pipResize
        anchors.fill: pipResizeIcon
        preventStealing: true
        cursorShape: Qt.PointingHandCursor

        property real initialX:     0
        property real initialWidth: 0

        onPressed: (mouse) => {
            // Remove the anchor so the our mouse coordinates stay in the same original place for drag tracking
            pipResize.anchors.fill  = undefined
            pipResize.initialX      = mouse.x
            pipResize.initialWidth  = m_root.width
        }

        onReleased: pipResize.anchors.fill = pipResizeIcon

        // Drag
        onPositionChanged: (mouse) => {
            if (pipResize.pressed) {
                var parentWidth = m_root.parent.width
                var newWidth = pipResize.initialWidth + mouse.x - pipResize.initialX
                if (newWidth < parentWidth * m_maxSize && newWidth > parentWidth * m_minSize) {
                    m_pipSize = newWidth
                }
            }
        }
    }

    // Resize icon
    Image {
        id:                 pipResizeIcon
        source:             "/qmlimages/pipResize.svg"
        fillMode:           Image.PreserveAspectFit
        mipmap:             true
        anchors.right:      parent.right
        anchors.top:        parent.top
        visible:            m_isExpanded && (ScreenTools.isMobile || pipMouseArea.containsMouse) &&
                            m_pipOrWindowItem.pipState.state !== m_pipOrWindowItem.pipState.windowState
        height:             ScreenTools.defaultFontPixelHeight*2.5
        width:              ScreenTools.defaultFontPixelHeight*2.5
        sourceSize.height:  height
    }

    // Check min/max constraints on pip size when when parent is resized
    Connections {
        target: m_root.parent

        function onWidthChanged() {
           if (!m_componentComplete) {
                // Wait until first time setup is done
                return
           }
           var parentWidth = m_root.parent.width
           if (m_root.width > parentWidth*m_maxSize) {
               m_pipSize = parentWidth*m_maxSize
           } else if (m_root.width < parentWidth*m_minSize) {
               m_pipSize = parentWidth*m_minSize
           }
        }
    }

    // Pip to Window
    Image {
        id:                 popupPIP
        source:             "/qmlimages/PiP.svg"
        mipmap:             true
        fillMode:           Image.PreserveAspectFit
        anchors.left:       parent.left
        anchors.top:        parent.top
        visible:            m_isExpanded && !ScreenTools.isMobile && pipMouseArea.containsMouse &&
                            m_pipOrWindowItem.pipState.state !== m_pipOrWindowItem.pipState.windowState
        height:             ScreenTools.defaultFontPixelHeight*2.5
        width:              ScreenTools.defaultFontPixelHeight*2.5
        sourceSize.height:  height

        MouseArea {
            anchors.fill:   parent
            onClicked:      m_pipOrWindowItem.pipState.state = m_pipOrWindowItem.pipState.windowState
        }
    }

    Image {
        id: hidePIP
        source: "/qmlimages/pipHide.svg"
        mipmap: true
        fillMode: Image.PreserveAspectFit
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        visible: m_isExpanded && (ScreenTools.isMobile || pipMouseArea.containsMouse) &&
                 m_pipOrWindowItem.pipState.state !== m_pipOrWindowItem.pipState.windowState
        height: ScreenTools.defaultFontPixelHeight*2.5
        width: ScreenTools.defaultFontPixelHeight*2.5
        sourceSize.height: height
        MouseArea {
            anchors.fill: parent
            onClicked: m_root.m_setPipIsExpanded(false)
        }
    }

    Rectangle {
        id: showPip
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        height: ScreenTools.defaultFontPixelHeight*2
        width: ScreenTools.defaultFontPixelHeight*2
        radius: ScreenTools.defaultFontPixelHeight/3
        visible: !m_isExpanded && m_pipOrWindowItem.pipState.state !== m_pipOrWindowItem.pipState.windowState
        color: Qt.rgba(0,0,0,0.5)
        Image {
            width: parent.width*0.75
            height: parent.height*0.75
            sourceSize.height: height
            source: "/res/buttonRight.svg"
            mipmap: true
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }
        MouseArea {
            anchors.fill: parent
            onClicked: m_root.m_setPipIsExpanded(true)
        }
    }

    Timer {
        id: timer
        interval: 1000;
        running:  false;
        repeat:   true;
        onTriggered: {
            if (globals.cameraStreamAct)
            {
                if (item2 !== null && !m_videoManager.isUvc)
                {
                    if (
                        item1.pipState.state === item1.pipState.windowState || item2.pipState.state === item2.pipState.windowState
                    ) window.close()

                    item2 = null
                }
                else if (item2 == null && m_videoManager.isUvc)
                {
                    item2 = m_videoControl
                }
            }
        }
    }
}
