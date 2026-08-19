#pragma onc

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMap>

/// Maintains a list of MissionCommandUIInfo objects loaded from a json file.
class MissionCommandList : public QObject
{
    Q_OBJECT
    
public:
    /// @param baseCommandList true: bottomost level of mission command hierarchy (partial spec allowed), false: override level of hierarchy
    MissionCommandList(const QString& jsonFilename, bool baseCommandList, QObject* parent = nullptr);

    static constexpr const char* quavFileType = "MavCmdInfo";

private:
    static constexpr const char* m_versionJsonKey =       "version";
    static constexpr const char* m_mavCmdInfoJsonKey =    "mavCmdInfo";
};
