
#include "DeviceInfo.h"

#include <QtCore/qapplicationstatic.h>
#include <QtNetwork/QNetworkInformation>


namespace QUAVDeviceInfo
{
bool isInternetAvailable()
{
    if (QNetworkInformation::availableBackends().isEmpty()) return false;

    if (!QNetworkInformation::loadDefaultBackend()) return false;

    if (!QNetworkInformation::loadBackendByFeatures(QNetworkInformation::Feature::Reachability)) return false;

    const QNetworkInformation::Reachability reachability = QNetworkInformation::instance()->reachability();

    return (reachability == QNetworkInformation::Reachability::Online);
}

} // namespace QUAVDeviceInfo
