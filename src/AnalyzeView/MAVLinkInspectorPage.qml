import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Window
import QtCharts

import QUAV
import QUAV.Palette
import QUAV.Controls
import QUAV.Controllers
import QUAV.ScreenTools

AnalyzePage {
    id: root
    headerComponent: headerComponent
    pageComponent:   pageComponent
    allowPopout:     true

    property var    curSystem:          controller ? controller.activeSystem : null
    property var    curMessage:         curSystem && curSystem.messages.count ? curSystem.messages.get(curSystem.selected) : null
    property int    curCompID:          0
    property real   maxButtonWidth:     0

    MAVLinkInspectorController {    id: controller }

    QUAVPalette { id: quavPal }

    Component {
        id: headerComponent

        RowLayout {
            id: header
            anchors.left: parent.left
            anchors.right: parent.right

            QUAVLabel {
                text: qsTr("Inspect real time MAVLink messages.!!!")
            }

            RowLayout {
                Layout.alignment:   Qt.AlignRight
                visible:            curSystem ? controller.systemNames.length > 0 || curSystem.compIDsStr.length > 0 : false
                QUAVComboBox {
                    id:             systemCombo
                    model:          controller.systemNames
                    sizeToContents: true
                    visible:        controller.systemNames.length > 0
                    onActivated: (index) =>  {  controller.setActiveSystem(controller.systems.get(index).id) }

                    Connections {
                        target: controller
                        function onActiveSystemChanged() {
                            for (var systemIndex=0; systemIndex<controller.systems.count; systemIndex++) {
                                if (controller.systems.get(systemIndex) == curSystem) {
                                    systemCombo.currentIndex = systemIndex
                                    curCompID = 0
                                    cidCombo.currentIndex = 0
                                    break
                                }
                            }
                        }
                    }
                }

                QUAVComboBox {
                    id:             cidCombo
                    model:          curSystem ? curSystem.compIDsStr : []
                    sizeToContents: true
                    visible:        curSystem ? curSystem.compIDsStr.length > 0 : false
                    onActivated: (index) => {
                        if(curSystem && curSystem.compIDsStr.length > 0) {
                            if(index < 1)
                                curCompID = 0
                            else
                                curCompID = curSystem.compIDs[index - 1]
                        }
                    }
                }
            }
        }
    }

    Component {
        id: pageComponent

        Row {
            width: availableWidth
            height: availableHeight
            spacing: ScreenTools.defaultFontPixelWidth

            //-- Messages (Buttons)
            QUAVFlickable {
                id:                 buttonGrid
                flickableDirection: Flickable.VerticalFlick
                width:              maxButtonWidth
                height:             parent.height
                contentWidth:       width
                contentHeight:      buttonCol.height
                ColumnLayout {
                    id:             buttonCol
                    anchors.left:   parent.left
                    anchors.right:  parent.right
                    spacing:        ScreenTools.defaultFontPixelHeight * 0.25
                    Repeater {
                        model:      curSystem ? curSystem.messages : []
                        delegate:   MAVLinkMessageButton {
                            text:       object.name + (object.fieldSelected ?  " *" : "")
                            compID:     object.compId
                            checked:    curSystem ? (curSystem.selected === index) : false
                            messageHz:  object.actualRateHz
                            visible:    curCompID === 0 || curCompID === compID
                            onClicked: {
                                curSystem.selected = index
                            }
                            Layout.fillWidth: true
                        }
                    }
                }
            }

            //-- Message Data
            QUAVFlickable {
                id:                 messageGrid
                visible:            curMessage !== null && (curCompID === 0 || curCompID === curMessage.compId)
                flickableDirection: Flickable.VerticalFlick
                width:              parent.width - buttonGrid.width - ScreenTools.defaultFontPixelWidth
                height:             parent.height
                contentWidth:       width
                contentHeight:      messageCol.height
                Column {
                    id:                 messageCol
                    width:              parent.width
                    spacing:            ScreenTools.defaultFontPixelHeight * 0.25
                    GridLayout {
                        columns:        2
                        columnSpacing:  ScreenTools.defaultFontPixelWidth
                        rowSpacing:     ScreenTools.defaultFontPixelHeight * 0.25
                        QUAVLabel {
                            text: qsTr("Message:")
                            Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 20
                        }
                        QUAVLabel {
                            color: quavPal.buttonHighlight
                            text: curMessage ? curMessage.name + ' (' + curMessage.id + ')' : ""
                        }

                        QUAVLabel { text: qsTr("Component:") }
                        QUAVLabel { text: curMessage ? curMessage.compId : "" }

                        QUAVLabel { text: qsTr("Count:") }
                        QUAVLabel { text: curMessage ? curMessage.count : "" }

                        QUAVLabel { text: qsTr("Actual Rate:") }
                        QUAVLabel { text: curMessage ? curMessage.actualRateHz.toFixed(1) + qsTr("Hz") : "" }

                        QUAVLabel { text: qsTr("Set Rate:") }
                        QUAVComboBox {
                            id: msgRateCombo
                            textRole: "text"
                            valueRole: "value"
                            model: [
                                { value: -1, text: qsTr("Disabled") },
                                { value: 0, text: qsTr("Default") },
                                { value: 1, text: qsTr("1Hz") },
                                { value: 2, text: qsTr("2Hz") },
                                { value: 3, text: qsTr("3Hz") },
                                { value: 4, text: qsTr("4Hz") },
                                { value: 5, text: qsTr("5Hz") },
                                { value: 6, text: qsTr("6Hz") },
                                { value: 7, text: qsTr("7Hz") },
                                { value: 8, text: qsTr("8Hz") },
                                { value: 9, text: qsTr("9Hz") },
                                { value: 10, text: qsTr("10Hz") },
                                { value: 25, text: qsTr("25Hz") },
                                { value: 50, text: qsTr("50Hz") },
                                { value: 100, text: qsTr("100Hz") }
                            ]
                            Layout.alignment: Qt.AlignLeft
                            sizeToContents: true
                            Component.onCompleted: reset()
                            onActivated: (index) => controller.setMessageInterval(currentValue)
                            function reset() { currentIndex = indexOfValue(0) }
                            Connections {
                                target: root
                                function onCurMessageChanged() { msgRateCombo.reset() }
                            }
                            Connections {
                                target: curMessage
                                function onTargetRateHzChanged() {
                                    const target_index = indexOfValue(curMessage.targetRateHz)
                                    if(target_index != -1) {
                                        currentIndex = target_index
                                    }
                                }
                            }
                        }
                    }
                    Item { height: ScreenTools.defaultFontPixelHeight; width: 1 }
                    //---------------------------------------------------------
                    GridLayout {
                        id:                 msgInfoGrid
                        columns:            3
                        columnSpacing:      ScreenTools.defaultFontPixelWidth  * 0.25
                        rowSpacing:         ScreenTools.defaultFontPixelHeight * 0.25
                        width:              parent.width
                        QUAVLabel {
                            text:       qsTr("Name")
                        }
                        QUAVLabel {
                            text:       qsTr("Value")
                        }
                        QUAVLabel {
                            text:       qsTr("Type")
                        }

                        //---------------------------------------------------------
                        Rectangle {
                            Layout.columnSpan:  3
                            Layout.fillWidth:   true
                            height:             1
                            color:              quavPal.text
                        }
                        //---------------------------------------------------------

                        Repeater {
                            model:      curMessage ? curMessage.fields : []
                            delegate:   QUAVLabel {
                                Layout.row:         index + 2
                                Layout.column:      0
                                Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 20
                                text:               object.name
                            }
                        }
                        Repeater {
                            model:      curMessage ? curMessage.fields : []
                            delegate:   QUAVLabel {
                                Layout.row:         index + 2
                                Layout.column:      1
                                Layout.minimumWidth: msgInfoGrid.width * 0.25
                                Layout.maximumWidth: msgInfoGrid.width * 0.25
                                text:               object.value
                                elide:              Text.ElideRight
                            }
                        }
                        Repeater {
                            model:      curMessage ? curMessage.fields : []
                            delegate:   QUAVLabel {
                                Layout.row:         index + 2
                                Layout.column:      2
                                Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 10
                                text:               object.type
                                elide:              Text.ElideRight
                            }
                        }
                    }
                }
            }
        }
    }
}