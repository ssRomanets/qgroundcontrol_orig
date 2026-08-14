#include "QUAVSerialPortInfo.h"
#include "JsonHelper.h"

#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

bool QUAVSerialPortInfo::m_jsonLoaded = false;
bool QUAVSerialPortInfo::m_jsonDataValid = false;
QList<QUAVSerialPortInfo::BoardInfo_t> QUAVSerialPortInfo::m_boardInfoList;
QList<QUAVSerialPortInfo::BoardRegExpFallback_t> QUAVSerialPortInfo::m_boardDescriptionFallbackList;
QList<QUAVSerialPortInfo::BoardRegExpFallback_t> QUAVSerialPortInfo::m_boardManufacturerFallbackList;

QUAVSerialPortInfo::QUAVSerialPortInfo()
                  : QSerialPortInfo()
{
   // qDebug() << Q_FUNC_INFO << this;
}

QUAVSerialPortInfo::QUAVSerialPortInfo(const QSerialPort &port)
    : QSerialPortInfo(port)
{
    qDebug() <<"QUAVSerialPortInfo::QUAVSerialPortInfo(const QSerialPort &port)"<<" "<< Q_FUNC_INFO << this;
}

QUAVSerialPortInfo::~QUAVSerialPortInfo()
{
   // qDebug() << Q_FUNC_INFO << this;
}

bool QUAVSerialPortInfo::m_loadJsonData()
{
    if (m_jsonLoaded) {  return m_jsonDataValid;  }

    m_jsonLoaded = true;

    QString errorString;
    int version;
    const QJsonObject json = JsonHelper::openInternalQUAVJsonFile(
        QStringLiteral(":/json/USBBoardInfo.json"), QString(m_jsonFileTypeValue), 1, 1, version, errorString
    );
    if (!errorString.isEmpty()) {
    //    qWarning() << "Internal Error:" << errorString;
        return false;
    }

    static const QList<JsonHelper::KeyValidateInfo> rootKeyInfoList = {
        { m_jsonBoardInfoKey, QJsonValue::Array, true },
        { m_jsonBoardDescriptionFallbackKey, QJsonValue::Array, true },
        { m_jsonBoardManufacturerFallbackKey, QJsonValue::Array, true },
    };

    if (!JsonHelper::validateKeys(json, rootKeyInfoList, errorString)) {
    //    qWarning() << errorString;
        return false;
    }

    static const QList<JsonHelper::KeyValidateInfo> boardKeyInfoList = {
        { m_jsonVendorIDKey, QJsonValue::Double, true },
        { m_jsonProductIDKey, QJsonValue::Double, true },
        { m_jsonBoardClassKey, QJsonValue::String, true },
        { m_jsonNameKey, QJsonValue::String, true }
    };

    const QJsonArray rgBoardInfo = json[m_jsonBoardInfoKey].toArray();
    for (const QJsonValue &jsonValue : rgBoardInfo) {
        if (!jsonValue.isObject()) {
    //        qWarning() << "Entry in boardInfo array is not object";
            return false;
        }

        const QJsonObject boardObject = jsonValue.toObject();
        if (!JsonHelper::validateKeys(boardObject, boardKeyInfoList, errorString)) {
        //    qWarning() << errorString;
            return false;
        }

        const BoardInfo_t boardInfo = {
            boardObject[m_jsonVendorIDKey].toInt(),
            boardObject[m_jsonProductIDKey].toInt(),
            m_boardClassStringToType(boardObject[m_jsonBoardClassKey].toString()),
            boardObject[m_jsonNameKey].toString()
        };
        if (boardInfo.boardType == BoardTypeUnknown) {
        //    qWarning() << "Bad board class" << boardObject[m_jsonBoardClassKey].toString();
            return false;
        }

        m_boardInfoList.append(boardInfo);
    }

    static const QList<JsonHelper::KeyValidateInfo> fallbackKeyInfoList = {
        { m_jsonRegExpKey, QJsonValue::String, true },
        { m_jsonBoardClassKey, QJsonValue::String, true },
        { m_jsonAndroidOnlyKey, QJsonValue::Bool, false },
    };
    const QJsonArray rgBoardDescriptionFallback = json[m_jsonBoardDescriptionFallbackKey].toArray();
    for (const QJsonValue &jsonValue : rgBoardDescriptionFallback) {
        if (!jsonValue.isObject()) {
        //    qWarning() << "Entry in boardFallback array is not object";
            return false;
        }

        const QJsonObject fallbackObject = jsonValue.toObject();
        if (!JsonHelper::validateKeys(fallbackObject, fallbackKeyInfoList, errorString)) {
        //    qWarning() << errorString;
            return false;
        }

        const BoardRegExpFallback_t boardFallback = {
            fallbackObject[m_jsonRegExpKey].toString(),
            m_boardClassStringToType(fallbackObject[m_jsonBoardClassKey].toString()),
            fallbackObject[m_jsonAndroidOnlyKey].toBool(false)
        };
        if (boardFallback.boardType == BoardTypeUnknown) {
        //    qWarning() << "Bad board class" << fallbackObject[m_jsonBoardClassKey].toString();
            return false;
        }

        m_boardDescriptionFallbackList.append(boardFallback);
    }

    const QJsonArray rgBoardManufacturerFallback = json[m_jsonBoardManufacturerFallbackKey].toArray();
    for (const QJsonValue &jsonValue : rgBoardManufacturerFallback) {
        if (!jsonValue.isObject()) {
        //    qWarning() << "Entry in boardFallback array is not object";
            return false;
        }

        const QJsonObject fallbackObject = jsonValue.toObject();
        if (!JsonHelper::validateKeys(fallbackObject, fallbackKeyInfoList, errorString)) {
        //    qWarning() << errorString;
            return false;
        }

        const BoardRegExpFallback_t boardFallback = {
            fallbackObject[m_jsonRegExpKey].toString(),
            m_boardClassStringToType(fallbackObject[m_jsonBoardClassKey].toString()),
            fallbackObject[m_jsonAndroidOnlyKey].toBool(false)
        };
        if (boardFallback.boardType == BoardTypeUnknown) {
        //    qWarning() << "Bad board class" << fallbackObject[m_jsonBoardClassKey].toString();
            return false;
        }

        m_boardManufacturerFallbackList.append(boardFallback);
    }

    m_jsonDataValid = true;

    return true;
}

QUAVSerialPortInfo::BoardType_t QUAVSerialPortInfo::m_boardClassStringToType(const QString &boardClass)
{
    static const BoardClassString2BoardType_t rgBoardClass2BoardType[BoardTypeUnknown] = {
        { m_boardTypeToString(BoardTypePixhawk),   BoardTypePixhawk },
        { m_boardTypeToString(BoardTypeRTKGPS),    BoardTypeRTKGPS },
        { m_boardTypeToString(BoardTypeSiKRadio),  BoardTypeSiKRadio },
        { m_boardTypeToString(BoardTypeOpenPilot), BoardTypeOpenPilot },
    };

    for (const BoardClassString2BoardType_t& board : rgBoardClass2BoardType) {
        if (boardClass == board.classString) {
            return board.boardType;
        }
    }

    return BoardTypeUnknown;
}

bool QUAVSerialPortInfo::getBoardInfo(QUAVSerialPortInfo::BoardType_t &boardType, QString &name) const
{
    boardType = BoardTypeUnknown;

    if (!m_loadJsonData()) {
        return false;
    }

    if (isNull()) {
        return false;
    }

    for (const BoardInfo_t& boardInfo : m_boardInfoList) {
        if (
            (vendorIdentifier() == boardInfo.vendorId) &&
            ((productIdentifier() == boardInfo.productId) || (boardInfo.productId == 0))
        ) {
            boardType = boardInfo.boardType;
            name = boardInfo.name;
            return true;
        }
    }

    Q_ASSERT(boardType == BoardTypeUnknown);

    for (const BoardRegExpFallback_t & boardFallback : m_boardDescriptionFallbackList) {
        if (description().contains(QRegularExpression(boardFallback.regExp, QRegularExpression::CaseInsensitiveOption))) {
            boardType = boardFallback.boardType;
            name = m_boardTypeToString(boardType);
            return true;
        }
    }

    for (const BoardRegExpFallback_t& boardFallback : m_boardManufacturerFallbackList) {
        if (manufacturer().contains(QRegularExpression(boardFallback.regExp, QRegularExpression::CaseInsensitiveOption))) {
            boardType = boardFallback.boardType;
            name = m_boardTypeToString(boardType);
            return true;
        }
    }

    return false;
}

QString QUAVSerialPortInfo::m_boardTypeToString(BoardType_t boardType)
{
    switch (boardType) {
        case BoardTypePixhawk:
            return QStringLiteral("Pixhawk");
        case BoardTypeSiKRadio:
            return QStringLiteral("SiK Radio");
        case BoardTypeOpenPilot:
            return QStringLiteral("OpenPilot");
        case BoardTypeRTKGPS:
            return QStringLiteral("RTK GPS");
        case BoardTypeUnknown:
        default:
            return QStringLiteral("Unknown");
    }
}

QList<QUAVSerialPortInfo> QUAVSerialPortInfo::availablePorts()
{
    QList<QUAVSerialPortInfo> list;

    const QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo& portInfo : availablePorts) {
        if (isSystemPort(portInfo)) { continue; }

        const QUAVSerialPortInfo* const quavPortInfo = reinterpret_cast<const QUAVSerialPortInfo*>(&portInfo);
        list<<*quavPortInfo;
    }

    return list;
}

bool QUAVSerialPortInfo::isSystemPort(const QSerialPortInfo& port){ return false; }

bool QUAVSerialPortInfo::isBootloader() const
{
    BoardType_t boardType;
    QString name;
    if (!getBoardInfo(boardType, name)) {
        return false;
    }

    return ((boardType == BoardTypePixhawk) && description().contains(QStringLiteral("BL")));
}
























