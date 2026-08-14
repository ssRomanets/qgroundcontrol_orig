
#include "Fact.h"
#include "QUAVApplication.h"
#include "QUAVCorePlugin.h"

Fact::Fact(QObject *parent)
    : QObject(parent)
{
    // qDebug() << Q_FUNC_INFO << this;

    FactMetaData *const metaData = new FactMetaData(m_type, this);
    setMetaData(metaData);

    m_init();
}

Fact::Fact(int componentId, const QString &name, FactMetaData::ValueType_t type, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_componentId(componentId)
    , m_type(type)
{
    // qDebug() << Q_FUNC_INFO << this;

    FactMetaData *const metaData = new FactMetaData(m_type, this);
    setMetaData(metaData);

    m_init();
}

Fact::Fact(const QString& settingsGroup, FactMetaData *metaData, QObject *parent)
    : QObject(parent)
    , m_name(metaData->name())
    , m_componentId(0)
    , m_type(metaData->type())
{
    // qDebug() << Q_FUNC_INFO << this;

    QUAVCorePlugin::instance()->adjustSettingMetaData(settingsGroup, *metaData);
    setMetaData(metaData, true /* setDefaultFromMetaData */);

    m_init();
}

Fact::Fact(const Fact &other, QObject *parent)
    : QObject(parent)
{
    // qDebug() << Q_FUNC_INFO << this;

    *this = other;

    m_init();
}

Fact::~Fact()
{
    // qDebug() << Q_FUNC_INFO << this;
}

void Fact::m_init()
{
    (void) connect(this, &Fact::containerRawValueChanged, this, &Fact::m_checkForRebootMessaging);
}

const Fact &Fact::operator=(const Fact& other)
{
    m_name = other.m_name;
    m_componentId = other.m_componentId;
    m_rawValue = other.m_rawValue;
    m_type = other.m_type;
    m_sendValueChangedSignals = other.m_sendValueChangedSignals;
    m_deferredValueChangeSignal = other.m_deferredValueChangeSignal;
    if (m_metaData && other.m_metaData) {
        *m_metaData = *other.m_metaData;
    } else {
        m_metaData = nullptr;
    }

    return *this;
}

void Fact::forceSetRawValue(const QVariant &value)
{
    if (m_metaData) {
        QVariant typedValue;
        QString errorString;

        if (m_metaData->convertAndValidateRaw(value, true /* convertOnly */, typedValue, errorString)) {
            m_rawValue.setValue(typedValue);
            m_sendValueChangedSignal(cookedValue());
            //-- Must be in this order
            emit containerRawValueChanged(rawValue());
            emit rawValueChanged(m_rawValue);
        }
    } else {
        qWarning() << kMissingMetadata << name();
    }
}

void Fact::setRawValue(const QVariant &value)
{
    if (m_metaData) {
        QVariant typedValue;
        QString errorString;

        if (m_metaData->convertAndValidateRaw(value, true /* convertOnly */, typedValue, errorString)) {
            if (typedValue != m_rawValue) {
                m_rawValue.setValue(typedValue);
                m_sendValueChangedSignal(cookedValue());
                //-- Must be in this order
                emit containerRawValueChanged(rawValue());
                emit rawValueChanged(m_rawValue);
            }
        }
    } else {
        qWarning() << kMissingMetadata << name();
    }
}

void Fact::setCookedValue(const QVariant& value)
{
    if (m_metaData) {
        setRawValue(m_metaData->cookedTranslator()(value));
    } else {
        qWarning() << kMissingMetadata << name();
    }
}

int Fact::valueIndex(const QString &value) const
{
    if (m_metaData) {
        return m_metaData->enumStrings().indexOf(value);
    }
    qWarning() << kMissingMetadata << name();
    return -1;
}

void Fact::setEnumStringValue(const QString &value)
{
    const int index = valueIndex(value);
    if (index != -1) {
        setCookedValue(m_metaData->enumValues()[index]);
    }
}

void Fact::setEnumIndex(int index)
{
    if (m_metaData) {
        setCookedValue(m_metaData->enumValues()[index]);
    } else {
        qWarning() << kMissingMetadata << name();
    }
}

void Fact::containerSetRawValue(const QVariant &value)
{
    if (m_rawValue != value) {
        m_rawValue = value;
        m_sendValueChangedSignal(cookedValue());
        emit rawValueChanged(m_rawValue);
    }

    // This always need to be signalled in order to support forceSetRawValue usage and waiting for vehicleUpdated signal
    emit vehicleUpdated(m_rawValue);
}

QVariant Fact::cookedValue() const
{
    if (m_metaData) {
        return m_metaData->rawTranslator()(m_rawValue);
    } else {
        qWarning() << kMissingMetadata << name();
        return m_rawValue;
    }
}

QString Fact::enumStringValue()
{
    if (m_metaData) {
        const int enumIndex = this->enumIndex();
        if ((enumIndex >= 0) && (enumIndex < m_metaData->enumStrings().count())) {
            return m_metaData->enumStrings()[enumIndex];
        }
    } else {
        qWarning() << kMissingMetadata << name();
    }

    return QString();
}

int Fact::enumIndex()
{
    if (m_metaData) {
        //-- Only enums have an index
        if (!m_metaData->enumValues().isEmpty()) {
            int index = 0;
            for (const QVariant &enumValue: m_metaData->enumValues()) {
                if (enumValue == rawValue()) {
                    return index;
                }
                //-- Float comparisons don't always work
                if ((type() == FactMetaData::valueTypeFloat) || (type() == FactMetaData::valueTypeDouble)) {
                    const double diff = fabs(enumValue.toDouble() - rawValue().toDouble());
                    static constexpr double accuracy = 1.0 / 1000000.0;
                    if (diff < accuracy) {
                        return index;
                    }
                }
                index++;
            }
            // Current value is not in list, add it manually
            m_metaData->addEnumInfo(tr("Unknown: %1").arg(rawValue().toString()), rawValue());
            emit enumsChanged();
            return index;
        }
    } else {
        qWarning() << kMissingMetadata << name();
    }
    return -1;
}

QStringList Fact::enumStrings() const
{
    if (m_metaData) {
        return m_metaData->enumStrings();
    } else {
        qWarning() << kMissingMetadata << name();
        return QStringList();
    }
}

QVariantList Fact::enumValues() const
{
    if (m_metaData) {
        return m_metaData->enumValues();
    } else {
        qWarning() << kMissingMetadata << name();
        return QVariantList();
    }
}

void Fact::setEnumInfo(const QStringList &strings, const QVariantList &values)
{
    if (m_metaData) {
        m_metaData->setEnumInfo(strings, values);
        emit enumsChanged();
    } else {
        qWarning() << kMissingMetadata << name();
    }
}

QStringList Fact::bitmaskStrings() const
{
    if (m_metaData) {
        return m_metaData->bitmaskStrings();
    } else {
        qWarning() << kMissingMetadata << name();
        return QStringList();
    }
}

QVariantList Fact::bitmaskValues() const
{
    if (m_metaData) {
        return m_metaData->bitmaskValues();
    } else {
        qWarning() << kMissingMetadata << name();
        return QVariantList();
    }
}

QStringList Fact::selectedBitmaskStrings() const
{
    if (m_metaData) {
        const auto values = m_metaData->bitmaskValues();
        const auto strings = m_metaData->bitmaskStrings();
        if (values.size() != strings.size()) {
            qWarning() << "Size of bitmask value and string is different."  << name();
            return {};
        }

        QStringList selected;
        for (qsizetype i = 0; i < values.size(); i++) {
            if (rawValue().toInt() & values[i].toInt()) {
                selected += strings[i];
            }
        }

        if (selected.isEmpty()) {
            selected += "Not value selected";
        }

        return selected;
    } else {
        qWarning() << kMissingMetadata << name();
        return {};
    }
}

QString Fact::m_variantToString(const QVariant &variant, int decimalPlaces) const
{
    QString valueString;

    switch (type()) {
    case FactMetaData::valueTypeFloat:
    {
        const float fValue = variant.toFloat();
        if (qIsNaN(fValue)) {
            valueString = QStringLiteral("--.--");
        } else {
            valueString = QStringLiteral("%1").arg(fValue, 0, 'f', decimalPlaces);
        }
    }
        break;
    case FactMetaData::valueTypeDouble:
    {
        const double dValue = variant.toDouble();
        if (qIsNaN(dValue)) {
            valueString = QStringLiteral("--.--");
        } else {
            valueString = QStringLiteral("%1").arg(dValue, 0, 'f', decimalPlaces);
        }
        break;
    }
    case FactMetaData::valueTypeBool:
        valueString = variant.toBool() ? tr("true") : tr("false");
        break;
    case FactMetaData::valueTypeElapsedTimeInSeconds:
    {
        const double dValue = variant.toDouble();
        if (qIsNaN(dValue)) {
            valueString = QStringLiteral("--:--:--");
        } else {
            QTime time(0, 0, 0, 0);
            time = time.addSecs(dValue);
            valueString = time.toString(QStringLiteral("hh:mm:ss"));
        }
        break;
    }
    default:
        valueString = variant.toString();
        break;
    }

    return valueString;
}

QString Fact::rawValueStringFullPrecision() const
{
    return m_variantToString(rawValue(), 18);
}

QString Fact::rawValueString() const
{
    return m_variantToString(rawValue(), decimalPlaces());
}

QString Fact::cookedValueString() const
{
    return m_variantToString(cookedValue(), decimalPlaces());
}

QVariant Fact::rawDefaultValue() const
{
    if (m_metaData) {
        if (!m_metaData->defaultValueAvailable()) {
            qDebug() << "Access to unavailable default value";
        }
        return m_metaData->rawDefaultValue();
    } else {
        qWarning() << kMissingMetadata << name();
        return QVariant(0);
    }
}

QVariant Fact::cookedDefaultValue() const
{
    if (m_metaData) {
        if (!m_metaData->defaultValueAvailable()) {
            qDebug() << "Access to unavailable default value";
        }
        return m_metaData->cookedDefaultValue();
    } else {
        qWarning() << kMissingMetadata << name();
        return QVariant(0);
    }
}

QString Fact::cookedDefaultValueString() const
{
    return m_variantToString(cookedDefaultValue(), decimalPlaces());
}

QString Fact::shortDescription() const
{
    if (m_metaData) {
        return m_metaData->shortDescription();
    } else {
        qWarning() << kMissingMetadata << name();
        return QString();
    }
}

QString Fact::longDescription() const
{
    if (m_metaData) {
        return m_metaData->longDescription();
    } else {
        qWarning() << kMissingMetadata << name();
        return QString();
    }
}

QString Fact::rawUnits() const
{
    if (m_metaData) {
        return m_metaData->rawUnits();
    } else {
        qWarning() << kMissingMetadata << name();
        return QString();
    }
}

QString Fact::cookedUnits() const
{
    if (m_metaData) {
        return m_metaData->cookedUnits();
    } else {
        qWarning() << kMissingMetadata << name();
        return QString();
    }
}

QVariant Fact::rawMin() const
{
    if (m_metaData) {
        return m_metaData->rawMin();
    } else {
        qWarning() << kMissingMetadata << name();
        return QVariant(0);
    }
}

QVariant Fact::cookedMin() const
{
    if (m_metaData) {
        return m_metaData->cookedMin();
    } else {
        qWarning() << kMissingMetadata << name();
        return QVariant(0);
    }
}

QString Fact::cookedMinString() const
{
    return m_variantToString(cookedMin(), decimalPlaces());
}

QVariant Fact::rawMax() const
{
    if (m_metaData) {
        return m_metaData->rawMax();
    } else {
        qWarning() << kMissingMetadata << name();
        return QVariant(0);
    }
}

QVariant Fact::cookedMax() const
{
    if (m_metaData) {
        return m_metaData->cookedMax();
    } else {
        qWarning() << kMissingMetadata << name();
        return QVariant(0);
    }
}

QString Fact::cookedMaxString() const
{
    return m_variantToString(cookedMax(), decimalPlaces());
}

bool Fact::minIsDefaultForType() const
{
    if (m_metaData) {
        return m_metaData->minIsDefaultForType();
    } else {
        qWarning() << kMissingMetadata << name();
        return false;
    }
}

bool Fact::maxIsDefaultForType() const
{
    if (m_metaData) {
        return m_metaData->maxIsDefaultForType();
    } else {
        qWarning() << kMissingMetadata << name();
        return false;
    }
}

int Fact::decimalPlaces() const
{
    if (m_metaData) {
        return m_metaData->decimalPlaces();
    } else {
        qWarning() << kMissingMetadata << name();
        return FactMetaData::kDefaultDecimalPlaces;
    }
}

QString Fact::category() const
{
    if (m_metaData) {
        return m_metaData->category();
    } else {
        qWarning() << kMissingMetadata << name();
        return QString();
    }
}

QString Fact::group() const
{
    if (m_metaData) {
        return m_metaData->group();
    } else {
        qWarning() << kMissingMetadata << name();
        return QString();
    }
}

void Fact::setMetaData(FactMetaData *metaData, bool setDefaultFromMetaData)
{
    m_metaData = metaData;
    if (setDefaultFromMetaData && metaData->defaultValueAvailable()) {
        setRawValue(rawDefaultValue());
    }
    emit valueChanged(cookedValue());
}

bool Fact::valueEqualsDefault() const
{
    if (m_metaData) {
        if (m_metaData->defaultValueAvailable()) {
            return m_metaData->rawDefaultValue() == rawValue();
        } else {
            return false;
        }
    } else {
        qWarning() << kMissingMetadata << name();
        return false;
    }
}

bool Fact::defaultValueAvailable() const
{
    if (m_metaData) {
        return m_metaData->defaultValueAvailable();
    } else {
        qWarning() << kMissingMetadata << name();
        return false;
    }
}

QString Fact::validate(const QString &cookedValue, bool convertOnly)
{
    if (m_metaData) {
        QVariant typedValue;
        QString errorString;

        m_metaData->convertAndValidateCooked(cookedValue, convertOnly, typedValue, errorString);

        return errorString;
    } else {
        qWarning() << kMissingMetadata << name();
        return QStringLiteral("Internal error: Meta data pointer missing");
    }
}

QVariant Fact::clamp(const QString &cookedValue)
{
    if (m_metaData) {
        QVariant typedValue;
        if (m_metaData->clampValue(cookedValue, typedValue)) {
            return typedValue;
        } else {
            //-- If conversion failed, return current value
            return rawValue();
        }
    } else {
        qWarning() << kMissingMetadata << name();
    }
    return QVariant();
}

bool Fact::vehicleRebootRequired() const
{
    if (m_metaData) {
        return m_metaData->vehicleRebootRequired();
    } else {
        qWarning() << kMissingMetadata << name();
        return false;
    }
}

bool Fact::quavRebootRequired() const
{
    if (m_metaData) {
        return m_metaData->quavRebootRequired();
    } else {
        qWarning() << kMissingMetadata << name();
        return false;
    }
}

void Fact::setSendValueChangedSignals(bool sendValueChangedSignals)
{
    if (sendValueChangedSignals != m_sendValueChangedSignals) {
        m_sendValueChangedSignals = sendValueChangedSignals;
        emit sendValueChangedSignalsChanged(m_sendValueChangedSignals);
    }
}

void Fact::m_sendValueChangedSignal(const QVariant &value)
{
    if (m_sendValueChangedSignals) {
        emit valueChanged(value);
        m_deferredValueChangeSignal = false;
    } else {
        m_deferredValueChangeSignal = true;
    }
}

void Fact::sendDeferredValueChangedSignal()
{
    if (m_deferredValueChangeSignal) {
        m_deferredValueChangeSignal = false;
        emit valueChanged(cookedValue());
    }
}

QString Fact::enumOrValueString()
{
    if (m_metaData) {
        if (m_metaData->enumStrings().count()) {
            return enumStringValue();
        } else {
            return cookedValueString();
        }
    } else {
        qWarning() << kMissingMetadata << name();
    }
    return QString();
}

double Fact::rawIncrement() const
{
    if (m_metaData) {
        return m_metaData->rawIncrement();
    } else {
        qWarning() << kMissingMetadata << name();
    }
    return std::numeric_limits<double>::quiet_NaN();
}

double Fact::cookedIncrement() const
{
    if (m_metaData) {
        return m_metaData->cookedIncrement();
    } else {
        qWarning() << kMissingMetadata << name();
    }
    return std::numeric_limits<double>::quiet_NaN();
}

bool Fact::hasControl() const
{
    if (m_metaData) {
        return m_metaData->hasControl();
    } else {
        qWarning() << kMissingMetadata << name();
        return false;
    }
}

bool Fact::readOnly() const
{
    if (m_metaData) {
        return m_metaData->readOnly();
    } else {
        qWarning() << kMissingMetadata << name();
        return false;
    }
}

bool Fact::writeOnly() const
{
    if (m_metaData) {
        return m_metaData->writeOnly();
    } else {
        qWarning() << kMissingMetadata << name();
        return false;
    }
}

bool Fact::volatileValue() const
{
    if (m_metaData) {
        return m_metaData->volatileValue();
    } else {
        qWarning() << kMissingMetadata << name();
        return false;
    }
}

void Fact::m_checkForRebootMessaging()
{
    if (quavApp()) {
        if (!quavApp()->runningUnitTests()) {
            if (vehicleRebootRequired()) {
                quavApp()->showRebootAppMessage(tr("Reboot vehicle for changes to take effect."));
            } else if (quavRebootRequired()) {
                quavApp()->showRebootAppMessage(tr("Restart application for changes to take effect."));
            }
        }
    }
}
