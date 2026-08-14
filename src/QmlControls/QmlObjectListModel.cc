#include "QmlObjectListModel.h"

#include <QtCore/QDebug>
#include <QtQml/QQmlEngine>

QmlObjectListModel::QmlObjectListModel(QObject* parent)
    : QAbstractListModel        (parent)
    , m_dirty                    (false)
    , m_skipDirtyFirstItem       (false)
{

}

QmlObjectListModel::~QmlObjectListModel()
{
    if (m_resetModelNestingCount > 0) {
//        qWarning() << "QmlObjectListModel destroyed with unbalanced nesting of begin/endResetModel calls - _resetModelNestingCount:" << _resetModelNestingCount << this;
    }
}

QObject* QmlObjectListModel::get(int index)
{
    if (index < 0 || index >= m_objectList.count()) {
        return nullptr;
    }
    return m_objectList[index];
}

int QmlObjectListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    
    return m_objectList.count();
}

QVariant QmlObjectListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    
    if (index.row() < 0 || index.row() >= m_objectList.count()) {
        return QVariant();
    }
    
    if (role == ObjectRole) {
        return QVariant::fromValue(m_objectList[index.row()]);
    } else if (role == TextRole) {
        return QVariant::fromValue(m_objectList[index.row()]->objectName());
    } else {
        return QVariant();
    }
}

QHash<int, QByteArray> QmlObjectListModel::roleNames(void) const
{
    QHash<int, QByteArray> hash;
    
    hash[ObjectRole] = "object";
    hash[TextRole] = "text";
    
    return hash;
}

bool QmlObjectListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid() && role == ObjectRole) {
        m_objectList.replace(index.row(), value.value<QObject*>());
        emit dataChanged(index, index);
        return true;
    }
    
    return false;
}

bool QmlObjectListModel::insertRows(int position, int rows, const QModelIndex& parent)
{
    Q_UNUSED(parent);
    
    if (position < 0 || position > m_objectList.count() + 1) {
    //    qWarning() << "Invalid position - position:count" << position << m_objectList.count() << this;
    }
    
    beginInsertRows(QModelIndex(), position, position + rows - 1);
    endInsertRows();
    
    emit countChanged(count());
    
    return true;
}

bool QmlObjectListModel::removeRows(int position, int rows, const QModelIndex& parent)
{
    Q_UNUSED(parent);
    
    if (position < 0 || position >= m_objectList.count()) {
    //    qWarning() << "Invalid position - position:count" << position << _objectList.count() << this;
    } else if (position + rows > m_objectList.count()) {
    //    qWarning() << "Invalid rows - position:rows:count" << position << rows << _objectList.count() << this;
    }
    
    beginRemoveRows(QModelIndex(), position, position + rows - 1);
    for (int row=0; row<rows; row++) {
        m_objectList.removeAt(position);
    }
    endRemoveRows();
    
    emit countChanged(count());
    
    return true;
}

void QmlObjectListModel::move(int from, int to)
{
    if(0 <= from && from < count() && 0 <= to && to < count() && from != to) {
        // Workaround to allow move item to the bottom. Done according to
        // beginMoveRows() documentation and implementation specificity:
        // https://doc.qt.io/qt-5/qabstractitemmodel.html#beginMoveRows
        // (see 3rd picture explanation there)
        if(from == to - 1) {
            to = from++;
        }
        beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
        m_objectList.move(from, to);
        endMoveRows();
    }
}

QObject* QmlObjectListModel::operator[](int index)
{
    if (index < 0 || index >= m_objectList.count()) {
        return nullptr;
    }
    return m_objectList[index];
}

const QObject* QmlObjectListModel::operator[](int index) const
{
    if (index < 0 || index >= m_objectList.count()) {
        return nullptr;
    }
    return m_objectList[index];
}

void QmlObjectListModel::clear()
{
    beginResetModel();
    m_objectList.clear();
    endResetModel();
}

QObject* QmlObjectListModel::removeAt(int i)
{
    QObject* removedObject = m_objectList[i];
    if(removedObject) {
        // Look for a dirtyChanged signal on the object
        if (m_objectList[i]->metaObject()->indexOfSignal(QMetaObject::normalizedSignature("dirtyChanged(bool)").constData()) != -1) {
            if (!m_skipDirtyFirstItem || i != 0) {
                QObject::disconnect(m_objectList[i], SIGNAL(dirtyChanged(bool)), this, SLOT(m_childDirtyChanged(bool)));
            }
        }
    }
    removeRows(i, 1);
    setDirty(true);
    return removedObject;
}

void QmlObjectListModel::insert(int i, QObject* object)
{
    if (i < 0 || i > m_objectList.count()) {
    //    qWarning() << "Invalid index - index:count" << i << _objectList.count() << this;
    }
    if(object) {
        QQmlEngine::setObjectOwnership(object, QQmlEngine::CppOwnership);
        // Look for a dirtyChanged signal on the object
        if (object->metaObject()->indexOfSignal(QMetaObject::normalizedSignature("dirtyChanged(bool)").constData()) != -1) {
            if (!m_skipDirtyFirstItem || i != 0) {
                QObject::connect(object, SIGNAL(dirtyChanged(bool)), this, SLOT(m_childDirtyChanged(bool)));
            }
        }
    }
    m_objectList.insert(i, object);
    insertRows(i, 1);
    setDirty(true);
}

void QmlObjectListModel::insert(int i, QList<QObject*> objects)
{
    if (i < 0 || i > m_objectList.count()) {
    //    qWarning() << "Invalid index - index:count" << i << _objectList.count() << this;
    }

    int j = i;
    for (QObject* object: objects) {
        QQmlEngine::setObjectOwnership(object, QQmlEngine::CppOwnership);

        // Look for a dirtyChanged signal on the object
        if (object->metaObject()->indexOfSignal(QMetaObject::normalizedSignature("dirtyChanged(bool)").constData()) != -1) {
            if (!m_skipDirtyFirstItem || j != 0) {
                QObject::connect(object, SIGNAL(dirtyChanged(bool)), this, SLOT(m_childDirtyChanged(bool)));
            }
        }

        m_objectList.insert(j, object);
        j++;
    }

    insertRows(i, objects.count());

    setDirty(true);
}

void QmlObjectListModel::append(QObject* object)
{
    insert(m_objectList.count(), object);
}

void QmlObjectListModel::append(QList<QObject*> objects)
{
    insert(m_objectList.count(), objects);
}

QObjectList QmlObjectListModel::swapObjectList(const QObjectList& newlist)
{
    QObjectList oldlist(m_objectList);
    beginResetModel();
    m_objectList = newlist;
    endResetModel();
    return oldlist;
}

int QmlObjectListModel::count() const
{
    return rowCount();
}

void QmlObjectListModel::setDirty(bool dirty)
{
    if (m_dirty != dirty) {
        m_dirty = dirty;
        if (!dirty) {
            // Need to clear dirty from all children
            for(QObject* object: m_objectList) {
                if (object->property("dirty").isValid()) {
                    object->setProperty("dirty", false);
                }
            }
        }
        emit dirtyChanged(m_dirty);
    }
}

void QmlObjectListModel::_childDirtyChanged(bool dirty)
{
    m_dirty |= dirty;
    // We want to emit dirtyChanged even if the actual value of _dirty didn't change. It can be a useful
    // signal to know when a child has changed dirty state
    emit dirtyChanged(m_dirty);
}

void QmlObjectListModel::deleteListAndContents()
{
    for (int i=0; i<m_objectList.count(); i++) {
        m_objectList[i]->deleteLater();
    }
    deleteLater();
}

void QmlObjectListModel::clearAndDeleteContents()
{
    for (int i=0; i<m_objectList.count(); i++) {
        m_objectList[i]->deleteLater();
    }
    clear();
}

void QmlObjectListModel::beginResetModel()
{
    if (m_resetModelNestingCount == 0) {
    //    qDebug() << "First call to begindResetModel - calling QAbstractListModel::beginResetModel" << this;
        QAbstractListModel::beginResetModel();
    }
    m_resetModelNestingCount++;
    //qDebug() << "_resetModelNestingCount:" << _resetModelNestingCount << this;
}

void QmlObjectListModel::endResetModel()
{
    if (m_resetModelNestingCount == 0) {
    //    qWarning() << "QmlObjectListModel::endResetModel called without prior beginResetModel";
        return;
    }
    m_resetModelNestingCount--;
    // qDebug() << "m_resetModelNestingCount:" << m_resetModelNestingCount << this;
    if (m_resetModelNestingCount == 0) {
    //    qDebug() << "Last call to endResetModel - calling QAbstractListModel::endResetModel" << this;
        QAbstractListModel::endResetModel();
        emit countChanged(count());
    }
}
