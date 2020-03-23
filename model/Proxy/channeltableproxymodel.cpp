#include "channeltableproxymodel.h"
#include "../model/channelconfigmodel.h"

ChannelTableProxyModel::ChannelTableProxyModel(QObject *parent) : QAbstractProxyModel(parent)
{
    setParent(parent);
}

ChannelTableProxyModel::~ChannelTableProxyModel()
{

}

QModelIndex	ChannelTableProxyModel::mapFromSource(const QModelIndex & sourceIndex) const
{
    int column = m_sourceColumns.indexOf(sourceIndex.column());
    if (column == -1)
        return QModelIndex();
    else
        return index(sourceIndex.row(), column, QModelIndex());
}

QModelIndex	ChannelTableProxyModel::mapToSource(const QModelIndex & proxyIndex) const
{
    if (proxyIndex.column() < m_sourceColumns.size() && proxyIndex.column() >= 0)
    {
        int column = m_sourceColumns[proxyIndex.column()];
        return sourceModel()->index(proxyIndex.row(), column, QModelIndex());
    }
    else
        return QModelIndex();
}

QModelIndex ChannelTableProxyModel::index(int row, int column, const QModelIndex & parent) const
{
    if (column >= 0 && column < m_sourceColumns.size())
    {
        if (!hasIndex(row, column, parent))
            return QModelIndex();

        channelItem *parentItem = 0;

        if (parent.isValid())
            parentItem = static_cast<channelItem*>(parent.internalPointer());
        else
            parentItem = ((ChannelConfigModel *)sourceModel())->getRoot();

        if (parentItem != 0)
        {
            channelItem *childItem = parentItem->child(row);
            if (childItem)            
                return createIndex(row, column, childItem);            
            else
                return QModelIndex();
        }        
    }

    return QModelIndex();
}

QModelIndex ChannelTableProxyModel::parent(const QModelIndex & index) const
{   
    if (!index.isValid())
        return QModelIndex();

    channelItem *childItem = static_cast<channelItem*>(index.internalPointer());
    channelItem *parentItem = childItem->parentItem();

    if (parentItem == NULL || ((ChannelConfigModel *)sourceModel())->getRoot() == parentItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);

}

int ChannelTableProxyModel::columnCount(const QModelIndex & parent) const
{
    return m_sourceColumns.size();
}

int ChannelTableProxyModel::rowCount(const QModelIndex & parent) const
{
    return sourceModel()->rowCount(mapToSource(parent));
}


QVariant ChannelTableProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section >= 0)
    {
        if (section < m_sourceColumns.size() && section >= 0)        
            return sourceModel()->headerData(m_sourceColumns[section], orientation, role);
        else
            return QVariant("*");
    }
    else
        return QVariant("*");
}

void ChannelTableProxyModel::setChannelTable()
{
    // let the views know we're doing this
    beginResetModel();

    // wipe whatever is there first
    clearColumns();

    // Figure out which columns to display
    for (int i(0); i < sourceModel()->columnCount(); ++i)
    {
        if (((ChannelConfigModel *)sourceModel())->userHasAccess(i))
            m_sourceColumns.append(i);
    }
    // all done. let the views know everything changed
    endResetModel();
}


QVariant ChannelTableProxyModel::data(const QModelIndex & proxyIndex, int role) const
{
    if (!proxyIndex.isValid()) return QVariant();

    QVariant returning;
    if (proxyIndex.column() >= 0) 
    {
        if (role == Qt::DisplayRole)
            role = ChannelTableRole;

        returning = sourceModel()->data(mapToSource(proxyIndex), role);
    }
    return returning;
}

void ChannelTableProxyModel::clearColumns()
{
    m_sourceColumns.clear();
}


void ChannelTableProxyModel::slotDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    emit QAbstractItemModel::dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight), roles);
}