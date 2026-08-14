import QtQuick
import QtQuick.Controls

import QUAV.FactSystem
import QUAV.Palette
import QUAV.Controls

QUAVComboBox {
    property Fact fact: Fact{}

    property var listStrings: fact ? (fact.enumStrings)
                                     .filter(line => !line.includes('Unknown')) : [];

    property bool indexModel: fact ? fact.enumValues.length === 0 : true // true: Fact values are indices, false: Fact values are FactMetadata.enumValues
    model: fact ? (fact.enumStrings.filter(line => !line.includes('Unknown'))) : null

    currentIndex: fact ?
                      (indexModel ? fact.value : fact.enumIndex <= (typeof fact.enumValues[0] === "number" ? fact.enumValues : listStrings ).length-1 ?
                                    fact.enumIndex : (typeof fact.enumValues[0] === "number" ? fact.enumValues : listStrings ).indexOf(fact.value)) !== -1 ?
                                    (typeof fact.enumValues[0] === "number" ? fact.enumValues : listStrings ).indexOf(fact.value) : 0 : 0

    onModelChanged: {
        Qt.callLater(function() {
            currentIndex = fact ?
                        (indexModel ? fact.value : fact.enumIndex <= (typeof fact.enumValues[0] === "number" ? fact.enumValues : listStrings).length-1 ?
                                      fact.enumIndex : (typeof fact.enumValues[0] === "number" ? fact.enumValues : listStrings).indexOf(fact.value)) !== -1 ?
                                      (typeof fact.enumValues[0] === "number" ? fact.enumValues : listStrings ).indexOf(fact.value) : 0 : 0
        })
    }

    onActivated: (index) => { if (indexModel) {  fact.value = index } else { fact.value = (typeof fact.enumValues[0] === "number" ? fact.enumValues : listStrings)[index] } }
}
