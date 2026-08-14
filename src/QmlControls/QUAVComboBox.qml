import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Templates as T

import QUAV.ScreenTools
import QUAV.Palette
import QUAV.Controls

T.ComboBox {
    id:             control
    padding:        ScreenTools.comboBoxPadding
    spacing:        ScreenTools.defaultFontPixelWidth
    font.pointSize: ScreenTools.defaultFontPointSize
    font.family:    ScreenTools.normalFontFamily
    implicitWidth:  Math.max(
                        background ? background.implicitWidth : 0,
                        contentItem.implicitWidth + leftPadding + rightPadding + padding
                    )
    implicitHeight: Math.max(
                        background ? background.implicitHeight : 0,
                        Math.max(
                            contentItem.implicitHeight, 
                            indicator ? indicator.implicitHeight : 0
                        ) + topPadding + bottomPadding
                    )
    baselineOffset: contentItem.y + text.baselineOffset
    leftPadding:    padding + (!control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)
    rightPadding:   padding + (control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width)

    property bool   centeredLabel:  false
    property bool   sizeToContents: false
    property string alternateText:  ""

    property real   m_popupWidth:    width
    property bool   m_onCompleted:   false
    property bool   m_showBorder:    quavPal.globalTheme === QUAVPalette.Light

    QUAVPalette { id: quavPal; colorGroupEnabled: enabled }

    TextMetrics {
        id:                 textMetrics
        font.family:        control.font.family
        font.pointSize:     control.font.pointSize
    }

    ItemDelegate {
        id:             itemDelegateMetrics
        visible:        false
        font.family:    control.font.family
        font.pointSize: control.font.pointSize
    }

    function m_calcPopupWidth() {
        if (m_onCompleted && sizeToContents && model) {
            var largestTextWidth = 0
            for (var i = 0; i < model.length; i++){
                textMetrics.text = control.textRole ? model[i][control.textRole] : model[i]
                largestTextWidth = Math.max(textMetrics.width, largestTextWidth)
            }
            m_popupWidth = largestTextWidth + itemDelegateMetrics.leftPadding + itemDelegateMetrics.rightPadding
        }
    }

    onModelChanged: m_calcPopupWidth()

    Component.onCompleted: {
        m_onCompleted = true
        m_calcPopupWidth()
    }

    // The items in the popup
    delegate: ItemDelegate {
        width:  m_popupWidth
        height: Math.round(popupItemMetrics.height * 1.75)

        property string m_text: control.textRole ? 
                                    (model.hasOwnProperty(control.textRole) ? model[control.textRole] : modelData[control.textRole]) :
                                    modelData

        TextMetrics {
            id:             popupItemMetrics
            font:           control.font
            text:           m_text
        }

        contentItem: Text {
            text:                   m_text
            font:                   control.font
            color:                  control.currentIndex === index ? quavPal.buttonHighlightText : quavPal.buttonText
            verticalAlignment:      Text.AlignVCenter
        }

        background: Rectangle {
            color:                  control.currentIndex === index ? quavPal.buttonHighlight : quavPal.button
        }

        highlighted:                control.highlightedIndex === index
    }

    indicator: QUAVColoredImage {
        anchors.rightMargin:    control.padding
        anchors.right:          parent.right
        anchors.verticalCenter: parent.verticalCenter
        height:                 ScreenTools.defaultFontPixelWidth
        width:                  height
        source:                 "/qmlimages/arrow-down.png"
        color:                  quavPal.buttonText
    }

    // The label of the button
    contentItem: QUAVLabel {
        id:                         text
        anchors.verticalCenter:     parent.verticalCenter
        anchors.horizontalCenter:   centeredLabel ? parent.horizontalCenter : undefined
        text:                       control.alternateText === "" ? control.currentText : control.alternateText
        font:                       control.font
        color:                      quavPal.buttonText
    }

    background: Rectangle {
        color:          quavPal.button
        border.color:   quavPal.buttonBorder
        border.width:   m_showBorder ? 1 : 0
        radius:         ScreenTools.buttonBorderRadius
    }

    popup: T.Popup {
        x:              control.width - m_popupWidth
        y:              control.height
        width:          m_popupWidth
        height:         Math.min(contentItem.implicitHeight, control.Window.height - topMargin - bottomMargin)
        topMargin:      6
        bottomMargin:   6

        contentItem: ListView {
            clip:                   true
            implicitHeight:         contentHeight
            model:                  control.delegateModel
            currentIndex:           control.highlightedIndex
            highlightMoveDuration:  0

            Rectangle {
                z:              10
                width:          parent.width
                height:         parent.height
                color:          "transparent"
                border.color:   quavPal.text
            }

            T.ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            color: quavPal.window
        }
    }
}
