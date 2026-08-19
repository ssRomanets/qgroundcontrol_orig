
#pragma once

#include <QtCore/QtSystemDetection>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/// QUAV's version of Qt QSerialPortInfo. It provides additional information about board types
/// that QUAV cares about.
class QUAVSerialPortInfo : public QSerialPortInfo
{
public:
    QUAVSerialPortInfo();
    explicit QUAVSerialPortInfo(const QSerialPort &port);
    ~QUAVSerialPortInfo();

    enum BoardType_t {
        BoardTypePixhawk = 0,
        BoardTypeSiKRadio,
        BoardTypeOpenPilot,
        BoardTypeRTKGPS,
        BoardTypeUnknown
    };

    bool getBoardInfo(BoardType_t &boardType, QString &name) const;

    /// @return true: Board is currently in bootloader
    bool isBootloader() const;

    /// Known operating system peripherals that are NEVER a peripheral that we should connect to.
    ///     @return true: Port is a system port and not an autopilot
    static bool isSystemPort(const QSerialPortInfo& port);

    /// Override of QSerialPortInfo::availablePorts
    static QList<QUAVSerialPortInfo> availablePorts();

private:
    struct BoardClassString2BoardType_t {
        const QString classString;
        const BoardType_t boardType = BoardTypeUnknown;
    };

    static bool        m_loadJsonData();
    static BoardType_t m_boardClassStringToType(const QString &boardClass);
    static QString     m_boardTypeToString(BoardType_t boardType);

    static bool m_jsonLoaded;
    static bool m_jsonDataValid;

    struct BoardInfo_t {
        int vendorId;
        int productId;
        BoardType_t boardType;
        QString name;
    };
    static QList<BoardInfo_t> m_boardInfoList;

    struct BoardRegExpFallback_t {
        QString regExp;
        BoardType_t boardType;
        bool androidOnly;
    };
    static QList<BoardRegExpFallback_t> m_boardDescriptionFallbackList;
    static QList<BoardRegExpFallback_t> m_boardManufacturerFallbackList;

    static constexpr const char* m_jsonFileTypeValue = "USBBoardInfo";
    static constexpr const char* m_jsonBoardInfoKey = "boardInfo";
    static constexpr const char* m_jsonBoardDescriptionFallbackKey = "boardDescriptionFallback";
    static constexpr const char* m_jsonBoardManufacturerFallbackKey = "boardManufacturerFallback";
    static constexpr const char* m_jsonVendorIDKey = "vendorID";
    static constexpr const char* m_jsonProductIDKey = "productID";
    static constexpr const char* m_jsonBoardClassKey = "boardClass";
    static constexpr const char* m_jsonNameKey = "name";
    static constexpr const char* m_jsonRegExpKey = "regExp";
    static constexpr const char* m_jsonAndroidOnlyKey = "androidOnly";
};
























