#include "QUAVOptions.h"

QUAVFlyViewOptions::QUAVFlyViewOptions(QUAVOptions *options, QObject *parent)
    : QObject(parent)
    , m_options(options)
{

}

QUAVFlyViewOptions::~QUAVFlyViewOptions()
{

}

QUAVOptions::QUAVOptions(QObject *parent)
    : QObject(parent)
    , m_defaultFlyViewOptions(new QUAVFlyViewOptions(this))
{
}

QUAVOptions::~QUAVOptions()
{

}