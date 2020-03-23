/////////////////////////////////////////////////////////////////
///
/// @file ChannelData.h
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  April 29, 2015
///
/// @brief ChannelData contains all the information for a channel that is saved in the model.
///
/// @note
/// ChannelData subclasses channelItem and represents a Channel Model tree item.
/// Each instance of ChannelData contains the information for a channel.
/// ChannelData has children representing each time segment.
///

#ifndef CHANNELDATA_H_INCLUDED
#define CHANNELDATA_H_INCLUDED

#include <QItemSelection>
#include <QAbstractTableModel>
#include <QString>
#include <QVariant>
#include <QLineF>
#include <QXmlStreamWriter>


#include "../model/channelItem.h"
#include "../model/intervalData.h"
#include "../model/ProcessQ.h"
#include "../model/spikedata.h"
#include "../model/EditLines.h"
#include "../model/EditAction.h"
#include "../model/clusterdata.h"
#include "../backend/file/fileinterface.h"
#include "../model/groupselection.h"


/// @note
/// columns in a ChannelData tree item
enum ChannelColumn
{
    // General
    CC_STATE,                     // channel state
    CC_NAME,                      // channel name
    CC_NWAVEFORMS,                // number of waveforms
    CC_NUNITS,                    // number of units
    CC_CURRENT_INTERVAL,          // selected interval
    CC_FIRST_TIME,                // first sample time in file
    CC_LAST_TIME,                 // last sample time in file

    // State
    CC_NSAMPLES,                  // number of continuous samples
    CC_PAGE,                      // selected wizard page
    CC_SAMPLERATE,                // sample rate

    // Preprocessing
    CC_HIGHPASS_FILTER,           // enum HighpassFilter
    CC_FILTER_ORDER,              // int  filter order
    CC_CUTOFF_FREQ,               // int  cutoff frequency
    CC_MAX_CUTOFF_FREQ,           // int  max cutoff frequency
    CC_SIGNAL_ENERGY,             // enum SignalEnergy
    CC_SE_WIDTH,                  // int  window width
    CC_ARTIFACT_SUBTRACTION,      // bool use artifact subtraction

    // Threshold
    CC_THRESHOLD,                 // enum ThresholdType
    CC_SYMETRICAL_THRESHOLD,      // bool threshold1 = -threshold2
    CC_RMS_MULTIPLIER,            // int  RMS Multiplier for threshold proportional to noise RMS
    CC_THRESHOLD1,                // int  threshold1
    CC_THRESHOLD2,                // int  threshold2
    CC_ENERGY_THRESHOLD,          // int  energy threshold
    CC_THRESHOLD1_SET,            // bool threshold1 set by user or backend detection
    CC_THRESHOLD2_SET,            // bool threshold2 set by user or backend detection
    CC_THRESHOLDE_SET,            // bool Energy threshold set by user or backend detection

    // Waveform Duration
    CC_WF_PRETHRESHOLD,           // int  samples to save before threshold
    CC_WF_LENGTH,                 // int  total length of waveform
    CC_DEAD_TIME,                 // int  deadtime between waveforms

    // Features
    CC_FEATURE,                   // bitmap of features checked
    CC_SELECTED_FEATURES,         // QMap<QString, QVariant>  (row, column) map of selected features in feature correlation matrix

    //  Sorting
    CC_SORT_METHOD,               // enum SortMethod
    CC_X_3DAXIS,                  // enum FeatureID of 3D X Axis
    CC_Y_3DAXIS,                  // enum FeatureID of 3D Y Axis
    CC_Z_3DAXIS,                  // enum FeatureID of 3D Z Axis
    CC_3D_EDIT_STATE,             // enum GLWidgetState
    CC_X_2DAXIS,                  // enum FeatureID of 2D X Axis
    CC_Y_2DAXIS,                  // enum FeatureID of 2D Y Axis
    CC_NBR_SORT_UNITS,            // int  number of units to sort into
    CC_PENALTY_FACTOR,            // double penalty factor for T-Distribution E-M
    CC_CENTROIDS,                 // centroids from GL Widget

    // Waveform Editing
    CC_WAVEFORM_ALIGNMENT,        // enum AlignMode
    CC_MAX_SHIFT,                 // quint64 max alignment shift
    CC_UNITS_ALIGN,               // QByteArray of integers specifying the unit numbers to align
    CC_UNITS_VISIBLE,             // QByteArray of integers specifying the unit numbers that are visible
    CC_EDIT_LINES,                // QList<QList<QLineF>> the list of edit lines for each unit
    CC_EDIT_BOXES,                // QList<QList<QRectF>> the list of edit boxes for each unit
    CC_EDIT_MODE,                 // ENUM EditMode

    // Nev values
    CC_VALID_WAVEFORMS,
    CC_INVALID_WAVEFORMS,
    CC_NBR_SAMPLES_IN_WAVEFORM,

    // timeline range - for syncronizing timeline signal energy and timeline continuous
    // This holds the currently visible timeline range which changes dynamically as the time line is zoomed
    CC_TIMELINE_VISIBLE_START,    // int  start of the zoomed timeline range
    CC_TIMELINE_VISIBLE_END,      // int  end of the zoomed timeline range
    CC_TIMESPIKE_VISIBLE_START,   // int  start of the zoomed timeline spike range
    CC_TIMESPIKE_VISIBLE_END,     // int  end of the zoomed timeline spike range
    CC_TIMELINE_VERTICAL_RANGE,   // int  current vertical range displayed in timeline
    CC_SE_VERTICAL_RANGE,         // int  current vertical range displayed in signal energy

    // timeline range - specified by the current position of the scroll range slider
    CC_TIMELINE_START,            // int  start of the timeline range in continuous view
    CC_TIMELINE_END,              // int  end of the timeline range in continuous view
    CC_TIMESPIKE_START,           // int  start of the timeline range in spike view
    CC_TIMESPIKE_END,             // int  end of the timeline range in spike view
    CC_SPIKE_START,               // int  first spike id in the timeline range in spike view
    CC_SPIKE_END,                 // int  last spike id in the timeline range in spike view

    // Nsx values from the extended headers in the nsx file
    CC_NSX_ELECTRODEID,
    CC_NSX_CONNECTOR,
    CC_NSX_PIN,
    CC_NSX_LABEL,
    CC_NSX_CUTOFF_FREQ,
    CC_NSX_ORDER,
    CC_NSX_TYPE,
    CC_CDFILE_INIT,
    CC_SDFILE_INIT,

    // Spike Data
    CC_SPIKEDATA_FEATURES,
    CC_SPIKEDATA_UNITS,
    CC_SPIKEDATA_CLUSTERING_MATRIX,

    // state variable
    CC_DETECTED,                  // Waveforms have been detected on this channel
    CC_DISPLAYED,                 // bool the user is looking at this channel

    // Error Handling
    CC_LAST_ERROR,

    // File Separators
    CC_FILE_SEPARATORS,

    // Undo-Redo 
    CC_CHANGE_COUNTER,

    CC_END
};


class ChannelData : public channelItem
{
    Q_OBJECT

public:
    ChannelData(QString sName, channelItem *parent = 0);
    ~ChannelData();
    void initialize(int sampleRate = 0);

    virtual int columnCount() const;
    virtual QVariant data(int column, int role) const;
    virtual bool setData(int column, const QVariant &value, int role = Qt::UserRole);
    void setTimelineRange(qint64 min, qint64 max);
    void setTimespikeRange(qint64 min, qint64 max, qint64 firstID, qint64 lastID);

    // to/from xml
    virtual bool saveToXML(QXmlStreamWriter *writer, bool bPartialSave = false);
    virtual bool getFromXML(QXmlStreamReader *reader);
    virtual bool enabled();

    // channel data specific functions
    void maxDisplayedSamplesChanged();
    void maxDisplayedWaveformsChanged();
    void addDetectRequest();
    void addSortRequest();
    void addUpdateRequest();
    void addUpdatePCARequest();
    void addMoveRequest(quint64 nValue = 0, SpikeMoveReference reference = SPIKEMOVE_START);
    RequestType processNextRequest(bool bupdateLastRequest);

    void resetSpikeData() 
    {
        if (m_spikeData)
            m_spikeData->reset(); 
    }
    void initUnits(int nSamples, int nUnits)
    {
        if (m_spikeData)
            m_spikeData->initUnits(nSamples, nUnits);
    }
    void copyDetectionSortingParametersFrom(ChannelData * other);

    EditActionManager * actionManager() { return &m_editActionManager; }

    GroupSelection getGroupSelection(int nGroupSize, QString sText);
    void setGroupSelection(GroupSelection groupSelection);

    // Each of the following string lists correspond to an enum that is displayed
    static QStringList ColumnNames;
    static QStringList WizardPageNames;
    static QStringList HighPassFilterNames;
    static QStringList SignalEnergyNames;
    static QStringList ThresholdTypeNames;
    static QStringList FeatureIDNames;
    static QStringList SortMethodNames;
    static QStringList AlignModeNames;
    static QStringList GLWidgetStateNames;
    static QStringList EditModeNames;

    // The FeatureNames string list corresponds to a bitmap that is displayed
    static QStringList FeatureNames;

    // images used for selection
    static QPixmap *m_displayPixmap;
    static QPixmap *m_displaySortPixmap;
    static QPixmap *m_displayDetectPixmap;
    static QPixmap *m_displayAlignPixmap;
    static QPixmap *m_selectPixmap;
    static QPixmap *m_detectPixmap;
    static QPixmap *m_sortPixmap;
    static QPixmap *m_alignPixmap;
    static QPixmap *m_blankPixmap; 

private:
    GroupSelection m_groupSelection;

    // time range functions
    void initializeContinuousTimes();
    void setContinuousTimes(quint64 nValue = 0, SpikeMoveReference reference = CONTINUOUSMOVE_START);
    void initializeSpikeTimes();

    // get()/set() functions for the channel data
    bool setThreshold1(QVariant var);
    bool setThreshold2(QVariant var);
    bool setEnergyThreshold(QVariant var);

    // Waveform Duration
    bool setWaveformPrethreshold(QVariant var);
    bool setWaveformLength(QVariant var, int role = Qt::UserRole);

    // units
    QVariant getVisibleUnits(int role) const;
    void setVisibleUnits(const QVariant &value, int role);

    // General function for double
    QVariant getDouble(double nMember, int role) const;

    // General function for bool
    QVariant getBool(int nMember, int role) const;

    // General function for integer
    QVariant getInteger(int nMember, int role) const;

    // General functions for enum and QStringList
    QVariant getEnum(QStringList *names, int index, int role) const;
    bool setEnum(QStringList *names, int *member, const QVariant &value, int role);

    // General functions for bit mask and QStringList
    QVariant getBitMask(QStringList *names, uint bitMask, int role) const;
    bool setBitMask(QStringList *names, uint *member, QVariant value, int role);

    bool setCutoffFreq(const QVariant &value);
    bool setSampleRate(const QVariant &value);

    bool switchPages(const QVariant &value, int role);
    void clearSelection();

    void addUpdateFilterRequest();
    void emitSignalIfDisplayed(int channel, ChannelModelSignals signal, QVariant value1 = QVariant(), QVariant value2 = QVariant(), QVariant value3 = QVariant());

    // Each of the following attributes correspond to a model column
    // General 
    QString        m_sName;                    //CC_NAME
    quint64        m_nCurrentInterval;         //CC_CURRENT_INTERVAL
    quint64        m_nTimeStampMin;            //CC_FIRST_TIME
    quint64        m_nTimeStampMax;            //CC_LAST_TIME

    // State
    quint64        m_nSamples;                 //CC_NSAMPLES
    quint64        m_nWaveforms;               //CC_NWAVEFORMS
    WizardPage     m_ePage;                    //CC_PAGE
    quint64        m_nSampleRate;              //CC_SAMPLERATE

    // Preprocessing
    HighpassFilter m_eHighpassFilter;          //CC_HIGHPASS_FILTER
    quint64        m_nFilterOrder;             //CC_FILTER_ORDER
    quint64        m_nCutoffFreq;              //CC_CUTOFF_FREQ
    quint64        m_nMaxCutoffFreq;           //CC_MAX_CUTOFF_FREQ
    SignalEnergy   m_eSignalEnergy;            //CC_SIGNAL_ENERGY
    quint64        m_nWindowWidth;             //CC_SE_WIDTH
    bool           m_bArtifactSubtraction;     //CC_ARTIFACT_SUBTRACTION

    // Threshold
    ThresholdType  m_eThreshold;               //CC_THRESHOLD
    ThresholdType  m_eLastThreshold;           // not accessible through setData()
    bool           m_bThresholdSymetrical;     //CC_SYMETRICAL_THRESHOLD
    double         m_nRMSMultiplier;           //CC_RMS_MULTIPLIER
    qint64         m_nThreshold1;              //CC_THRESHOLD1
    qint64         m_nThreshold2;              //CC_THRESHOLD2
    qint64         m_nEnergyThreshold;         //CC_ENERGY_THRESHOLD
    bool           m_bThreshold1Set;           //CC_THRESHOLD1_SET
    bool           m_bThreshold2Set;           //CC_THRESHOLD2_SET
    bool           m_bThresholdESet;           //CC_THRESHOLDE_SET

    // Waveform Duration
    quint64        m_nWaveformPrethreshold;    //CC_WF_PRETHRESHOLD
    quint64        m_nWaveformLength;          //CC_WF_LENGTH
    quint64        m_nWaveformDeadTime;        //CC_DEAD_TIME

    // Features
    quint64        m_nFeaturesSelected;        //CC_FEATURE
    QMultiMap<QString, QVariant> m_mapSelectedFeatures; //CC_SELECTED_FEATURES (row, column)

    //  Sorting
    SortMethod     m_eSortMethod;              //CC_SORT_METHOD
    FeatureID      m_eThreeDXAxis;             //CC_X_3DAXIS
    FeatureID      m_eThreeDYAxis;             //CC_Y_3DAXIS
    FeatureID      m_eThreeDZAxis;             //CC_Z_3DAXIS
    GLWidgetState  m_eThreeDMode;              //CC_3D_EDIT_STATE
    FeatureID      m_eTwoDXAxis;               //CC_X_2DAXIS
    FeatureID      m_eTwoDYAxis;               //CC_Y_2DAXIS
    quint64        m_nSortUnits;               //CC_NBR_SORT_UNITS
    double         m_fPenaltyFactor;           //CC_PENALTY_FACTOR
    ClusterDataPtr m_clusterData;              //CC_CENTROIDS

    // Waveform Editing
    AlignMode      m_eAlignMode;               //CC_WAVEFORM_ALIGNMENT
    quint64        m_nMaxShift;                //CC_MAX_SHIFT
    QStringList    m_UnitsToAlign;             //CC_UNITS_ALIGN
    QStringList    m_UnitsVisible;             //CC_UNITS_VISIBLE
    EditLinesData  m_pEditLines;               //CC_EDIT_LINES
    EditLinesData  m_pEditBoxes;               //CC_EDIT_BOXES
    EditMode       m_eEditMode;                //CC_EDIT_MODE

    // Nev values
    quint64        m_nValidWaveforms;          //CC_VALID_WAVEFORMS
    quint64        m_nInvalidWaveforms;        //CC_INVALID_WAVEFORMS
    quint64        m_nSamplesPerWaveform;      //CC_NBR_SAMPLES_IN_WAVEFORM

   // timeline range - for syncronizing timeline signal energy and timeline continuous
   // This holds the currently visible timeline range which changes dynamically as the time line is zoomed
    quint64        m_nTimelineVisibleStart;    //CC_TIMELINE_VISIBLE_START,
    quint64        m_nTimelineVisibleEnd;      //CC_TIMELINE_VISIBLE_END,
    quint64        m_nTimespikeVisibleStart;   //CC_TIMESPIKE_VISIBLE_START,
    quint64        m_nTimespikeVisibleEnd;     //CC_TIMESPIKE_VISIBLE_END,
    quint64        m_nTimelineVerticalRange;   //CC_TIMELINE_VERTICAL_RANGE,
    quint64        m_nSEVerticalRange;         //CC_SE_VERTICAL_RANGE
    //internal variable for keeping track of the beginning of the visible range for spike and timeline
    quint64        m_nFileVisibleStart;
    quint64        m_nFileVisibleEnd;

    // timeline range - specified by the current position of the scroll range slider
    quint64        m_nTimelineStart;           //CC_TIMELINE_START,
    quint64        m_nTimelineEnd;             //CC_TIMELINE_END,
    quint64        m_nTimespikeStart;          //CC_TIMESPIKE_START,
    quint64        m_nTimespikeEnd;            //CC_TIMESPIKE_END,
    quint64        m_nSpikeStart;              //CC_SPIKE_START,
    quint64        m_nSpikeEnd;                //CC_SPIKE_END,

    // Nsx values
    quint64        m_nElectrodeID;             //CC_NSX_ELECTRODEID
    quint64        m_nConnector;               //CC_NSX_CONNECTOR
    quint64        m_nPin;                     //CC_NSX_PIN
    QString        m_sLabel;                   //CC_NSX_LABEL
    quint64        m_nNSxCutoffFreq;           //CC_NSX_CUTOFF_FREQ
    quint64        m_nOrder;                   //CC_NSX_ORDER
    quint64        m_nType;                    //CC_NSX_TYPE

    // list of units to compare so no signal is sent unless they change
    QStringList    m_usedUnits;

    // file/back end state data
    bool           m_bCdFileInitialized;       //CC_CDFILE_INIT
    bool           m_bSdFileInitialized;       //CC_SDFILE_INIT

    //state variables
    bool           m_bDetected;                //CC_DETECTED
    bool           m_bDisplayed;               //CC_DISPLAYED
    ChannelState   m_eState;                   //CC_STATE

    QString        m_sLastError;               //CC_LAST_ERROR
    
    // File Separators
    QStringList m_fileSeparators;              //CC_FILE_SEPARATORS

    ProcessQ      *m_processQueue;
    SpikeData     *m_spikeData;

    quint32            m_nChangeCounter;
    EditActionManager  m_editActionManager; // for undoable operations

    QString getColumnName(ChannelColumn column);
};

#endif // CHANNELDATA_H_INCLUDED


