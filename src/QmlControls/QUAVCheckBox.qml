import QtQuick
import QtQuick.Controls

import QUAV
import QUAV.Palette
import QUAV.ScreenTools

CheckBox {
    id:             control
    spacing:        m_noText ? 0 : ScreenTools.defaultFontPixelWidth
    focusPolicy:    Qt.ClickFocus

    property color  textColor:          m_quavPal.text
    property bool   textBold:           false
    property real   textFontPointSize:  ScreenTools.defaultFontPointSize

    property var    m_quavPal: QUAVPalette { colorGroupEnabled: enabled }
    property bool   m_noText: text === ""

    property ButtonGroup buttonGroup: null
    onButtonGroupChanged: { if (buttonGroup) {  buttonGroup.addButton(control) } }

    contentItem: Text {
        leftPadding:        control.indicator.width + control.spacing
        verticalAlignment:  Text.AlignVCenter
        text:               control.text
        font.pointSize:     textFontPointSize
        font.bold:          control.textBold
        font.family:        ScreenTools.normalFontFamily
        color:              control.textColor
    }

    indicator:  Rectangle {
        implicitWidth:  ScreenTools.implicitCheckBoxHeight
        implicitHeight: implicitWidth
        x:              control.leftPadding
        y:              parent.height / 2 - height / 2
        color:          control.enabled ? "white" : _qgcPal.text
        border.color:   m_quavPal.text
        border.width:   1
        radius:         ScreenTools.buttonBorderRadius
        opacity:        control.checkedState === Qt.PartiallyChecked ? 0.5 : 1

        QUAVColoredImage {
            source:             "/qmlimages/checkbox-check.svg"
            color:              "black"
            mipmap:             true
            fillMode:           Image.PreserveAspectFit
            width:              parent.implicitWidth * 0.75
            height:             width
            sourceSize.height:  height
            visible:            control.checked
            anchors.centerIn:   parent
        }
    }
}

