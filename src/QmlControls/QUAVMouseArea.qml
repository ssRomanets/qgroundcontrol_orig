import QtQuick

import QUAV
import QUAV.ScreenTools

MouseArea {
    anchors.leftMargin:     fillItem ? -m_touchMarginHorizontal : 0
    anchors.rightMargin:    fillItem ? -m_touchMarginHorizontal : 0
    anchors.topMargin:      fillItem ? -m_touchMarginVertical : 0
    anchors.bottomMargin:   fillItem ? -m_touchMarginVertical : 0
    anchors.fill:           fillItem ? fillItem : undefined

    property var fillItem
    property bool debugMobile: false

    property real m_itemWidth:               fillItem ? fillItem.width : width
    property real m_itemHeight:              fillItem ? fillItem.height : height
    property real m_touchWidth:              Math.max(m_itemWidth, ScreenTools.minTouchPixels)
    property real m_touchHeight:             Math.max(m_itemHeight, ScreenTools.minTouchPixels)
    property real m_touchMarginHorizontal:   debugMobile || ScreenTools.isMobile ? (m_touchWidth - m_itemWidth) / 2 : 0
    property real m_touchMarginVertical:     debugMobile || ScreenTools.isMobile ? (m_touchHeight - m_itemHeight) / 2 : 0

    Rectangle {
        anchors.fill:   parent
        border.color:   "red"
        border.width:   QUAV.corePlugin.showTouchAreas ? 3 : 0
        color:          "transparent"
    }
}



