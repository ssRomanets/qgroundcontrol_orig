
#pragma once

#include "AutoPilotPlugin.h"

/// This is the generic implementation of the AutoPilotPlugin class for mavs
/// we do not have a specific AutoPilotPlugin implementation.
class GenericAutoPilotPlugin : public AutoPilotPlugin
{
    Q_OBJECT

public:
    explicit GenericAutoPilotPlugin(Vehicle* vehicle, QObject* parent = nullptr);
};
