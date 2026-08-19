import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import QtLocation
import QtPositioning
import QtQuick.Window
import QtQml.Models

import QUAV
import QUAV.Controls
import QUAV.FlightDisplay
import QUAV.ScreenTools

Item {
    id: m_root

    property real   m_toolsMargin:     ScreenTools.defaultFontPixelWidth * 0.75
    property var    m_mapControl:      mapControl
    property var    m_videoControl:    videoControl

    property real   m_fullItemZorder:  0

    FlyViewToolBar {
        id:         toolbar
        visible:    !QUAV.videoManager.fullScreen
    }

    Item {
        id:                 mapHolder
        anchors.top:        toolbar.bottom
        anchors.bottom:     parent.bottom
        anchors.left:       parent.left
        anchors.right:      parent.right

        FlyViewMap {
            id:           mapControl
            pipView:      m_pipView
        }

        FlyViewVideo {
            id:         videoControl
            pipView:    m_pipView
        }

        PipView {
            id:                      m_pipView
            anchors.left:            parent.left
            anchors.bottom:          parent.bottom
            anchors.margins:         m_toolsMargin
            item1:                   mapControl
            item2:                   (QUAV.videoManager.hasVideo || !globals.cameraStreamAct) ? videoControl : null

            show:                    QUAV.videoManager.hasVideo && !QUAV.videoManager.fullScreen || !globals.cameraStreamAct

            z:                       QUAV.zOrderWidgets
        }

        FlyViewWidgetLayer {
            id:                widgetLayer
            anchors.top:       parent.top
            anchors.bottom:    parent.bottom
            anchors.left:      parent.left
            anchors.right:     parent.right
            z:                 m_fullItemZorder + 1
            visible:           !QUAV.videoManager.fullScreen
        }
    }
}