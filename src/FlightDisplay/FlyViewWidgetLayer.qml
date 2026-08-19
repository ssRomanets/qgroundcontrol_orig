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
import QUAV.FlightMap
import QUAV.Palette
import QUAV.ScreenTools

// This is the ui overlay layer for the widgets/tools for Fly View
Item {
    id: m_root

    property real   m_layoutMargin:          ScreenTools.defaultFontPixelWidth * 0.75
    property real   m_layoutSpacing:         ScreenTools.defaultFontPixelWidth
    property real   m_margins:               ScreenTools.defaultFontPixelWidth/2
    property real   m_rightPanelWidth:       ScreenTools.defaultFontPixelWidth*30

    property bool m_showSingleVehicleUI: true

    FlyViewTopRightPanel {
        id:                     topRightPanel
        anchors.top:            parent.top
        anchors.right:          parent.right
        anchors.topMargin:      m_layoutMargin
        anchors.rightMargin:    m_layoutMargin
    }

    FlyViewTopRightColumnLayout {
        id:                 topRightColumnLayout
        anchors.margins:    m_layoutMargin
        anchors.top:        parent.top
        anchors.bottom:     bottomRightRowLayout.top
        anchors.right:      parent.right
        visible:           !topRightPanel.visible
    }

    FlyViewBottomRightRowLayout {
        id: bottomRightRowLayout
        anchors.margins: m_layoutMargin
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        spacing: m_layoutSpacing
    }

    MapScale {
        id:              mapScale
        anchors.margins: m_toolsMargin
        anchors.left:    parent.left
        anchors.top:     parent.top
        mapControl:      m_mapControl
        buttonsOnLeft:   true
        visible:         !ScreenTools.isTinyScreen &&
                         QUAV.corePlugin.options.flyView.showMapScale &&
                         !globals.videoOnWindow
    }
}