import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

import QUAV.Palette

Item {
    property color color: "white"   // Image color

    property alias fillMode:            image.fillMode
    property alias mipmap:              image.mipmap
    property alias source:              image.source
    property alias sourceSize:          image.sourceSize

    width:  image.width
    height: image.height

    Image {
        id:                 image
        smooth:             true
        mipmap:             true
        antialiasing:       true
        visible:            false
        fillMode:           Image.PreserveAspectFit
        anchors.fill:       parent
        sourceSize.height:  height
    }

    ColorOverlay {
        anchors.fill:       image
        source:             image
        color:              parent.color
    }
}
