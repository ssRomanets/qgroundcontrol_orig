
/// @file
///     @author Don Gagne <don@thegagnes.com>

#include "QUAVPalette.h"
#include "QUAVCorePlugin.h"

#include <QtCore/QDebug>

QList<QUAVPalette*>   QUAVPalette::m_paletteObjects;

QUAVPalette::Theme QUAVPalette::m_theme = QUAVPalette::Dark;

QMap<int, QMap<int, QMap<QString, QColor>>> QUAVPalette::m_colorInfoMap;

QStringList QUAVPalette::m_colors;

QUAVPalette::QUAVPalette(QObject* parent)
    : QObject(parent)
    , m_colorGroupEnabled(true)
{
    if (m_colorInfoMap.isEmpty()) {
        m_buildMap();
    }

    // We have to keep track of all QUAVPalette objects in the system so we can signal theme change to all of them
    m_paletteObjects += this;
}

QUAVPalette::~QUAVPalette()
{
    bool fSuccess = m_paletteObjects.removeOne(this);
    if (!fSuccess) {
        qWarning() << "Internal error";
    }
}

void QUAVPalette::m_buildMap()
{
    //                                      Light                 Dark
    //                                      Disabled   Enabled    Disabled   Enabled
    DECLARE_QUAV_COLOR(window,               "#ffffff", "#ffffff", "#222222", "#222222")
    DECLARE_QUAV_COLOR(windowShadeLight,     "#909090", "#828282", "#707070", "#626262")
    DECLARE_QUAV_COLOR(windowShade,          "#d9d9d9", "#d9d9d9", "#333333", "#333333")
    DECLARE_QUAV_COLOR(windowShadeDark,      "#bdbdbd", "#bdbdbd", "#282828", "#282828")
    DECLARE_QUAV_COLOR(text,                 "#9d9d9d", "#000000", "#707070", "#ffffff")
    DECLARE_QUAV_COLOR(button,               "#ffffff", "#ffffff", "#707070", "#626270")
    DECLARE_QUAV_COLOR(buttonBorder,         "#ffffff", "#d9d9d9", "#707070", "#adadb8")
    DECLARE_QUAV_COLOR(buttonText,           "#9d9d9d", "#000000", "#A6A6A6", "#ffffff")
    DECLARE_QUAV_COLOR(buttonHighlight,      "#e4e4e4", "#946120", "#3a3a3a", "#fff291")
    DECLARE_QUAV_COLOR(buttonHighlightText,  "#2c2c2c", "#ffffff", "#2c2c2c", "#000000")
    DECLARE_QUAV_COLOR(primaryButton,        "#585858", "#8cb3be", "#585858", "#8cb3be")
    DECLARE_QUAV_COLOR(primaryButtonText,    "#2c2c2c", "#000000", "#2c2c2c", "#000000")
    DECLARE_QUAV_COLOR(textField,            "#ffffff", "#ffffff", "#707070", "#ffffff")
    DECLARE_QUAV_COLOR(textFieldText,        "#808080", "#000000", "#000000", "#000000")
    DECLARE_QUAV_COLOR(mapIndicator,         "#585858", "#be781c", "#585858", "#be781c")
    DECLARE_QUAV_COLOR(colorRed,             "#b52b2b", "#b52b2b", "#f32836", "#f32836")
    DECLARE_QUAV_COLOR(colorGreen,           "#008f2d", "#008f2d", "#00e04b", "#00e04b")
    DECLARE_QUAV_COLOR(colorGrey,            "#808080", "#808080", "#bfbfbf", "#bfbfbf")
    DECLARE_QUAV_COLOR(toolbarBackground,    "#ffffff", "#ffffff", "#222222", "#222222")
    DECLARE_QUAV_COLOR(groupBorder,          "#bbbbbb", "#bbbbbb", "#707070", "#707070")

    // Colors not affecting by theming
    //                                              Disabled    Enabled
    DECLARE_QUAV_NONTHEMED_COLOR(brandingPurple,     "#4A2C6D", "#4A2C6D")
}

void QUAVPalette::setColorGroupEnabled(bool enabled)
{
    m_colorGroupEnabled = enabled;
    emit paletteChanged();
}

void QUAVPalette::setGlobalTheme(Theme newTheme)
{
    // Mobile build does not have themes
    if (m_theme != newTheme) {
        m_theme = newTheme;
        m_signalPaletteChangeToAll();
    }
}

void QUAVPalette::m_signalPaletteChangeToAll()
{
    // Notify all objects of the new theme
    for (QUAVPalette *palette : std::as_const(m_paletteObjects)) {
        palette->m_signalPaletteChanged();
    }
}

void QUAVPalette::m_signalPaletteChanged()
{
    emit paletteChanged();
}
