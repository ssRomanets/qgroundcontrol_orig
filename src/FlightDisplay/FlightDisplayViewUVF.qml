import QtQuick
import QtQuick.Window
import QtQuick.Controls
import Qt.labs.platform
import QtQuick.Layouts

import Qt5Compat.GraphicalEffects
import QtMultimedia

import QUAV.Controls
import QUAV.FlightMap
import QUAV.ScreenTools

import QUAV.FlightDisplay 1.0
import QUAV.FlightMap     1.0
import QUAV               1.0

Rectangle {
    id:                 m_root
    width:              parent.width
    height:             parent.height
    color:              Qt.rgba(0,0,0,0.75)
    clip:               true
    anchors.centerIn:   parent

    property string videoFileName: ""

    MediaPlayer {
        id: mediaPlayer
        videoOutput: videoOutput
        loops: MediaPlayer.Infinite
    }

    VideoOutput {
        id: videoOutput
        anchors.fill: m_root
        anchors.centerIn: m_root
        fillMode: VideoOutput.PreserveAspectCrop
    }

    Timer {
        id: timer
        interval: 1000;
        running:  true;
        repeat:   true;
        onTriggered: {

            if (videoFileName === "" && globals.videoFileName !== "")
            {
                videoFileName = globals.videoFileName
                mediaPlayer.setSource(videoFileName)
                mediaPlayer.play()
            }
            else if (globals.videoFileName !== videoFileName)
            {
                mediaPlayer.stop()
                videoFileName = globals.videoFileName
                mediaPlayer.setSource(videoFileName)
                mediaPlayer.play()
            }
        }
    }

}
