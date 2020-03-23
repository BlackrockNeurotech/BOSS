/////////////////////////////////////////////////////////////////
///
/// @file channelItem.cpp
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  April 29, 2015
///
/// @brief channelItem is the base class for a Channel Model tree item.
///
/// @note
/// This class should be subclassed when creating a new Channel Model tree item type.
/// It is not abstract because it is used directly for the root item which contains no data.
/// It includes the basic functionallity that Channel Model tree items must implement.
/// It is used for traversing the Channel Model tree.
///

#include "../model/channelItem.h"

/// @note
/// Following are basic methods used for traversing the tree

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Constructor for channelItem class
/// @param [in]  parent tree item
channelItem::channelItem(channelItem *parent)
{
    m_parentItem = parent;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Destructor for channelItem class - deletes all child items
channelItem::~channelItem()
{
    qDeleteAll(m_childItems);
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  append a child to this tree item
/// @param [in]  child tree item
void channelItem::appendChild(channelItem *item)
{
    m_childItems.append(item);
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  get the child item corresponding to the row
/// @param [in]  row in this tree item
/// @return child item
channelItem *channelItem::child(int row)
{
    return m_childItems.value(row);
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  remove the child item corresponding to the row
/// @param [in] row in this tree item
void channelItem::removeChild(int row)
{
    m_childItems.removeAt(row);
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  remove all children
void channelItem::removeChildren()
{
    qDeleteAll(m_childItems);
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  get the number of child items belonging to this tree item
/// @return child item count
int channelItem::childCount() const
{
    return m_childItems.count();
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  get the parent of this tree item
/// @return parent
channelItem *channelItem::parentItem()
{
    return m_parentItem;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  get the row this item corresponds to in it's parent
/// @return row
int channelItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<channelItem*>(this));

    return 0;
}

/// @note
/// Following are utility methods used for saving QVariant values
/// They return true if the value changed. A signal is only emitted when the value is changed.

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Transform a qint64 into a QVariant containing a string or a LongLong depending on the role.
/// @param [in] qint64 value
/// @param [in] role
/// @return QVariant containing a string or a LongLong depending on the role
QVariant channelItem::getInt64(qint64 nValue, int role) const
{
    if (role == Qt::UserRole)
        return QVariant(nValue);

    return QVariant(QString::number(nValue));
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Save a qint64 value
/// @param [in]  address of the qint64 class member where the value will be saved, value in the form of a QVariant
/// @return TRUE if the value changed; FALSE otherwise
bool channelItem::setInt64(qint64 *member, const QVariant &value)
{
    qint64 v = value.toLongLong();

    if (*member != v)
    {
        *member = v;
        return true;
    }
    return false;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Transform a quint64 into a QVariant containing a string or a ULongLong depending on the role.
/// @param [in] quint64 value
/// @param [in] role
/// @return QVariant containing a string or a ULongLong depending on the role
QVariant channelItem::getUInt64(quint64 nValue, int role) const
{
    if (role == Qt::UserRole)
        return QVariant(nValue);

    return QVariant(QString::number(nValue));
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Save a quint64 value
/// @param [in]  address of the quint64 class member where the value will be saved, value in the form of a QVariant
/// @return TRUE if the value changed; FALSE otherwise
bool channelItem::setUInt64(quint64 *member, const QVariant &value)
{
    quint64 v = value.toULongLong();

    if (*member != v)
    {
        *member = v;
        return true;
    }
    return false;
}
