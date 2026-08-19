pragma Singleton

import QtQuick
import QtQuick.Controls
import QtQuick.Window

import QUAV
import QUAV.ScreenToolsController

Item {
    id: m_screenTools

    //-- The point and pixel font size values are computed at runtime

    property real defaultFontPointSize:     10
    property real platformFontPointSize:    10

    readonly property real smallFontPointRatio:      0.75
    readonly property real mediumFontPointRatio:     1.25
    readonly property real largeFontPointRatio:      1.5

    /// You can use these properties to position ui elements in a screen resolution independent manner. Using fixed positioning values should not
    /// be done. All positioning should be done using anchors or a ratio of the defaultFontPixelHeight and defaultFontPixelWidth values. This way
    /// your ui elements will reposition themselves appropriately on varying screen sizes and resolutions.
    property real defaultFontPixelHeight:   10

    /// You can use these properties to position ui elements in a screen resolution independent manner. Using fixed positioning values should not
    /// be done. All positioning should be done using anchors or a ratio of the defaultFontPixelHeight and defaultFontPixelWidth values. This way
    /// your ui elements will reposition themselves appropriately on varying screen sizes and resolutions.
    property real defaultFontPixelWidth:    10

    property real smallFontPointSize:       10
    property real mediumFontPointSize:      10
    property real largeFontPointSize:       10

    property real toolbarHeight: 0

    property real realPixelDensity: {
        //-- If a plugin defines it, just use what it tells us
        if(QUAV.corePlugin.options.devicePixelDensity != 0) {
            return QUAV.corePlugin.options.devicePixelDensity
        }
        //-- Let's use what the system tells us
        return Screen.pixelDensity
    }

    // These properties allow us to create simulated mobile sizing for a desktop build.
    // This makes testing the UI for smaller mobile sizing much easier.
    // The 731x411 size is the size of the Herelink screen which is our target lower bound
    property real screenWidth:  ScreenToolsController.fakeMobile ? 731 : Screen.width
    property real screenHeight: ScreenToolsController.fakeMobile ? 411 : Screen.height

    property bool isMobile:                         ScreenToolsController.isMobile
    property bool isFakeMobile:                     ScreenToolsController.fakeMobile
    property bool isTinyScreen:                     (Screen.width / realPixelDensity) < 120 // 120mm
    property bool isShortScreen:                    ((Screen.height / realPixelDensity) < 120) || (ScreenToolsController.isMobile && ((Screen.height / Screen.width) < 0.6))

    readonly property real minTouchMillimeters:     5
    property real minTouchPixels: 0

    // The implicit heights/widths for our custom control set
    property real implicitButtonWidth:              Math.round(defaultFontPixelWidth *  (isMobile ? 7.0 : 5.0))
    property real implicitButtonHeight:             Math.round(defaultFontPixelHeight * (isMobile ? 2.0 : 1.6))
    property real implicitCheckBoxHeight:           Math.round(defaultFontPixelHeight * (isMobile ? 1.2 : 1.0))
    property real implicitTextFieldWidth:           defaultFontPixelWidth*13
    property real implicitTextFieldHeight:          Math.round(defaultFontPixelHeight*(isMobile ? 2.0 : 1.6))
    property real comboBoxPadding:                  defaultFontPixelWidth
    property real buttonBorderRadius:               defaultFontPixelWidth / 2
    
    readonly property string normalFontFamily:      ScreenToolsController.normalFontFamily

    Connections {
        target: QUAV.settingsManager.appSettings.appFontPointSize
        function onValueChanged() {
            m_setBasePointSize(QUAV.settingsManager.appSettings.appFontPointSize.value)
        }
    }

    onRealPixelDensityChanged: {
        m_setBasePointSize(defaultFontPointSize)
    }

    /// \private
    function m_setBasePointSize(pointSize) {
        m_textMeasure.font.pointSize = pointSize
        defaultFontPointSize    = pointSize
        defaultFontPixelHeight  = Math.round(m_textMeasure.fontHeight/2.0)*2
        defaultFontPixelWidth   = Math.round(m_textMeasure.fontWidth/2.0)*2
        smallFontPointSize      = defaultFontPointSize  * m_screenTools.smallFontPointRatio
        mediumFontPointSize     = defaultFontPointSize  * m_screenTools.mediumFontPointRatio
        largeFontPointSize      = defaultFontPointSize  * m_screenTools.largeFontPointRatio
        minTouchPixels          = Math.round(minTouchMillimeters * realPixelDensity)
        if (minTouchPixels / Screen.height > 0.15) {
            // If using physical sizing takes up too much of the vertical real estate fall back to font based sizing
            minTouchPixels      = defaultFontPixelHeight * 3
        }
        toolbarHeight = defaultFontPixelHeight*3
        toolbarHeight = toolbarHeight*QUAV.corePlugin.options.toolbarHeightMultiplier
    }

    Text {
        id:     m_defaultFont
        text:   "X"
    }

    Text {
        id:     m_textMeasure
        text:   "X"
        font.family:    normalFontFamily
        property real   fontWidth:    contentWidth
        property real   fontHeight:   contentHeight
        Component.onCompleted: {
            platformFontPointSize = m_defaultFont.font.pointSize;
            //-- See if we are using a custom size
            var m_appFontPointSizeFact = QUAV.settingsManager.appSettings.appFontPointSize
            var baseSize = m_appFontPointSizeFact.value
            //-- Sanity check
            if(baseSize < m_appFontPointSizeFact.min || baseSize > m_appFontPointSizeFact.max) {
                baseSize = platformFontPointSize;
                m_appFontPointSizeFact.value = baseSize
            }
            //-- Set size saved in settings
            m_screenTools.m_setBasePointSize(baseSize);
        }
    }
}
