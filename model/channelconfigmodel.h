/////////////////////////////////////////////////////////////////
///
/// @file ChannelConfigModel.h
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

#ifndef CHANNELCONFIGMODEL_H_INCLUDED
#define CHANNELCONFIGMODEL_H_INCLUDED

#include <QItemSelection>
#include <QAbstractItemModel>
#include <QString>

#include "../model/RootData.h"
#include "../model/ChannelData.h"
#include "../model/BatchQ.h"
#include "../backend/dataprocessing/continuous/continuousbase.h"
#include "../backend/dataprocessing/continuous/timesliceaccumulator.h"
#include "../model/groupselection.h"

// These columns relate to the entire file ie. they are the same for all channels
enum FileColumn
{
    FILEMODEL_COLUMN_PAUSE_MAP = CC_END,              // QMap<int, QVariant>
    FILEMODEL_COLUMN_END
};


const int ChannelTableRole = Qt::UserRole + 1;
const int MAX_CHANNELS_NO_LICENSE = 16;

// The BOSS model is a tree format. ChannelData is the top, with IntervalData children

// unit edit
Q_DECLARE_METATYPE(EditLinesData)

// spike data
Q_DECLARE_METATYPE(FeaturesAccum)
Q_DECLARE_METATYPE(SortAccum)
Q_DECLARE_METATYPE(UnitAccum)
Q_DECLARE_METATYPE(ClusteringTendencyAccum)
Q_DECLARE_METATYPE(SortMethod)
Q_DECLARE_METATYPE(QVector<quint32>)
Q_DECLARE_METATYPE(QVector<int>)
Q_DECLARE_METATYPE(ClusterDataPtr)
//Q_DECLARE_METATYPE(EnergyDataAccum)
Q_DECLARE_METATYPE(FftDataAccum)

// ui data
Q_DECLARE_METATYPE(SliderType)
Q_DECLARE_METATYPE(UnitOperation)
Q_DECLARE_METATYPE(ChannelState)
Q_DECLARE_METATYPE(WizardPage)
Q_DECLARE_METATYPE(ChannelColumn)
Q_DECLARE_METATYPE(FeatureID)
Q_DECLARE_METATYPE(GLWidgetState)
Q_DECLARE_METATYPE(SpikeMoveReference)
//Q_DECLARE_METATYPE(GroupSelection)


// Column Properties Bitmap
const int COLUMN_PROPERTY_USERACCESS = 0X01;

class ChannelConfigModel : public QAbstractItemModel
{
    Q_OBJECT
private:
    static ChannelConfigModel *inst;

public:
    static ChannelConfigModel* instance();
    explicit ChannelConfigModel(QObject *parent = 0);
    ~ChannelConfigModel();

    void nukeModel();
    void initializeModel(int nChannels, int sampleRate = 30000, bool bLimitChannelCount = false);
    void initializeModelChannel(int nChannel, qint64 first, qint64 last);
    void addFileSeparator(int nChannel, quint64 separator_timestamp);
    quint64 getFileSeparator(int fileIndex);
    PauseWrapper getPauseWrapper();
    void addPauseMap(QMultiMap<quint64, quint64> pauseMap);
    void changeGroupSize();

    // required for ChannelConfigModel not to be abstract
    virtual QModelIndex	index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    virtual QModelIndex	parent(const QModelIndex & index) const;
    virtual int	rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual int	columnCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index, int role = Qt::UserRole) const;

    channelItem * getRoot() { return m_rootItem; }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::UserRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void updateAll();

    int channelSelectionCount();

    void removeSegments(int row);
    void addSegment(int row, int start, int end);

    bool saveToXML(QString path, bool bDetectSortOnly = false);
    bool getFromXML(QString path);
    bool getParameters(QString path, QList<int> & channels);

    bool userHasAccess(int column);

    static QStringList ColumnNames;

    ////////////////////////////////////////////////////////////////////////////////////////////
    // specific functions

    // non channel specific data
    void     setRootData(int column, const QVariant &value, int role = Qt::UserRole);
    QVariant rootData(int column, int role = Qt::UserRole);

    // utility functions
    int CurrentChannel() { return m_CurrentChannel; }
    bool unitVisible(int channel, int unit);
    void assignSegments(int channel, int unit);

    void setTimelineRange(int channel, qint64 min, qint64 max);
    void setTimespikeRange(int channel, qint64 min, qint64 max, qint64 firstID, qint64 lastID);
    void setTimelineVisibleRange(int channel, qint64 min, qint64 max);
    void setTimespikeVisibleRange(int channel, qint64 min, qint64 max);

    void updateProgress(int channel, int column, int percent);
    // communication to the backend
    void moveTimeline(quint64 nValue, SpikeMoveReference reference);
    void QBatch(RequestType eType);
    void doBatch(int nChannel, RequestType eType);
    int batchCount();
    void exitBatch();
    void cancelDetect();
    void cancelDetect(int nChannel);
    bool detectionInProgress();

    // communication from the backend
    void backendFinished(int channel, bool bUpdate);

    // file interval methods
    int fileIntervalStart(int channel, int interval = -1);
    int fileIntervalEnd(int channel, int interval = -1);
    bool setFileIntervalStart(int channel, int interval, quint64 start);
    bool setFileIntervalEnd(int channel, int interval, quint64 end);

    // units
    void initUnits(int channel, int nSamples, int nUnits);

    void emitSignal(int channel, ChannelModelSignals signal, QVariant value1 = QVariant(), QVariant value2 = QVariant(), QVariant value3 = QVariant());

    // Undo-Redo functionality
    void addEditAction(int channel, EditAction * action);
    void undo(int channel);
    void redo(int channel);

    // Continuous methods
    void sendTimeReady(quint32 channel, RawDataAccum accum);
    void sendEnergyReady(quint32 channel, EnergyDataAccum accum);
    void sendFftReady(quint32 channel, FftDataAccum accum);
    void sendHistogramReady(DisplayDataType type, quint32 channel, HistogramDataAccum accum);
    void sendTimestampsReady(quint32, QVector<quint32>);

private:
    int m_CurrentChannel;
    RootData *m_rootItem;
    bool    m_bLimitChannelCount;

    QString m_xmlVersion;

    static QVector<ChannelColumn> internalColumns;
    static QVector<ChannelColumn> userColumns;
    BatchQ m_batchQ;

    PauseWrapper   m_pauseWrapper;
    void initPauseMap();

    QList<int> getColumnSelection();
    QList<int> getColumnSelection(QModelIndexList &modelIndexList);
    bool setChannelData(int row, int column, const QVariant &value, int role = Qt::UserRole);

public slots:
    // from channel table view
    void slotSelectionChanged(QList<int> &newSelection, QList<int> &oldSelection);
    void slotRowChanged(int row);

    // Alignment and SetUnit
    void slotUpdateChannel(int channel);
    // Update PCA for current channel
    void slotUpdatePCA();
    void slotMaxDisplayedSamplesChanged();
    void slotMaxDisplayedWaveformsChanged();

signals:
    // signals to the UI only
    // used to coordinate thresholds
    void sliderChanged(int channel, SliderType type);
    // used to show or hide or make symetrical thresholds
    void thresholdTypeChanged(int channel);
    // used to show or hide signal energy windows
    void signalEnergyTypeChanged(int channel);
    // the graphs shown in feature vs feature changed, used by the feature vs feature widget
    void viewFeaturesChanged(int channel);
    // high pass filter changed, show or hide the fft threshold (cut off freq)
    void filteringChanged(int channel);
    // state variable, used to show or hide windows
    void wizardPageChanged(int channel, WizardPage page);
    // selected features, used for all the feature windows &toolbars
    void sortFeaturesChanged(int channel);
    // used by graphic views to show or hide waveforms by unit
    void visibleUnitsChanged(int channel, QStringList visibleUnits);
    // selected axis changed on the 3D view
    void threeDAxisChanged(int channel, ChannelColumn axis, FeatureID newFeature);
    // selected axis changed on the 2D view
    void twoDAxisChanged(int channel, ChannelColumn axis, FeatureID newFeature);
    // The edit/rotate/select cluster state changed in the gl window
    void glEditStateChanged(int channel, GLWidgetState editState);
    // the visible time range in the time graphics view changed
    void timeRangeChanged(int channel, quint64 min, quint64 max); 
    // the visible time range in the time waveform graphics view changed
    void spikeTimeRangeChanged(int channel, quint64 min, quint64 max);
    // the state of detecting waveforms changed
    void verticalRangeChanged(int channel);
    // sdFile for current channel has been initialized
    void spikesInitialized(int channel);
    // features and waveforms are now available
    void featuresChanged(int channel);
    // average units for the file changed
    void unitsChanged(int channel);
    // unit correlograms have changed
    void correlogramsChanged(int channel);
    // the clustering tendency matrix changed
    void clusteringTendencyChanged(int channel);
    // the selection of waveforms has changed
    void waveformSelectionChanged(int channel);
    // the waveform edit mode has changed
    void waveformEditModeChanged(int channel);
    // User requested to update the PCA
    void updatePCARequested(int channel);
    // Error handling
    void errorFound(int channel, QString error);
    // the state of detecting waveforms changed
    void channelStateChanged(int channel, ChannelState state);
    // for channel table
    void clearSelection();
    // the selected channels changed
    void channelSelectionChanged();
    // update the progress on the channel sorting/detecting
    void channelProgressChanged(int channel, int column, int percent);

    // signals to the backend only
    void spikeSliceChanged(int channel, quint64 value, SpikeMoveReference reference);
    void timeSliceChanged(int channel, quint64 start);
    void spikeSliceTimestamps(int channel);
    void detectWaveforms(int channel);
    void sortWaveforms(int channel);

    // Continuous time signals - ccm acts as a mid-station
    void timeseriesReady(quint32 channel, RawDataAccum accum);
    void energyReady(quint32 channel, EnergyDataAccum accum);
    void fftReady(quint32 channel, FftDataAccum accum);
    void histogramReady(DisplayDataType type, quint32 channel, HistogramDataAccum accum);
    void timestampsReady(quint32 channel, QVector<quint32> ts);

    // model mapping
    void currentChannelChanged(int channel);
    void groupChannels();

    // Undo-Redo
    void setUndoEnabled(bool enabled);
    void setRedoEnabled(bool enabled);
};

#endif // CHANNELCONFIGMODEL_H_INCLUDED
