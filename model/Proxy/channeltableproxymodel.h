#ifndef CHANNELTABLEPROXYMODEL_H
#define CHANNELTABLEPROXYMODEL_H

#include <QAbstractProxyModel>

class ChannelTableProxyModel : public QAbstractProxyModel
{
    Q_OBJECT

public:
    ChannelTableProxyModel(QObject *parent = NULL);
    
    ~ChannelTableProxyModel();

    /* Required implementation to avoid abstraction*/
    virtual QModelIndex	mapFromSource(const QModelIndex & sourceIndex) const;
    virtual QModelIndex	mapToSource(const QModelIndex & proxyIndex) const; 
    virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex & index) const;
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void setChannelTable();

    void setSourceModel(QAbstractItemModel *model)
    {
        QAbstractProxyModel::setSourceModel(model);
        connect(model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this, SLOT(slotDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));
        setChannelTable();
    }

private: 
    void clearColumns();
    QVector<int> m_sourceColumns;

public slots:
    void slotDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
};

#endif // CHANNELTABLEPROXYMODEL_H
