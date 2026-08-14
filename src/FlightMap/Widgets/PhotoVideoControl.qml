import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs

import QUAV
import QUAV.ScreenTools
import QUAV.Controls
import QUAV.Palette
import QUAV.Vehicle
import QUAV.FactSystem
import QUAV.FactControls

Rectangle {
    id:         m_root
    width:      mainLayout.width+(m_margins*2)
    height:     mainLayout.height+(m_margins*2)
    color:      Qt.rgba(quavPal.window.r, quavPal.window.g, quavPal.window.b, 0.5)
    radius:     m_margins
    visible:    m_camera.capturesVideo || m_camera.capturesPhotos

    property real m_margins:        ScreenTools.defaultFontPixelHeight/2
    property real m_smallMargins:   ScreenTools.defaultFontPixelWidth/2
    property var m_activeVehicle:   globals.activeVehicle
    property var m_cameraManager:   m_activeVehicle.cameraManager
    property var m_camera:          m_cameraManager.currentCameraInstance

    property bool m_cameraInPhotoMode: m_camera.cameraMode === MavlinkCameraControl.CAM_MODE_PHOTO
    property bool m_cameraInVideoMode: !m_cameraInPhotoMode

    property bool m_videoCaptureIdle:           m_camera.videoCaptureStatus === MavlinkCameraControl.VIDEO_CAPTURE_STATUS_STOPPED
    property bool m_photoCaptureSingleIdle:     m_camera.photoCaptureStatus === MavlinkCameraControl.PHOTO_CAPTURE_IDLE
    property bool m_photoCaptureIntervalIdle:   m_camera.photoCaptureStatus === MavlinkCameraControl.PHOTO_CAPTURE_INTERVAL_IDLE
    property bool m_photoCaptureIdle:           m_photoCaptureSingleIdle || m_photoCaptureIntervalIdle

    QUAVPalette {id: quavPal;}

    RowLayout {
        id:                 mainLayout
        anchors.margins:    m_margins
        anchors.top:        parent.top
        anchors.left:       parent.left
        spacing:            m_margins

        ColumnLayout {
            spacing: m_margins*2

            ColumnLayout {
                spacing: m_margins
                
                //Photo/Video Mode Selector
                Rectangle {
                    Layout.alignment:   Qt.AlignHCenter
                    width:              ScreenTools.defaultFontPixelWidth*10
                    height:             width/2
                    color:              quavPal.windowShadeLight
                    radius:             height*0.5
                    visible:            m_camera.hasModes
                    
                    //-- Video Mode
                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        width:                  parent.height
                        height:                 parent.height
                        color:                  m_cameraInVideoMode ? quavPal.window : quavPal.windowShadeLight
                        radius:                 height*0.5
                        anchors.left:           parent.left
                        border.color:           quavPal.text
                        border.width:           m_cameraInPhotoMode? 0 : 1
                        
                        QUAVColoredImage {
                            height:             parent.height*0.5
                            width:              height
                            anchors.centerIn:   parent
                            source:             "/qmlimages/camera_video.svg"
                            fillMode:           Image.PreserveAspectFit
                            sourceSize.height:  height
                            color:              m_cameraInVideoMode ? quavPal.colorGreen : quavPal.text

                            MouseArea {
                                anchors.fill:   parent
                                enabled:        m_cameraInPhotoMode ? m_photoCaptureIdle : true
                                onClicked:      m_camera.setCameraModeVideo()
                            }
                        }
                    }

                    //-- Photo Mode
                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        width:                  parent.height
                        height:                 parent.height
                        color:                  m_cameraInPhotoMode ? quavPal.window : quavPal.windowShadeLight
                        radius:                 height*0.5
                        anchors.right:          parent.right
                        border.color:           quavPal.text
                        border.width:           m_cameraInPhotoMode ? 1 : 0

                        QUAVColoredImage {
                            height:             parent.height*0.5
                            width:              height
                            anchors.centerIn:   parent
                            source:             "/qmlimages/camera_photo.svg"
                            fillMode:           Image.PreserveAspectFit
                            sourceSize.height:  height
                            color:              m_cameraInPhotoMode ? quavPal.colorGreen : quavPal.text

                            MouseArea {
                                anchors.fill:   parent
                                enabled:        m_cameraInVideoMode ? m_videoCaptureIdle : true
                                onClicked:      m_camera.setCameraModePhoto()
                            }
                        }
                    }
                }

                // Take Photo, Start/Stop Video button
                Rectangle {
                    Layout.alignment:   Qt.AlignHCenter
                    color:              Qt.rgba(0,0,0,0)
                    width:              ScreenTools.defaultFontPixelWidth*6
                    height:             width
                    radius:             width*0.5
                    border.color:       quavPal.buttonText
                    border.width:       3

                    Rectangle {
                        // anchors.centerIn snaps to integer coordinates, which
                        // depending on DPI can throw the centering off.
                        // Setting alignWhenCentered to false avoids this issue.
                        anchors {
                            centerIn: parent
                            alignWhenCentered: false
                        }
                        width:      parent.width*(m_isShootingInCurrentMode ? 0.5 : 0.75)
                        height:     width
                        radius:     m_isShootingInCurrentMode ? 0 : width*0.5
                        color:      m_isShootingInCurrentMode || m_canShootInCurrentMode ? quavPal.colorRed : quavPal.colorGrey

                        property bool m_isShootingInPhotoMode: m_cameraInPhotoMode && m_camera.photoCaptureStatus === MavlinkCameraControl.PHOTO_CAPTURE_IN_PROGRESS
                        property bool m_isShootingInVideoMode:   (!m_cameraInPhotoMode && m_camera.videoCaptureStatus === MavlinkCameraControl.VIDEO_CAPTURE_STATUS_RUNNING)
                        property bool m_isShootingInCurrentMode: m_cameraInPhotoMode ?  m_isShootingInPhotoMode : m_isShootingInVideoMode

                        property bool m_canShootInCurrentMode:   true
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: toggleShooting()

                        function toggleShooting() {
                            if (m_camera.capturesVideo)
                            {
                                if (m_cameraInPhotoMode) {
                                    if (m_camera.photoCaptureStatus === MavlinkCameraControl.PHOTO_CAPTURE_IDLE)
                                    {
                                        globals.togglePhotoCaptureAct = true
                                    }
                                } else {
                                    globals.toggleVideoRecordingAct =  !globals.toggleVideoRecordingAct
                                }
                            }
                        }
                    }

                    Timer {
                        id: timer
                        interval: 1000;
                        running:  true;
                        repeat:   true;
                        onTriggered: {
                            if (!m_camera.capturesVideo && globals.toggleVideoRecordingAct)
                            {
                                globals.toggleVideoRecordingAct = !globals.toggleVideoRecordingAct
                            }
                        }
                    }
                }

                // Record time / Capture count
                Rectangle {
                    Layout.alignment:       Qt.AlignHCenter
                    color:                  !m_videoCaptureIdle && !m_photoCaptureIdle ? "transparent" : quavPal.colorRed
                    Layout.preferredWidth:  (m_cameraInVideoMode ? videoRecordTime.width : photoCaptureCount.width) + (m_smallMargins*2)
                    Layout.preferredHeight: (m_cameraInVideoMode ? videoRecordTime.height : photoCaptureCount.height)
                    radius:                 m_margins/2

                    // Video record time
                    QUAVLabel {
                        id:                 videoRecordTime
                        anchors.leftMargin: m_smallMargins
                        anchors.left:       parent.left
                        anchors.top:        parent.top
                        text:               m_videoCaptureIdle ? "00:00:00" : m_camera.recordTimeStr
                        font.pointSize:     ScreenTools.largeFontPointSize
                        visible:            m_cameraInVideoMode
                    }

                    // Photo capture count
                    QUAVLabel {
                        id:                 photoCaptureCount
                        anchors.leftMargin: m_smallMargins
                        anchors.left:       parent.left
                        anchors.top:        parent.top
                        text:               m_activeVehicle ? m_camera.photoCount : '000000'
                        font.pointSize:     ScreenTools.largeFontPointSize
                        visible:            m_cameraInPhotoMode
                    }
                }
            }

            QUAVColoredImage {
                Layout.alignment:       Qt.AlignHCenter
                source:                 "/res/gear-black.svg"
                mipmap:                 true
                Layout.preferredHeight: ScreenTools.defaultFontPixelHeight*1.5
                Layout.preferredWidth:  Layout.preferredHeight
                sourceSize.height:      Layout.preferredHeight
                color:                  quavPal.text
                fillMode:               Image.PreserveAspectFit

                QUAVMouseArea {
                    fillItem: parent
                    onClicked: settingsDialogComponent.createObject(mainWindow).open()
                }
            }
        }

        Component {
            id: settingsDialogComponent

            QUAVPopupDialog {
                title: qsTr("Settings")
                buttons: Dialog.Close

                property var m_videoSettings: QUAV.settingsManager.videoSettings

                ColumnLayout {
                    spacing: m_margins

                    GridLayout {
                        id: gridLayout
                        flow: GridLayout.TopToBottom
                        rows: dynamicsRows + m_camera.activeSettings.length

                        property int dynamicsRows: 4

                        // First column
                        QUAVLabel {
                            text: qsTr("Photo Mode")
                            visible: m_camera.capturesPhotos
                            onVisibleChanged: gridLayout.dynamicsRows += visible ? 1 : -1
                        }

                        QUAVLabel {
                            text: qsTr("Video Grid Lines")
                            visible: m_camera.hasVideoStream
                            onVisibleChanged: gridLayout.dynamicsRows += visible ? 1 : -1
                        }

                        QUAVLabel {
                            text: qsTr("Video Screen Fit")
                            visible: m_camera.hasVideoStream
                            onVisibleChanged: gridLayout.dynamicsRows += visible ? 1 : -1
                        }

                        QUAVLabel {
                            text: qsTr("Reset Camera Defaults")
                            onVisibleChanged: gridLayout.dynamicsRows += visible ? 1 : -1
                        }

                        // Second column
                        QUAVComboBox {
                            Layout.fillWidth:   true
                            sizeToContents:     true
                            model:              [qsTr("Single"), qsTr("Time Lapse")]
                            currentIndex:       m_camera.photoCaptureMode
                            visible:            m_camera.capturesPhotos
                            onActivated:        (index) => {m_canera.photoCaptureMode = index}
                        }

                        QUAVSwitch {
                            checked: m_videoSettings.gridLines.rawValue
                            visible: m_camera.hasVideoStream
                            onClicked: m_videoSettings.gridLines.rawValue = checked ? 1 : 0
                        }

                        FactComboBox {
                            Layout.fillWidth: true
                            sizeToContents: true
                            fact: m_videoSettings.videoFit
                            indexModel: false
                            visible: m_camera.hasVideoStream
                        }

                        QUAVButton {
                            Layout.fillWidth: true
                            text: qsTr("Reset")
                            onClicked: resetPrompt.open()

                            MessageDialog {
                                id: resetPrompt
                                title: qsTr("Reset Camera To Factory Settings")
                                text:  qsTr("Confirm resetting all settings?")
                                buttons: MessageDialog.Yes | MessageDialog.No

                                onButtonClicked: function (button, role) {
                                    switch (button) {
                                        case MessageDialog.Yes:
                                        {
                                            m_camera.resetSetings();
                                            resetPrompt.close()
                                            break;
                                        }
                                        case MessageDialog.No:
                                        {
                                            resetPrompt.close()
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}