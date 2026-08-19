#pragma once

#include <QtCore/QObject>
#include <QtGui/QColor>
#include <QtCore/QMap>
#include <QtQmlIntegration/QtQmlIntegration>

#define DECLARE_QUAV_COLOR(name, lightDisabled, lightEnabled, darkDisabled, darkEnabled) \
    { \
        PaletteColorInfo_t colorInfo = { \
            { QColor(lightDisabled), QColor(lightEnabled) }, \
            { QColor(darkDisabled), QColor(darkEnabled) } \
        }; \
        QUAVCorePlugin::instance()->paletteOverride(#name, colorInfo); \
        m_colorInfoMap[Light][ColorGroupEnabled][QStringLiteral(#name)] = colorInfo[Light][ColorGroupEnabled]; \
        m_colorInfoMap[Light][ColorGroupDisabled][QStringLiteral(#name)] = colorInfo[Light][ColorGroupDisabled]; \
        m_colorInfoMap[Dark][ColorGroupEnabled][QStringLiteral(#name)] = colorInfo[Dark][ColorGroupEnabled]; \
        m_colorInfoMap[Dark][ColorGroupDisabled][QStringLiteral(#name)] = colorInfo[Dark][ColorGroupDisabled]; \
        m_colors << #name; \
    }

#define DECLARE_QUAV_NONTHEMED_COLOR(name, disabledColor, enabledColor) \
    { \
        PaletteColorInfo_t colorInfo = { \
            { QColor(disabledColor), QColor(enabledColor) }, \
            { QColor(disabledColor), QColor(enabledColor) } \
        }; \
        QUAVCorePlugin::instance()->paletteOverride(#name, colorInfo); \
        m_colorInfoMap[Light][ColorGroupEnabled][QStringLiteral(#name)] = colorInfo[Light][ColorGroupEnabled]; \
        m_colorInfoMap[Light][ColorGroupDisabled][QStringLiteral(#name)] = colorInfo[Light][ColorGroupDisabled]; \
        m_colorInfoMap[Dark][ColorGroupEnabled][QStringLiteral(#name)] = colorInfo[Dark][ColorGroupEnabled]; \
        m_colorInfoMap[Dark][ColorGroupDisabled][QStringLiteral(#name)] = colorInfo[Dark][ColorGroupDisabled]; \
        m_colors << #name; \
    }

#define DECLARE_QUAV_SINGLE_COLOR(name, color) \
    { \
        PaletteColorInfo_t colorInfo = { \
            { QColor(color), QColor(color) }, \
            { QColor(color), QColor(color) } \
        }; \
        QUAVCorePlugin::instance()->paletteOverride(#name, colorInfo); \
        m_colorInfoMap[Light][ColorGroupEnabled][QStringLiteral(#name)] = colorInfo[Light][ColorGroupEnabled]; \
        m_colorInfoMap[Light][ColorGroupDisabled][QStringLiteral(#name)] = colorInfo[Light][ColorGroupDisabled]; \
        m_colorInfoMap[Dark][ColorGroupEnabled][QStringLiteral(#name)] = colorInfo[Dark][ColorGroupEnabled]; \
        m_colorInfoMap[Dark][ColorGroupDisabled][QStringLiteral(#name)] = colorInfo[Dark][ColorGroupDisabled]; \
        m_colors << #name; \
    }

#define DEFINE_QUAV_COLOR(NAME, SETNAME) \
    Q_PROPERTY(QColor NAME READ NAME WRITE SETNAME NOTIFY paletteChanged) \
    Q_PROPERTY(QStringList NAME ## Colors READ NAME ## Colors NOTIFY paletteChanged) \
    QColor NAME() const { return m_colorInfoMap[m_theme][m_colorGroupEnabled  ? ColorGroupEnabled : ColorGroupDisabled][QStringLiteral(#NAME)]; } \
    QStringList NAME ## Colors() const { \
        QStringList c; \
        c << m_colorInfoMap[Light][ColorGroupEnabled][QStringLiteral(#NAME)].name(QColor::HexRgb); \
        c << m_colorInfoMap[Light][ColorGroupDisabled][QStringLiteral(#NAME)].name(QColor::HexRgb); \
        c << m_colorInfoMap[Dark][ColorGroupEnabled][QStringLiteral(#NAME)].name(QColor::HexRgb); \
        c << m_colorInfoMap[Dark][ColorGroupDisabled][QStringLiteral(#NAME)].name(QColor::HexRgb); \
        return c; \
    } \
    void SETNAME(const QColor& color) { m_colorInfoMap[m_theme][m_colorGroupEnabled  ? ColorGroupEnabled : ColorGroupDisabled][QStringLiteral(#NAME)] = color; m_signalPaletteChangeToAll(); }


class QUAVPalette : public QObject
{
    Q_OBJECT
    // QML_ELEMENT

public:
    enum ColorGroup {
        ColorGroupDisabled = 0,
        ColorGroupEnabled,
        cMaxColorGroup
    };

    enum Theme {
        Light = 0,
        Dark,
        cMaxTheme
    };
    Q_ENUM(Theme)

    typedef QColor PaletteColorInfo_t[cMaxTheme][cMaxColorGroup];

    Q_PROPERTY(Theme        globalTheme         READ globalTheme        WRITE setGlobalTheme        NOTIFY paletteChanged)
    Q_PROPERTY(bool         colorGroupEnabled   READ colorGroupEnabled  WRITE setColorGroupEnabled  NOTIFY paletteChanged)
    Q_PROPERTY(QStringList  colors              READ colors             CONSTANT)

    DEFINE_QUAV_COLOR(window,                        setWindow)
    DEFINE_QUAV_COLOR(windowShadeLight,              setWindowShadeLight)
    DEFINE_QUAV_COLOR(windowShade,                   setWindowShade)
    DEFINE_QUAV_COLOR(windowShadeDark,               setWindowShadeDark)
    DEFINE_QUAV_COLOR(text,                          setText)
    DEFINE_QUAV_COLOR(warningText,                   setWarningText)
    DEFINE_QUAV_COLOR(button,                        setButton)
    DEFINE_QUAV_COLOR(buttonBorder,                  setButtonBorder)
    DEFINE_QUAV_COLOR(buttonText,                    setButtonText)
    DEFINE_QUAV_COLOR(buttonHighlight,               setButtonHighlight)
    DEFINE_QUAV_COLOR(buttonHighlightText,           setButtonHighlightText)
    DEFINE_QUAV_COLOR(brandingPurple,                setBrandingPurple)
    DEFINE_QUAV_COLOR(primaryButton,                 setPrimaryButton)
    DEFINE_QUAV_COLOR(primaryButtonText,             setPrimaryButtonText)
    DEFINE_QUAV_COLOR(textField,                     setTextField)
    DEFINE_QUAV_COLOR(textFieldText,                 setTextFieldText)
    DEFINE_QUAV_COLOR(mapIndicator,                  setMapIndicator)
    DEFINE_QUAV_COLOR(colorRed,                      setColorRed)
    DEFINE_QUAV_COLOR(colorGreen,                    setColorGreen)
    DEFINE_QUAV_COLOR(colorGrey,                     setColorGrey)
    DEFINE_QUAV_COLOR(toolbarBackground,             setToolbarBackground)
    DEFINE_QUAV_COLOR(groupBorder,                   setGroupBorder)

    QUAVPalette(QObject* parent = nullptr);
    ~QUAVPalette();

    QStringList colors                      () const { return m_colors; }
    bool        colorGroupEnabled           () const { return m_colorGroupEnabled; }
    void        setColorGroupEnabled        (bool enabled);

    static Theme    globalTheme             () { return m_theme; }
    static void     setGlobalTheme          (Theme newTheme);

signals:
    void paletteChanged ();

private:
    static void m_buildMap                   ();
    static void m_signalPaletteChangeToAll   ();
    void        m_signalPaletteChanged       ();
    void        m_themeChanged               ();

    static Theme                m_theme;             ///< There is a single theme for all palettes
    bool                        m_colorGroupEnabled; ///< Currently selected ColorGroup. true: enabled, false: disabled
    static QStringList          m_colors;

    static QMap<int, QMap<int, QMap<QString, QColor>>> m_colorInfoMap;   // theme -> colorGroup -> color name -> color
    static QList<QUAVPalette*> m_paletteObjects;    ///< List of all active QUAVPalette objects
};
