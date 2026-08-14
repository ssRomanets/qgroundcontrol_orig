
#include "QGeoServiceProviderPluginQUAV.h"
#include "QGeoTiledMappingManagerEngineQUAV.h"

#include <QtQml/QQmlEngine>

QGeoServiceProviderFactoryQUAV::QGeoServiceProviderFactoryQUAV(QObject *parent)
    : QObject(parent)
{
//    qDebug() << Q_FUNC_INFO << this;
}

QGeoServiceProviderFactoryQUAV::~QGeoServiceProviderFactoryQUAV()
{
//    qDebug() << Q_FUNC_INFO << this;
}

QGeoCodingManagerEngine *QGeoServiceProviderFactoryQUAV::createGeocodingManagerEngine(
   const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    Q_UNUSED(parameters);
    if (error) {
        *error = QGeoServiceProvider::NotSupportedError;
    }
    if (errorString) {
        *errorString = "Geocoding Not Supported";
    }

    return nullptr;
}

QGeoMappingManagerEngine *QGeoServiceProviderFactoryQUAV::createMappingManagerEngine(
   const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    if (error) {
        *error = QGeoServiceProvider::NoError;
    }
    if (errorString) {
        *errorString = "";
    }

    QNetworkAccessManager *networkManager = nullptr;
    if (m_engine) {
        networkManager = m_engine->networkAccessManager();
    }

    return new QGeoTiledMappingManagerEngineQUAV(parameters, error, errorString, networkManager, nullptr);
}

QGeoRoutingManagerEngine *QGeoServiceProviderFactoryQUAV::createRoutingManagerEngine(
   const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    Q_UNUSED(parameters);
    if (error) {
        *error = QGeoServiceProvider::NotSupportedError;
    }
    if (errorString) {
        *errorString = "Routing Not Supported";
    }

    return nullptr;
}

QPlaceManagerEngine *QGeoServiceProviderFactoryQUAV::createPlaceManagerEngine(
   const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    Q_UNUSED(parameters);
    if (error) {
        *error = QGeoServiceProvider::NotSupportedError;
    }
    if (errorString) {
        *errorString = "Place Not Supported";
    }

    return nullptr;
}
