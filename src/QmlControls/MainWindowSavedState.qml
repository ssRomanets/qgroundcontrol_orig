
import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtCore

import QUAV
import QUAV.ScreenTools

Item {
    property Window window

    property bool m_enabled: !ScreenTools.isMobile && !ScreenTools.fakeMobile && QUAV.corePlugin.options.enableSaveMainWindowPosition

    Settings {
        id:         s
        category:   "MainWindowState"

        property int x
        property int y
        property int width
        property int height
        property int visibility
    }

    function _setDefaultDesktopWindowSize() {
        window.width = Math.min(250 * Screen.pixelDensity, Screen.width);
        window.height = Math.min(150 * Screen.pixelDensity, Screen.height);
    }

    Component.onCompleted: {
        if (ScreenTools.fakeMobile) {
            window.width = ScreenTools.screenWidth
            window.height = ScreenTools.screenHeight
        } else if (ScreenTools.isMobile) {
            window.showFullScreen();
        } else if (QUAV.corePlugin.options.enableSaveMainWindowPosition) {
            window.minimumWidth = Math.min(ScreenTools.defaultFontPixelWidth * 100, Screen.width)
            window.minimumHeight = Math.min(ScreenTools.defaultFontPixelWidth * 50, Screen.height)
            if (s.width && s.height) {
                window.x = s.x;
                window.y = s.y;
                window.width = s.width;
                window.height = s.height;
                window.visibility = s.visibility;
            } else {
                m_setDefaultDesktopWindowSize()
            }
        } else {
            m_setDefaultDesktopWindowSize()
        }
    }

    Connections {
        target:                         window
        function onXChanged()           { if(m_enabled) saveSettingsTimer.restart() }
        function onYChanged()           { if(m_enabled) saveSettingsTimer.restart() }
        function onWidthChanged()       { if(m_enabled) saveSettingsTimer.restart() }
        function onHeightChanged()      { if(m_enabled) saveSettingsTimer.restart() }
        function onVisibilityChanged()  { if(m_enabled) saveSettingsTimer.restart() }
    }

    Timer {
        id:             saveSettingsTimer
        interval:       500
        repeat:         false
        onTriggered:    saveSettings()
    }

    function saveSettings() {
        if (m_enabled) {
            switch(window.visibility) {
            case ApplicationWindow.Windowed:
                s.x = window.x;
                s.y = window.y;
                s.width = window.width;
                s.height = window.height;
                s.visibility = window.visibility;
                break;
            case ApplicationWindow.FullScreen:
                s.visibility = window.visibility;
                break;
            case ApplicationWindow.Maximized:
                s.visibility = window.visibility;
                break;
            }
        }
    }
}
