#pragma once

#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtLocation/QGeoServiceProviderFactory>

class QGeoServiceProviderFactoryQUAV: public QObject, public QGeoServiceProviderFactory
{
    Q_OBJECT
    Q_INTERFACES(QGeoServiceProviderFactory)
    Q_PLUGIN_METADATA(IID "org.qt-project.qt.geoservice.serviceproviderfactory/6.0" FILE "qgc_maps_plugin.json")

public:
    QGeoServiceProviderFactoryQUAV(QObject *parent = nullptr);
    ~QGeoServiceProviderFactoryQUAV();

    QGeoCodingManagerEngine* createGeocodingManagerEngine(const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const final;
    QGeoMappingManagerEngine* createMappingManagerEngine(const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const final;
    QGeoRoutingManagerEngine* createRoutingManagerEngine(const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const final;
    QPlaceManagerEngine* createPlaceManagerEngine(const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const final;

    void setQmlEngine(QQmlEngine* engine) final { m_engine = engine; }

private:
    QQmlEngine *m_engine = nullptr;
};
