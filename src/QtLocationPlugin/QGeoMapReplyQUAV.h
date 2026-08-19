
#pragma once

#include <QtLocation/private/qgeotiledmapreply_p.h>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "QUAVMapTasks.h"

class QNetworkAccessManager;
class QSslError;

class QGeoTiledMapReplyQUAV : public QGeoTiledMapReply
{
    Q_OBJECT

public:
    QGeoTiledMapReplyQUAV(QNetworkAccessManager *networkManager, const QNetworkRequest &request, const QGeoTileSpec &spec, QObject *parent = nullptr);
    ~QGeoTiledMapReplyQUAV();

    void abort() final;

private slots:
    void m_networkReplyFinished();
    void m_networkReplyError(QNetworkReply::NetworkError error);
    void m_networkReplySslErrors(const QList<QSslError> &errors);
    void m_cacheReply(QUAVCacheTile *tile);
    void m_cacheError(QUAVMapTask::TaskType type, QStringView errorString);

private:
    static void _initDataFromResources();

    QNetworkAccessManager* m_networkManager = nullptr;
    QNetworkRequest        m_request;

    static QByteArray m_bingNoTileImage;
    static QByteArray m_badTile;

    enum HTTP_Response {
        SUCCESS_OK = 200,
        REDIRECTION_MULTIPLE_CHOICES = 300
    };
};
