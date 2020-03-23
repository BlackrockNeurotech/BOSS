#ifndef CHANNELTABLEVIEW_H
#define CHANNELTABLEVIEW_H

#include <QTableView>
#include <QHeaderView>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include "../model/Proxy/channeltableproxymodel.h"
#include "../model/channelconfigmodel.h"
#include "../model/groupdelegate.h"

class ChannelTableView : public QTableView
{
    Q_OBJECT
public:
    explicit ChannelTableView(QWidget *parent = 0);
    ~ChannelTableView();

	QSize sizeHint() const { return this->size(); }
    void setFirstChannelWithData(int nChannel) { m_firstChannelWithData = nChannel; }

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

public slots:
    void slotSelectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection);
    void slotChannelStateChanged(int channel, ChannelState state);
    void slotChannelProgressChanged(int channel, int column, int percent);
    void slotCancelProgress(int channel);
    void slotClearSelection();
    void slotGroupChannels();
    void slotGroupSelected(int group);

signals:
    void rowChanged(int row);

private:
    ChannelTableProxyModel  *m_proxy;
    
    bool m_bInitialized;
    bool m_bMouseDown;
    int  m_nGroupSize;
    GroupDelegate m_groupDelegate;
    int m_firstChannelWithData;

    void selectRow(int row);
    QList<int> getColumnSelection(int column, QModelIndexList &modelIndexList);
};

#endif // CHANNELTABLEVIEW_H
