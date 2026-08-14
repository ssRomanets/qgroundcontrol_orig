#include "MissionCommandList.h"
#include <QtCore/QJsonArray>

MissionCommandList::MissionCommandList(const QString& jsonFilename, bool baseCommandList, QObject* parent)
    : QObject(parent)
{
}