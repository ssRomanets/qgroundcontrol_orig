
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import QUAV
import QUAV.Controls
import QUAV.Palette
import QUAV.ScreenTools

Popup {
    id:             root
    width:          mainWindow.width
    height:         mainWindow.height
    parent:         Overlay.overlay
    modal:          true
    focus:          true
    margins:        0

    property string title
    property var    buttons:         Dialog.Ok
    property bool   destroyOnClose:  true
    property bool   preventClose:    false

    readonly property real headerMinWidth: titleLabel.implicitWidth + rejectButton.width +
                                           acceptButton.width + titleRowLayout.spacing*2

    signal accepted
    signal rejected

    property var  m_quavPal:                      QUAV.globalPalette
    property real m_contentMargin:                ScreenTools.defaultFontPixelHeight/2
    property bool m_acceptAllowed:                acceptButton.visible
    property bool m_rejectAllowed:                rejectButton.visible
    property int  m_previousValidationErrorCount: 0

    background: QUAVMouseArea {
        width: mainWindow.width
        height: mainWindow.height

        onClicked: { if (closePolicy & Popup.CloseOnPressOutside) {  m_reject() } }
    }

    Component.onCompleted: { contentChildren[contentChildren.length-1].parent = dialogContentParent }

    onAboutToShow: {
        m_previousValidationErrorCount = globals.validationErrorCount
        setupDialogButtons(buttons)
    }

    onClosed: {
        globals.validationErrorCount = m_previousValidationErrorCount
        Qt.inputMethod.hide()
        if (destroyOnClose) { root.destroy() }
    }

    function m_accept() {
        if (m_acceptAllowed && mainWindow.allowViewSwitch(m_previousValidationErrorCount)) {
            accepted()
            if (preventClose) { preventClose = false } else { close() }
        }
    }

    function m_reject() {
        if (m_rejectAllowed && ((buttons & Dialog.Cancel) || mainWindow.allowViewSwitch(m_previousValidationErrorCount))) {
             rejected()
             if (preventClose) {  preventClose = false  } else {close()}
        }
    }

    function setupDialogButtons(buttons) {
        acceptButton.visible = false
        rejectButton.visible = false

        // Accept role buttons
        if (buttons & Dialog.Ok) {
            acceptButton.text = qsTr("Ok")
            acceptButton.visible = true
        } else if (buttons & Dialog.Yes) {
            acceptButton.text = qsTr("Yes")
            acceptButton.visible = true
        }

        // Reject role buttons
        if (buttons & Dialog.Close) {
            rejectButton.text = qsTr("Close")
            rejectButton.visible = true
        }
        else if (buttons & Dialog.No) {
            rejectButton.text = qsTr("No")
            rejectButton.visible = true
        }

        closePolicy = Popup.NoAutoClose
        if (buttons & Dialog.Cancel) {  closePolicy |= Popup.CloseOnEscape }
    }

    Rectangle {
        x:              mainLayout.x - m_contentMargin
        y:              mainLayout.y - m_contentMargin
        width:          mainLayout.width  + m_contentMargin*2
        height:         mainLayout.height + m_contentMargin*2
        color:          m_quavPal.windowShade
        radius:         root.padding/2
        border.width:   1
        border.color:   m_quavPal.windowShadeLight
    }

    ColumnLayout {
        id:                 mainLayout
        anchors.centerIn:   parent
        x:                  m_contentMargin
        y:                  m_contentMargin
        spacing:            m_contentMargin

        RowLayout {
            id:                 titleRowLayout
            Layout.fillWidth:   true
            spacing:            m_contentMargin

            QUAVLabel {
                id: titleLabel
                Layout.fillWidth: true
                text:  root.title
                font.pointSize: ScreenTools.mediumFontPointSize
                verticalAlignment: Text.AlignVCenter
            }

            QUAVButton {
                id: rejectButton
                onClicked: m_reject()
                Layout.minimumWidth: height*1.5
            }

            QUAVButton {
                id: acceptButton
                primary: true
                onClicked: m_accept()
                Layout.minimumWidth: height*1.5
            }
        }

        Rectangle {
            Layout.fillWidth:       true
            Layout.preferredWidth:  Math.min(maxAvailableWidth, totalContentWidth)
            Layout.preferredHeight: Math.min(maxAvailableHeight, totalContentHeight)
            color:                  m_quavPal.window

            property real maxAvailableWidth:  mainWindow.width - m_contentMargin*4
            property real maxAvailableHeight: mainWindow.height - titleRowLayout.height - m_contentMargin*5
            property real totalContentWidth:  dialogContentParent.childrenRect.width    + m_contentMargin*2
            property real totalContentHeight: dialogContentParent.childrenRect.height   + m_contentMargin*2

            QUAVFlickable {
                anchors.margins: m_contentMargin
                anchors.fill:    parent
                contentWidth:    dialogContentParent.childrenRect.width
                contentHeight:   dialogContentParent.childrenRect.height

                Item {
                    id:     dialogContentParent
                    focus:  true

                    Keys.onPressed: (event) => {
                        if (event.key === Qt.Key_Escape && m_rejectAllowed) {
                            m_reject()
                            event.accepted = true
                        }
                    }
                }
            }
        }
    }
}
