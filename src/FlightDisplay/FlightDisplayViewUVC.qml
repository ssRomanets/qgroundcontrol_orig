import QtQuick
import QtMultimedia

import QUAV
import QUAV.FlightMap
import QUAV.ScreenTools
import QUAV.Controls
import QUAV.Palette
import QUAV.Vehicle

Rectangle {
    id:                 m_root
    width:              parent.width
    height:             parent.height
    z: 0
    color:              Qt.rgba(0,0,0,0.75)
    clip:               true
    anchors.centerIn:   parent

    visible:            m_videoManager.isUvc

    property bool toggleVideoRecordingAct: false
    property var m_videoManager: QUAV.videoManager

    property var m_captureSession:          captureSession


    property var m_activeVehicle:   globals.activeVehicle
    property var m_cameraManager:   (m_activeVehicle !== null && m_activeVehicle !== undefined) ?
                                    m_activeVehicle.cameraManager ? m_activeVehicle.cameraManager : null : null
    property var m_camera:          (m_cameraManager !== null && m_cameraManager !== undefined) ?
                                     m_cameraManager.currentCameraInstance !== undefined ? m_cameraManager.currentCameraInstance : undefined : undefined

    function adjustaspectRatio() {
        //-- Set aspect ratio
        var resolution = camera.cameraFormat.resolution
        if (resolution.height > 0 && resolution.width > 0) {
            var aspectRatio = resolution.width / resolution.height
            m_root.height = parent.height*aspectRatio
        }
    }

    MediaDevices {
        id: mediaDevices

        function findCameraDevice(cameraId) {
            var videoInputs = mediaDevices.videoInputs
            for (var i=0; i < videoInputs.length; i++) {
                if (videoInputs[i].description === cameraId) {
                    return videoInputs[i]
                }
            }
            return mediaDevices.defaultVideoInput
        }
    }

    CaptureSession {
        id: captureSession
        camera: Camera {
            id:             camera
            cameraDevice:   mediaDevices.findCameraDevice(m_videoManager.uvcVideoSourceID)
            active:         m_videoManager.isUvc

            onCameraDeviceChanged: { if (active) { adjustaspectRatio()}}

            onActiveChanged: { if (active) {adjustaspectRatio()}}
        }
        videoOutput: videoOutput
    }

    VideoOutput {
        id:               videoOutput
        anchors.fill:     parent
        anchors.centerIn: parent
        fillMode:       VideoOutput.PreserveAspectCrop

        ProximityRadarVideoView {
            anchors.fill:   videoOutput
            camera:         (globals.videoUVCCamera)
            visible:        globals.videoOnWindow
        }
    }

    Timer {
        id: timer
        interval: 500;
        running:  true;
        repeat:   true;
        onTriggered: {
            if (globals.cameraStreamAct)
            {
                if (globals.videoUVCCamera === null) globals.videoUVCCamera = camera;

                if (toggleVideoRecordingAct !== globals.toggleVideoRecordingAct || !(globals.videoUVCCamera).active)
                {
                    if (!(globals.videoUVCCamera).active && toggleVideoRecordingAct == true && toggleVideoRecordingAct === globals.toggleVideoRecordingAct)
                    {
                        toggleVideoRecordingAct         = false
                        globals.toggleVideoRecordingAct = false
                        if ((m_camera !== null && m_camera !== undefined))  m_camera.toggleVideoRecording(m_captureSession)
                    }
                    else if ((globals.videoUVCCamera).active && toggleVideoRecordingAct !== globals.toggleVideoRecordingAct)
                    {
                        toggleVideoRecordingAct  =  globals.toggleVideoRecordingAct
                        if ((m_camera !== null && m_camera !== undefined) )  m_camera.toggleVideoRecording(m_captureSession)
                    }
                }

                if (globals.togglePhotoCaptureAct)
                {
                    if ((globals.videoUVCCamera).active) m_camera.takePhoto(m_captureSession)
                    globals.togglePhotoCaptureAct = false
                }
            }
        }
    }
}