#include "QUAV.h"

#include <QtCore/QDateTime>
#include <QtCore/QtNumeric>

#include <float.h>

namespace QUAV {

float limitAngleToPMPIf(double angle)
{
    if ((angle > (-20*M_PI)) && (angle < (20*M_PI)))
    {
        while (angle > ((float)M_PI+FLT_EPSILON))
        {
            angle -= 2.0f * (float)M_PI;
        }

        while (angle <= -((float)M_PI+FLT_EPSILON))
        {
            angle += 2.0f * (float)M_PI;
        }
    }
    else
    {
        // Approximate
        angle = fmodf(angle, (float)M_PI);
    }

    return angle;
}

double limitAngleToPMPId(double angle)
{
    if ((angle > (-20*M_PI)) && (angle < (20*M_PI)))
    {
        if (angle < -M_PI)
        {
            while (angle < -M_PI)
            {
                angle += 2.0f * M_PI;
            }
        }
        else if (angle > M_PI)
        {
            while (angle > M_PI)
            {
                angle -= 2.0f * M_PI;
            }
        }
    }
    else
    {
        // Approximate
        angle = fmod(angle, M_PI);
    }

    return angle;
}


}
