#include "ParameterManager.h"
#include "Vehicle.h"
#include "AutoPilotPlugin.h"
#include "LinkInterface.h"
#include "MAVLinkProtocol.h"

#include <QtCore/QEasingCurve>
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>
#include <QtCore/QVariantAnimation>
#include <QtQml/qqml.h>

ParameterManager::ParameterManager(Vehicle* vehicle)
    : QObject(vehicle)
    , m_vehicle(vehicle)
{

}

ParameterManager::~ParameterManager()
{
}

void ParameterManager::registerQmlTypes()
{
    (void) qmlRegisterType<Fact>("QUAV.FactSystem", 1, 0, "Fact");
    (void) qmlRegisterType<Fact>("QUAV.FactSystem", 1, 0, "FactMetaData");
}


void ParameterManager::mavlinkMessageReceived(const mavlink_message_t& message)
{
    if (m_tryftp && (message.compid == MAV_COMP_ID_AUTOPILOT1) && !m_initialLoadComplete) return;

    if (message.msgid == MAVLINK_MSG_ID_PARAM_VALUE) {
        mavlink_param_value_t param_value{};
        mavlink_msg_param_value_decode(&message, &param_value);

        // This will null terminate the name string
        char parameterNameWithNull[MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN + 1] = {};
        (void) strncpy(parameterNameWithNull, param_value.param_id, MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN);
        const QString parameterName(parameterNameWithNull);

        mavlink_param_union_t paramUnion{};
        paramUnion.param_float = param_value.param_value;
        paramUnion.type = param_value.param_type;

        QVariant parameterValue;

        switch (paramUnion.type) {
            case MAV_PARAM_TYPE_REAL32:
            {
                parameterValue = QVariant(paramUnion.param_float);
                break;
            }
            case MAV_PARAM_TYPE_UINT8:
            {
                parameterValue = QVariant(paramUnion.param_uint8);
                break;
            }
            case MAV_PARAM_TYPE_INT8:
            {
                parameterValue = QVariant(paramUnion.param_int8);
                break;
            }
            case MAV_PARAM_TYPE_UINT16:
            {
                parameterValue = QVariant(paramUnion.param_uint16);
                break;
            }
            case MAV_PARAM_TYPE_INT16:
            {
                parameterValue = QVariant(paramUnion.param_int16);
                break;
            }
            case MAV_PARAM_TYPE_UINT32:
            {
                parameterValue = QVariant(paramUnion.param_uint32);
                break;
            }
            case MAV_PARAM_TYPE_INT32:
            {
                parameterValue = QVariant(paramUnion.param_int32);
                break;
            }
            default:
            //    qCritical() << "ParameterManager::_handleParamValue - unsupported MAV_PARAM_TYPE" << paramUnion.type;
            break;
        }

        m_handleParamValue(
            message.compid, parameterName, param_value.param_count, param_value.param_index,
            static_cast<MAV_PARAM_TYPE>(param_value.param_type), parameterValue
        );
    }

}

void ParameterManager::refreshAllParameters(uint8_t componentId)
{
    const SharedLinkInterfacePtr sharedLink = m_vehicle->vehicleLinkManager()->primaryLink().lock();
    if (!sharedLink) { return;}

    if (m_tryftp && ((componentId == MAV_COMP_ID_ALL) || (componentId == MAV_COMP_ID_AUTOPILOT1))) {

    } else {
        mavlink_message_t msg{};
        mavlink_msg_param_request_list_pack_chan(
            MAVLinkProtocol::instance()->getSystemId(), MAVLinkProtocol::getComponentId(),
            sharedLink->mavlinkChannel(), &msg, m_vehicle->id(), componentId
        );
        (void) m_vehicle->sendMessageOnLinkThreadSafe(sharedLink.get(), msg);
    }
}

void ParameterManager::m_handleParamValue(
    int componentId,    const QString& parameterName, int parameterCount,
    int parameterIndex, MAV_PARAM_TYPE mavParamType,  const QVariant& parameterValue
) {
    m_checkInitialLoadComplete();
}

void ParameterManager::m_checkInitialLoadComplete()
{
    if (m_initialLoadComplete) {
        return;
    }

    m_initialLoadComplete = true;

    m_vehicle->autoPilotPlugin()->parametersReadyPreChecks();
}

















