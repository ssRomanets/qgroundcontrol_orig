#include "StandardModes.h"
#include "Vehicle.h"

static void requestMessageResultHandler(
    void* resultHandlerData, MAV_RESULT result,
    [[maybe_unused]] Vehicle::RequestMessageResultHandlerFailureCode_t failureCode,
    const mavlink_message_t& message
) {
    StandardModes* standardModes = static_cast<StandardModes*>(resultHandlerData);
    standardModes->gotMessage(result, message);
}

StandardModes::StandardModes(QObject* parent, Vehicle* vehicle)
             : QObject(parent)
             , m_vehicle(vehicle)
{

}

void StandardModes::gotMessage(MAV_RESULT result, const mavlink_message_t& message)
{
    m_requestActive = false;
    if (m_wantReset) {
        m_wantReset = false;
        request();
        return;
    }

    if (result == MAV_RESULT_ACCEPTED) {
        mavlink_available_modes_t availableModes;
        mavlink_msg_available_modes_decode(&message, &availableModes);

        if (availableModes.mode_index >= availableModes.number_modes) {
            qDebug() << "Completed, num modes:" << availableModes.number_modes;
            emit requestCompleted();
        } else {
            requestMode(availableModes.mode_index + 1);
        }

    } else {
        qDebug() << "Failed to retrieve available modes - REQUEST_MESSAGE:MAV_RESULT" << result;
        emit requestCompleted();
    }
}

void StandardModes::request()
{
    if (m_requestActive) {
        m_wantReset = true;
        return;
    }

    StandardModes::requestMode(1);
}

void StandardModes::requestMode(int modeIndex)
{
    m_requestActive = true;
    m_vehicle->requestMessage(
        requestMessageResultHandler, this,
        MAV_COMP_ID_AUTOPILOT1, MAVLINK_MSG_ID_AVAILABLE_MODES, modeIndex
    );
}





























