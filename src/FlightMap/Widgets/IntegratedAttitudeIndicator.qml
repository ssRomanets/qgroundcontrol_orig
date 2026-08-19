import QtQuick

import QUAV
import QUAV.Controls
import QUAV.ScreenTools
import QUAV.FlightDisplay
import QUAV.FlightMap

Item {
    implicitWidth:  m_totalRadius * 2
    implicitHeight: implicitWidth

    property real compassRadius:        ScreenTools.defaultFontPixelHeight * 6 / 2
    property real attitudeAngleDegrees: 0

    property real attitudeSize:         ScreenTools.defaultFontPixelHeight * 0.75
    property real attitudeSpacing:      ScreenTools.defaultFontPixelHeight / 4

    property real m_totalRadius:             compassRadius + attitudeSpacing + attitudeSize
    property real m_attitudeRadius:          (width / 2) - (attitudeSize / 2)
    property real m_maxAngleDegrees:         30
    property real m_maxRadians:              m_maxAngleDegrees * Math.PI / 180
    property real m_zeroAttitudeRadians:     Math.PI * 1.5
    property real m_clampedAngleDegrees:     Math.min(Math.max(attitudeAngleDegrees, -m_maxAngleDegrees), m_maxAngleDegrees)
    property real m_attitudeAnglePercent:    m_clampedAngleDegrees / m_maxAngleDegrees

    property var quavPal:  QUAV.globalPalette

    onM_attitudeAnglePercentChanged: angleIndicator.requestPaint()

    // Roll background
    Canvas {
        anchors.fill: parent

        onPaint: {
            var centerX = width / 2
            var centerY = height / 2
            var ctx = getContext("2d")
            ctx.reset()
            ctx.strokeStyle = quavPal.window
            ctx.lineWidth = attitudeSize
            ctx.beginPath()
            ctx.arc(centerX, centerY, m_attitudeRadius, m_zeroAttitudeRadians - m_maxRadians, m_zeroAttitudeRadians + m_maxRadians)
            ctx.stroke()
        }
    }

    // Roll value indicator
    Canvas {
        id:             angleIndicator
        anchors.fill:   parent
        visible:        Math.abs(attitudeAngleDegrees) > 1

        property real startRollRadiansRaw:      m_zeroAttitudeRadians
        property real endRollRadiansRaw:        m_zeroAttitudeRadians + (m_attitudeAnglePercent * m_maxRadians)
        property real startRollRadiansOrdered:  Math.min(startRollRadiansRaw, endRollRadiansRaw)
        property real endRollRadiansOrdered:    Math.max(startRollRadiansRaw, endRollRadiansRaw)

        onPaint: {
            var centerX = width / 2
            var centerY = height / 2
            var ctx = getContext("2d")
            ctx.reset()
            ctx.strokeStyle = quavPal.primaryButton
            ctx.lineWidth = attitudeSize
            ctx.beginPath()
            ctx.arc(centerX, centerY, m_attitudeRadius, startRollRadiansOrdered, endRollRadiansOrdered)
            ctx.stroke()
        }
    }

    // Roll 0 value tick mark
    Canvas {
        anchors.fill: parent

        onPaint: {
            var centerX = width / 2
            var centerY = height / 2
            var ctx = getContext("2d")
            ctx.reset()
            ctx.strokeStyle = quavPal.text
            ctx.lineWidth = 2
            ctx.beginPath()
            ctx.moveTo(centerX, 0)
            ctx.lineTo(centerX, attitudeSize)
            ctx.stroke()
        }
    }
}