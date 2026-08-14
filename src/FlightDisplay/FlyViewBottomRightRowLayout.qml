import QtQuick
import QtQuick.Layouts

import QUAV
import QUAV.Controls
import QUAV.FlightDisplay

RowLayout {

    FlyViewInstrumentPanel {
        id: instrumentalPanel
        Layout.alignment: Qt.AlignBottom
        visible: QUAV.corePlugin.options.flyView.showInstrumentPanel && m_showSingleVehicleUI && !globals.videoOnWindow
    }
}
