/////////////////////////////////////////////////////////////////
///
/// @file ChannelConfigModel.cpp
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  April 29, 2015
///
/// @brief ChannelConfigModel contains channel specific information.
///
/// @note
/// ChannelConfigModel subclasses QAbstractItemModel and and is in a tree format.
/// Each node subclasses channelItem.
/// The root top node has ChannelData children; one child for each channel.
/// Each ChannelData child has IntervalData children; one child for each time segment.
/// QAbstractItemModel is a singleton.
///

#include "../model/channelconfigmodel.h"
#include "../model/ChannelData.h"
#include "../model/RootData.h"
#include "../model/configmodel.h"
#include "../backend/file/fileinterface.h"
#include "../model/groupselection.h"

/// @note
/// header string data for each file column in the model
/* static */
QStringList ChannelConfigModel::ColumnNames = QStringList()
/* File */
<< "Pauses nsx";

 
/* static */
ChannelConfigModel *ChannelConfigModel::inst = 0;


/* static */ // These columns are not displayed in the channel table
QVector<ChannelColumn> ChannelConfigModel::internalColumns = QVector<ChannelColumn>()
<< CC_CURRENT_INTERVAL << CC_3D_EDIT_STATE << CC_THRESHOLD1_SET << CC_THRESHOLD2_SET << CC_THRESHOLDE_SET
<< CC_CENTROIDS << CC_UNITS_ALIGN << CC_UNITS_VISIBLE << CC_EDIT_LINES << CC_PAGE
<< CC_EDIT_BOXES << CC_EDIT_MODE << CC_TIMELINE_VISIBLE_START << CC_TIMELINE_VISIBLE_END
<< CC_TIMESPIKE_VISIBLE_START << CC_TIMESPIKE_VISIBLE_END << CC_TIMELINE_VERTICAL_RANGE
<< CC_SE_VERTICAL_RANGE << CC_TIMELINE_START << CC_TIMELINE_END
<< CC_TIMESPIKE_START << CC_TIMESPIKE_END << CC_SPIKE_START << CC_SPIKE_END
<< CC_CDFILE_INIT << CC_SDFILE_INIT << CC_SPIKEDATA_FEATURES << CC_SPIKEDATA_UNITS
<< CC_SPIKEDATA_CLUSTERING_MATRIX << CC_LAST_ERROR << CC_CHANGE_COUNTER << CC_NSX_LABEL 
<< CC_FEATURE << CC_SELECTED_FEATURES << CC_ARTIFACT_SUBTRACTION << CC_DISPLAYED << CC_DETECTED << CC_FILE_SEPARATORS;

/* static */ // These are columns that the user can change through the UI
QVector<ChannelColumn> ChannelConfigModel::userColumns = QVector<ChannelColumn>()
<< CC_PAGE << CC_HIGHPASS_FILTER << CC_FILTER_ORDER << CC_CUTOFF_FREQ << CC_SIGNAL_ENERGY << CC_SE_WIDTH << CC_ARTIFACT_SUBTRACTION
<< CC_THRESHOLD << CC_SYMETRICAL_THRESHOLD << CC_RMS_MULTIPLIER << CC_THRESHOLD1 << CC_THRESHOLD2 << CC_ENERGY_THRESHOLD
<< CC_WF_PRETHRESHOLD << CC_WF_LENGTH << CC_DEAD_TIME
<< CC_FEATURE << CC_SELECTED_FEATURES
<< CC_SORT_METHOD << CC_X_3DAXIS << CC_Y_3DAXIS << CC_Z_3DAXIS << CC_3D_EDIT_STATE << CC_X_2DAXIS << CC_Y_2DAXIS << CC_NBR_SORT_UNITS << CC_PENALTY_FACTOR << CC_CENTROIDS
//<< CC_WAVEFORM_ALIGNMENT << CC_MAX_SHIFT << CC_UNITS_ALIGN << CC_UNITS_VISIBLE << CC_EDIT_LINES << CC_EDIT_BOXES << CC_EDIT_MODE
<< CC_CDFILE_INIT << CC_SDFILE_INIT << CC_SPIKEDATA_FEATURES << CC_SPIKEDATA_UNITS
<< CC_SPIKEDATA_CLUSTERING_MATRIX << CC_LAST_ERROR << CC_CHANGE_COUNTER << CC_NSX_LABEL;


/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  ChannelConfigModel is a singleton. Return the instance or create it if it doesn't exist.
/// @return one and only instance
/* static */
ChannelConfigModel *ChannelConfigModel::instance()
{
    if (!inst)
        inst = new ChannelConfigModel();
    return inst;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Constructor for ChannelConfigModel class - initializes class variables
/// @param [in]  parent object
ChannelConfigModel::ChannelConfigModel(QObject *parent)
    :QAbstractItemModel(parent)
{
    qRegisterMetaType<EditLinesData>("EditLinesData");
    qRegisterMetaType<FeaturesAccum>("FeaturesAccum");
    qRegisterMetaType<SortAccum>("SortAccum");
    qRegisterMetaType<UnitAccum>("UnitAccum");
    qRegisterMetaType<ClusteringTendencyAccum>("ClusteringTendencyAccum");
    qRegisterMetaType<SortMethod>("SortMethod");
    qRegisterMetaType<QVector<quint32> >("QVector<quint32>");
	qRegisterMetaType<QVector<int> >("QVector<int>");
	qRegisterMetaType<ClusterDataPtr>("ClusterDataPtr");

    // continuous data
    qRegisterMetaType<RawDataAccum>("RawDataAccum");
    qRegisterMetaType<HistogramDataAccum>("HistogramDataAccum");
    qRegisterMetaType<EnergyDataAccum>("EnergyDataAccum");
    qRegisterMetaType<FftDataAccum>("FftDataAccum");
    qRegisterMetaType<DisplayDataType>("DisplayDataType");

    // ui data
    qRegisterMetaType<SliderType>("SliderType");
    qRegisterMetaType<UnitOperation>("UnitOperation");
    qRegisterMetaType<ChannelState>("ChannelState");
    qRegisterMetaType<WizardPage>("WizardPage");
    qRegisterMetaType<ChannelColumn>("ChannelColumn");
    qRegisterMetaType<FeatureID>("FeatureID");
    qRegisterMetaType<GLWidgetState>("GLWidgetState");
    qRegisterMetaType<SpikeMoveReference>("SpikeMoveReference");
    qRegisterMetaType<GroupSelection>("GroupSelection");

    m_xmlVersion = "2.0";
    m_rootItem = 0;
    m_CurrentChannel = 0;
    m_bLimitChannelCount = false;

    ChannelData::m_blankPixmap = new QPixmap(":/new/Bitmaps/Bitmaps/ChannelBlank.png");
    ChannelData::m_displayPixmap = new QPixmap(":/new/Bitmaps/Bitmaps/ChannelView.png");
    ChannelData::m_displaySortPixmap = new QPixmap(":/new/Bitmaps/Bitmaps/ChannelViewSort.png");
    ChannelData::m_displayDetectPixmap = new QPixmap(":/new/Bitmaps/Bitmaps/ChannelViewDetect.png");
    ChannelData::m_displayAlignPixmap = new QPixmap(":/new/Bitmaps/Bitmaps/ChannelViewAlign.png");
    ChannelData::m_selectPixmap = new QPixmap(":/new/Bitmaps/Bitmaps/ChannelSelect.png");
    ChannelData::m_detectPixmap = new QPixmap(":/new/Bitmaps/Bitmaps/ChannelDetect.png");
    ChannelData::m_sortPixmap = new QPixmap(":/new/Bitmaps/Bitmaps/ChannelSort.png");
    ChannelData::m_alignPixmap = new QPixmap(":/new/Bitmaps/Bitmaps/ChannelAlign.png");
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Destructor for ChannelConfigModel class - deletes the tree
/// @param [in]  parent object
ChannelConfigModel::~ChannelConfigModel()
{
    delete m_rootItem;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  create the tree with initial data
/// @param [in] number of rows (channels)
/// @param [in] WizardPage = WIZARDPAGE_DETECT(nsx file)/WIZARDPAGE_SORT(nev file)
/// @param [in] sample rate
void ChannelConfigModel::nukeModel()
{
    beginResetModel();
    blockSignals(true);

    if (m_rootItem != 0)
        delete m_rootItem;
    m_rootItem = new RootData();

    m_CurrentChannel = 0;

    blockSignals(false);
    endResetModel();
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  create the tree with initial data
/// @param [in] number of rows (channels)
/// @param [in] WizardPage = WIZARDPAGE_DETECT(nsx file)/WIZARDPAGE_SORT(nev file)
/// @param [in] sample rate
void ChannelConfigModel::initializeModel(int nChannels, int sampleRate, bool bLimitChannelCount)
{
    beginResetModel();
    blockSignals(true);

    m_bLimitChannelCount = bLimitChannelCount;
    m_CurrentChannel = 0;
    int previousRows = rowCount();
    m_pauseWrapper.clear();
    setRootData(RC_TIME_STAMP_RESOLUTION, sampleRate);

    for (int chan = previousRows; chan < nChannels; chan++)
    {
        QString sName("Channel" + QString::number(chan + 1));

        ChannelData *cd = new ChannelData(sName, m_rootItem);
        m_rootItem->appendChild(cd);
        cd->initialize(sampleRate);
    }

    // Set the column header text
    for (int chan = previousRows; chan < CC_END; chan++)
    {
        setHeaderData(chan, Qt::Horizontal, ChannelData::ColumnNames[chan]);
    }        
    blockSignals(false);
    endResetModel();
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Set the beginning and end of the time ranges
void ChannelConfigModel::initializeModelChannel(int nChannel, qint64 first, qint64 last)
{
	ConfigModel *cm = ConfigModel::instance();
	quint64 nSamples = cm->item(0, OFSMODEL_COLUMN_MAX_DISPLAY_SAMPLES)->data(Qt::DisplayRole).toULongLong();

	qint64 end = first + nSamples;
    if (end > last)
        end = last;

    ChannelData *cd = (ChannelData*)m_rootItem->child(nChannel);
    if (cd)
    {
        cd->setData(CC_FIRST_TIME, QVariant(first), Qt::UserRole);
        cd->setData(CC_TIMESPIKE_START, QVariant(first), Qt::UserRole);
        cd->setData(CC_TIMELINE_VISIBLE_START, QVariant(first), Qt::UserRole);
        cd->setData(CC_TIMESPIKE_VISIBLE_START, QVariant(first), Qt::UserRole);
        cd->setData(CC_LAST_TIME, QVariant(last), Qt::UserRole);
        cd->setData(CC_TIMELINE_END, QVariant(end), Qt::UserRole);
        cd->setData(CC_TIMELINE_VISIBLE_END, QVariant(end), Qt::UserRole);
    }
}

/// @author Sylvana Alpert
/// @date   May 14, 2016
/// @brief  add a file separator to the model to indicate where a file ends and a new one begins in the timeline
/// @param [in] nChannel channel number
/// @param [in] the number of timestamp ticks where the separator is located
void ChannelConfigModel::addFileSeparator(int nChannel, quint64 separator_timestamp)
{
    ChannelData *cd = (ChannelData*)m_rootItem->child(nChannel);
    if (cd)
    {
        QStringList separators = cd->data(CC_FILE_SEPARATORS, Qt::UserRole).toStringList();
        separators << QString::number(separator_timestamp);
        cd->setData(CC_FILE_SEPARATORS, separators);
    }
}

/// @author Sylvana Alpert
/// @date   May 14, 2016
/// @brief  add a file separator to the model to indicate where a file ends and a new one begins in the timeline
/// @param [in] nChannel channel number
/// @param [in] the number of timestamp ticks where the separator is located
quint64 ChannelConfigModel::getFileSeparator(int fileIndex)
{
    quint64 separator = 0;

    ChannelData *cd = (ChannelData*)m_rootItem->child(0);
    if (cd)
    {
        QStringList separators = cd->data(CC_FILE_SEPARATORS, Qt::UserRole).toStringList();

        if (separators.size() > fileIndex)
        {
            separator = separators[fileIndex].toULongLong();
        }
    }
    return separator;
}

PauseWrapper ChannelConfigModel::getPauseWrapper()
{
    if (!m_pauseWrapper.initialized())
    {
        initPauseMap();
    }
    return m_pauseWrapper;
}


void ChannelConfigModel::addPauseMap(QMultiMap<quint64, quint64> pauseMap)
{
    if (pauseMap.count())
    {
        m_pauseWrapper.clear();
        RootData *ri = (RootData*)m_rootItem;
        ri->addPauseMap(pauseMap);
    }
}


void ChannelConfigModel::setRootData(int column, const QVariant &value, int role)
{
    m_rootItem->setData(column, value, role);
}

QVariant ChannelConfigModel::rootData(int column, int role)
{
    return m_rootItem->data(column, role);
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  get the number of columns belonging to the tree item specified by the model index
/// @param [in] model index specifying the tree item
/// @return column count
int ChannelConfigModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<channelItem*>(parent.internalPointer())->columnCount();
    else
        return CC_END;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  return the QVariant value of the data at the requested model index for the specified role
/// @param [in] QModelIndex of the tree item
/// @param [in] role
/// @return QVariant containing the value
QVariant ChannelConfigModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    ChannelColumn column = (ChannelColumn)index.column();
    QVariant compositeValue = QVariant();

    channelItem *indexitem = m_rootItem->child(index.row());
    ChannelState indexstate = (ChannelState)indexitem->data(CC_STATE, Qt::UserRole).toInt();

    // process this here for grouping
    if ((column == CC_STATE) && (role == ChannelTableRole))
    {
        int nGroupSize = m_rootItem->data(RC_GROUP_SIZE, Qt::UserRole).toInt();

        int row = index.row();
        ChannelData *item = (ChannelData *)m_rootItem->child(index.row());
        int groupNumber = index.row() / nGroupSize;
        QString sText = QString::number(groupNumber + 1);

        return QVariant(QVariant::fromValue(item->getGroupSelection(nGroupSize, sText)));
    }
    // the selected features are a bitmask, so process the multiple select seperately
    if ((column == CC_FEATURE) && (role != Qt::UserRole) && (role != ChannelTableRole))
    {
        bool initialSet = false;
        uint andMask = 0;
        uint orMask = 0;

        for (int i = 0; i < m_rootItem->childCount(); i++)
        {
            channelItem *item = m_rootItem->child(i);
            ChannelState state = (ChannelState)item->data(CC_STATE, Qt::UserRole).toInt();

            if (state == CHANNELSTATE_SELECTED)
            {
                QVariant value = item->data(column, role);

                if (initialSet == false)
                {
                    andMask = value.toUInt();
                    orMask = value.toUInt();
                    compositeValue = value;
                    initialSet = true;
                }
                if (compositeValue != value)
                {
                    andMask &= value.toUInt();
                    orMask |= value.toUInt();
                }
            }
        }
        QList<QVariant> features;
        features.push_back(QVariant(andMask));
        features.push_back(QVariant(orMask));
        compositeValue = QVariant(features);
    }
    // if this is a column the user changes, and it's one of the UI roles but not the ChannelTable Role, and this channel is selected
    else if (userColumns.contains(column) && (role != Qt::UserRole) && (role != ChannelTableRole) && (indexstate == CHANNELSTATE_SELECTED))
    {
        bool initialSet = false;

        for (int i = 0; i < m_rootItem->childCount(); i++)
        {
            channelItem *item = m_rootItem->child(i);
            ChannelState state = (ChannelState)item->data(CC_STATE, Qt::UserRole).toInt();

            if (state == CHANNELSTATE_SELECTED)
            {
                QVariant value = item->data(column, role);

                if (initialSet == false)
                {
                    compositeValue = value;
                    initialSet = true;
                }
                if (compositeValue != value)
                {
                    switch (role)
                    {
                    case Qt::EditRole:
                    case Qt::DisplayRole:
                         compositeValue = QVariant(QString(""));
                        break;
                    case Qt::CheckStateRole:
                        compositeValue = QVariant(Qt::PartiallyChecked);
                        break;
                    }
                    break;
                }
            }
        }
    }
    else
    {
        channelItem *item = static_cast<channelItem*>(index.internalPointer());

        if (role == ChannelTableRole)
            compositeValue = item->data(index.column(), Qt::DisplayRole);
        else
            compositeValue = item->data(index.column(), role);
    }
    return compositeValue;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  set the value of the data at the requested model index and emit appropriate signals
/// @param [in] QModelIndex of the tree item
/// @param [in] value in the form of a QVariant
/// @param [in] role
/// @return TRUE if the value changed; FALSE otherwise
bool ChannelConfigModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    bool changed = false;
    ChannelColumn column = (ChannelColumn)index.column();

    if ((column == CC_FEATURE) && (role != Qt::UserRole))
    {
        QList<QVariant> list = value.toList();
        
        if (list.size() == 2)
        {
            uint andMask = list.at(0).toUInt();
            uint orMask = list.at(1).toUInt();

            for (int i = 0; i < m_rootItem->childCount(); i++)
            {
                if (i != CurrentChannel())
                {
                    channelItem *item = m_rootItem->child(i);
                    QVariant v = item->data(column, role);
                    uint current = v.toUInt();
                    current |= andMask;
                    current &= orMask;
                    changed |= setChannelData(i, column, QVariant(current), role);
                }
            }
            // change the current channel last so all the data is set before the signals are sent
            changed |= setChannelData(CurrentChannel(), column, QVariant(andMask), role);
        }
    }
    // process this here for grouping
    if ((column == CC_STATE) && (role == ChannelTableRole))
    {
        int nGroupSize = m_rootItem->data(RC_GROUP_SIZE, Qt::UserRole).toInt();

        int row = index.row();
        ChannelData *item = (ChannelData *)m_rootItem->child(index.row());
        int groupNumber = index.row() / nGroupSize;
        QString sText = QString::number(groupNumber);
        item->setGroupSelection(qvariant_cast<GroupSelection>(value));
    }
    else if (userColumns.contains(column) && (role != Qt::UserRole))
    {
        for (int i = 0; i < m_rootItem->childCount(); i++)
        {
            if (i != CurrentChannel())
                changed |= setChannelData(i, column, value, role);
        }
        // change the current channel last so all the data is set before the signals are sent
        changed |= setChannelData(CurrentChannel(), column, value, role);
    }
    else
    {
        channelItem *item = static_cast<channelItem*>(index.internalPointer());
        changed |= item->setData(index.column(), value, role);
    }
    if (index.column() == 0)
        changed = false;

    if (changed)
    {
        QModelIndex firstIndex = this->index(0, index.column());
        QModelIndex lastIndex = this->index(rowCount() - 1, index.column());
        emit QAbstractItemModel::dataChanged(firstIndex, lastIndex, QVector<int>(1, Qt::DisplayRole));
    }
    return true;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  set the value of the data at the requested model index and emit appropriate signals
/// @param [in] QModelIndex of the tree item
/// @param [in] value in the form of a QVariant
/// @param [in] role
/// @return TRUE if the value changed; FALSE otherwise
bool ChannelConfigModel::setChannelData(int row, int column, const QVariant &value, int role)
{
    channelItem *item = m_rootItem->child(row);
    ChannelState state = (ChannelState)item->data(CC_STATE, Qt::UserRole).toInt();
    bool changed = false;

    if (state == CHANNELSTATE_SELECTED)
    {
        switch (role)
        {
        case Qt::EditRole:
        case Qt::DisplayRole:
            if (!value.toString().isEmpty())
                changed = item->setData(column, value, role);
            break;
        case Qt::CheckStateRole:
            if ((Qt::CheckState)value.toInt() != Qt::PartiallyChecked)
                changed = item->setData(column, value, role);
            break;
        default:
            changed = item->setData(column, value, role);
            break;
        }
    }
    return changed;
}

/// @author Holly Ausbeck
/// @date   May 4, 2015
/// @brief  Emits the specified signal
/// @param [in] channel
/// @param [in] signal to emit
/// @param [in] value1 - depends on signal
/// @param [in] value2 - depends on signal
void ChannelConfigModel::emitSignal(int channel, ChannelModelSignals signal, QVariant value1, QVariant value2, QVariant value3)
{
    QModelIndex index;

    switch (signal)
    {
    case SIGNAL_DETECT:
        emit detectWaveforms(channel);
        break;
    case SIGNAL_SORT:
        emit sortWaveforms(channel);
        break;
    case SIGNAL_STATE_CHANGED:
        emit channelStateChanged(channel, (ChannelState)value1.toUInt());
        break;
    case SIGNAL_CLEAR_SELECTION:
        emit clearSelection();
        break;
    case SIGNAL_CHANNEL_SELECTION_CHANGED:
        emit channelSelectionChanged();
        break;
    }
    if (channel == CurrentChannel())
    {
        switch (signal)
        {
        case SIGNAL_SLIDER_CHANGED:
            emit sliderChanged(channel, (SliderType)value1.toUInt());
            break;
        case SIGNAL_THRESHOLD_TYPE_CHANGED:
            emit thresholdTypeChanged(channel);
            break;
        case SIGNAL_ENERGY_TYPE_CHANGED:
            emit signalEnergyTypeChanged(channel);
            break;

        case SIGNAL_WIZARDPAGE_CHANGED:
            emit wizardPageChanged(channel, (WizardPage)value1.toUInt());
            break;
        case SIGNAL_SORT_FEATURES_CHANGED:
            emit sortFeaturesChanged(channel);
            break;
        case SIGNAL_VIEW_FEATURES_CHANGED:
            emit viewFeaturesChanged(channel);
            break;
        case SIGNAL_VISIBLE_UNITS_CHANGED:
            emit visibleUnitsChanged(channel, value1.toStringList());
            break;
        case SIGNAL_2D_AXIS_CHANGED:
            emit twoDAxisChanged(channel, (ChannelColumn)value1.toUInt(), (FeatureID)value2.toUInt());
            break;
        case SIGNAL_3D_AXIS_CHANGED:
            emit threeDAxisChanged(channel, (ChannelColumn)value1.toUInt(), (FeatureID)value2.toUInt());
            break;
        case SIGNAL_3D_EDIT_STATE_CHANGED:
            emit glEditStateChanged(channel, (GLWidgetState)value1.toUInt());
            break;
        case SIGNAL_DATA_CHANGED:
            index = this->index(channel, value1.toUInt());
            emit QAbstractItemModel::dataChanged(index, index, QVector<int>(1, Qt::DisplayRole));
            break;
        case SIGNAL_VISIBLE_TIME_RANGE_CHANGED:
            emit timeRangeChanged(channel, value1.toULongLong(), value2.toULongLong());
            break;
        case SIGNAL_VISIBLE_SPIKE_TIME_RANGE_CHANGED:
            emit spikeTimeRangeChanged(channel, value1.toULongLong(), value2.toULongLong());
            break;
        case SIGNAL_VERTICAL_RANGE_CHANGED:
            emit verticalRangeChanged(channel);
            break;
        case SIGNAL_FILTERING_CHANGED:
            emit filteringChanged(channel);
            break;
        case SIGNAL_RECALC_TIME_SLICE:
            emit timeSliceChanged(channel, value1.toULongLong());
            break;
        case SIGNAL_RECALC_TIME_STAMP:
            emit spikeSliceTimestamps(channel);
            break;
        case SIGNAL_RECALC_SPIKE_SLICE:
            emit spikeSliceChanged(channel, value1.toULongLong(), (SpikeMoveReference)value2.toUInt());
            break;
        case SIGNAL_SPIKES_INITIALIZED:
            emit spikesInitialized(channel);
            break;
        case SIGNAL_FEATURES_CHANGED:
            emit featuresChanged(channel);
            break;
        case SIGNAL_UNITS_CHANGED:
            emit unitsChanged(channel);
            break;
        case SIGNAL_CORRELOGRAMS_CHANGED:
            emit correlogramsChanged(channel);
            break;
        case SIGNAL_CLUSTERING_MATRIX_CHANGED:
            emit clusteringTendencyChanged(channel);
            break;
        case SIGNAL_WAVEFORM_SELECTION_CHANGED:
            emit waveformSelectionChanged(channel);
            break;
        case SIGNAL_WAVEFORM_EDIT_MODE_CHANGED:
            emit waveformEditModeChanged(channel);
            break;
        case SIGNAL_UPDATE_PCA:
            emit updatePCARequested(channel);
            break;
        case SIGNAL_ERROR:
            emit errorFound(channel, value1.toString());
            break;
        }
    }
}

/// @author Holly Ausbeck
/// @date   March 30, 2016
/// @brief  Emit dataChanged for the entire model
void ChannelConfigModel::updateAll()
{
    QModelIndex index1 = this->index(m_CurrentChannel, 0);
    QModelIndex index2 = this->index(m_CurrentChannel, this->columnCount() - 1);
    emit QAbstractItemModel::dataChanged(index1, index2, QVector<int>(1, Qt::DisplayRole));
}



void ChannelConfigModel::addEditAction(int channel, EditAction * action)
{
    ChannelData *cd = (ChannelData*)m_rootItem->child(channel);
    cd->actionManager()->addAction(action);

    emit setUndoEnabled(true);
    emit setRedoEnabled(false);
}

void ChannelConfigModel::undo(int channel)
{
    ChannelData *cd = (ChannelData*)m_rootItem->child(channel);
    cd->actionManager()->undo();

    emit setRedoEnabled(true);
    emit setUndoEnabled(!cd->actionManager()->isUndoEmpty());
}

void ChannelConfigModel::redo(int channel)
{
    ChannelData *cd = (ChannelData*)m_rootItem->child(channel);
    cd->actionManager()->redo();
    
    emit setUndoEnabled(!cd->actionManager()->isUndoEmpty());
    emit setRedoEnabled(!cd->actionManager()->isRedoEmpty());
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  returns a combination of flags that enables the item (ItemIsEnabled) and allows it to be selected (ItemIsSelectable).
/// @param [in] QModelIndex of the tree item
/// @return Item flags
Qt::ItemFlags ChannelConfigModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = 0;

    if (!index.isValid())
        return flags;

    ChannelData *cd = (ChannelData*)m_rootItem->child(index.row());

    if (!cd->enabled())
        return flags;

    int c = index.column();

    switch (c)
    {
    case CC_STATE:
        flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;  // | Qt::ItemIsUserCheckable;
        break;
    default:
        flags = Qt::NoItemFlags;
        break;
    }
    return flags;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  get the name of the column
/// @param [in] section or column
/// @param [in] orientation unused, we have a tree model
/// @param [in] role unused
/// @return QVariant containing the name of the column
QVariant ChannelConfigModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && (role == Qt::DisplayRole) || (role == Qt::DecorationRole))
    {
        return QVariant(ChannelData::ColumnNames.at(section));
    }
    else if (orientation == Qt::Vertical && (role == Qt::DisplayRole) || (role == Qt::DecorationRole))
    {
        return QVariant(QString("  "));
    }
    return QVariant();
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  create a model index from a parent tree item model index and a row-column pair
/// @param [in] row is the channel if the parent is the root, and a time segment if the parent is a channel
/// @param [in] column is the particular data
/// @param [in] QModelIndex of the parent
/// @return QModelIndex of the child value
QModelIndex ChannelConfigModel::index(int row, int column, const QModelIndex &parent) const
{
    if (m_bLimitChannelCount)
    {
        if (row > MAX_CHANNELS_NO_LICENSE)
            row = MAX_CHANNELS_NO_LICENSE;
    }
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    channelItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<channelItem*>(parent.internalPointer());

    if (parentItem != 0)
    {
        channelItem *childItem = parentItem->child(row);
        if (childItem)
            return createIndex(row, column, childItem);
        else
            return QModelIndex();
    }
    return QModelIndex();
}


/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  get the parent of a model index
/// @param [in] QModelIndex of the child
/// @return QModelIndex of the parent
QModelIndex ChannelConfigModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    int row = index.row();

    channelItem *childItem = static_cast<channelItem*>(index.internalPointer());
    channelItem *parentItem = childItem->parentItem();

    if (parentItem == m_rootItem)
        return QModelIndex();

    if (parentItem == 0)
        row = index.row();

    return createIndex(parentItem->row(), 0, parentItem);
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  return the number of children belonging to the parent
/// @param [in] QModelIndex of the parent
/// @return number of children
int ChannelConfigModel::rowCount(const QModelIndex &parent) const
{
    channelItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<channelItem*>(parent.internalPointer());

    int channelCount = parentItem->childCount();

    if (m_bLimitChannelCount && (channelCount > MAX_CHANNELS_NO_LICENSE))
    {
        channelCount = MAX_CHANNELS_NO_LICENSE;
    }
    return channelCount;
}

/// @author Holly Ausbeck
/// @date   May 7, 2015
/// @brief  Add a time segment to a channel
/// @param [in] channel = row
/// @param [in] start of segment
/// @param [in] end of segment
void ChannelConfigModel::addSegment(int channel, int start, int end)
{
    QModelIndex i = index(channel, 0);
    channelItem *item = static_cast<channelItem*>(i.internalPointer());

    IntervalData *id = new IntervalData(item);
    id->setData(IC_INTERVAL_START, QVariant(start));
    id->setData(IC_INTERVAL_END, QVariant(end));
    item->appendChild(id);
}

/// @author Holly Ausbeck
/// @date   May 7, 2015
/// @brief  Clear all the time segment in a channel
/// @param [in] channel = row
void ChannelConfigModel::removeSegments(int channel)
{
    QModelIndex i = index(channel, 0);
    channelItem *item = static_cast<channelItem*>(i.internalPointer());

    item->removeChildren();
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Write the contents of the tree to XML
/// @param [in] path of the file that will hold the XML
/// @return TRUE if success; FALSE otherwise
bool ChannelConfigModel::saveToXML(QString path, bool bDetectSortOnly)
{
    if (m_rootItem == 0)
        return false;

    QFile file(path);

    if (!file.open(QFile::WriteOnly|QIODevice::Text))
        return false;

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("BOSS");
  
    writer.writeTextElement("XML_Version", m_xmlVersion);

    m_rootItem->saveToXML(&writer, bDetectSortOnly);

    for (int i = 0; i < m_rootItem->childCount(); i++)
    {
        channelItem *item = m_rootItem->child(i);
        item->saveToXML(&writer, bDetectSortOnly);
    }
    writer.writeEndElement();
    writer.writeEndDocument();
    return true;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Create the model tree with the contents of the XML
/// @param [in] path of the XML file to read
/// @return TRUE if success; FALSE otherwise
bool ChannelConfigModel::getFromXML(QString path)
{
    beginResetModel();
    blockSignals(true);

    if (m_rootItem != 0)
        delete m_rootItem;
    m_rootItem = new RootData();

    m_CurrentChannel = 0;

    // Set the collumn header text
    for (int i = 0; i < CC_END; i++)
    {
        setHeaderData(i, Qt::Horizontal, ChannelData::ColumnNames[i]);
    }
    bool status = false;
    QFile file(path);

    if (!file.open(QFile::ReadOnly|QIODevice::Text))
        return status;

    QByteArray byteArray = file.readAll();
    file.close();

    QXmlStreamReader reader(byteArray);
    reader.setNamespaceProcessing(false);

    QStringRef name = reader.name();

    while(!reader.atEnd())
    {
        reader.readNextStartElement();
        name = reader.name();

        if (name == "Root")
        {
            m_rootItem->getFromXML(&reader);
        }
        else if (name == "Channel")
        {
			ChannelData *channel = new ChannelData("channel", m_rootItem);
            channel->getFromXML(&reader);
            m_rootItem->appendChild(channel);
            channel->initialize();
            status = true;
        }      
    }
    blockSignals(false);
    endResetModel();
    return status;
}


/// @author Sylvana Alpert
/// @date   August 31, 2015
/// @brief  Updates the contents of the XML
/// @param [in] path of the XML file to read
/// @return TRUE if success; FALSE otherwise
bool ChannelConfigModel::getParameters(QString path, QList<int> & channels)
{
    blockSignals(true);
    bool status = false;

    if (m_rootItem == 0)
        return status;
    
    QFile file(path);

    if (!file.open(QFile::ReadOnly | QIODevice::Text))
        return status;

    QByteArray byteArray = file.readAll();

    QXmlStreamReader reader(byteArray);
    reader.setNamespaceProcessing(false);

    QStringRef name = reader.name();
    if (name != "BOSS")
        reader.readNextStartElement();

    // Get version
    while (!reader.atEnd())
    {
        QStringRef name = reader.name();
        if (name != "XML_Version")
            reader.readNextStartElement();
        else
        {
            QString version = reader.readElementText();
            if (version != m_xmlVersion)
            {
                blockSignals(false);
                return status;
            }
            break;
        }
    }

    bool bCurrentChanged = false;
    while (!reader.atEnd())
    {
        QStringRef name = reader.name();
        reader.readNextStartElement();
        name = reader.name();
        
        ChannelData channel("temporary");

        if (name == "Channel")
        {            
            channel.getFromXML(&reader);
            QString chanInXML = channel.data(CC_NSX_LABEL, Qt::UserRole).toString();
            bool ok;
            int nChan = chanInXML.mid(7).toInt(&ok);        // All channel labels are "ChannelX" - discard "Channel"

            // If the user has asked for this channel, copy parameters
            if (ok && (channels.contains(nChan) || channels.isEmpty()))     // An empty list of channels means "get all"
            {
                ChannelData * cd = (ChannelData *)m_rootItem->child(nChan - 1);   // UI channel numbering starts at 1
                if (cd)
                    cd->copyDetectionSortingParametersFrom(&channel);

                if (nChan - 1 == m_CurrentChannel)
                    bCurrentChanged = true;

            }
            status = true;
        }

    }
    blockSignals(false);

	if (bCurrentChanged)
	{
        slotRowChanged(m_CurrentChannel);
	}
    return status;
}



bool ChannelConfigModel::userHasAccess(int column)
{
    if (!internalColumns.contains((ChannelColumn)column))
        return true;

    return false;
}

/// @author Holly Ausbeck
/// @date   Aug 5, 2015
/// @brief  Assign the unassigned segments to a unit
/// @param [in] channel
/// @param [in] unit
void ChannelConfigModel::assignSegments(int channel, int unit)
{
    UnitAccum unitData = data(index(channel, CC_SPIKEDATA_UNITS), Qt::UserRole).value<UnitAccum>();
    EditLinesData linesData = data(index(channel, CC_EDIT_LINES), Qt::UserRole).value<EditLinesData>();

    if (unitData)
    {
        UnitData* ud;
        QVector<int> usedUnitNumbers = unitData->getUnitNumbers(true);
        if (!usedUnitNumbers.contains(unit))
        {
            ud = unitData->addUnit(unit);
        }
        else        
            ud = unitData->unit(unit);

        for (int i = 0; i < linesData->m_data.count(); i++)
        {
            QLineF line = linesData->m_data.getLine(i);
            ud->lines.addLine(line);
        }
        linesData->m_data.clear();
            
        
    }
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Utility function determines if a unit is visible
/// @param [in] channel
/// @param [in] unit
/// @return TRUE if the unit is visible; FALSE otherwise
bool ChannelConfigModel::unitVisible(int channel, int unit)
{
	UnitAccum unitData = data(index(channel, CC_SPIKEDATA_UNITS), Qt::UserRole).value<UnitAccum>();

    if (unitData)
    {
        return unitData->isVisible(unit);
    }
}

/// @author Holly Ausbeck
/// @date   May 14, 2015
/// @brief  Utility function to set the visible (zoomed) timeline range
/// @param [in] channel
/// @param [in] min - min time in view
/// @param [in] max - max time in view
void ChannelConfigModel::setTimelineVisibleRange(int channel, qint64 min, qint64 max)
{
    setData(index(channel, CC_TIMELINE_VISIBLE_START), QVariant(min), Qt::UserRole);
    setData(index(channel, CC_TIMELINE_VISIBLE_END), QVariant(max), Qt::UserRole);

    //    if (startChanged || endChanged)
    emitSignal(channel, SIGNAL_VISIBLE_TIME_RANGE_CHANGED, QVariant(min), QVariant(max));
}

/// @author Holly Ausbeck
/// @date   May 14, 2015
/// @brief  Utility function to set the visible (zoomed) timeline range
/// @param [in] channel
/// @param [in] min - min time in view
/// @param [in] max - max time in view
void ChannelConfigModel::setTimespikeVisibleRange(int channel, qint64 min, qint64 max)
{
    setData(index(channel, CC_TIMESPIKE_VISIBLE_START), QVariant(min), Qt::UserRole);
    setData(index(channel, CC_TIMESPIKE_VISIBLE_END), QVariant(max), Qt::UserRole);

    //    if (startChanged || endChanged)
    emitSignal(channel, SIGNAL_VISIBLE_SPIKE_TIME_RANGE_CHANGED, QVariant(min), QVariant(max));
}

/// @author Holly Ausbeck
/// @date   May 14, 2015
/// @brief  Utility function to set the visible (zoomed) timeline range
/// @param [in] channel
/// @param [in] min - min time in view
/// @param [in] max - max time in view
void ChannelConfigModel::setTimelineRange(int channel, qint64 min, qint64 max)
{
    setData(index(channel, CC_TIMELINE_START), QVariant(min), Qt::UserRole);
    setData(index(channel, CC_TIMELINE_END), QVariant(max), Qt::UserRole);

    emitSignal(channel, SIGNAL_SLIDER_CHANGED, QVariant(SLIDERTYPE_TIME));
}

/// @author Holly Ausbeck
/// @date   May 14, 2015
/// @brief  Utility function to set the visible (zoomed) timeline range
/// @param [in] channel
/// @param [in] min - min time in view
/// @param [in] max - max time in view
void ChannelConfigModel::setTimespikeRange(int channel, qint64 min, qint64 max, qint64 firstID, qint64 lastID)
{
    setData(index(channel, CC_TIMESPIKE_START), QVariant(min), Qt::UserRole);
    setData(index(channel, CC_TIMESPIKE_END), QVariant(max), Qt::UserRole);

    quint64 range = max - min;
    int VisibleMin = data(index(channel, CC_TIMESPIKE_VISIBLE_START), Qt::UserRole).toInt();
    int VisibleMax = data(index(channel, CC_TIMESPIKE_VISIBLE_END), Qt::UserRole).toInt();

    if (VisibleMax > range)
    {
        VisibleMax = range;
    }
    if (VisibleMin > VisibleMax)
    {
        VisibleMin = 0;
    }
    setData(index(channel, CC_TIMESPIKE_VISIBLE_START), VisibleMin);
    setData(index(channel, CC_TIMESPIKE_VISIBLE_END), VisibleMax);
    setData(index(channel, CC_SPIKE_START), firstID);
    setData(index(channel, CC_SPIKE_END), lastID);

    emitSignal(channel, SIGNAL_SLIDER_CHANGED, QVariant(SLIDERTYPE_TIME));
}

/// @author Holly Ausbeck
/// @date   May 14, 2015
/// @brief  Notification of processing complete by the backend - goes to the channel Process Queue
/// @param [in] channel
void ChannelConfigModel::backendFinished(int channel, bool bUpdate)
{
    ChannelData *cd = (ChannelData*)m_rootItem->child(channel);

    RequestType type = cd->processNextRequest(bUpdate);

    if ((type == REQUESTTYPE_DETECT) || (type == REQUESTTYPE_SORT))
    {
        m_batchQ.finished();
        m_batchQ.next();
    }
}

/// @author Holly Ausbeck
/// @date   May 14, 2015
/// @brief  Queue a batch request for backend detection or sorting
/// @param [in] RequestType = REQUESTTYPE_SORT/REQUESTTYPE_DETECT
void ChannelConfigModel::QBatch(RequestType eType)
{
    ChannelState state = CHANNELSTATE_INACTIVE;

    switch (eType)
    {
    case REQUESTTYPE_SORT:
        state = CHANNELSTATE_SORTING;
        break;
    case REQUESTTYPE_DETECT:
        state = CHANNELSTATE_DETECTING;
        break;
    }
    // queue the operation for all selected channels
    for (int i = 0; i < m_rootItem->childCount(); i++)
    {
        channelItem *item = m_rootItem->child(i);

        if ((ChannelState)item->data(CC_STATE, Qt::UserRole).toInt() == CHANNELSTATE_SELECTED)
        {
            int nWaveforms = item->data(CC_NWAVEFORMS, Qt::UserRole).toInt();

            if ((state == CHANNELSTATE_SORTING) && (nWaveforms == 0))
                continue;

            item->setData(CC_STATE, QVariant(state), Qt::UserRole);
            m_batchQ.enQ(i, eType);
        }
    }
    emitSignal(0, SIGNAL_CLEAR_SELECTION);
    m_batchQ.next();
}

/// @author Holly Ausbeck
/// @date   March 18, 2016
/// @brief  Request for backend batch operation on a channel - goes to the channel Process Queue
/// @param [in] channel
void ChannelConfigModel::doBatch(int nChannel, RequestType eType)
{
    ChannelData *cd = (ChannelData*)m_rootItem->child(nChannel);

    switch (eType)
    {
    case REQUESTTYPE_DETECT:
        cd->addDetectRequest();
        break;
    case REQUESTTYPE_SORT:
        cd->addSortRequest();
        break;
    }
}

/// @author Holly Ausbeck
/// @date   April 21, 2016
/// @brief  Clear the batch queue and reset.
/// @param [in] channel
void ChannelConfigModel::exitBatch()
{
    m_batchQ.exit();
}

/// @author Holly Ausbeck
/// @date   April 21, 2016
/// @brief  return the number of tasks pending in the batch queue
/// @param [in] channel
int ChannelConfigModel::batchCount()
{
    return m_batchQ.count();
}

/// @author Holly Ausbeck
/// @date   Oct 14, 2016
/// @brief  Clear the batch queue of detect requests
void ChannelConfigModel::cancelDetect()
{
    m_batchQ.cancelRequests(REQUESTTYPE_DETECT);

    for (int i = 0; i < m_rootItem->childCount(); i++)
    {
        channelItem *item = m_rootItem->child(i);

        if ((ChannelState)item->data(CC_STATE, Qt::UserRole).toInt() == CHANNELSTATE_DETECTING)
        {
            if (i == CurrentChannel())
            {
                item->setData(CC_STATE, QVariant(CHANNELSTATE_SELECTED), Qt::UserRole);
            }
            else
            {
                item->setData(CC_STATE, QVariant(CHANNELSTATE_INACTIVE), Qt::UserRole);
            }
        }
    }
    FileInterface * fileinterface = FileInterface::instance();
    fileinterface->cancelDetect();
}

/// @author Holly Ausbeck
/// @date   Oct 14, 2016
/// @brief  remove the detect request for channel = nChannel from the batch queue
void ChannelConfigModel::cancelDetect(int nChannel)
{
    if (nChannel < m_rootItem->childCount())
    {
        if (m_batchQ.cancelRequest(nChannel, REQUESTTYPE_DETECT) == 1)
        {
            channelItem *item = m_rootItem->child(nChannel);

            if ((ChannelState)item->data(CC_STATE, Qt::UserRole).toInt() == CHANNELSTATE_DETECTING)
            {
                if (nChannel == CurrentChannel())
                {
                    item->setData(CC_STATE, QVariant(CHANNELSTATE_SELECTED), Qt::UserRole);
                }
                else
                {
                    item->setData(CC_STATE, QVariant(CHANNELSTATE_INACTIVE), Qt::UserRole);
                }
            }
        }
        else
        {
            FileInterface * fileinterface = FileInterface::instance();
            fileinterface->cancelDetect(nChannel);
        }
    }
}

/// @author Holly Ausbeck
/// @date   Oct 14, 2016
/// @brief  returns true if detection is in progress on any channels
bool ChannelConfigModel::detectionInProgress()
{
    for (int i = 0; i < m_rootItem->childCount(); i++)
    {
        channelItem *item = m_rootItem->child(i);
        ChannelState state = (ChannelState)item->data(CC_STATE, Qt::UserRole).toInt();

        if (state == CHANNELSTATE_DETECTING)
        {
            return true;
        }
    }
    return false;
}

/// @author Holly Ausbeck
/// @date   May 14, 2015
/// @brief  Utility function to move the timeline - goes to the channel Process Queue
/// @param [in] channel
/// @param [in] value - min or max time in view (specified by change)
/// @param [in] change - move the beginning or end
void ChannelConfigModel::moveTimeline(quint64 nValue, SpikeMoveReference reference)
{
    ChannelData *cd = (ChannelData*)m_rootItem->child(m_CurrentChannel);
    cd->addMoveRequest(nValue, reference);
}

void ChannelConfigModel::changeGroupSize()
{
    int nGroupSize = m_rootItem->data(RC_GROUP_SIZE, Qt::UserRole).toInt();
    {

        emit groupChannels();
    }
}

/// @author Holly Ausbeck
/// @date   May 14, 2015
/// @brief  process a channel selection change
///         this slot is called from the channel list control when the selection changes
/// @param [in] newSelection - list of selected channels
/// @param [in] noldSelection - list of last selected channels
void ChannelConfigModel::slotRowChanged(int row)
{
    if (row < 0)
        return;

    // don't allow a channel change to a disabled channel
    if (row < m_rootItem->childCount())
    {
        channelItem *item = m_rootItem->child(row);
        
        if ((item == 0) || !item->enabled())
            return;
    }
    for (int i = 0; i < m_rootItem->childCount(); i++)
    {
        //if (i != row)
        //{
            channelItem *item = m_rootItem->child(i);
            ChannelState state = (ChannelState)item->data(CC_STATE, Qt::UserRole).toInt();
            bool displayed = item->data(CC_DISPLAYED, Qt::UserRole).toBool();

            if (displayed == true)
            {
                item->setData(CC_DISPLAYED, QVariant(false), Qt::UserRole);
                emitSignal(m_CurrentChannel, SIGNAL_DATA_CHANGED, QVariant(CC_DISPLAYED));
            }
            //if (state == CHANNELSTATE_SELECTED)
            //{
            //    item->setData(CC_STATE, QVariant(CHANNELSTATE_INACTIVE), Qt::UserRole);
            //    emitSignal(m_CurrentChannel, SIGNAL_DATA_CHANGED, QVariant(CC_STATE));
            //}
        //}
    }
    m_CurrentChannel = row;
    ChannelData *cd = (ChannelData*)m_rootItem->child(row);
    WizardPage page = (WizardPage)cd->data(CC_PAGE, Qt::UserRole).toInt();
    emit currentChannelChanged(m_CurrentChannel);
    emit wizardPageChanged(m_CurrentChannel, page);

    cd->setData(CC_DISPLAYED, QVariant(true), Qt::UserRole);
    cd->setData(CC_STATE, QVariant(CHANNELSTATE_SELECTED), Qt::UserRole);
    cd->addUpdateRequest();

    updateAll();

    emit setUndoEnabled(!cd->actionManager()->isUndoEmpty());
    emit setRedoEnabled(!cd->actionManager()->isRedoEmpty());
}

/// @author Holly Ausbeck
/// @date   May 14, 2015
/// @brief  process a channel selection change
///         this slot is called from the channel list control when the selection changes
/// @param [in] newSelection - list of selected channels
/// @param [in] noldSelection - list of last selected channels
void ChannelConfigModel::slotSelectionChanged(QList<int> &newSelection, QList<int> &oldSelection)
{
    // select the new items
    foreach(int row, newSelection)
    {
        ChannelData *cd = (ChannelData*)m_rootItem->child(row);
        ChannelState state = (ChannelState)cd->data(CC_STATE, Qt::UserRole).toInt();

        if (!oldSelection.contains(row))
            cd->setData(CC_STATE, QVariant(CHANNELSTATE_SELECTED), Qt::UserRole);
    }
    // unselect the old items
    foreach(int row, oldSelection)
    {
        ChannelData *cd = (ChannelData*)m_rootItem->child(row);
        ChannelState state = (ChannelState)cd->data(CC_STATE, Qt::UserRole).toInt();

        if (!newSelection.contains(row))
            cd->setData(CC_STATE, QVariant(CHANNELSTATE_INACTIVE), Qt::UserRole);
    }
    updateAll();
}

int ChannelConfigModel::channelSelectionCount()
{
    int count = 1;  // set to one for the visible channel

    for (int i = 0; i < m_rootItem->childCount(); i++)
    {
        channelItem *item = m_rootItem->child(i);
        ChannelState state = (ChannelState)item->data(CC_STATE, Qt::UserRole).toInt();

        if (state == CHANNELSTATE_SELECTED)
        {
            count++;
        }
    }
    return count;
}

QList<int> ChannelConfigModel::getColumnSelection()
{
    QList<int> columnSelection;

    for (int i = 0; i < m_rootItem->childCount(); i++)
    {
        channelItem *item = m_rootItem->child(i);
        ChannelState state = (ChannelState)item->data(CC_STATE, Qt::UserRole).toInt();

        if (state == CHANNELSTATE_SELECTED)
        {
            columnSelection.append(i);
        }
    }
    return columnSelection;
}

QList<int> ChannelConfigModel::getColumnSelection(QModelIndexList &modelIndexList)
{
    QList<int> columnSelection;

    foreach(QModelIndex modelIndex, modelIndexList)
    {
        columnSelection.append(modelIndex.row());
    }
    return columnSelection;
}

void ChannelConfigModel::slotMaxDisplayedSamplesChanged()
{
    ChannelData *cd = (ChannelData*)m_rootItem->child(m_CurrentChannel);
    cd->maxDisplayedSamplesChanged();
}

void ChannelConfigModel::slotMaxDisplayedWaveformsChanged()
{
    ChannelData *cd = (ChannelData*)m_rootItem->child(m_CurrentChannel);
    cd->maxDisplayedWaveformsChanged();
}

void ChannelConfigModel::slotUpdateChannel(int channel)
{
    if (channel == m_CurrentChannel)
    {
        ChannelData *cd = (ChannelData*)m_rootItem->child(channel);
        cd->addUpdateRequest();
    }
}

void ChannelConfigModel::slotUpdatePCA()
{
    ChannelData *cd = (ChannelData*)m_rootItem->child(m_CurrentChannel);
    if (cd)
        cd->addUpdatePCARequest();
}

/// @author Holly Ausbeck
/// @date   May 14, 2015
/// @brief  file interval functions - only partially implemented
int ChannelConfigModel::fileIntervalStart(int channel, int interval)
{
    if (interval == -1)
        interval = data(index(channel, CC_CURRENT_INTERVAL), Qt::UserRole).toInt();

    QModelIndex intervalIndex = index(interval, IC_INTERVAL_START, index(channel, 0));

    return data(intervalIndex, Qt::UserRole).toInt();
}

int ChannelConfigModel::fileIntervalEnd(int channel, int interval)
{
    if (interval == -1)
        interval = data(index(channel, CC_CURRENT_INTERVAL), Qt::UserRole).toInt();

    QModelIndex intervalIndex = index(interval, IC_INTERVAL_END, index(channel, 0));

    return data(intervalIndex, Qt::UserRole).toInt();
}

bool ChannelConfigModel::setFileIntervalStart(int channel, int interval, quint64 start)
{
    QModelIndex intervalIndex = index(interval, IC_INTERVAL_START, index(channel, 0));

    return setData(intervalIndex, QVariant(start), Qt::UserRole);
}

bool ChannelConfigModel::setFileIntervalEnd(int channel, int interval, quint64 end)
{
    QModelIndex intervalIndex = index(interval, IC_INTERVAL_END, index(channel, 0));

    return setData(intervalIndex, QVariant(end), Qt::UserRole);
}

void ChannelConfigModel::initUnits(int channel, int nSamples, int nUnits)
{
    ChannelData *cd = (ChannelData*)m_rootItem->child(channel);
    cd->initUnits(nSamples, nUnits);
}

void ChannelConfigModel::sendTimeReady(quint32 channel, RawDataAccum accum)
{   
    if (channel == CurrentChannel())
        emit timeseriesReady(channel, accum);    
}

void ChannelConfigModel::sendEnergyReady(quint32 channel, EnergyDataAccum accum)
{
    if (channel == CurrentChannel())
        emit energyReady(channel, accum);
}

void ChannelConfigModel::sendFftReady(quint32 channel, FftDataAccum accum)
{
    if (channel == CurrentChannel())
        emit fftReady(channel, accum);
}

void ChannelConfigModel::sendHistogramReady(DisplayDataType type, quint32 channel, HistogramDataAccum accum)
{
    if (channel == CurrentChannel())
        emit histogramReady(type, channel, accum);
}

void ChannelConfigModel::sendTimestampsReady(quint32 channel, QVector<quint32> ts)
{
    if (channel == CurrentChannel())
        emit timestampsReady(channel, ts);
}

void ChannelConfigModel::updateProgress(int channel, int column, int percent)
{
    emit channelProgressChanged(channel, column, percent);
}

/// @author Holly Ausbeck
/// @date   Sept 28, 2016
/// @brief  Initializes the pause map with the pauses from the nsx file
void ChannelConfigModel::initPauseMap()
{
    RootData *ri = (RootData*)m_rootItem;
    m_pauseWrapper.setPauseMaps(ri->getPauseMaps());

    QStringList separators = data(index(0, CC_FILE_SEPARATORS), Qt::UserRole).toStringList();
    QVector<quint64> timestamps;

    for (int sep(0); sep < separators.size(); ++sep)
    {
        timestamps.append(separators[sep].toULongLong());
    }
    m_pauseWrapper.combineMaps(timestamps);
}

