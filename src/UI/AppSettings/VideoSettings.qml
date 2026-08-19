import QtQuick
import QtQuick.Window
import QtQuick.Controls
import Qt.labs.platform
import QtQuick.Layouts

import QUAV
import QUAV.FactSystem
import QUAV.FactControls
import QUAV.Controls
import QUAV.ScreenTools
import QUAV.FlightDisplay

SettingsPage {
    property var  m_settingsManager:        QUAV.settingsManager
    property var  m_videoManager:           QUAV.videoManager
    property var  m_videoSettings:          m_settingsManager.videoSettings

    readonly property real m_minWidth: ScreenTools.defaultFontPixelWidth*20
    readonly property real m_maxWidth: ScreenTools.defaultFontPixelWidth*30

    QUAVCheckBox {
        id: checkBoxCameraStreamAct
        text: "video stream act"
        visible: true

        checked:            globals.cameraStreamAct
        onClicked:          {globals.cameraStreamAct = checkBoxCameraStreamAct.checked}
    }

    SettingsGroupLayout {
        Layout.fillWidth:   true
        heading:            qsTr("Video loading")
        headingDescription: (globals.videoFileName !== "") ? qsTr("Loaded video file - ") + globals.videoFileName : ""
        visible:            !globals.cameraStreamAct

        QUAVButton {
            id: fileVideoSelectButton

            text: qsTr("Select video file")
            onClicked: fileVideoDialog.open()
            Layout.minimumWidth: m_minWidth
            Layout.maximumWidth: m_maxWidth
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
        }

        FileDialog {
            id: fileVideoDialog
            title: qsTr("Select video file")
            folder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
            nameFilters: [qsTr("Avi file (*.avi)")]
            defaultSuffix: "avi"
            selectedNameFilter.index: 0
            fileMode: FileDialog.OpenFile

            onAccepted: {
                globals.videoFileName = fileVideoDialog.file

                if (mainWindow.allowViewSwitch()) {
                    toolDrawer.visible = false
                }
                close()
            }

            onRejected: {
                console.log("Dialog cancelled")
            }
        }
    }

    SettingsGroupLayout {
        Layout.fillWidth:   true
        heading:            qsTr("Video Source")
        visible:            globals.cameraStreamAct

        LabelledFactComboBox {
            Layout.fillWidth: true
            label: qsTr("Source")
            indexModel: false
            fact: m_videoSettings.videoSource
            visible: fact.visible
        }
    }

    SettingsGroupLayout {
        Layout.fillWidth: true
        heading: qsTr("Local Video Storage")
        visible: globals.cameraStreamAct

        LabelledFactComboBox {
            Layout.fillWidth:   true
            label:              qsTr("Record File Format")
            fact:               m_videoSettings.recordingFormat
            visible:            m_videoSettings.recordingFormat.visible
        }

        FactCheckBoxSlider {
            Layout.fillWidth:   true
            text:               qsTr("Auto-delete Saved Recordings")
            fact:               m_videoSettings.enableStorageLimit
            visible:            fact.visible && QUAV.videoManager.hasVideo
        }

        LabelledFactTextField {
            Layout.fillWidth:   true
            label:              qsTr("Max Storage Usage")
            fact:               m_videoSettings.maxVideoSize
            visible:            fact.visible && QUAV.videoManager.hasVideo
            enabled:            m_videoSettings.enableStorageLimit.rawValue
        }
    }
}