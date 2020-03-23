#include <QFontMetrics>
#include <QApplication>

#include "channeltableview.h"
#include "../model/channelconfigmodel.h"
#include "ProgressButton.h"
#include "../model/configmodel.h"
#include "../backend/file/fileinterface.h"
#include "../widgets/GroupButton.h"


ChannelTableView::ChannelTableView(QWidget *parent) :
    QTableView(parent)
{
    m_proxy = new ChannelTableProxyModel();
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    m_proxy->setSourceModel(ccm);    
    setModel(m_proxy);
    setMouseTracking(true);
    m_bInitialized = false;
    m_bMouseDown = false;
    m_nGroupSize = 1;
    m_firstChannelWithData = 0;

    this->verticalHeader()->hide();
    
    QPalette* newPalette = new QPalette();
    const QPalette palette = this->palette();
    newPalette->setColor(QPalette::Highlight, ConfigModel::ColorHighlight);
    newPalette->setColor(QPalette::HighlightedText, Qt::black);
    setPalette(*newPalette);

//    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    
    // item delegate draws group selection, and allows single selection within a group
    setItemDelegate(&m_groupDelegate);
    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(this->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this, SLOT(slotSelectionChanged(const QItemSelection&, const QItemSelection&)));

    connect(ccm, SIGNAL(clearSelection()), this, SLOT(slotClearSelection()));
    connect(ccm, SIGNAL(channelStateChanged(int, ChannelState)), this, SLOT(slotChannelStateChanged(int, ChannelState)));
    connect(ccm, SIGNAL(channelProgressChanged(int, int, int)), this, SLOT(slotChannelProgressChanged(int, int, int)));

    connect(ccm, SIGNAL(groupChannels()), this, SLOT(slotGroupChannels()));

//    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);
}

ChannelTableView::~ChannelTableView()
{
    if (m_proxy)
        delete m_proxy;
}

void ChannelTableView::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);

    if (m_bInitialized == false)
    {
        m_bInitialized = true;
        resizeColumnsToContents();
        resizeRowsToContents();

        // set the width of the selection column or it is too wide
        QSize size = ChannelData::m_selectPixmap->size();
        setColumnWidth(CC_STATE, size.width() + 6);

        // select row 0, this starts the backend processing
        ChannelConfigModel *ccm = ChannelConfigModel::instance();
        ccm->slotRowChanged(m_firstChannelWithData);

        // initialize the selection model with row 0 selected
        QModelIndex index = m_proxy->index(0, CC_STATE);
        QItemSelection selection(index, index);
        this->selectionModel()->clearSelection();
        this->selectionModel()->select(selection, QItemSelectionModel::Select);

        // set the extraction progress in the name column
        for (int channel = 0; channel < ccm->rowCount(); channel++)
        {
            QString name = ccm->data(m_proxy->index(channel, CC_NAME), Qt::DisplayRole).toString();
            ProgressButton *b = new ProgressButton(this, channel, name, false);
            
            if (ccm->data(ccm->index(channel, CC_CDFILE_INIT), Qt::UserRole).toBool())
            {
                b->setProgress(100);
            }
            this->setIndexWidget(m_proxy->index(channel, CC_NAME), b);
        }
        // sets the row span in QTableView by group size
        slotGroupChannels();
    }
}

void ChannelTableView::mousePressEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton) && (Qt::ShiftModifier != QApplication::keyboardModifiers()) && (columnAt(event->x()) == CC_STATE))
    { 
        // reselect the current channel so the selection model doesn't get confused
        ChannelConfigModel *ccm = ChannelConfigModel::instance();
        QModelIndex index = m_proxy->index(ccm->CurrentChannel(), CC_STATE);
        this->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);

        //if ((Qt::ControlModifier != QApplication::keyboardModifiers()))
        //    this->selectionModel()->clearSelection();

        m_bMouseDown = true;
        int row = rowAt(event->y());
        ccm->slotRowChanged(row);
    }
    int column = columnAt(event->x());
    QTableView::mousePressEvent(event);
}

void ChannelTableView::mouseMoveEvent(QMouseEvent *event)
{
    QTableView::mouseMoveEvent(event);
}

void ChannelTableView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_bMouseDown)
    {
        m_bMouseDown = false;
    }
    // if the ChannelTable has focus on mouse release, and the mouse hovers over the progress widget, the selection is inadvertently changed
    this->parentWidget()->setFocus();
    QTableView::mouseReleaseEvent(event);
}

void ChannelTableView::keyPressEvent(QKeyEvent *event)
{
    if (!this->hasFocus())
        return;
    if (!this->isVisible())
        return;

    int newCurrentRow = 0;
    bool bUpdateCurrent = false;
}


void ChannelTableView::slotChannelStateChanged(int channel, ChannelState state)
{
    QModelIndex indexDetect = m_proxy->index(channel, CC_NWAVEFORMS);
//    QModelIndex indexSort = m_proxy->index(channel, CC_NUNITS);

    if (state == CHANNELSTATE_DETECTING)
    {
        ChannelConfigModel *ccm = ChannelConfigModel::instance();
        QString sText = QString("Extracting...");
        ProgressButton *b = new ProgressButton(this, channel, sText);
        connect(b, SIGNAL(cancelProgress(int)), this, SLOT(slotCancelProgress(int)));

        if (!ccm->data(ccm->index(channel, CC_CDFILE_INIT), Qt::UserRole).toBool())
        {
            b->showText(true);
        }
        this->setIndexWidget(indexDetect, b);
    }
    else
    {
        this->setIndexWidget(indexDetect, 0);
    }
    update();
}

void ChannelTableView::slotChannelProgressChanged(int channel, int column, int percent)
{
    QModelIndex index = m_proxy->index(channel, column);
    ProgressButton *b = (ProgressButton *)indexWidget(index);

    if (b)
    {
        b->setProgress(percent);
    }
}

void ChannelTableView::slotGroupChannels()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int nGroupSize = ccm->rootData(RC_GROUP_SIZE, Qt::UserRole).toInt();
    int nGroups = ccm->rowCount() / nGroupSize;
       
    for (int i = 0; i < nGroups; i++)
    {
        int firstchannel = i * nGroupSize;
        int lastchannel = (i + 1) * nGroupSize;
        // setSpan is a QTableView function that groups rows/columns
        setSpan(firstchannel, CC_STATE, nGroupSize, 1);
    }
    m_nGroupSize = nGroupSize;
}

void ChannelTableView::slotGroupSelected(int group)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int nGroups = ccm->rowCount() / m_nGroupSize;
}

/// @author Holly Ausbeck
/// @date   May 14, 2015
/// @brief  process a channel selection change
///         this slot is called from the channel list control when the selection changes
/// @param [in] newSelection - list of selected channels
/// @param [in] noldSelection - list of last selected channels
void ChannelTableView::slotSelectionChanged(const QItemSelection &newItemSelection, const QItemSelection &oldItemSelection)
{
    QModelIndexList modelIndexListNew = newItemSelection.indexes();
    QModelIndexList modelIndexListOld = oldItemSelection.indexes();
    QList<int> newSelection = getColumnSelection(CC_STATE, modelIndexListNew);
    QList<int> oldSelection = getColumnSelection(CC_STATE, modelIndexListOld);

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    ccm->slotSelectionChanged(newSelection, oldSelection);

    // reselect the current channel so the selection model doesn't get confused
    QModelIndex index = m_proxy->index(ccm->CurrentChannel(), CC_STATE);
    this->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Current);
}

QList<int> ChannelTableView::getColumnSelection(int column, QModelIndexList &modelIndexList)
{
    QList<int> columnSelection;

    foreach(QModelIndex modelIndex, modelIndexList)
    {
        if (modelIndex.column() == column)
            columnSelection.append(modelIndex.row());
    }
    return columnSelection;
}

void ChannelTableView::slotClearSelection()
{
    clearSelection();
    // reselect the current channel so the selection model doesn't get confused
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    QModelIndex index = m_proxy->index(ccm->CurrentChannel(), CC_STATE);
    this->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Current);
    update();
}

void ChannelTableView::slotCancelProgress(int channel)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    ccm->cancelDetect(channel);
}

