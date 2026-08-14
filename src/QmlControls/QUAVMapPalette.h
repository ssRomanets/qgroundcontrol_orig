#pragma once

#include <QtCore/QObject>
#include <QtGui/QColor>
#include <QtQmlIntegration/QtQmlIntegration>

class QUAVMapPalette : public QObject
{
    Q_OBJECT
    // QML_ELEMENT

    Q_PROPERTY(bool lightColors READ lightColors WRITE setLightColors NOTIFY paletteChanged)

    Q_PROPERTY(QColor text          READ text          NOTIFY paletteChanged)
    Q_PROPERTY(QColor textOutline   READ textOutline   NOTIFY paletteChanged)

public:
    QUAVMapPalette(QObject* parent = nullptr);

    /// Text color
    QColor text(void)           const { return m_text[m_lightColors ? 0 : 1]; }
    QColor textOutline(void)    const { return m_textOutline[m_lightColors ? 0 : 1]; }

    bool lightColors(void) const { return m_lightColors; }
    void setLightColors(bool lightColors);

signals:
    void paletteChanged(void);
    void lightColorsChanged(bool lightColors);

private:
    bool m_lightColors = false;

    static const int m_cColorGroups = 2;

    static QColor m_text[m_cColorGroups];
    static QColor m_textOutline[m_cColorGroups];
};