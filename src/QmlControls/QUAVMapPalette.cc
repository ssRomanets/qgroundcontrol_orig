#include "QUAVMapPalette.h"

QColor QUAVMapPalette::m_text         [QUAVMapPalette::m_cColorGroups] = { QColor(255,255,255),     QColor(0,0,0) };
QColor QUAVMapPalette::m_textOutline  [QUAVMapPalette::m_cColorGroups] = { QColor(0,0,0,192),       QColor(255,255,255,192) };

QUAVMapPalette::QUAVMapPalette(QObject* parent)
             : QObject(parent)
{

}

void QUAVMapPalette::setLightColors(bool lightColors)
{
    if ( m_lightColors != lightColors) {
        m_lightColors = lightColors;
        emit paletteChanged();
    }
}