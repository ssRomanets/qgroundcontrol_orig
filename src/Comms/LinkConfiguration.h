#pragma once

#include <QtCore/QSettings>
#include <QtCore/QString>

class LinkInterface;

/// Interface holding link specific settings.
class LinkConfiguration : public QObject
{
    Q_OBJECT
    Q_MOC_INCLUDE("LinkInterface.h")

    Q_PROPERTY(QString          name            READ name           WRITE setName           NOTIFY nameChanged)
    Q_PROPERTY(LinkInterface*   link            READ link                                   NOTIFY linkChanged)
    Q_PROPERTY(bool             dynamic         READ isDynamic      WRITE setDynamic        NOTIFY dynamicChanged)
    Q_PROPERTY(bool             autoConnect     READ isAutoConnect  WRITE setAutoConnect    NOTIFY autoConnectChanged)
    Q_PROPERTY(bool             highLatency     READ isHighLatency  WRITE setHighLatency    NOTIFY highLatencyChanged)

public:
    LinkConfiguration(const QString &name, QObject *parent = nullptr);
    LinkConfiguration(const LinkConfiguration *copy, QObject *parent = nullptr);
    virtual ~LinkConfiguration();

    QString name() const { return m_name; }
    void setName(const QString& name);

    LinkInterface* link() const { return m_link.lock().get(); }
    void setLink(const std::shared_ptr<LinkInterface> link);

    /// Is this a dynamic configuration?
    ///     @return True if not persisted
    bool isDynamic() const { return m_dynamic; }

    /// Set if this is this a dynamic configuration. (decided at runtime)
    void setDynamic(bool dynamic = true);

    bool isAutoConnect() const { return m_autoConnect; }

            /// Set if this is this an Auto Connect configuration.
    virtual void setAutoConnect(bool autoc = true);

    /// Is this a High Latency configuration?
    ///     @return True if this is an High Latency configuration (link with large delays).
    bool isHighLatency() const { return m_highLatency; }

    /// Set if this is this an High Latency configuration.
    void setHighLatency(bool hl = false);

    /// The link types supported by QUAV
    /// Any changes here MUST be reflected in LinkManager::linkTypeStrings()
    enum LinkType {
        TypeSerial,     ///< Serial Link
        TypeUdp,        ///< UDP Link
        TypeTcp,        ///< TCP Link
        TypeBluetooth,  ///< Bluetooth Link
        AirLink,
        TypeLogReplay,
        TypeLast        // Last type value (type >= TypeLast == invalid)
    };
    Q_ENUM(LinkType)

    /// Connection type, pure virtual method returning one of the -TypeXxx types above.
    ///     @return The type of links these settings belong to.
    virtual LinkType type() const = 0;

    /// Copy instance data, When manipulating data, you create a copy of the configuration using the copy constructor,
    /// edit it and then transfer its content to the original using this method.
    ///     @param[in] source The source instance (the edited copy)
    virtual void copyFrom(const LinkConfiguration *source);

signals:
    void nameChanged(const QString& name);
    void linkChanged();
    void dynamicChanged();
    void autoConnectChanged();
    void highLatencyChanged();

protected:
    std::weak_ptr<LinkInterface> m_link; ///< Link currently using this configuration (if any)

private:
    QString m_name;
    bool m_dynamic = false;
    bool m_autoConnect = false;
    bool m_highLatency = false;
};

typedef std::shared_ptr<LinkConfiguration> SharedLinkConfigurationPtr;