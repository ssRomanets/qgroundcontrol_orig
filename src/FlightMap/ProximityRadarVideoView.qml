import QtQuick
import QtLocation
import QtPositioning
import Qt5Compat.GraphicalEffects

import QUAV             1.0
import QUAV.Controls    1.0
import QUAV.ScreenTools 1.0

Item {
     id: m_root
     property var camera:  null
     property var vehicle: QUAV.multiVehicleManager.activeVehicle

     property real m_defaultHorizonSize: ScreenTools.defaultFontPixelHeight*20
     property real m_defaultCompassSize: ScreenTools.defaultFontPixelHeight*35

     property real m_defaultScaleSize: ScreenTools.defaultFontPixelHeight*22

     property real m_heading:            vehicle ? vehicle.heading.rawValue : 0.0
     property real m_pitch:              vehicle ? vehicle.pitch.rawValue   : 0

     property real m_vxy:                vehicle ? Math.sqrt(vehicle.vx.rawValue*vehicle.vx.rawValue + vehicle.vy.rawValue*vehicle.vy.rawValue) : 0.0
     property real m_vz:                 vehicle ? vehicle.vz.rawValue      : 0.0
     property real m_z:                  vehicle ? vehicle.z.rawValue       : 0.0

     property real m_deltaZ:             0.0
     property real m_deltaExtZ:          0.0

     property var  m_ctx
     property real m_fixRadius:          16.0

     property real m_xCompassIn:         0.0;
     property real m_xCompassN:          0.0;
     property real m_xCompassFn:         0.0;
     property real m_deltaF:             0.0000001;
     property real m_xCompassFix:        0.0;

     property real m_zFix:               0.0;
     property real m_vzFix:              0.0;

     property var maxValue:  1.5;
     property var minValue:  -1.5;
     property var  m_values: [-1.0, -0.5 , 0, 0.5 , 1.0]

     property var m_compassOrientNames:
     [
         "195", "210", "SW", "240", "255", "W", "285", "300", "NW", "330", "345", "N",
         "15" , "30" , "NE", "60" , "75" , "E", "105", "120", "SE", "150", "165", "S", "195"
     ]

     function outputLine(lineColor, lineWidth, inX, inY, fnX, fnY) {
         m_ctx.beginPath();
         m_ctx.strokeStyle = lineColor;
         m_ctx.lineWidth = lineWidth;
         m_ctx.moveTo(inX, inY);
         m_ctx.lineTo(fnX, fnY)
         m_ctx.stroke();
     }

     function outputCircle(x0, y0, radius, color)
     {
         m_ctx.beginPath();
         m_ctx.strokeStyle = color;
         m_ctx.lineWidth = 5;
         m_ctx.arc( x0, y0, radius, -Math.PI/2 , -Math.PI/2 + 2*Math.PI, false)
         m_ctx.stroke();
     }

     function outputRect(x, y, width, height, lineWidth, fillColor, strokeColor)
     {
         m_ctx.beginPath();
         m_ctx.fillStyle = fillColor
         m_ctx.strokeStyle = strokeColor
         m_ctx.lineWidth=lineWidth;
         m_ctx.rect(x, y, width, height);
         m_ctx.fill();
         m_ctx.stroke();
     }

     function defFixZData(value)
     {
          var result
          if       (value > maxValue)                       result = (50-0.62*m_defaultCompassSize);
          if       (value < minValue)                       result = (0.62*m_defaultCompassSize-50);

          if       (value >= 0.0      && value <= maxValue) result = (50-0.62*m_defaultCompassSize)*(m_vz/maxValue);
          else if  (value >= minValue && value <  0.0     ) result = (0.62*m_defaultCompassSize-50)*(m_vz/minValue);
          return result
     }

     ProximityRadarValues {
         id:                     proximityRadarValues
         vehicle:                m_root.vehicle
     }

     Canvas {
          id: compassCanvas
          anchors.fill:     m_root
          opacity:          0.5

          onPaint: {
               m_ctx = getContext("2d")
               m_ctx.reset();
               if (proximityRadarValues.telemetryAvailable && globals.telemOnVideo)
               {
                    m_ctx.translate(m_root.width /2, m_root.height /2 )

                    if (m_pitch >=  90.0) m_pitch =  90.0
                    if (m_pitch <= -90.0) m_pitch = -90.0

                    //линия горизонта
                    outputLine("white", 5.0,  160.0, -1.0*m_defaultHorizonSize*m_pitch/90,  32.0, -1.0*m_defaultHorizonSize*m_pitch/90.0)
                    outputLine("white", 5.0,   24.0, -1.0*m_defaultHorizonSize*m_pitch/90, -24.0, -1.0*m_defaultHorizonSize*m_pitch/90.0)
                    outputLine("white", 5.0, -160.0, -1.0*m_defaultHorizonSize*m_pitch/90, -32.0, -1.0*m_defaultHorizonSize*m_pitch/90.0)
                    outputCircle(0.0,       -1.0*m_defaultHorizonSize*m_pitch/90, 12, "white")

                    //Линия компаса
                    outputLine (
                        "white", 6.0,
                        -m_defaultCompassSize,  -0.62*m_defaultCompassSize,
                         m_defaultCompassSize,  -0.62*m_defaultCompassSize
                    )

                    m_xCompassIn = -m_defaultCompassSize;
                    m_xCompassFn =  m_defaultCompassSize;

                    for (var count = 0; count <=m_compassOrientNames.length-1; count++)
                    {
                        outputLine(
                              "white", 3.0,
                              m_defaultCompassSize - 2.0*count*m_defaultCompassSize/(m_compassOrientNames.length-1),
                              -0.62*m_defaultCompassSize,
                              m_defaultCompassSize - 2.0*count*m_defaultCompassSize/(m_compassOrientNames.length-1),
                              -0.63*m_defaultCompassSize
                        )
                        if ((m_compassOrientNames[m_compassOrientNames.length-1-count] === "N") && (Math.abs(m_xCompassN) <= m_deltaF))
                            m_xCompassN = m_defaultCompassSize - 2.0*count*m_defaultCompassSize/(m_compassOrientNames.length-1);
                    }

                    if (m_heading >= 195.0 && m_heading < 360.0)
                        m_xCompassFix = m_xCompassIn + (m_heading-195.0)*(m_xCompassN-m_xCompassIn)/165.0;
                    else if (m_heading >= 0 && m_heading < 195.0)
                        m_xCompassFix = m_xCompassN + (m_heading)*(m_xCompassFn-m_xCompassN)/195.0;

                    outputLine("white", 4.0,  m_xCompassFix, -0.66*m_defaultCompassSize,  m_xCompassFix, -0.58*m_defaultCompassSize)

                    //шкала скорости по высоте
                    outputRect(
                        -m_defaultCompassSize-120,  50-0.62*m_defaultScaleSize, 90, 2*(0.62*m_defaultScaleSize-50),
                        3, "transparent", "white"
                    )

                    m_deltaZ = 2*(0.62*m_defaultScaleSize-50)/(m_values.length+1)

                    for (var count = 0; count <= m_values.length-1; count++)
                    {
                        outputLine(
                            "white", 3.0,
                            -m_defaultCompassSize-30,
                            -0.62*m_defaultScaleSize+50+(count+1)*m_deltaZ,
                            -m_defaultCompassSize-60,
                            -0.62*m_defaultScaleSize+50+(count+1)*m_deltaZ
                        )
                    }

                    //расширенная шкала скорости по высоте
                    m_deltaExtZ = 2*(0.62*m_defaultScaleSize-80)/(2*m_values.length+1)
                    outputLine (
                        "white", 4.0,
                        -m_defaultCompassSize-30, 50-0.62*m_defaultScaleSize,
                        -m_defaultCompassSize,    80-0.62*m_defaultScaleSize
                    )

                    outputLine (
                        "white", 4.0,
                        -m_defaultCompassSize, 80-0.62*m_defaultScaleSize,
                        -m_defaultCompassSize, 0.62*m_defaultScaleSize-80
                    )

                    for (var count = 0; count <= 2*m_values.length+1; count++)
                    {
                        outputLine(
                            "white", 2.0,
                            -m_defaultCompassSize   , -0.62*m_defaultScaleSize+80+count*m_deltaExtZ,
                            -m_defaultCompassSize-15, -0.62*m_defaultScaleSize+80+count*m_deltaExtZ
                        )
                    }

                    outputLine (
                        "white", 4.0,
                        -m_defaultCompassSize, 0.62*m_defaultScaleSize-80,
                        -m_defaultCompassSize-30, 0.62*m_defaultScaleSize-50,
                    )

                    //линейная отметка скорости по высоте
                    m_vzFix = defFixZData(m_vz)
                    outputLine ("black", 4.0, -m_defaultCompassSize-30, m_vzFix, -m_defaultCompassSize-120, m_vzFix)

                    ///

                    //шкала по высоте
                    outputRect(
                         m_defaultCompassSize+30,  -0.62*m_defaultScaleSize+50, 90, 2*(0.62*m_defaultScaleSize-50),
                         3, "transparent", "white"
                    )

                    m_deltaZ = 2*(0.62*m_defaultScaleSize-50)/(m_values.length+1)

                    for (var count = 0; count <= m_values.length-1; count++)
                    {
                         outputLine(
                              "white", 3.0,
                              m_defaultCompassSize+30,
                              -0.62*m_defaultScaleSize+50+(count+1)*m_deltaZ,
                              m_defaultCompassSize+60,
                              -0.62*m_defaultScaleSize+50+(count+1)*m_deltaZ
                         )
                    }

                    //расширенная шкала скорости по высоте
                    m_deltaExtZ = 2*(0.62*m_defaultScaleSize-80)/(2*m_values.length+1)
                    outputLine (
                         "white", 4.0,
                         m_defaultCompassSize+30, 50-0.62*m_defaultScaleSize,
                         m_defaultCompassSize, 80-0.62*m_defaultScaleSize
                    )

                    outputLine (
                         "white", 4.0,
                         m_defaultCompassSize, 80-0.62*m_defaultScaleSize,
                         m_defaultCompassSize, 0.62*m_defaultScaleSize-80
                    )

                    for (var count = 0; count <= 2*m_values.length+1; count++)
                    {
                         outputLine(
                              "white", 2.0,
                              m_defaultCompassSize   , -0.62*m_defaultScaleSize+80+count*m_deltaExtZ,
                              m_defaultCompassSize+15, -0.62*m_defaultScaleSize+80+count*m_deltaExtZ
                         )
                    }

                    outputLine (
                         "white", 4.0,
                         m_defaultCompassSize,    0.62*m_defaultScaleSize-80,
                         m_defaultCompassSize+30, 0.62*m_defaultScaleSize-50,
                    )

                    //линейная отметка по высоте
                    m_zFix = defFixZData(m_z)
                    outputLine("black", 4.0, m_defaultCompassSize+30, m_zFix, m_defaultCompassSize+120, m_zFix)
               }
          }

          Timer {
               interval: 100;
               running:  true;
               repeat:   true;
               onTriggered: {  compassCanvas.requestPaint()}
          }
     }

     //отметка угла компаса
     Item {
         id: headingLabelId
         anchors.fill: parent
         visible: proximityRadarValues.telemetryAvailable && globals.telemOnVideo

         QUAVLabel{
             x:                      (m_root.width /2) + m_xCompassFix + 5
             y:                      (m_root.height /2) - 0.6*m_defaultCompassSize
             text:                   "heading " + m_heading.toFixed(2).toString()
             font {
                 family:    "Helvetica"
                 pointSize: 10
             }
             color:                  "white"
         }
     }

     // Ввод ориентаций компаса
     Item {
          id: compassOrientItemId
          anchors.fill: parent
          visible: proximityRadarValues.telemetryAvailable && globals.telemOnVideo

          Repeater {
               model: m_compassOrientNames.length

               Text {
                    x:       (m_root.width /2) -
                             (m_defaultCompassSize - 2.0*index*m_defaultCompassSize/(m_compassOrientNames.length-1)) -
                             5*m_compassOrientNames[index].length
                    y:       (m_root.height /2) - 0.67*m_defaultCompassSize
                    text:    m_compassOrientNames[index]
                    font {
                        family:    "Helvetica"
                        pointSize: 10
                        bold:    true
                    }
                    color: "white"
               }
          }
     }

     //отметка угла горизонта
     Item {
          id: pitchLabelId
          anchors.fill: parent
          visible: proximityRadarValues.telemetryAvailable && globals.telemOnVideo

          QUAVLabel{
               x:                      (m_root.width /2) -10.0*m_fixRadius
               y:                      (m_root.height /2) -1.0*m_defaultHorizonSize*m_pitch/90
               text:                   "pitch " + m_pitch.toFixed(2).toString()
               font {
                    family:    "Helvetica"
                    pointSize: 10
               }
               color:                  "white"
          }
     }

     // Вывод шкалы скорости по высоте
     Item {
         id: vzItemId
         anchors.fill: parent
         visible: proximityRadarValues.telemetryAvailable && globals.telemOnVideo

         Repeater {
             model: m_values.length

             Text {
                 x:       (m_root.width  /2) - (m_defaultCompassSize+90)
                 y:       (m_root.height /2) + (0.62*m_defaultScaleSize-50-(index+1)*m_deltaZ) - 10
                 text:    m_values[index]
                 font {
                     family:    "Helvetica"
                     pointSize: 10
                     bold:    true
                 }
                 color: "white"
             }
         }
     }

     //отметка скорости по высоте
     Item {
         id: vzLabelId
         anchors.fill: parent
         visible: proximityRadarValues.telemetryAvailable && globals.telemOnVideo

         QUAVLabel{
             x:                      (m_root.width  /2) - (m_defaultCompassSize+120)
             y:                      (m_root.height /2) + (0.62*m_defaultScaleSize)-40
             text:                   "vz = " + m_vz.toFixed(6).toString() + " m/sec"
             font {
                 family:    "Helvetica"
                 pointSize: 10
             }
             color:                  "white"
         }
     }

     //отметка скорости по горизонтали
     Item {
         id: vчнLabelId
         anchors.fill: parent
         visible: proximityRadarValues.telemetryAvailable && globals.telemOnVideo

         QUAVLabel{
             x:                      (m_root.width  /2) - (m_defaultCompassSize-40)
             y:                      (m_root.height /2) + (0.62*m_defaultScaleSize)-40
             text:                   "vxy = " + m_vxy.toFixed(6).toString() + " m/sec"
             font {
                 family:    "Helvetica"
                 pointSize: 10
             }
             color:                  "white"
         }
     }

     // Вывод шкалы по высоте
     Item {
         id: zItemId
         anchors.fill: parent
         visible: proximityRadarValues.telemetryAvailable && globals.telemOnVideo

         Repeater {
             model: m_values.length

             Text {
                 x:       (m_root.width  /2) + (m_defaultCompassSize+60)
                 y:       (m_root.height /2) + (0.62*m_defaultScaleSize-50-(index+1)*m_deltaZ) - 10
                 text:    m_values[index]
                 font {
                     family:    "Helvetica"
                     pointSize: 10
                     bold:    true
                 }
                 color: "white"
             }
         }
     }

     //отметка по высоте
     Item {
         id: zLabelId
         anchors.fill: parent
         visible: proximityRadarValues.telemetryAvailable && globals.telemOnVideo

         QUAVLabel{
             x:                      (m_root.width /2) + (m_defaultCompassSize+30)
             y:                      (m_root.height /2)  + (0.62*m_defaultScaleSize-50)
             text:                   "z = " + m_z.toFixed(4).toString() + " m"
             font {
                 family:    "Helvetica"
                 pointSize: 10
             }
             color:                  "white"
         }
     }

     QUAVButton {
         id:        startCameraButton
         text:      qsTr("start camera")
         onClicked: { if (!camera.active) camera.start()}
         x:                      m_root.width - 300
         y:                      m_root.height - 50
     }

     QUAVButton {
         id:        stopCameraButton
         text:      qsTr("stop camera")
         onClicked: {if (camera.active) camera.stop()}
         x:                      m_root.width - 150
         y:                      m_root.height - 50
     }
}