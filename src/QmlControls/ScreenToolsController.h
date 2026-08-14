
/// @file
///     @author Gus Grubba <gus@auterion.com>

#pragma once

#include <QtCore/QObject>
#include <QtQmlIntegration/QtQmlIntegration>

/// This Qml control is used to return screen parameters
class ScreenToolsController : public QObject
{
    Q_OBJECT
    // QML_ELEMENT
    // TODO: Q_NAMESPACE

    Q_PROPERTY(bool     isMobile            READ isMobile           CONSTANT)
    Q_PROPERTY(bool     fakeMobile          READ fakeMobile         CONSTANT)
    Q_PROPERTY(QString  normalFontFamily    READ normalFontFamily   CONSTANT)

public:
    explicit ScreenToolsController(QObject *parent = nullptr);
    ~ScreenToolsController();

    static bool isMobile() { return fakeMobile(); }
    static bool fakeMobile();

    static QString normalFontFamily();
};
