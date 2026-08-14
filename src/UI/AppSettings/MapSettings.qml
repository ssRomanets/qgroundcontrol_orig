import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import QUAV
import QUAV.FactSystem
import QUAV.FactControls
import QUAV.Controls
import QUAV.ScreenTools
import QUAV.Palette

Item {
    id: root

    property var    m_settingsManager:       QUAV.settingsManager
    property var    m_mapEngineManager:      QUAV.mapEngineManager

    property Fact   m_mapProviderFact:       m_settingsManager.flightMapSettings.mapProvider
    property Fact   m_mapTypeFact:           m_settingsManager.flightMapSettings.mapType
    property Fact   m_elevationProviderFact: m_settingsManager.flightMapSettings.elevationMapProvider

    SettingsPage {
        id:           settingsPage
        anchors.fill: parent

        Component.onCompleted: {
            QUAV.mapEngineManager.loadTileSets()
        }

        Connections {
            target:                           m_mapEngineManager
            function onErrorMessageChanged()  {}
        }

        SettingsGroupLayout {
            Layout.fillWidth: true

            LabelledComboBox {
                label:      qsTr("Provider")
                model:      m_mapEngineManager.mapProviderList

                onActivated: (index) => {
                    m_mapProviderFact.rawValue = comboBox.textAt(index)
                    m_mapTypeFact.rawValue = m_mapEngineManager.mapTypeList(comboBox.textAt(index))[0]
                }

                Component.onCompleted: {
                    var index = comboBox.find(m_mapProviderFact.rawValue)
                    if (index < 0) index = 0
                    comboBox.currentIndex = index
                }
            }

            LabelledComboBox {
                label: qsTr("Type")
                model: m_mapEngineManager.mapTypeList(m_mapProviderFact.rawValue)

                onActivated: (index) => {m_mapTypeFact.rawValue = comboBox.textAt(index) }

                Component.onCompleted: {
                    var index = comboBox.find(m_mapTypeFact.rawValue)
                    if (index < 0) index = 0
                    comboBox.currentIndex = index
                }
            }

            LabelledComboBox {
                label: qsTr("Elevation Provider")
                model: m_mapEngineManager.elevationProviderList

                onActivated: (index) => {  m_elevationProviderFact.rawValue = comboBox.textAt(index)}

                Component.onCompleted: {
                    var index = comboBox.find(m_elevationProviderFact.rawValue)
                    if (index < 0) index = 0
                    comboBox.currentIndex = index
                }
            }
        }
    }
}