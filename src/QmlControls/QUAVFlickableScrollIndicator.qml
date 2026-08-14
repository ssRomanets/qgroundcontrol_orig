import QtQuick

import QUAV.ScreenTools

Rectangle {
    id:         control
    z:          10
    color:      m_flickable.indicatorColor
    opacity:    m_opacity
    visible:    showIndicator
    state:      orientation == QUAVFlickableScrollIndicator.Vertical ? "vertical" : "horizontal"

    property bool showIndicator: false
    property int  orientation:   QUAVFlickableScrollIndicator.Vertical

    enum Orientation { Vertical,  Horizontal }

    property real m_opacity:     0.5
    property var  m_flickable:   parent

    states: [
            State {
                name: "vertical"
                AnchorChanges {
                    target:         control
                    anchors.right: m_flickable.right
                }

                PropertyChanges {
                    target:         control
                    y:              m_flickable.height * (m_flickable.contentY / m_flickable.contentHeight)
                    width:          ScreenTools.defaultFontPixelWidth / 2
                    height:         m_flickable.height * (m_flickable.height / m_flickable.contentHeight)
                    showIndicator:  (m_flickable.flickableDirection === Flickable.AutoFlickDirection ||
                                        m_flickable.flickableDirection === Flickable.VerticalFlick ||
                                        m_flickable.flickableDirection === Flickable.HorizontalAndVerticalFlick) &&
                                        (m_flickable.contentHeight > m_flickable.height)
                }
            },

            State {
                name: "horizontal"
                AnchorChanges {
                    target:         control
                    anchors.bottom: m_flickable.bottom
                }

                PropertyChanges {
                    target:         control
                    x:              m_flickable.width * (m_flickable.contentX / m_flickable.contentWidth)
                    height:         ScreenTools.defaultFontPixelWidth / 2
                    width:          m_flickable.width * (m_flickable.width / m_flickable.contentWidth)
                    showIndicator:  (m_flickable.flickableDirection === Flickable.AutoFlickDirection ||
                                     m_flickable.flickableDirection === Flickable.HorizontalFlick ||
                                     m_flickable.flickableDirection === Flickable.HorizontalAndVerticalFlick) &&
                                    (m_flickable.contentWidth > m_flickable.width)
                }
            }
        ]

    Component.onCompleted:  { if (animateOpacity) animateOpacity.restart() }
    onVisibleChanged:       { if (animateOpacity) animateOpacity.restart() }
    onHeightChanged:        { if (animateOpacity) animateOpacity.restart() }
    onWidthChanged:         { if (animateOpacity) animateOpacity.restart() }

    Connections {
        target: control.m_flickable
        function onMovementStarted()        { control.opacity = control._opacity }
        function onMovementEnded()          { animateOpacity.restart() }
        function onContentHeightChanged()   { animateOpacity.restart() }
    }

    NumberAnimation {
        id:            animateOpacity
        target:        control
        properties:    "opacity"
        from:          control.m_opacity
        to:            0.0
        duration:      2000
        easing.type:   Easing.InQuint
    }
}
