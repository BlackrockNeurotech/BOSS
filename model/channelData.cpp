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

#include "../model/ChannelData.h"
#include "../model/channelconfigmodel.h"
#include "../model/configmodel.h"

QPixmap *ChannelData::m_displayPixmap;
QPixmap *ChannelData::m_displaySortPixmap;
QPixmap *ChannelData::m_displayDetectPixmap;
QPixmap *ChannelData::m_displayAlignPixmap;
QPixmap *ChannelData::m_selectPixmap;
QPixmap *ChannelData::m_detectPixmap;
QPixmap *ChannelData::m_sortPixmap;
QPixmap *ChannelData::m_alignPixmap;
QPixmap *ChannelData::m_blankPixmap;

/// @note
/// header string data for each column in a ChannelData tree item
/* static */
QStringList ChannelData::ColumnNames = QStringList()
    /* General */
    << "" << "Name" << "Waveforms" << "  Units  " << "Interval" << "Start" << "End"

    /* State */
    << "Samples" << "Page" << "Sample Rate"

    /* Preprocessing */
    << "HP Filter" << "HP Order" << "Cutoff Freq" << "Max Cutoff Freq" << "SE" << "SE Width" << "Artifacts"

    /* Threshold */
    << "Threshold" << "Symetrical" << "RMS Mult" << "T 1" << "T 2" << "T Energy" << "T1Set" << "T2Set" << "TESet"

    /* Waveform Duration */
    << "Prethreshold" << "WF Length" << "Refractory Period"

    /* Features */
    << "Sort Features" << "Feature vs Feature"

    /*  Sorting */
    << "Sort Method" << "3D x-axis" << "3D y-axis" << "3D z-axis" << "3D Edit State"
    << "2D x-axis" << "2D y-axis" << "Nbr Units to Sort" << "Penalty Factor" << "centroids"

    /* Waveform Editing */
    << "Waveform Alignment" << "Max Shift" << "Units Aligned" << "Units Visible" << "Edit Lines" << "Edit Boxes" << "Edit Mode"

    /* Nev values */
    << "Valid Waveforms" << "Invalid Waveforms" << "Samples per Waveform"

    /* timeline range - for syncronizing timeline signal energy and timeline continuous */
    << "visible Start" << "Visible End" << "visible Spike Start" << "Visible Spike End" << "Timeline Vertical Range" << "Signal Energy Vertical Range"
    << "Timeline Start" << "Timeline End" << "Timespike Start" << "Timespike End" << "Spike Start" << "Spike End"

    /* Nsx values from the extended headers in the nsx file */
    << "Electrode" << "Connector" << "Pin" << "Label" << "File Cutoff Freq" << "Order" << "Type"

    /* CdFile & SdFile initialization status */
    << "CDFile Init" << "SdFile Init"

    /* Spike Data*/
    << "Features" << "Units" << "Clustering Matrix"

    /*State variables*/
    << "Detected"

    /*State variables*/
    << "Displayed"

    /*Error handling*/
    << "Last Error"

    /* File Separators*/
    << "Separators"

    /*Undo-Redo*/
    << "Change Counter"

    /* Interval data */
    << "Interval";

/// @note
/// Following are names for each of the enums used in ChannelData values
/// The names are used for displaying information to the user
///
/* static */
QStringList ChannelData::WizardPageNames = QStringList()
    << "Detect" << "Sort" << "Publish";

/* static */
QStringList ChannelData::HighPassFilterNames = QStringList()
    << "None" << "Butterworth" << "Chebyshev1" << "Bessel";

/* static */
QStringList ChannelData::SignalEnergyNames = QStringList()
    << "None" << "SignalEnergy" << "NonLinearSignalEnergy" << "SignedSignalEnergy" << "Template";

/* static */
QStringList ChannelData::FeatureIDNames = QStringList()
    << "PCA 1" << "PCA 2" << "PCA 3" << "PCA 4" << "PCA 5" << "PCA 6" << "PCA 7" << "PCA 8" << "PCA 9" << "PCA 10"
    << "PeakValue" << "ValleyValue" << "TimeToMax" << "TimeToMin" << "Area" << "PeakFWHM" << "ValleyFWHM" << "Time";

/* static */
QStringList ChannelData::FeatureNames = QStringList()
    << "PCAComponents"<< "PeakValue" << "ValleyValue" << "TimeToMax"
    << "TimeToMin" << "Area" << "PeakFWHM" << "ValleyFWHM";

/* static */
QStringList ChannelData::ThresholdTypeNames = QStringList()
    << "Single" << "Dual" << "NoiseRMS" << "Energy" << "None";

/* static */
QStringList ChannelData::SortMethodNames = QStringList()
    << "Kmeans" << "NNearestNeighbors" << "TDistributionEM" << "Template" << "ValleySeeking" << "None";

/* static */
QStringList ChannelData::AlignModeNames = QStringList()
    << "Original" << "Global Min" << "Global Max";  //<< "First Local Min" << "First Local Max" << "First Local Extremum" << "First Local Extremum After Threshold" 

/* static */
QStringList ChannelData::EditModeNames = QStringList()
<< "Select" << "Lines";// << "Boxes";

/* static */
QStringList ChannelData::GLWidgetStateNames = QStringList()
    << "Rotate" << "Edit" << "Clusters";


/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Constructor for ChannelData class - initializes class variables
/// @param [in]  parent tree item
ChannelData::ChannelData(QString sName, channelItem *parent)
    : channelItem(parent)
{
    // General
    m_sName = "";
    m_nTimeStampMin = 0;
    m_nTimeStampMax = 200;
    m_nCurrentInterval = 0;

    // State
    m_nSamples = 0;
    m_nWaveforms = 0;
    m_ePage = WIZARDPAGE_DETECT;
    m_nSampleRate = 30000;

    // Preprocessing
    m_eHighpassFilter = HIGHPASS_BUTTERWORTH;
    m_nFilterOrder = 4;
    m_nCutoffFreq = 250;
    m_nMaxCutoffFreq = 250;
    m_eSignalEnergy = ADDITIONAL_NONE;
    m_eLastThreshold = THRESHOLDTYPE_SINGLE;
    m_nWindowWidth = 15;
    m_bArtifactSubtraction = false;

    // Threshold
    m_eThreshold = THRESHOLDTYPE_SINGLE;
    m_bThresholdSymetrical = false;
    m_nRMSMultiplier = 3.75;
    m_nThreshold1 = 0;
    m_nThreshold2 = 0;
    m_nEnergyThreshold = 20000;
    m_bThreshold1Set = false;
    m_bThreshold2Set = false;
    m_bThresholdESet = false;

    // Waveform Duration
    m_nWaveformPrethreshold = 333;
    setWaveformLength(1600);
    m_nWaveformDeadTime = 1000;

    // Features
    m_nFeaturesSelected = 0X7;
    // m_mapSelectedFeatures;   leave the map empty - it is filled in when the correlation matrix is ready

    //  Sorting
    m_eSortMethod = SORTMETHOD_KMEANS;
    m_eThreeDXAxis = FEATUREID_PCA1;
    m_eThreeDYAxis = FEATUREID_PCA2;
    m_eThreeDZAxis = FEATUREID_PCA3;
    m_eThreeDMode = GLWIDGETSTATE_ROTATE;
    m_eTwoDXAxis = FEATUREID_PCA1;
    m_eTwoDYAxis = FEATUREID_PCA2;
    m_nSortUnits = 2;
    m_fPenaltyFactor = 1.0;

    // Waveform Editing
    m_eAlignMode = ALIGNMODE_ORIGINAL;
    m_nMaxShift = 1;
    m_UnitsToAlign.push_back("0");
    m_UnitsVisible.push_back("0");
    m_pEditLines = new EditLines();
    m_pEditBoxes = new EditLines();
    m_eEditMode = EDITMODE_SELECT;

    // Nev values
    m_nValidWaveforms = 0;
    m_nInvalidWaveforms = 0;
    m_nSamplesPerWaveform = 0;

    // timeline range - for syncronizing timeline signal energy and timeline continuous
    // This holds the currently visible timeline range which changes dynamically as the time line is zoomed
    m_nTimelineVisibleStart = 0;
    m_nTimelineVisibleEnd = DEFAULT_TIMELINE_SAMPLES;
    m_nTimespikeVisibleStart = 0;
    m_nTimespikeVisibleEnd = 0;
    m_nTimelineVerticalRange = 0;
    m_nSEVerticalRange = 0;

    // timeline range - specified by the current position of the scroll range slider
    m_nTimelineStart = 0;
    m_nTimelineEnd = DEFAULT_TIMELINE_SAMPLES;
    m_nTimespikeStart = 0;
    m_nTimespikeEnd = DEFAULT_TIMELINE_SAMPLES;
    m_nSpikeStart = 0;
    m_nSpikeEnd = DEFAULT_TIMELINE_SPIKES;

    // Nsx values
    m_nElectrodeID = 0;
    m_nConnector = 0;
    m_nPin = 0;
    m_sLabel = sName;
    m_nNSxCutoffFreq = 0;
    m_nOrder = 0;
    m_nType = 0;

    m_bCdFileInitialized = false;
    m_bSdFileInitialized = false;
    m_sLastError = "";

    m_processQueue = new ProcessQ();
    m_spikeData = new SpikeData();
    m_clusterData = new ClusterData();

    m_bDetected = false;
    m_bDisplayed = false;
    m_eState = CHANNELSTATE_INACTIVE;

    m_nChangeCounter = 0;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Destructor for ChannelData class
ChannelData::~ChannelData()
{
    delete m_processQueue;
    delete m_spikeData;
    delete m_clusterData;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  set the channel number in the process queue and the waveform characteristics
void ChannelData::initialize(int sampleRate)
{
    m_processQueue->setChannel(row());

    if (sampleRate != 0)
    {
        //int prethresholdms = 1000 * 1000 * 24 / sampleRate;
        quint64 cf = sampleRate / 4;

        if (cf > 250)
            cf = 250;

        setSampleRate(QVariant(sampleRate));
        setCutoffFreq(QVariant(cf));

        //m_nWaveformLength = prethresholdms * 2;
        //m_nWaveformPrethreshold = prethresholdms;
        //m_nWaveformDeadTime = prethresholdms;
    }
}


/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  get the number of columns belonging to this tree item
/// @return column count
int ChannelData::columnCount() const
{
    return CC_END;
}

/// @author Holly Ausbeck
/// @date   June 12, 2015
/// @brief  return true if this channel has data
/// @return bool has samples or waveforms
bool ChannelData::enabled()
{
    // only enable channels that have data or are not extracted
	// the !extracted is so the user can click ahead on an unextracted channel and it will be extracted next
    if ((m_nSamples != 0) || (m_nWaveforms != 0) || !m_bSdFileInitialized)
    {
        // if detecting or sorting is in progress, disable the channel
        if ((m_eState == CHANNELSTATE_INACTIVE) || (m_eState == CHANNELSTATE_SELECTED))
            return true;
    }
    return false;
}

GroupSelection ChannelData::getGroupSelection(int nGroupSize, QString sText)
{
    m_groupSelection.setGroupSize(nGroupSize);
    m_groupSelection.setState(m_eState);
    m_groupSelection.setDisplayed(m_bDisplayed);
    m_groupSelection.setText(sText);

    if (!m_bDisplayed)
        m_groupSelection.setApplyOptions(-1);

    return m_groupSelection;
}

void ChannelData::setGroupSelection(GroupSelection groupSelection)
{
    m_eState = groupSelection.getState();
    m_bDisplayed = groupSelection.getDisplayed();
    m_groupSelection = groupSelection;
}


/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  return the QVariant value of the data at the requested column for the role
/// @param [in] column number
/// @param [in] role
/// @return QVariant containing the value
QVariant ChannelData::data(int column, int role) const
{
    QVariant v = QVariant();

    if (role == Qt::SizeHintRole)
    {
        ChannelConfigModel *ccm = ChannelConfigModel::instance();
        int nGroupSize = ccm->rootData(RC_GROUP_SIZE, Qt::UserRole).toInt();

        switch (column)
        {
        case CC_STATE:
            v = QSize(m_displayPixmap->width(), m_displayPixmap->height() * nGroupSize);
        }
    }
    if ((role != Qt::DisplayRole) && (role != Qt::EditRole) && (role != Qt::UserRole))
        return QVariant();

    switch (column)
    {
    case CC_STATE:
        if (role == Qt::UserRole)
        {
            v = QVariant(m_eState);
        }
        else if (role == Qt::DisplayRole)
        {
            switch (m_eState)
            {
            case CHANNELSTATE_INACTIVE:
            case CHANNELSTATE_SELECTED:
                v = QVariant();
                break;
            case CHANNELSTATE_DETECTING:
            case CHANNELSTATE_SORTING:
            case CHANNELSTATE_ALIGNING:
                v = QString("...");
                break;
            }
        }
        break;
    case CC_NAME:
        v = QVariant(m_sName);
        break;
    case CC_CURRENT_INTERVAL:
        v = getUInt64(m_nCurrentInterval, role);
        break;
    case CC_FIRST_TIME:
        v = getUInt64(m_nTimeStampMin, role);
        break;
    case CC_LAST_TIME:
        v = getUInt64(m_nTimeStampMax, role);
        break;
    case CC_NSAMPLES:
        v = getUInt64(m_nSamples, role);
        break;
    case CC_NWAVEFORMS:
        v = getUInt64(m_nWaveforms, role);
        break;
    case CC_PAGE:
        v = getEnum(&WizardPageNames, (int)m_ePage, role);
        break;
    case CC_SAMPLERATE:
        v = getUInt64(m_nSampleRate, role);
        break;
    case CC_HIGHPASS_FILTER:
        v = getEnum(&HighPassFilterNames, (int)m_eHighpassFilter, role);
        break;
    case CC_FILTER_ORDER:
        v = getUInt64(m_nFilterOrder, role);
        break;
    case CC_CUTOFF_FREQ:
        v = getUInt64(m_nCutoffFreq, role);
        break;
    case CC_MAX_CUTOFF_FREQ:
        v = getUInt64(m_nMaxCutoffFreq, role);
        break;
    case CC_SIGNAL_ENERGY:
        v = getEnum(&SignalEnergyNames, (int)m_eSignalEnergy, role);
        break;
    case CC_SE_WIDTH:
        v = getUInt64(m_nWindowWidth, role);
        break;
    case CC_ARTIFACT_SUBTRACTION:
        v = getBool(m_bArtifactSubtraction, role);
        break;
    case CC_THRESHOLD:
        v = getEnum(&ThresholdTypeNames, (int)m_eThreshold, role);
        break;
    case CC_SYMETRICAL_THRESHOLD:
        v = getBool(m_bThresholdSymetrical, role);
        break;
    case CC_RMS_MULTIPLIER:
        v = getDouble(m_nRMSMultiplier, role);
        break;
    case CC_THRESHOLD1:
        v = getInt64(m_nThreshold1, role);
        break;
    case CC_THRESHOLD2:
        v = getInt64(m_nThreshold2, role);
        break;
    case CC_ENERGY_THRESHOLD:
        v = getInt64(m_nEnergyThreshold, role);
        break;
    case CC_THRESHOLD1_SET:
        v = getBool(m_bThreshold1Set, role);
        break;
    case CC_THRESHOLD2_SET:
        v = getBool(m_bThreshold2Set, role);
        break;
    case CC_THRESHOLDE_SET:
        v = getBool(m_bThresholdESet, role);
        break;
    case CC_WF_PRETHRESHOLD:
        v = getUInt64(m_nWaveformPrethreshold, role);
        break;
    case CC_WF_LENGTH:
        v = getUInt64(m_nWaveformLength, role);
        break;
    case CC_DEAD_TIME:
        v = getUInt64(m_nWaveformDeadTime, role);
        break;
    case CC_FEATURE:
        v = getBitMask(&FeatureIDNames, m_nFeaturesSelected, role);
        break;
    case CC_SELECTED_FEATURES:
        v = QVariant(m_mapSelectedFeatures);
        break;
    case CC_SORT_METHOD:
        v = getEnum(&SortMethodNames, (int)m_eSortMethod, role);
        break;
    case CC_X_3DAXIS:
        v = getEnum(&FeatureIDNames, (int)m_eThreeDXAxis, role);
        break;
    case CC_Y_3DAXIS:
        v = getEnum(&FeatureIDNames, (int)m_eThreeDYAxis, role);
        break;
    case CC_Z_3DAXIS:
        v = getEnum(&FeatureIDNames, (int)m_eThreeDZAxis, role);
        break;
    case CC_3D_EDIT_STATE:
        v = getEnum(&GLWidgetStateNames, (int)m_eThreeDMode, role);
        break;
    case CC_X_2DAXIS:
        v = getEnum(&FeatureIDNames, (int)m_eTwoDXAxis, role);
        break;
    case CC_Y_2DAXIS:
        v = getEnum(&FeatureIDNames, (int)m_eTwoDYAxis, role);
        break;
    case CC_NBR_SORT_UNITS:
        v = getUInt64(m_nSortUnits, role);
        break;
    case CC_PENALTY_FACTOR:
        v = getDouble(m_fPenaltyFactor, role);
        break;
    case CC_CENTROIDS:
        v.setValue(m_clusterData);
        break;
    case CC_WAVEFORM_ALIGNMENT:
        v = getEnum(&AlignModeNames, (int)m_eAlignMode, role);
        break;
    case CC_MAX_SHIFT:
        v = getUInt64(m_nMaxShift, role);
        break;
    case CC_UNITS_ALIGN:
        v = QVariant(m_UnitsToAlign);
        break;
    case CC_UNITS_VISIBLE:
        v = getVisibleUnits(role);
        break;
    case CC_EDIT_LINES:
        v.setValue(m_pEditLines);
        break;
    case CC_EDIT_BOXES:
        v.setValue(m_pEditBoxes);
        break;
    case CC_EDIT_MODE:
        v = getEnum(&EditModeNames, (int)m_eEditMode, role);
        break;
    case CC_VALID_WAVEFORMS:
        v = getUInt64(m_nValidWaveforms, role);
        break;
    case CC_INVALID_WAVEFORMS:
        v = getUInt64(m_nInvalidWaveforms, role);
        break;
    case CC_NBR_SAMPLES_IN_WAVEFORM:
        v = getUInt64(m_nSamplesPerWaveform, role);
        break;
    case CC_TIMELINE_VISIBLE_START:
        v = getUInt64(m_nTimelineVisibleStart, role);
        break;
    case CC_TIMELINE_VISIBLE_END:
        v = getUInt64(m_nTimelineVisibleEnd, role);
        break;
    case CC_TIMESPIKE_VISIBLE_START:
        v = getUInt64(m_nTimespikeVisibleStart, role);
        break;
    case CC_TIMESPIKE_VISIBLE_END:
        v = getUInt64(m_nTimespikeVisibleEnd, role);
        break;
    case CC_TIMELINE_VERTICAL_RANGE:
        v = getUInt64(m_nTimelineVerticalRange, role);
        break;
    case CC_SE_VERTICAL_RANGE:
        v = getUInt64(m_nSEVerticalRange, role);
        break;
    case CC_TIMELINE_START:
        v = getUInt64(m_nTimelineStart, role);
        break;
    case CC_TIMELINE_END:
        v = getUInt64(m_nTimelineEnd, role);
        break;
    case CC_TIMESPIKE_START:
        v = getUInt64(m_nTimespikeStart, role);
        break;
    case CC_TIMESPIKE_END:
        v = getUInt64(m_nTimespikeEnd, role);
        break;
    case CC_SPIKE_START:
        v = getUInt64(m_nSpikeStart, role);
        break;
    case CC_SPIKE_END:
        v = getUInt64(m_nSpikeEnd, role);
        break;
    case CC_NSX_ELECTRODEID:
        v = getUInt64(m_nElectrodeID, role);
        break;
    case CC_NSX_CONNECTOR:
        v = getUInt64(m_nConnector, role);
        break;
    case CC_NSX_PIN:
        v = getUInt64(m_nPin, role);
        break;
    case CC_NSX_LABEL:
        v = QVariant(m_sLabel);
        break;
    case CC_NSX_CUTOFF_FREQ:
        v = getUInt64(m_nNSxCutoffFreq, role);
        break;
    case CC_NSX_ORDER:
        v = getUInt64(m_nOrder, role);
        break;
    case CC_NSX_TYPE:
        v = getUInt64(m_nType, role);
        break;
    case CC_CDFILE_INIT:
        v = getBool(m_bCdFileInitialized, role);
        break;
    case CC_SDFILE_INIT:
        v = getBool(m_bSdFileInitialized, role);
        break;
    case CC_SPIKEDATA_FEATURES:
        v.setValue(m_spikeData->features());
        break;
    case CC_SPIKEDATA_UNITS:
        v.setValue(m_spikeData->units());
        break;
    case CC_NUNITS:
        if (m_spikeData->units())
            v.setValue(m_spikeData->units()->getNumberUnits());
        break;
    case CC_SPIKEDATA_CLUSTERING_MATRIX:
        v.setValue(m_spikeData->clusteringTendency());
        break;
    case CC_DETECTED:
        v = getBool(m_bDetected, role);
        break;
    case CC_DISPLAYED:
        v = getBool(m_bDisplayed, role);
        break;
    case CC_LAST_ERROR:
        v = QVariant(m_sLastError);
        break;
    case CC_FILE_SEPARATORS:
        v = QVariant(m_fileSeparators);
        break;
    case CC_CHANGE_COUNTER:
        v = QVariant(m_nChangeCounter);
        break;
    }
    return v;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  set the value of the data at the requested column
/// @param [in] column number
/// @param [in] value in the form of a QVariant
/// @param [in] address of the signal type to emit if anyone wants to know about the change
/// @param [in] role
/// @return TRUE if the value changed; FALSE otherwise
bool ChannelData::setData(int column, const QVariant &value, int role)
{
    // if we're detecting or sorting, don't allow changes on this channel
//    if ((m_eState != CHANNELSTATE_INACTIVE) && (m_eState != CHANNELSTATE_SELECTED))
//        return false;

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    bool changed = false;

    switch (column)
    {
    case CC_STATE:
        //if (m_bDisplayed && (CHANNELSTATE_INACTIVE == (ChannelState)value.toInt()))
        //    break;

        m_eState = (ChannelState)value.toInt();
//        ccm->emitSignal(row(), SIGNAL_DATA_CHANGED, QVariant(CC_STATE));
        ccm->emitSignal(row(), SIGNAL_CHANNEL_SELECTION_CHANGED); 
        ccm->emitSignal(row(), SIGNAL_STATE_CHANGED, value);
        break;
    case CC_NAME:
        if (m_sName != value.toString())
        {
            m_sName = value.toString();
            changed = true;
        }
        break;
    case CC_CURRENT_INTERVAL:
        if ((value >= 0) && (value < this->childCount()))
        {
            changed = setUInt64(&m_nCurrentInterval, value);

            if (changed)
            {
                emitSignalIfDisplayed(row(), SIGNAL_WIZARDPAGE_CHANGED, QVariant(m_ePage));
            }
        }
        break;
    case CC_FIRST_TIME:
        changed = setUInt64(&m_nTimeStampMin, value);
        break;
    case CC_LAST_TIME:
        changed = setUInt64(&m_nTimeStampMax, value);
        break;
    case CC_NSAMPLES:
        changed = setUInt64(&m_nSamples, value);
        break;
    case CC_NWAVEFORMS:
        changed = setUInt64(&m_nWaveforms, value);
        break;
    case CC_PAGE:
        if (switchPages(value, role))
        {
            emitSignalIfDisplayed(row(), SIGNAL_WIZARDPAGE_CHANGED, QVariant(m_ePage));
            changed = true;
            addUpdateRequest();
        }
        break;
    case CC_SAMPLERATE:
        changed = setSampleRate(value);
        break;
    case CC_HIGHPASS_FILTER:
        if (setEnum(&HighPassFilterNames, (int*)&m_eHighpassFilter, value, role))
        {
            emitSignalIfDisplayed(row(), SIGNAL_FILTERING_CHANGED);
            addUpdateFilterRequest();
            changed = true;
        }
        break;
    case CC_FILTER_ORDER:
        if (setUInt64(&m_nFilterOrder, value))
        {
            emitSignalIfDisplayed(row(), SIGNAL_FILTERING_CHANGED);
            addUpdateFilterRequest();
            changed = true;
        }
        break;
    case CC_CUTOFF_FREQ:
        if (setCutoffFreq(value))
        {
            emitSignalIfDisplayed(row(), SIGNAL_FILTERING_CHANGED);
            addUpdateFilterRequest();
            changed = true;
        }
        break;
    case CC_MAX_CUTOFF_FREQ:
        changed = setUInt64(&m_nMaxCutoffFreq, value);
        break;
    case CC_SIGNAL_ENERGY:
        if (setEnum(&SignalEnergyNames, (int*)&m_eSignalEnergy, value, role))
        {
            // if signal energy was turned off, restore the threshold type to the previous type
            QVariant thresholdType = (m_eSignalEnergy == ADDITIONAL_NONE) ? QVariant(m_eLastThreshold) : QVariant(THRESHOLDTYPE_ENERGY);

            // Save the last threshold type if it wasn't signal energy so it can be restored.
            if (m_eThreshold != THRESHOLDTYPE_ENERGY)
                m_eLastThreshold = m_eThreshold;

            // set the threshold type to signal energy or restore the previous type.
            if (setEnum(&ThresholdTypeNames, (int*)&m_eThreshold, thresholdType, Qt::UserRole))
            {
                // if the threshold type is signal energy, set the threshold value.
                if (m_eSignalEnergy != ADDITIONAL_NONE)
                    setEnergyThreshold(QVariant(m_nEnergyThreshold));

                emitSignalIfDisplayed(row(), SIGNAL_THRESHOLD_TYPE_CHANGED);
                emitSignalIfDisplayed(row(), SIGNAL_DATA_CHANGED, QVariant(CC_THRESHOLD));
            }
            addUpdateFilterRequest();
            changed = true;
        }
        emitSignalIfDisplayed(row(), SIGNAL_ENERGY_TYPE_CHANGED);
        break;
    case CC_SE_WIDTH:
        if (setUInt64(&m_nWindowWidth, value))
        {
            addUpdateFilterRequest();
            emitSignalIfDisplayed(row(), SIGNAL_FILTERING_CHANGED);
            changed = true;
        }
        break;
    case CC_ARTIFACT_SUBTRACTION:
        if (m_bArtifactSubtraction != value.toBool())
        {
            m_bArtifactSubtraction = value.toBool();
            changed = true;
        }
        break;
    case CC_THRESHOLD:
        {
            ThresholdType eLastThreshold = m_eThreshold;

            if (setEnum(&ThresholdTypeNames, (int*)&m_eThreshold, value, role))
            {
                // if we just switched to dual threshold, make sure threshold1 positive
                if (m_eThreshold == THRESHOLDTYPE_DUAL)
                {
                    if (m_nThreshold1 < 0)
                    {
                        setThreshold2(abs(m_nThreshold2));
                        emitSignalIfDisplayed(row(), SIGNAL_DATA_CHANGED, QVariant(CC_THRESHOLD2));
                    }
                    else if ((m_nThreshold1 >= 0) && (m_nThreshold2 >= 0))
                    {
                        setThreshold2(m_nThreshold2 * -1);
                        emitSignalIfDisplayed(row(), SIGNAL_DATA_CHANGED, QVariant(CC_THRESHOLD2));
                    }
                }
            }
            emitSignalIfDisplayed(row(), SIGNAL_THRESHOLD_TYPE_CHANGED);
            changed = true;
    }
        break;
    case CC_SYMETRICAL_THRESHOLD:
        if (m_bThresholdSymetrical != value.toBool())
        {
            m_bThresholdSymetrical = value.toBool();
            setThreshold1(m_nThreshold1);
            //            ccm->emitSignal(row(), SIGNAL_THRESHOLD_TYPE_CHANGED);
            changed = true;
        }
        break;
    case CC_RMS_MULTIPLIER:
        if (m_nRMSMultiplier != value.toDouble())
        {
            m_nRMSMultiplier = value.toDouble();
            emitSignalIfDisplayed(row(), SIGNAL_THRESHOLD_TYPE_CHANGED);
            changed = true;
        }
        break;
    case CC_THRESHOLD1:
        if (setThreshold1(value))
            changed = true;
        break;
    case CC_THRESHOLD2:
        if (setThreshold2(value))
            changed = true;
        break;
    case CC_ENERGY_THRESHOLD:
        changed = setEnergyThreshold(value);
        break;
    case CC_WF_PRETHRESHOLD:
        changed = setWaveformPrethreshold(value);
        break;
    case CC_WF_LENGTH:
        changed = setWaveformLength(value, role);
        break;
    case CC_DEAD_TIME:
        changed = setUInt64(&m_nWaveformDeadTime, value);
        break;
    case CC_FEATURE:
        if (setBitMask(&FeatureIDNames, (uint*)&m_nFeaturesSelected, value, role))
        {
            emitSignalIfDisplayed(row(), SIGNAL_SORT_FEATURES_CHANGED);
            changed = true;
        }
        break;
    case CC_SELECTED_FEATURES:
        if (m_mapSelectedFeatures != value.toMap())
        {
            m_mapSelectedFeatures = value.toMap();
            emitSignalIfDisplayed(row(), SIGNAL_VIEW_FEATURES_CHANGED);
            changed = true;
        }
        break;
    case CC_SORT_METHOD:
        changed = setEnum(&SortMethodNames, (int*)&m_eSortMethod, value, role);

        if (changed)
        {
            if ((m_eSortMethod != SORTMETHOD_KMEANS) && (m_eThreeDMode == GLWIDGETSTATE_CENTROIDS))
            {
                m_eThreeDMode = GLWIDGETSTATE_ROTATE;
                emitSignalIfDisplayed(row(), SIGNAL_DATA_CHANGED, QVariant(CC_3D_EDIT_STATE));
            }
            emitSignalIfDisplayed(row(), SIGNAL_3D_EDIT_STATE_CHANGED, QVariant(m_eThreeDMode));
        }
        break;
    case CC_X_3DAXIS:
        if (setEnum(&FeatureIDNames, (int*)&m_eThreeDXAxis, value, role))
        {
            emitSignalIfDisplayed(row(), SIGNAL_3D_AXIS_CHANGED, QVariant(column), QVariant(m_eThreeDXAxis));
            changed = true;
        }
        break;
    case CC_Y_3DAXIS:
        if (setEnum(&FeatureIDNames, (int*)&m_eThreeDYAxis, value, role))
        {
            emitSignalIfDisplayed(row(), SIGNAL_3D_AXIS_CHANGED, QVariant(column), QVariant(m_eThreeDYAxis));
            changed = true;
        }
        break;
    case CC_Z_3DAXIS:
        if (setEnum(&FeatureIDNames, (int*)&m_eThreeDZAxis, value, role))
        {
            emitSignalIfDisplayed(row(), SIGNAL_3D_AXIS_CHANGED, QVariant(column), QVariant(m_eThreeDZAxis));
            changed = true;
        }
        break;
    case CC_3D_EDIT_STATE:
        if (setEnum(&GLWidgetStateNames, (int*)&m_eThreeDMode, value, role))
        {
            if (m_eThreeDMode == GLWIDGETSTATE_CENTROIDS)
            {
                if (m_eSortMethod != SORTMETHOD_KMEANS)
                    break;

                clearSelection();
            }
            m_clusterData->clearClusters();
            emitSignalIfDisplayed(row(), SIGNAL_3D_EDIT_STATE_CHANGED, QVariant(m_eThreeDMode));
            emitSignalIfDisplayed(row(), SIGNAL_DATA_CHANGED, QVariant(CC_3D_EDIT_STATE));
            changed = true;
        }
        break;
    case CC_X_2DAXIS:
        if (setEnum(&FeatureIDNames, (int*)&m_eTwoDXAxis, value, role))
        {
            emitSignalIfDisplayed(row(), SIGNAL_2D_AXIS_CHANGED, QVariant(column), QVariant(m_eTwoDXAxis));
            changed = true;
        }
        break;
    case CC_Y_2DAXIS:
        if (setEnum(&FeatureIDNames, (int*)&m_eTwoDYAxis, value, role))
        {
            emitSignalIfDisplayed(row(), SIGNAL_2D_AXIS_CHANGED, QVariant(column), QVariant(m_eTwoDYAxis));
            changed = true;
        }
        break;
    case CC_NBR_SORT_UNITS:
        changed = setUInt64(&m_nSortUnits, value);
        break;
    case CC_PENALTY_FACTOR:
        if (m_fPenaltyFactor != value.toDouble())
        {
                m_fPenaltyFactor = value.toDouble();
                changed = true;
        }
        break;
    case CC_CENTROIDS:
        break;
    case CC_WAVEFORM_ALIGNMENT:
         changed = setEnum(&AlignModeNames, (int*)&m_eAlignMode, value, role);
        break;
    case CC_MAX_SHIFT:
        changed = setUInt64(&m_nMaxShift, value);
        break;
    case CC_UNITS_ALIGN:
        if (m_UnitsToAlign != value.toStringList())
        {
            m_UnitsToAlign = value.toStringList();
            changed = true;
        }
        break;
    case CC_UNITS_VISIBLE:
        setVisibleUnits(value, role);
        emitSignalIfDisplayed(row(), SIGNAL_VISIBLE_UNITS_CHANGED, value);
        emitSignalIfDisplayed(row(), SIGNAL_DATA_CHANGED, QVariant(CC_UNITS_ALIGN));
        changed = true;
        break;
    case CC_EDIT_LINES:
        // individual lines must be set through the EditLines API
        // signal emitted from class EditLines
        break;
    case CC_EDIT_BOXES:
        // individual lines must be set through the EditBoxes API
        // signal emitted from class EditLines
        break;
    case CC_EDIT_MODE:
        if (setEnum(&EditModeNames, (int*)&m_eEditMode, value, role))
        {
            if (m_spikeData)
            {
                clearSelection();
            }
            emitSignalIfDisplayed(row(), SIGNAL_WAVEFORM_EDIT_MODE_CHANGED);
            changed = true;
        }
        break;
    case CC_VALID_WAVEFORMS:
        changed = setUInt64(&m_nValidWaveforms, value);
        break;
    case CC_INVALID_WAVEFORMS:
        changed = setUInt64(&m_nInvalidWaveforms, value);
        m_nValidWaveforms = m_nWaveforms - m_nInvalidWaveforms;
        break;
    case CC_NBR_SAMPLES_IN_WAVEFORM:
        changed = setUInt64(&m_nSamplesPerWaveform, value);
        break;
    case CC_TIMELINE_VISIBLE_START:
        changed = setUInt64(&m_nTimelineVisibleStart, value);
        break;
    case CC_TIMELINE_VISIBLE_END:
        changed = setUInt64(&m_nTimelineVisibleEnd, value);
        break;
    case CC_TIMESPIKE_VISIBLE_START:
        changed = setUInt64(&m_nTimespikeVisibleStart, value);
        break;
    case CC_TIMESPIKE_VISIBLE_END:
        changed = setUInt64(&m_nTimespikeVisibleEnd, value);
        break;
    case CC_TIMELINE_VERTICAL_RANGE:
        changed = setUInt64(&m_nTimelineVerticalRange, value);

        if (changed)
            emitSignalIfDisplayed(row(), SIGNAL_VERTICAL_RANGE_CHANGED);
        break;
    case CC_SE_VERTICAL_RANGE:
        changed = setUInt64(&m_nSEVerticalRange, value);

        if (changed)
            emitSignalIfDisplayed(row(), SIGNAL_VERTICAL_RANGE_CHANGED);
        break;
    case CC_TIMELINE_START:
        changed = setUInt64(&m_nTimelineStart, value);
        break;
    case CC_TIMELINE_END:
        changed = setUInt64(&m_nTimelineEnd, value);
        break;
    case CC_TIMESPIKE_START:
        changed = setUInt64(&m_nTimespikeStart, value);
        break;
    case CC_TIMESPIKE_END:
        changed = setUInt64(&m_nTimespikeEnd, value);
        break;
    case CC_SPIKE_START:
        changed = setUInt64(&m_nSpikeStart, value);
        break;
    case CC_SPIKE_END:
        changed = setUInt64(&m_nSpikeEnd, value);
        break;
    case CC_NSX_ELECTRODEID:
        changed = setUInt64(&m_nElectrodeID, value);
        break;
    case CC_NSX_CONNECTOR:
        changed = setUInt64(&m_nConnector, value);
        break;
    case CC_NSX_PIN:
        changed = setUInt64(&m_nPin, value);
        break;
    case CC_NSX_LABEL:
        if (m_sLabel != value.toString())
        {
            m_sLabel = value.toString();
            changed = true;
        }
        break;
    case CC_NSX_CUTOFF_FREQ:
        changed = setUInt64(&m_nNSxCutoffFreq, value);
        break;
    case CC_NSX_ORDER:
        changed = setUInt64(&m_nOrder, value);
        break;
    case CC_NSX_TYPE:
        changed = setUInt64(&m_nType, value);
        break;
    case CC_CDFILE_INIT:
        m_bCdFileInitialized = value.toBool();
        break;
    case CC_SDFILE_INIT:
        m_bSdFileInitialized = value.toBool();
        if(ccm->CurrentChannel() == row())
            emitSignalIfDisplayed(row(), SIGNAL_SPIKES_INITIALIZED);
        break;
    case CC_SPIKEDATA_FEATURES:
        if (ccm->CurrentChannel() == row())
        {
            m_spikeData->setFeatures(value.value<FeaturesAccum>());
            emitSignalIfDisplayed(row(), SIGNAL_FEATURES_CHANGED);
        }
        break;
    case CC_SPIKEDATA_UNITS:
        if (ccm->CurrentChannel() == row())
        {
            bool bCorrelogramsUpdated = value.toBool();
            if (bCorrelogramsUpdated)
            {
                emitSignalIfDisplayed(row(), SIGNAL_CORRELOGRAMS_CHANGED);
            }
            else
            {
                if (getVisibleUnits(Qt::DisplayRole).toStringList() != m_usedUnits)
                {
                    m_usedUnits = getVisibleUnits(Qt::DisplayRole).toStringList();
                    emitSignalIfDisplayed(row(), SIGNAL_DATA_CHANGED, QVariant(CC_UNITS_VISIBLE));
                    emitSignalIfDisplayed(row(), SIGNAL_DATA_CHANGED, QVariant(CC_UNITS_ALIGN));
                    emitSignalIfDisplayed(row(), SIGNAL_VISIBLE_UNITS_CHANGED, getVisibleUnits(Qt::DisplayRole));
                }
                emitSignalIfDisplayed(row(), SIGNAL_UNITS_CHANGED);
            }            
        }
        break;
    case CC_SPIKEDATA_CLUSTERING_MATRIX:
        if (ccm->CurrentChannel() == row())
        {
            m_spikeData->setClusteringTendencyMatrix(value.value<ClusteringTendencyAccum>());
            emitSignalIfDisplayed(row(), SIGNAL_CLUSTERING_MATRIX_CHANGED);
        }        
        break;
    case CC_DETECTED:
        if (m_bDetected != value.toBool())
        {
            m_bDetected = value.toBool();
            changed = true;
        }
        break;
    case CC_DISPLAYED:
        if (m_bDisplayed != value.toBool())
        {
            m_bDisplayed = value.toBool();
            changed = true;
        }
        break;
    case CC_LAST_ERROR:
        if (m_sLastError != value.toString())
        {
            m_sLastError = value.toString();
            ccm->emitSignal(row(), SIGNAL_ERROR, QVariant(m_sLastError));
            changed = true;
        }
    case CC_FILE_SEPARATORS:
        m_fileSeparators = value.toStringList();
        break;
    case CC_CHANGE_COUNTER:
        m_nChangeCounter = value.toInt();
        break;
    }
    return changed;
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  Queue up a detect request
void ChannelData::addDetectRequest()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    m_eState = CHANNELSTATE_DETECTING;
    // update the views
    ccm->emitSignal(row(), SIGNAL_STATE_CHANGED, QVariant(CHANNELSTATE_DETECTING));
    ccm->updateAll();

    m_processQueue->addRecalcRequest(REQUESTTYPE_DETECT);
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  Queue up a sort request
void ChannelData::addSortRequest()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    clearSelection();   // clear the waveform selection
    m_eState = CHANNELSTATE_SORTING;
    // update the views
    ccm->emitSignal(row(), SIGNAL_STATE_CHANGED, QVariant(CHANNELSTATE_SORTING));
    ccm->updateAll();

    m_processQueue->addRecalcRequest(REQUESTTYPE_SORT);
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  Queue up a move request for the timeline
/// @param [in] value time stamp
void ChannelData::addMoveRequest(quint64 nValue, SpikeMoveReference reference)
{
    if (m_ePage == WIZARDPAGE_DETECT)
    {
        setContinuousTimes(nValue, reference);
        m_processQueue->addRecalcRequest(REQUESTTYPE_CONTIN_MOVE, m_nTimelineStart, reference);
    }
    else if (m_ePage == WIZARDPAGE_SORT)
    {
        m_processQueue->addRecalcRequest(REQUESTTYPE_SPIKE_MOVE, nValue, reference);
    }
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  The maximum number of samples displayed changed, if it impacts this channel, update
void ChannelData::maxDisplayedSamplesChanged()
{
    initializeContinuousTimes();
    
    if (this->m_bDisplayed)
    {
        if (m_ePage == WIZARDPAGE_DETECT)
        {
            m_processQueue->addRecalcRequest(REQUESTTYPE_CONTIN_MOVE, m_nTimelineStart);
        }
    }
}

/// @author Holly Ausbeck
/// @date   May 26, 2016
/// @brief  The user changed the max number of waveforms displayed, if it impacts this channel, update
void ChannelData::maxDisplayedWaveformsChanged()
{
    ConfigModel *cm = ConfigModel::instance();
    quint64 nMaxWaveforms = cm->item(0, OFSMODEL_COLUMN_MAX_DISPLAY_WAVEFORMS)->data(Qt::DisplayRole).toULongLong();
    initializeSpikeTimes();

    if (this->m_bDisplayed)
    {
        if (m_ePage == WIZARDPAGE_SORT)
        {
            m_processQueue->addRecalcRequest(REQUESTTYPE_SPIKE_MOVE, m_nTimespikeStart, SPIKEMOVE_START);
        }
    }
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  Queue up an update request with the current data - used for page change
void ChannelData::addUpdateRequest()
{
    if (this->m_bDisplayed)
    {
        ConfigModel *cm = ConfigModel::instance();
        quint64 nSamples = cm->item(0, OFSMODEL_COLUMN_MAX_DISPLAY_SAMPLES)->data(Qt::DisplayRole).toULongLong();

        if (m_ePage <= WIZARDPAGE_DETECT)
        {
            m_processQueue->addRecalcRequest(REQUESTTYPE_UPDATE_CONTIN, m_nTimelineStart);
        }
        else
        {
            if ((m_nWaveforms != 0) || !m_bSdFileInitialized)
                m_processQueue->addRecalcRequest(REQUESTTYPE_UPDATE_SPIKE, m_nTimespikeStart, SPIKEMOVE_START);
        }
    }
}

void ChannelData::addUpdatePCARequest()
{
    if (this->m_bDisplayed)
        m_processQueue->addRecalcRequest(REQUESTTYPE_UPDATE_PCA);
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  if there is another move request, replace the request. else add the request
/// @param [in]  RequestType - move request type
/// @param [in]  nStart - beginning continuous data sample
/// @param [in]  nSamples - number of continuous data samples
void ChannelData::addUpdateFilterRequest()
{
    if (this->m_bDisplayed)
        m_processQueue->addRecalcRequest(REQUESTTYPE_FILTER, m_nTimelineStart);
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  The Backend is finished, Process the next request in the Process Queue
RequestType ChannelData::processNextRequest(bool bupdateLastRequest)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    RequestType type = m_processQueue->processNextRequest();
    quint64 nTimeStart;
    quint64 nTimeEnd;

    if (bupdateLastRequest || (type == REQUESTTYPE_DETECT) || (type == REQUESTTYPE_SORT))
    {
        // update variables
        switch (type)
        {
        case REQUESTTYPE_CONTIN_MOVE:
            // the user moved the file scroll bar. Change the visible time to zoomed out.
            m_nTimelineVisibleStart = 0;
            m_nTimelineVisibleEnd = m_nTimelineEnd - m_nTimelineStart;

            if (m_bDisplayed)
            {
                if (m_nWaveforms > 0)
                {
                    m_processQueue->addRecalcRequest(REQUESTTYPE_CONTIN_SPIKE_TS, m_nTimelineStart);
                }
                ccm->emitSignal(row(), SIGNAL_SLIDER_CHANGED, QVariant(SLIDERTYPE_TIME));
                ccm->emitSignal(row(), SIGNAL_VISIBLE_TIME_RANGE_CHANGED, QVariant(m_nTimelineVisibleStart), QVariant(m_nTimelineVisibleEnd));
            }
            break;

        case REQUESTTYPE_UPDATE_CONTIN:
            if (m_bDisplayed)
            {
                if (m_nWaveforms > 0)
                {
                    m_processQueue->addRecalcRequest(REQUESTTYPE_CONTIN_SPIKE_TS, m_nTimelineStart);
                    ccm->emitSignal(row(), SIGNAL_VISIBLE_TIME_RANGE_CHANGED, QVariant(m_nTimelineVisibleStart), QVariant(m_nTimelineVisibleEnd));
                }
            }
            break;

        case REQUESTTYPE_FILTER:
            if (m_bDisplayed)
            {
                if (m_nWaveforms > 0)
                {
                    m_processQueue->addRecalcRequest(REQUESTTYPE_CONTIN_SPIKE_TS, m_nTimelineStart);
                }
            }
            break;

        case REQUESTTYPE_CONTIN_SPIKE_TS:
            break;

        case REQUESTTYPE_SPIKE_MOVE:
            // the user moved the file scroll bar. Change the visible time to zoomed out.
            m_nTimespikeVisibleStart = 0;
            m_nTimespikeVisibleEnd = m_nTimespikeEnd - m_nTimespikeStart;

            if (m_bDisplayed && (m_nWaveforms > 0))
            {
                ccm->emitSignal(row(), SIGNAL_SLIDER_CHANGED, QVariant(SLIDERTYPE_TIME));
            }
            break;

        case REQUESTTYPE_UPDATE_SPIKE:
            //if (m_bDisplayed && (m_nWaveforms > 0))
            //{
            //    ccm->emitSignal(row(), SIGNAL_VISIBLE_SPIKE_TIME_RANGE_CHANGED, QVariant(m_nTimespikeVisibleStart), QVariant(m_nTimespikeVisibleEnd));
            //}
            break;

        case REQUESTTYPE_DETECT:
            if (m_bDisplayed)
                m_eState = CHANNELSTATE_SELECTED;
            else
                m_eState = CHANNELSTATE_INACTIVE;

            // update the views
            ccm->emitSignal(row(), SIGNAL_STATE_CHANGED, QVariant(m_eState));
            ccm->updateAll();

            if (m_bDisplayed && (m_nWaveforms > 0))
            {
                m_processQueue->addRecalcRequest(REQUESTTYPE_CONTIN_SPIKE_TS, m_nTimelineStart);
            }
            m_bDetected = true;
            break;

        case REQUESTTYPE_SORT:
            if (m_bDisplayed)
                m_eState = CHANNELSTATE_SELECTED;
            else
                m_eState = CHANNELSTATE_INACTIVE;

            // update the views
            ccm->emitSignal(row(), SIGNAL_STATE_CHANGED, QVariant(m_eState));
            // update the channel table
            ccm->updateAll();

            if (m_bDisplayed && (m_nWaveforms > 0))
            {
                // emit sorting finished, and update the units
                m_processQueue->addRecalcRequest(REQUESTTYPE_UPDATE_SPIKE, m_nTimespikeStart);
            }
            break;
        }
    }
    return type;
}

void ChannelData::copyDetectionSortingParametersFrom(ChannelData * other)
{
    m_eHighpassFilter       =     (HighpassFilter)other->data(CC_HIGHPASS_FILTER, Qt::UserRole).toInt();
    m_nFilterOrder          =                     other->data(CC_FILTER_ORDER, Qt::UserRole).toULongLong();
    m_nCutoffFreq           =                     other->data(CC_CUTOFF_FREQ, Qt::UserRole).toULongLong();
    m_nMaxCutoffFreq        =                     other->data(CC_MAX_CUTOFF_FREQ, Qt::UserRole).toULongLong();
    m_eSignalEnergy         =       (SignalEnergy)other->data(CC_SIGNAL_ENERGY, Qt::UserRole).toInt();
    m_nWindowWidth          =                     other->data(CC_SE_WIDTH, Qt::UserRole).toULongLong();
    m_bArtifactSubtraction  =                     other->data(CC_ARTIFACT_SUBTRACTION, Qt::UserRole).toBool();
    m_eThreshold            =      (ThresholdType)other->data(CC_THRESHOLD, Qt::UserRole).toInt();
    m_bThresholdSymetrical  =                     other->data(CC_SYMETRICAL_THRESHOLD, Qt::UserRole).toBool(); 
    m_nRMSMultiplier        =                     other->data(CC_RMS_MULTIPLIER, Qt::UserRole).toDouble();      
    m_nThreshold1           =                     other->data(CC_THRESHOLD1, Qt::UserRole).toLongLong();           
    m_nThreshold2           =                     other->data(CC_THRESHOLD2, Qt::UserRole).toLongLong();    
    m_nEnergyThreshold      =                     other->data(CC_ENERGY_THRESHOLD, Qt::UserRole).toLongLong();
    m_bThreshold1Set        =                     other->data(CC_THRESHOLD1_SET, Qt::UserRole).toBool();
    m_bThreshold2Set        =                     other->data(CC_THRESHOLD2_SET, Qt::UserRole).toBool();
    m_bThresholdESet        =                     other->data(CC_THRESHOLDE_SET, Qt::UserRole).toBool();
    m_nWaveformPrethreshold =                     other->data(CC_WF_PRETHRESHOLD, Qt::UserRole).toULongLong();
    m_nWaveformLength       =                     other->data(CC_WF_LENGTH, Qt::UserRole).toULongLong();
    m_nWaveformDeadTime     =                     other->data(CC_DEAD_TIME, Qt::UserRole).toULongLong();
    m_nFeaturesSelected     =                     other->data(CC_FEATURE, Qt::UserRole).toULongLong();
    m_eSortMethod           =         (SortMethod)other->data(CC_SORT_METHOD, Qt::UserRole).toInt();       
    m_nSortUnits            =                     other->data(CC_NBR_SORT_UNITS, Qt::UserRole).toULongLong();          
    m_fPenaltyFactor        =                     other->data(CC_PENALTY_FACTOR, Qt::UserRole).toDouble();     
     
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Convert the data in this tree item to XML
/// @param [in] Address of the XML stream to append the XML
/// @param [in] If true only detecting and sorting parameters will be saved
/// @return TRUE if successfully writes the XML; FALSE otherwise
bool ChannelData::saveToXML(QXmlStreamWriter *writer, bool bPartialSave)
{
    writer->writeStartElement("Channel");
    writer->writeTextElement(getColumnName(CC_NAME), m_sName);
    if (!bPartialSave)
    {
        // General
        writer->writeStartElement("General");
        writer->writeTextElement(getColumnName(CC_FIRST_TIME), QString::number(m_nTimeStampMin));
        writer->writeTextElement(getColumnName(CC_LAST_TIME), QString::number(m_nTimeStampMax));
        writer->writeEndElement();

        // State
        writer->writeStartElement("State");
        writer->writeTextElement(getColumnName(CC_NSAMPLES), QString::number(m_nSamples));
        writer->writeTextElement(getColumnName(CC_NWAVEFORMS), QString::number(m_nWaveforms));
        writer->writeTextElement(getColumnName(CC_PAGE), QString::number((quint64)m_ePage));
        writer->writeTextElement(getColumnName(CC_SAMPLERATE), QString::number(m_nSampleRate));
        writer->writeEndElement();
    }

    // Preprocessing
    writer->writeStartElement("Preprocessing");
    writer->writeTextElement(getColumnName(CC_HIGHPASS_FILTER), QString::number((quint64)m_eHighpassFilter));
    writer->writeTextElement(getColumnName(CC_FILTER_ORDER), QString::number(m_nFilterOrder));
    writer->writeTextElement(getColumnName(CC_CUTOFF_FREQ), QString::number(m_nCutoffFreq));
    writer->writeTextElement(getColumnName(CC_SIGNAL_ENERGY), QString::number((quint64)m_eSignalEnergy));
    writer->writeTextElement(getColumnName(CC_SE_WIDTH), QString::number(m_nWindowWidth));
    writer->writeTextElement(getColumnName(CC_ARTIFACT_SUBTRACTION), QString::number(m_bArtifactSubtraction));
    writer->writeEndElement();

    // Threshold
    writer->writeStartElement("Threshold_Data");
    writer->writeTextElement(getColumnName(CC_THRESHOLD), QString::number((quint64)m_eThreshold));
    writer->writeTextElement(getColumnName(CC_SYMETRICAL_THRESHOLD), QString::number(m_bThresholdSymetrical));
    writer->writeTextElement(getColumnName(CC_RMS_MULTIPLIER), QString::number(m_nRMSMultiplier));
    writer->writeTextElement(getColumnName(CC_THRESHOLD1), QString::number(m_nThreshold1));
    writer->writeTextElement(getColumnName(CC_THRESHOLD2), QString::number(m_nThreshold2));
    writer->writeTextElement(getColumnName(CC_ENERGY_THRESHOLD), QString::number(m_nEnergyThreshold));
    writer->writeTextElement(getColumnName(CC_THRESHOLD1_SET), QString::number(m_bThreshold1Set));
    writer->writeTextElement(getColumnName(CC_THRESHOLD2_SET), QString::number(m_bThreshold2Set));
    writer->writeTextElement(getColumnName(CC_THRESHOLDE_SET), QString::number(m_bThresholdESet));
    writer->writeEndElement();

    // Waveform Duration
    writer->writeStartElement("Waveform_Duration");
    writer->writeTextElement(getColumnName(CC_WF_PRETHRESHOLD), QString::number(m_nWaveformPrethreshold));
    writer->writeTextElement(getColumnName(CC_WF_LENGTH), QString::number(m_nWaveformLength));
    writer->writeTextElement(getColumnName(CC_DEAD_TIME), QString::number(m_nWaveformDeadTime));
    writer->writeTextElement(getColumnName(CC_DETECTED), QString::number(m_bDetected));
    writer->writeEndElement();

    //  Sorting
    writer->writeStartElement("Sorting_Data");
    writer->writeTextElement(getColumnName(CC_SORT_METHOD), QString::number((quint64)m_eSortMethod));
    writer->writeTextElement(getColumnName(CC_NBR_SORT_UNITS), QString::number(m_nSortUnits));
    writer->writeTextElement(getColumnName(CC_PENALTY_FACTOR), QString::number(m_fPenaltyFactor));
    writer->writeEndElement();

    // Features
    writer->writeStartElement("Features_Data");
    writer->writeTextElement(getColumnName(CC_FEATURE), QString::number(m_nFeaturesSelected));


    

    if (!bPartialSave)
    {

        // More Features
        if (!m_mapSelectedFeatures.isEmpty())
        {
            writer->writeStartElement(getColumnName(CC_SELECTED_FEATURES));

            QMultiMap<QString, QVariant>::iterator it = m_mapSelectedFeatures.begin();

            while (it != m_mapSelectedFeatures.end())
            {
                writer->writeStartElement("Feature");
                writer->writeTextElement("FeatureX", it.key());
                writer->writeTextElement("FeatureY", it.value().toString());
                writer->writeEndElement();
                it++;
            }
            writer->writeEndElement();
        }

        writer->writeTextElement(getColumnName(CC_X_3DAXIS), QString::number((quint64)m_eThreeDXAxis));
        writer->writeTextElement(getColumnName(CC_Y_3DAXIS), QString::number((quint64)m_eThreeDYAxis));
        writer->writeTextElement(getColumnName(CC_Z_3DAXIS), QString::number((quint64)m_eThreeDZAxis));
        writer->writeTextElement(getColumnName(CC_3D_EDIT_STATE), QString::number((quint64)m_eThreeDMode));
        writer->writeTextElement(getColumnName(CC_X_2DAXIS), QString::number((quint64)m_eTwoDXAxis));
        writer->writeTextElement(getColumnName(CC_Y_2DAXIS), QString::number((quint64)m_eTwoDYAxis));
        writer->writeEndElement();

        

        // Waveform Editing
        writer->writeStartElement("Waveform_Editing");
        writer->writeTextElement(getColumnName(CC_WAVEFORM_ALIGNMENT), QString::number((quint64)m_eAlignMode));
        writer->writeTextElement(getColumnName(CC_MAX_SHIFT), QString::number(m_nMaxShift));

        writer->writeStartElement(getColumnName(CC_UNITS_ALIGN));

        foreach(QString str, m_UnitsToAlign)
        {
            writer->writeTextElement("unit", str);
        }
        writer->writeEndElement();
        writer->writeStartElement(getColumnName(CC_UNITS_VISIBLE));

        foreach(QString str, m_UnitsVisible)
        {
            writer->writeTextElement("unit", str);
        }
        writer->writeEndElement();
        
        if (m_pEditLines->m_data.count())
        {
            writer->writeStartElement(getColumnName(CC_EDIT_LINES));
            m_pEditLines->m_data.saveToXML(writer);
            writer->writeEndElement();
        }

        if (m_pEditBoxes->m_data.count())
        {
            writer->writeStartElement(getColumnName(CC_EDIT_BOXES));
            m_pEditBoxes->m_data.saveToXML(writer);
            writer->writeEndElement();
        }

        writer->writeTextElement(getColumnName(CC_EDIT_MODE), QString::number((quint64)m_eEditMode));
        writer->writeEndElement();

        // Nev values
        writer->writeStartElement("NEV");
        writer->writeTextElement(getColumnName(CC_VALID_WAVEFORMS), QString::number(m_nValidWaveforms));
        writer->writeTextElement(getColumnName(CC_INVALID_WAVEFORMS), QString::number(m_nInvalidWaveforms));
        writer->writeTextElement(getColumnName(CC_NBR_SAMPLES_IN_WAVEFORM), QString::number(m_nSamplesPerWaveform));
        writer->writeEndElement();

        // timeline range - for syncronizing timeline signal energy and timeline continuous
        writer->writeStartElement("Timeline_Cont");
        writer->writeTextElement(getColumnName(CC_TIMELINE_VISIBLE_START), QString::number(m_nTimelineVisibleStart));
        writer->writeTextElement(getColumnName(CC_TIMELINE_VISIBLE_END), QString::number(m_nTimelineVisibleEnd));
        writer->writeTextElement(getColumnName(CC_TIMESPIKE_VISIBLE_START), QString::number(m_nTimespikeVisibleStart));
        writer->writeTextElement(getColumnName(CC_TIMESPIKE_VISIBLE_END), QString::number(m_nTimespikeVisibleEnd));
        writer->writeTextElement(getColumnName(CC_TIMELINE_VERTICAL_RANGE), QString::number(m_nTimelineVerticalRange));
        writer->writeTextElement(getColumnName(CC_SE_VERTICAL_RANGE), QString::number(m_nSEVerticalRange));
        writer->writeEndElement();

        // timeline range - for calculating backend data
        writer->writeStartElement("Timeline_Backend");
        writer->writeTextElement(getColumnName(CC_TIMELINE_START), QString::number(m_nTimelineStart));
        writer->writeTextElement(getColumnName(CC_TIMELINE_END), QString::number(m_nTimelineEnd));
        writer->writeTextElement(getColumnName(CC_TIMESPIKE_START), QString::number(m_nTimespikeStart));
        writer->writeTextElement(getColumnName(CC_TIMESPIKE_END), QString::number(m_nTimespikeEnd));
        writer->writeTextElement(getColumnName(CC_SPIKE_START), QString::number(m_nSpikeStart));
        writer->writeTextElement(getColumnName(CC_SPIKE_END), QString::number(m_nSpikeEnd));
        writer->writeEndElement();

        // Nsx values
        writer->writeStartElement("NSX");
        writer->writeTextElement(getColumnName(CC_NSX_ELECTRODEID), QString::number(m_nElectrodeID));
        writer->writeTextElement(getColumnName(CC_NSX_CONNECTOR), QString::number(m_nConnector));
        writer->writeTextElement(getColumnName(CC_NSX_PIN), QString::number(m_nPin));
        writer->writeTextElement(getColumnName(CC_NSX_LABEL), m_sLabel);
        writer->writeTextElement(getColumnName(CC_NSX_CUTOFF_FREQ), QString::number(m_nNSxCutoffFreq));
        writer->writeTextElement(getColumnName(CC_NSX_ORDER), QString::number(m_nOrder));
        writer->writeTextElement(getColumnName(CC_NSX_TYPE), QString::number(m_nType));
        writer->writeTextElement(getColumnName(CC_CDFILE_INIT), QString::number(m_bCdFileInitialized));
        writer->writeTextElement(getColumnName(CC_SDFILE_INIT), QString::number(m_bSdFileInitialized));
        writer->writeEndElement();

        // Undo-Redo
        writer->writeTextElement(getColumnName(CC_CHANGE_COUNTER), QString::number(m_nChangeCounter));

        // spike data
        UnitAccum units = m_spikeData->units();
        if (units)
        {
            QVector<int> list = units->getUnitNumbers();
            if (!list.isEmpty())
            {
                writer->writeStartElement(getColumnName(CC_SPIKEDATA_UNITS));
                m_spikeData->saveToXML(writer);
                writer->writeEndElement();

            }
        }

        // file separators
        if (!m_fileSeparators.isEmpty())
        {
            writer->writeStartElement(getColumnName(CC_FILE_SEPARATORS));

            foreach(QString ts, m_fileSeparators)
            {
                writer->writeTextElement("sep", ts);
            }
            writer->writeEndElement();
        }

        // segments
        writer->writeTextElement(getColumnName(CC_CURRENT_INTERVAL), QString::number(m_nCurrentInterval));

        for (int i = 0; i < this->childCount(); i++)
        {
            channelItem *item = this->child(i);
            item->saveToXML(writer);
        }
    }
    else
    {
        writer->writeEndElement();

        // We need the name to match channels
        writer->writeStartElement("NSX");        
        writer->writeTextElement(getColumnName(CC_NSX_LABEL), m_sLabel);        
        writer->writeEndElement();
    }
    writer->writeEndElement();
    return true;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Populate this tree item with the data saved in the XML stream
/// @param [in] Address of the XML stream to read
/// @return TRUE if successfully writes the XML; FALSE otherwise
bool ChannelData::getFromXML(QXmlStreamReader *reader)
{

    if (reader->name() == "Channel")
    {
        while(reader->readNextStartElement())
        {
            QStringRef name = reader->name();

            if (name == "Channel")
            {
                continue;
            }
            if (name == getColumnName(CC_NAME))
            {
                m_sName = reader->readElementText();
            }
            else if (name == "General")
            {
                while (reader->readNextStartElement())
                {
                    name = reader->name();
                    if (name == getColumnName(CC_FIRST_TIME))
                    {
                        m_nTimeStampMin = reader->readElementText().toLongLong();
                    }
                    else if (name == getColumnName(CC_LAST_TIME))
                    {
                        m_nTimeStampMax = reader->readElementText().toLongLong();
                    }
                }

            }
            else if (name == "State")
            {
                while (reader->readNextStartElement())
                {
                    name = reader->name();
                    if (name == getColumnName(CC_NSAMPLES))
                    {
                        m_nSamples = reader->readElementText().toLongLong();
                    }
                    else if (name == getColumnName(CC_NWAVEFORMS))
                    {
                        m_nWaveforms = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_PAGE))
                    {
                        m_ePage = (WizardPage)reader->readElementText().toInt();
                    }
                    else if (name == getColumnName(CC_SAMPLERATE))
                    {
                        setSampleRate(QVariant(reader->readElementText().toULongLong()));
                    }
                }
            }
            else if (name == "Preprocessing")
            {
                while (reader->readNextStartElement())
                {
                    name = reader->name();
                    if (name == getColumnName(CC_HIGHPASS_FILTER))
                    {
                        m_eHighpassFilter = (HighpassFilter)reader->readElementText().toInt();
                    }
                    else if (name == getColumnName(CC_FILTER_ORDER))
                    {
                        m_nFilterOrder = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_CUTOFF_FREQ))
                    {
                        setCutoffFreq(QVariant(reader->readElementText().toULongLong()));
                    }
                    else if (name == getColumnName(CC_SIGNAL_ENERGY))
                    {
                        m_eSignalEnergy = (SignalEnergy)reader->readElementText().toInt();
                    }
                    else if (name == getColumnName(CC_SE_WIDTH))
                    {
                        m_nWindowWidth = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_ARTIFACT_SUBTRACTION))
                    {
                        m_bArtifactSubtraction = (bool)reader->readElementText().toInt();
                    }
                }
            }
            else if (name == "Threshold_Data")
            {
                while (reader->readNextStartElement())
                {
                    name = reader->name();
                    if (name == getColumnName(CC_THRESHOLD))
                    {
                        m_eThreshold = (ThresholdType)reader->readElementText().toInt();
                    }
                    else if (name == getColumnName(CC_SYMETRICAL_THRESHOLD))
                    {
                        m_bThresholdSymetrical = (bool)reader->readElementText().toInt();
                    }
                    else if (name == getColumnName(CC_RMS_MULTIPLIER))
                    {
                        m_nRMSMultiplier = reader->readElementText().toDouble();
                    }
                    else if (name == getColumnName(CC_THRESHOLD1))
                    {
                        m_nThreshold1 = reader->readElementText().toLongLong();
                    }
                    else if (name == getColumnName(CC_THRESHOLD2))
                    {
                        m_nThreshold2 = reader->readElementText().toLongLong();
                    }
                    else if (name == getColumnName(CC_ENERGY_THRESHOLD))
                    {
                        m_nEnergyThreshold = reader->readElementText().toLongLong();
                    }
                    else if (name == getColumnName(CC_THRESHOLD1_SET))
                    {
                        m_bThreshold1Set = (bool)reader->readElementText().toInt();
                    }
                    else if (name == getColumnName(CC_THRESHOLD2_SET))
                    {
                        m_bThreshold2Set = (bool)reader->readElementText().toInt();
                    }
                    else if (name == getColumnName(CC_THRESHOLDE_SET))
                    {
                        m_bThresholdESet = (bool)reader->readElementText().toInt();
                    }

                }
            }
            else if (name == "Waveform_Duration")
            {
                while (reader->readNextStartElement())
                {
                    name = reader->name();
                    if (name == getColumnName(CC_WF_PRETHRESHOLD))
                    {
                        m_nWaveformPrethreshold = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_WF_LENGTH))
                    {
                        m_nWaveformLength = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_DEAD_TIME))
                    {
                        m_nWaveformDeadTime = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_DETECTED))
                    {
                        m_bDetected = reader->readElementText().toInt();
                    }
                }
            }
            else if (name == "Sorting_Data")
            {
                while (reader->readNextStartElement())
                {
                    name = reader->name();
                    if (name == getColumnName(CC_SORT_METHOD))
                    {
                        m_eSortMethod = (SortMethod)reader->readElementText().toInt();
                    }
                    else if (name == getColumnName(CC_NBR_SORT_UNITS))
                    {
                        m_nSortUnits = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_PENALTY_FACTOR))
                    {
                        m_fPenaltyFactor = reader->readElementText().toDouble();
                    }
                }
            }
            else if (name == "Features_Data")
            {
                while (reader->readNextStartElement())
                {
                    name = reader->name();
                    if (name == getColumnName(CC_FEATURE))
                    {
                        m_nFeaturesSelected = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_SELECTED_FEATURES))
                    {
                        m_mapSelectedFeatures.clear();

                        while (reader->readNextStartElement())
                        {
                            QStringRef nam = reader->name();

                            if (nam == "Feature")
                            {
                                QString fx = "";
                                QString fy = "";

                                while (reader->readNextStartElement())
                                {
                                    QStringRef na = reader->name();

                                    if (na == "FeatureX")
                                    {
                                        fx = reader->readElementText();
                                    }
                                    if (na == "FeatureY")
                                    {
                                        fy = reader->readElementText();
                                    }
                                }
                                m_mapSelectedFeatures.insert(fx, fy);
                            }
                        }
                    }

                    else if (name == getColumnName(CC_X_3DAXIS))
                    {
                        m_eThreeDXAxis = (FeatureID)reader->readElementText().toInt();
                    }
                    else if (name == getColumnName(CC_Y_3DAXIS))
                    {
                        m_eThreeDYAxis = (FeatureID)reader->readElementText().toInt();
                    }
                    else if (name == getColumnName(CC_Z_3DAXIS))
                    {
                        m_eThreeDZAxis = (FeatureID)reader->readElementText().toInt();
                    }
                    else if (name == getColumnName(CC_3D_EDIT_STATE))
                    {
                        m_eThreeDMode = (GLWidgetState)reader->readElementText().toInt();
                    }
                    else if (name == getColumnName(CC_X_2DAXIS))
                    {
                        m_eTwoDXAxis = (FeatureID)reader->readElementText().toInt();
                    }
                    else if (name == getColumnName(CC_Y_2DAXIS))
                    {
                        m_eTwoDYAxis = (FeatureID)reader->readElementText().toInt();
                    }

                }
            }
            else if (name == "Waveform_Editing")
            {
                while (reader->readNextStartElement())
                {
                    name = reader->name();
                    if (name == getColumnName(CC_WAVEFORM_ALIGNMENT))
                    {
                        m_eAlignMode = (AlignMode)reader->readElementText().toInt();
                    }
                    else if (name == getColumnName(CC_MAX_SHIFT))
                    {
                        m_nMaxShift = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_UNITS_ALIGN))
                    {
                        m_UnitsToAlign.clear();

                        while (reader->readNextStartElement())
                        {
                            name = reader->name();

                            if (name != "unit")
                            {
                                break;
                            }
                            QString value = reader->readElementText();
                            m_UnitsToAlign.push_back(value);
                        }
                    }
                    else if (name == getColumnName(CC_UNITS_VISIBLE))
                    {
                        m_UnitsVisible.clear();

                        while (reader->readNextStartElement())
                        {
                            name = reader->name();

                            if (name != "unit")
                            {
                                break;
                            }
                            QString value = reader->readElementText();
                            m_UnitsVisible.push_back(value);
                        }
                    }
                    else if (name == getColumnName(CC_EDIT_LINES))
                    {
                        m_pEditLines->m_data.getFromXML(reader);
                    }
                    else if (name == getColumnName(CC_EDIT_BOXES))
                    {
                        m_pEditBoxes->m_data.getFromXML(reader);
                    }
                    else if (name == getColumnName(CC_EDIT_MODE))
                    {
                        m_eEditMode = (EditMode)reader->readElementText().toInt();
                    }
                }
            }
            else if (name == "NEV")
            {
                while (reader->readNextStartElement())
                {
                    name = reader->name();
                    if (name == getColumnName(CC_VALID_WAVEFORMS))
                    {
                        m_nValidWaveforms = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_INVALID_WAVEFORMS))
                    {
                        m_nInvalidWaveforms = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_NBR_SAMPLES_IN_WAVEFORM))
                    {
                        m_nSamplesPerWaveform = reader->readElementText().toULongLong();
                    }
                }
            }
            else if (name == "Timeline_Cont")
            {
                while (reader->readNextStartElement())
                {
                    name = reader->name();
                    if (name == getColumnName(CC_TIMELINE_VISIBLE_START))
                    {
                        m_nTimelineVisibleStart = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_TIMELINE_VISIBLE_END))
                    {
                        m_nTimelineVisibleEnd = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_TIMESPIKE_VISIBLE_START))
                    {
                        m_nTimespikeVisibleStart = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_TIMESPIKE_VISIBLE_END))
                    {
                        m_nTimespikeVisibleEnd = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_TIMELINE_VERTICAL_RANGE))
                    {
                        m_nTimelineVerticalRange = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_SE_VERTICAL_RANGE))
                    {
                        m_nSEVerticalRange = reader->readElementText().toULongLong();
                    }
                }
            }
            else if (name == "Timeline_Backend")
            {
                while (reader->readNextStartElement())
                {
                    name = reader->name();
                    if (name == getColumnName(CC_TIMELINE_START))
                    {
                        m_nTimelineStart = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_TIMELINE_END))
                    {
                        m_nTimelineEnd = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_TIMESPIKE_START))
                    {
                        m_nTimespikeStart = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_TIMESPIKE_END))
                    {
                        m_nTimespikeEnd = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_SPIKE_START))
                    {
                        m_nSpikeStart = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_SPIKE_END))
                    {
                        m_nSpikeEnd = reader->readElementText().toULongLong();
                    }
                }
            }
            else if (name == "NSX")
            {
                while (reader->readNextStartElement())
                {
                    name = reader->name();
                    if (name == getColumnName(CC_NSX_ELECTRODEID))
                    {
                        m_nElectrodeID = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_NSX_CONNECTOR))
                    {
                        m_nConnector = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_NSX_PIN))
                    {
                        m_nPin = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_NSX_LABEL))
                    {
                        m_sLabel = reader->readElementText();
                    }
                    else if (name == getColumnName(CC_NSX_CUTOFF_FREQ))
                    {
                        m_nNSxCutoffFreq = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_NSX_ORDER))
                    {
                        m_nOrder = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_NSX_TYPE))
                    {
                        m_nType = reader->readElementText().toULongLong();
                    }
                    else if (name == getColumnName(CC_CDFILE_INIT))
                    {
                        m_bCdFileInitialized = reader->readElementText().toInt();
                    }
                    else if (name == getColumnName(CC_SDFILE_INIT))
                    {
                        m_bSdFileInitialized = reader->readElementText().toInt();
                    }
                }
            }                       
            else if (name == getColumnName(CC_CHANGE_COUNTER))
            {
                m_nChangeCounter = reader->readElementText().toInt();
            }
            else if (name == getColumnName(CC_SPIKEDATA_UNITS))
            {
                m_spikeData->getFromXML(reader);
            }
            else if (name == getColumnName(CC_FILE_SEPARATORS))
            {
                m_fileSeparators.clear();

                while (reader->readNextStartElement())
                {
                    name = reader->name();

                    if (name != "sep")
                    {
                        break;
                    }
                    QString value = reader->readElementText();
                    m_fileSeparators.push_back(value);
                }                
            }
            else if (name == getColumnName(CC_CURRENT_INTERVAL))
            {
                m_nCurrentInterval = reader->readElementText().toULongLong();
            }
            else if (name == "IntervalChild")
            {
                // leave these out until intervals are implemented
                reader->skipCurrentElement();
                //IntervalData *interval = new IntervalData(this);
                //interval->getFromXML(reader);
                //this->appendChild(interval);
            }
            else
            {
                reader->skipCurrentElement();
            }
        }
    }
    return true;
}

/// @note
/// Following are utility methods used for saving QVariant values
/// They return true if the value changed. A signal is only emitted when the value is changed.


/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Set the energy threshold
/// @param [in]  new threshold value in the form of a QVariant
/// @param [in] role
/// @return TRUE if the value changed; FALSE otherwise
bool ChannelData::setEnergyThreshold(QVariant value)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int oldValue = m_nEnergyThreshold;
    m_bThresholdESet = true;

    if (m_eSignalEnergy == ADDITIONAL_SIGNED_SIGNAL_ENERGY)
    {
        if (value.toInt() == 0)
        {
            // if the old value is 1 from zero, skip past zero for the combobox arrows
            if (oldValue == 1)
                m_nEnergyThreshold = -1;
            else
                m_nEnergyThreshold = 1;
        }
        else
        {
            m_nEnergyThreshold = value.toInt();
        }
    }
    else
    {
        if (value.toInt() < 1)
            m_nEnergyThreshold = 1;
        else
            m_nEnergyThreshold = value.toInt();
    }
    if ((oldValue != m_nEnergyThreshold) || (value.toInt() != m_nEnergyThreshold))
    {
        emitSignalIfDisplayed(row(), SIGNAL_SLIDER_CHANGED, QVariant(SLIDERTYPE_THRESHOLD_ENERGY));
        emitSignalIfDisplayed(row(), SIGNAL_DATA_CHANGED, QVariant(CC_ENERGY_THRESHOLD));
        return true;
    }
    return false;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Set the threshold1 within range, if symetrical thresholds, set threshold2
/// @param [in]  new threshold value in the form of a QVariant
/// @param [in] role
/// @return TRUE if the value changed; FALSE otherwise
bool ChannelData::setThreshold1(QVariant value)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int oldValue = m_nThreshold1;
    // if the threshold is saved to the QVariant as string with a decimal point, toInt() doesn't work
    double temp = value.toDouble();
    int temp2 = m_nThreshold2;
    m_nThreshold1 = (int)temp;
    m_bThreshold1Set = true;

    if (m_bThresholdSymetrical)
    {
        m_nThreshold2 = -m_nThreshold1;

        if (temp2 != -m_nThreshold1)
        {
            m_bThreshold2Set = true;
            emitSignalIfDisplayed(row(), SIGNAL_SLIDER_CHANGED, QVariant(SLIDERTYPE_THRESHOLD1));
            emitSignalIfDisplayed(row(), SIGNAL_SLIDER_CHANGED, QVariant(SLIDERTYPE_THRESHOLD2));
            emitSignalIfDisplayed(row(), SIGNAL_DATA_CHANGED, QVariant(CC_THRESHOLD1));
            emitSignalIfDisplayed(row(), SIGNAL_DATA_CHANGED, QVariant(CC_THRESHOLD2));
            return false;
        }
    }
    else if (oldValue != m_nThreshold1)
    {
        emitSignalIfDisplayed(row(), SIGNAL_SLIDER_CHANGED, QVariant(SLIDERTYPE_THRESHOLD1));
        return true;
    }
    return false;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Set the threshold2 within range, if symetrical thresholds, set threshold1
/// @param [in]  new threshold value in the form of a QVariant
/// @return TRUE if the value changed; FALSE otherwise
bool ChannelData::setThreshold2(QVariant value)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int oldValue = m_nThreshold2;
    // if the threshold is saved to the QVariant as string with a decimal point, toInt() doesn't work
    double temp = value.toDouble();
    m_nThreshold2 = (int)temp;
    int temp1 = m_nThreshold1;
    m_bThreshold2Set = true;

    if (m_bThresholdSymetrical)
    {
        m_nThreshold1 = -m_nThreshold2;

        if (temp1 != -m_nThreshold2)
        {
            m_bThreshold1Set = true;

            emitSignalIfDisplayed(row(), SIGNAL_SLIDER_CHANGED, QVariant(SLIDERTYPE_THRESHOLD1));
            emitSignalIfDisplayed(row(), SIGNAL_SLIDER_CHANGED, QVariant(SLIDERTYPE_THRESHOLD2));
            emitSignalIfDisplayed(row(), SIGNAL_DATA_CHANGED, QVariant(CC_THRESHOLD1));
            emitSignalIfDisplayed(row(), SIGNAL_DATA_CHANGED, QVariant(CC_THRESHOLD2));
            return false;
        }
    }
    else if (oldValue != m_nThreshold2)
    {
        ccm->emitSignal(row(), SIGNAL_SLIDER_CHANGED, QVariant(SLIDERTYPE_THRESHOLD2));
        return true;
    }
    return false;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Save the waveform pre-threshold value and insure it is less than the waveform length
/// @param [in]  new waveform pre-threshold value in the form of a QVariant
/// @return TRUE if the value changed; FALSE otherwise
bool ChannelData::setWaveformPrethreshold(QVariant value) 
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    int oldValue = m_nWaveformPrethreshold;
    m_nWaveformPrethreshold = value.toInt();

    quint64 max = 128 * 1000000 / m_nSampleRate;
    quint64 increment = 1000000 / m_nSampleRate;
    quint64 maxFromPWaveformLength = m_nWaveformLength - (2 * increment) - 1;

    // if WaveformLength == Prethreshold, it doesn't detect
    max = max > maxFromPWaveformLength ? maxFromPWaveformLength : max;

    if (m_nWaveformPrethreshold > max)
        m_nWaveformPrethreshold = max;

    // if it didn't change, but also didn't change to the new value, emit changed so the controls know
    if ((oldValue != m_nWaveformPrethreshold) || (m_nWaveformPrethreshold != value.toInt()))
        return true;
    
    return false;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Save the waveform length and insure it is > than the waveform pre-threshold value
/// @param [in]  new waveform length in the form of a QVariant
/// @param [in] role
/// @return TRUE if the value changed; FALSE otherwise
bool ChannelData::setWaveformLength(QVariant value, int role)
{
    Q_UNUSED(role);

    int oldValue = m_nWaveformLength;
    m_nWaveformLength = value.toUInt();

    quint64 max = 128 * 1000000 / m_nSampleRate;
    quint64 min = 5 * 1000000 / m_nSampleRate;
    quint64 increment = 1000000 / m_nSampleRate;

    quint64 minFromPrethreshold = m_nWaveformPrethreshold + (2 * increment) + 1;

    // if WaveformLength == Prethreshold, it doesn't detect
    min = min < minFromPrethreshold ? minFromPrethreshold : min;

    if (m_nWaveformLength > max)
        m_nWaveformLength = max;

    if (m_nWaveformLength < min)
        m_nWaveformLength = min;

    // if it didn't change, but also didn't change to the new value, emit changed so the controls know
    if ((oldValue != m_nWaveformLength) || (m_nWaveformLength != value.toUInt()))
        return true;

    return false;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Transform a double into a QVariant containing a string or a double depending on the role.
/// @param [in] double value
/// @param [in] role
/// @return QVariant containing a string or a double depending on the role
QVariant ChannelData::getDouble(double nMember, int role) const
{
    if (role == Qt::UserRole)
        return QVariant(nMember);

    return QVariant(QString::number(nMember, 'f'));
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Transform a bool into a QVariant containing a string or a bool depending on the role.
/// @param [in] bool value
/// @param [in] role
/// @return QVariant containing a string or a bool depending on the role
QVariant ChannelData::getBool(int nMember, int role) const
{
    if (role == Qt::UserRole)
        return QVariant(nMember);

    QString state = (nMember) ? "true" : "false";
    return QVariant(state);
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Transform an int into a QVariant containing a string or an int depending on the role.
/// @param [in] int value
/// @param [in] role
/// @return QVariant containing a string or an int depending on the role
QVariant ChannelData::getInteger(int nMember, int role) const
{
    if (role == Qt::UserRole)
        return QVariant(nMember);

    return QVariant(QString::number(nMember));
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Transform an enum into a QVariant containing a string or an int depending on the role.
/// @param [in] QStringList of the names corresponding to the emum values
/// @param [in] index = enumeration value
/// @param [in] role
/// @return QVariant containing a string or an int depending on the role
QVariant ChannelData::getEnum(QStringList *names, int index, int role) const
{
    if (role == Qt::UserRole)
        return QVariant(index);

    if (index < names->size())
        return QVariant(names->at(index));

    return QVariant();
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Save an enum value in a member variable.
/// @param [in] QStringList of the names corresponding to the emum values
/// @param [in] address of member enum variable where the value is saved
/// @param [in] value in the form of a QVariant
/// @param [in] role
/// @return TRUE if the value changed; FALSE otherwise
bool ChannelData::setEnum(QStringList *names, int *member, const QVariant &value, int role)
{
    bool changed = false;

    if (role == Qt::UserRole)
    {
        *member = value.toInt();
        changed = true;
    }
    else
    {
        QString s = value.toString();
        int index = names->indexOf(s);

        if (index != -1)
        {
            if (*member != index)
            {
                *member = index;
                changed = true;
            }
        }
    }
    return changed;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Transform a list of enum names or a bitmask (depending on the role) into a QVariant
/// @param [in] QStringList of the names corresponding to bitmask values
/// @param [in] bitmask
/// @param [in] role
/// @return QVariant containing a QStringList or an int depending on the role
QVariant ChannelData::getBitMask(QStringList *names, uint bitMask, int role) const
{
//    if (role == Qt::UserRole)
    return QVariant(bitMask);

    QStringList sl;
    uint mask = 0X01;
    QStringListIterator it(*names);

    while (it.hasNext())
    {
        if (bitMask & mask)
        {
            sl.push_back(it.next());
        }
        else
            it.next();

        mask = mask << 1;
    }
    return QVariant(sl);
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Save a bitmask
/// @param [in] QStringList of the names corresponding to bitmask values
/// @param [in] address of member bitmask variable where the value is saved
/// @param [in] QVariant containing either a bitmask or a QStringList depending on the role
/// @param [in] role
/// @return TRUE if the value changed; FALSE otherwise
bool ChannelData::setBitMask(QStringList *names, uint *member, QVariant value, int role)
{
    uint oldBitMask = *member;

//    if (role == Qt::UserRole)
        *member = value.toUInt();
    //else
    //{
    //    uint bitMask = 0;
    //    uint mask = 0X01;
    //    QStringList sl = value.toStringList();
    //    QStringListIterator it(*names);

    //    while (it.hasNext())
    //    {
    //        if (sl.contains(it.next()))
    //        {
    //            bitMask |= mask;
    //        }
    //        mask = mask << 1;
    //    }
    //    *member = bitMask;
    //}
    return (oldBitMask != *member);
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Save the cutoff frequency. cutoff freq. has it's on function because it is capped
/// @param [in]  value in the form of a QVariant
/// @return TRUE if the value changed; FALSE otherwise
bool ChannelData::setCutoffFreq(const QVariant &value)
{
    quint64 oldValue = m_nCutoffFreq;

    if (value.toULongLong() > m_nMaxCutoffFreq)
        m_nCutoffFreq = m_nMaxCutoffFreq;
    else if (value.toULongLong() < 1)
        m_nCutoffFreq = 1;
    else
        m_nCutoffFreq = value.toULongLong();

    // if it didn't change, but also didn't change to the new value, emit changed so the controls know
    if ((oldValue != m_nCutoffFreq) || (m_nCutoffFreq != value.toULongLong()))
        return true;

    return false;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Save the Sample Rate. Sample Rate has it's on function because it also sets the max cutoff freq.
/// @param [in]  value in the form of a QVariant
/// @return TRUE if the value changed; FALSE otherwise
bool ChannelData::setSampleRate(const QVariant &value)
{
    quint64 v = value.toULongLong();

    if (m_nSampleRate != v)
    {
        m_nSampleRate = v;
        m_nMaxCutoffFreq = ((m_nSampleRate / 2) * 9) / 10;  // the data disappears at maxFrequency = sampleRate

        if (m_nCutoffFreq > m_nMaxCutoffFreq)
            m_nCutoffFreq = m_nMaxCutoffFreq;

        setWaveformLength(1600);
        return true;
    }
    return false;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  get the name of the column with no spaces or -'s which are illegal XML characters
/// @param [in] column
/// @return name
QString ChannelData::getColumnName(ChannelColumn column)
{
    QString name = ColumnNames.at(column);
    name = name.replace(QChar(' '), QString("_"));
    name = name.replace(QChar('2'), QString("Two"));
    name = name.replace(QChar('3'), QString("Three"));
    return name;
}

/// @author Holly Ausbeck
/// @date   Aug 10, 2015
/// @brief  get the visible status of the units from the spike data and return it as:
//          QByteArray for role = UserRole
//          QStringList for role = DisplayRole or EditRole
/// @param [in] role
/// @return QVariant containing a QByteArray or a QStringList depending on the role
QVariant ChannelData::getVisibleUnits(int role) const
{
    QVariant v;
    UnitAccum unitData = m_spikeData->units();

    if (unitData)
    {       
        QVector<int> usedUnits = unitData->getUnitNumbers();

        if (role == Qt::UserRole)
        {
            QByteArray unitsVisible;

            for (int u(0); u < usedUnits.size(); ++u)
            {
                if (unitData->isVisible(usedUnits[u]))
                    unitsVisible.push_back(usedUnits[u]);
            }
            v = QVariant(unitsVisible);
        }
        else
        {
            QStringList unitsVisible;

            for (int u(0); u < usedUnits.size(); ++u)
            {
                if (unitData->isVisible(usedUnits[u]))
                    unitsVisible.push_back(QString::number(usedUnits[u]));
            }
            v = QVariant(unitsVisible);
        }
    }
    return v;
}

/// @author Holly Ausbeck
/// @date   Aug 10, 2015
/// @brief  set the visible status of the units in the spike data from the list
/// @param [in] QVariant is one of the following:
//          QByteArray for role = UserRole
//          QStringList for role = DisplayRole or EditRole
/// @param [in] role
void ChannelData::setVisibleUnits(const QVariant &value, int role)
{
    UnitAccum unitData = m_spikeData->units();

    if (unitData)
    {
        QVector<int> usedUnits = unitData->getUnitNumbers();

        if (role == Qt::UserRole)
        {
            QByteArray unitsVisible = value.toByteArray();

            for (int u(0); u < usedUnits.size(); ++u)
            {
                if (unitsVisible.contains(usedUnits[u]))
                    unitData->setVisible(usedUnits[u], true);
                else
                    unitData->setVisible(usedUnits[u], false);
            }
        }
        else
        {
            QStringList unitsVisible = value.toStringList();

            for (int u(0); u < usedUnits.size(); ++u)
            {
                if (unitsVisible.contains(QString::number(usedUnits[u])))
                    unitData->setVisible(usedUnits[u], true);
                else
                    unitData->setVisible(usedUnits[u], false);
            }
        }
    }
}

/// @author Holly Ausbeck
/// @date   Aug 10, 2015
/// @brief  set the visible status of the units in the spike data from the list
/// @param [in] QVariant is one of the following:
//          QByteArray for role = UserRole
//          QStringList for role = DisplayRole or EditRole
/// @param [in] role
bool ChannelData::switchPages(const QVariant &value, int role)
{
    WizardPage lastPage = m_ePage;
    ConfigModel *cm = ConfigModel::instance();
    quint64 nMaxWaveforms = cm->item(0, OFSMODEL_COLUMN_MAX_DISPLAY_WAVEFORMS)->data(Qt::DisplayRole).toULongLong();

    m_ePage = (WizardPage)value.toInt();

    // if the user switched from detect to sort
    if ((lastPage == WIZARDPAGE_DETECT) && (m_ePage == WIZARDPAGE_SORT))
    {
        // if the user changes a bunch of channels from detect to sort and some channels have no waveforms, don't change them
        if ((m_nSamples != 0) && (m_nWaveforms == 0))
        {
            m_ePage = WIZARDPAGE_DETECT;
            return false;
        }
        // use continuous values to save the beginning and end of the visible time range from the file start
        m_nFileVisibleStart = m_nTimelineStart + m_nTimelineVisibleStart;
        m_nFileVisibleEnd = m_nTimelineStart + m_nTimelineVisibleEnd;

        // if all the waveforms fit in the timeline
        //    set the spike start and end to the entire time range
        if (m_nWaveforms <= nMaxWaveforms)
        {
            m_nTimespikeStart = m_nTimeStampMin;
            m_nTimespikeEnd = m_nTimeStampMax;
        }
        // if there are more waveforms than fit in the timeline
        //    set the spike start and end to the visible time range
        //    the range will be adjusted by the back end.
        //    Only the back end knows how many spikes there are in a particular time range
        else
        {
            m_nTimespikeStart = m_nFileVisibleStart;
            m_nTimespikeEnd = m_nTimeStampMax;
        }
        m_nTimespikeVisibleStart = m_nFileVisibleStart - m_nTimespikeStart;
        m_nTimespikeVisibleEnd = m_nFileVisibleEnd - m_nTimespikeStart;
    }
    // if the user switched from sort to detect
    else if ((m_ePage == WIZARDPAGE_DETECT) && (lastPage == WIZARDPAGE_SORT))
    {
        ConfigModel *cm = ConfigModel::instance();
        quint64 nMaxDisplaySamples = cm->item(0, OFSMODEL_COLUMN_MAX_DISPLAY_SAMPLES)->data(Qt::DisplayRole).toULongLong();

        // use spike values to save the beginning and end of the visible time range from the file start
        m_nFileVisibleStart = m_nTimespikeStart + m_nTimespikeVisibleStart;
        m_nFileVisibleEnd = m_nTimespikeStart + m_nTimespikeVisibleEnd;

        // don't display more than max samples
        if ((m_nFileVisibleEnd - m_nFileVisibleStart) > nMaxDisplaySamples)
        {
            m_nFileVisibleEnd = m_nFileVisibleStart + nMaxDisplaySamples;
        }
        // if the file total time fits in the timeline
        if ((m_nTimeStampMax - m_nTimeStampMin) <= nMaxDisplaySamples)
        {
            m_nTimelineStart = m_nTimeStampMin;
            m_nTimelineEnd = m_nTimeStampMax;
        }
        // the file total time doesn't fit in the timeline
        else
        {
            // if the current spike samples will fit in the timeline
            if ((m_nTimespikeEnd - m_nTimespikeStart) <= nMaxDisplaySamples)
            {
                m_nTimelineStart = m_nTimespikeStart;
            }
            // the current spike samples won't fit in the timeline
            else
            {
                // use the visible spike samples
                m_nTimelineStart = m_nTimespikeStart + m_nTimespikeVisibleStart;
            }
            // don't go past eof
            if ((m_nTimelineStart + nMaxDisplaySamples) > m_nTimeStampMax)
            {
                m_nTimelineStart = m_nTimeStampMax - nMaxDisplaySamples;
            }
            m_nTimelineEnd = m_nTimelineStart + nMaxDisplaySamples;
        }
        m_nTimelineVisibleStart = m_nFileVisibleStart - m_nTimelineStart;
        m_nTimelineVisibleEnd = m_nFileVisibleEnd - m_nTimelineStart;
    }
    return true;
}

/// @author Holly Ausbeck
/// @date   May 14, 2015
/// @brief  Utility function to set the visible (zoomed) timeline range
/// @param [in] channel
/// @param [in] min - min time in view
/// @param [in] max - max time in view
void ChannelData::setTimelineRange(qint64 min, qint64 max)
{
    // if the start time changes, adjust the visual start time to remain the same
    if ((m_nTimelineStart != min) || (m_nTimelineEnd != max))
    {
        m_nTimelineStart == min;
        m_nTimelineEnd == max;
        m_nTimelineVisibleStart = m_nFileVisibleStart - m_nTimelineStart;
        m_nTimelineVisibleEnd = m_nFileVisibleEnd - m_nTimelineStart;
    }
}

/// @author Holly Ausbeck
/// @date   May 14, 2015
/// @brief  Utility function to set the visible (zoomed) timeline range
/// @param [in] channel
/// @param [in] min - min time in view
/// @param [in] max - max time in view
void ChannelData::setTimespikeRange(qint64 min, qint64 max, qint64 firstID, qint64 lastID)
{
    // if the start time changes, adjust the visual start time to remain the same
    if ((m_nTimespikeStart != min) || (m_nTimespikeEnd != max))
    {
        m_nTimespikeStart == min;
        m_nTimespikeEnd == max;
        m_nTimespikeVisibleStart = m_nFileVisibleStart - m_nTimespikeStart;
        m_nTimespikeVisibleEnd = m_nFileVisibleEnd - m_nTimespikeEnd;
        m_nSpikeStart = firstID;
        m_nSpikeEnd = lastID;
    }
}
/// @author Holly Ausbeck
/// @date   May 26, 2016
/// @brief  Initialize the continuous times to display the max number of points from the beginning of the file
void ChannelData::initializeContinuousTimes()
{
    ConfigModel *cm = ConfigModel::instance();
    quint64 nSamples = cm->item(0, OFSMODEL_COLUMN_MAX_DISPLAY_SAMPLES)->data(Qt::DisplayRole).toULongLong();

    // reset the start to the beginning of the file
    m_nTimelineStart = m_nTimeStampMin;
    m_nTimelineVisibleStart = 0;
    quint64 range = m_nTimeStampMax - m_nTimeStampMin;

    // if all the samples fit, show all the samples
    if (range <= nSamples)
    {
        m_nTimelineEnd = m_nTimeStampMax;
        m_nTimelineVisibleEnd = range;
    }
    else // show the max samples starting at the beginning of the file
    {
        m_nTimelineEnd = m_nTimelineStart + nSamples;
        m_nTimelineVisibleEnd = nSamples;
    }
}

/// @author Holly Ausbeck
/// @date   May 26, 2016
/// @brief  Initialize the continuous times to display the max number of points from the given value
void ChannelData::setContinuousTimes(quint64 nValue, SpikeMoveReference reference)
{
    ConfigModel *cm = ConfigModel::instance();
    quint64 nSamples = cm->item(0, OFSMODEL_COLUMN_MAX_DISPLAY_SAMPLES)->data(Qt::DisplayRole).toULongLong();

    if (reference == CONTINUOUSMOVE_START)
    {
        if ((nValue + nSamples) < m_nTimeStampMax)
        {
            m_nTimelineStart = nValue;
            m_nTimelineEnd = m_nTimelineStart + nSamples;
        }
        else
        {
            m_nTimelineStart = m_nTimeStampMax - nSamples;
            m_nTimelineEnd = m_nTimeStampMax;
        }
    }
    else if (reference == CONTINUOUSMOVE_END)
    {
        if (nValue > nSamples)
        {
            m_nTimelineStart = nValue - nSamples;
            m_nTimelineEnd = nValue;
        }
        else
        {
            m_nTimelineStart = 0;
            m_nTimelineEnd = nSamples;
        }
    }
    m_nTimelineVisibleStart = 0;
    m_nTimelineVisibleEnd = m_nTimelineEnd - m_nTimelineVisibleStart;
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  Queue up a move request with the current data - used for channel change
void ChannelData::initializeSpikeTimes()
{
    m_nTimespikeStart = m_nTimeStampMin;
    m_nTimespikeVisibleStart = 0;
    quint64 range = m_nTimeStampMax - m_nTimeStampMin;

    // the back end calculates the time range for the max number of waveforms, so just set it display everything for now
    m_nTimespikeEnd = m_nTimeStampMax;
    m_nTimespikeVisibleEnd = range;
}

/// @author Holly Ausbeck
/// @date   Jan 29, 2016
/// @brief  clear the waveform selection and update the views
void ChannelData::clearSelection()
{
    FeaturesAccum featuresData = m_spikeData->features();

    if (featuresData)
    {
        featuresData->clearSelection();
        emitSignalIfDisplayed(row(), SIGNAL_WAVEFORM_SELECTION_CHANGED);
    }
}

void ChannelData::emitSignalIfDisplayed(int channel, ChannelModelSignals signal, QVariant value1, QVariant value2, QVariant value3)
{
    if (m_bDisplayed)
    {
        ChannelConfigModel *ccm = ChannelConfigModel::instance();
        ccm->emitSignal(channel, signal, value1, value2, value3);
    }
}