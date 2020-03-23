/////////////////////////////////////////////////////////////////
///
/// @file channelItem.h
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  April 29, 2015
///
/// @brief channelItem is the base class for a Channel Model tree element.
///
/// @note
/// This class should be subclassed when creating a new Channel Model tree element.
/// It is not abstract because it is used directly for the root item which contains no data.
/// It includes the basic functionallity that Channel Model tree elements must implement.
/// It is used for traversing the Channel Model tree.
///

#ifndef CHANNELITEM_H_INCLUDED
#define CHANNELITEM_H_INCLUDED

#include <QList>
#include <QVariant>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

/// @note
/// enums used by ChannelData values.

enum WizardPage
{
	WIZARDPAGE_DETECT = 0,
    WIZARDPAGE_SORT,
    WIZARDPAGE_EVALUATE,
    WIZARDPAGE_END
};

enum HighpassFilter
{
    HIGHPASS_FILTER_NONE,
    HIGHPASS_BUTTERWORTH,
    HIGHPASS_CHEBYSHEV1,
    HIGHPASS_BESSEL
};

enum SignalEnergy
{
    ADDITIONAL_NONE,
    ADDITIONAL_SIGNAL_ENERGY,
    ADDITIONAL_NONLINEAR_SIGNAL_ENERGY,
    ADDITIONAL_SIGNED_SIGNAL_ENERGY,
    ADDITIONAL_TEMPLATE_MATCHING
};

enum ThresholdType
{
    THRESHOLDTYPE_SINGLE,
    THRESHOLDTYPE_DUAL,
    THRESHOLDTYPE_NOISE_RMS,
    THRESHOLDTYPE_ENERGY,
    THRESHOLDTYPE_NONE
};

typedef enum SortMethod_
{
    SORTMETHOD_KMEANS,
    SORTMETHOD_KNN,
    SORTMETHOD_TDIST,
    SORTMETHOD_TEMPLATE,
    SORTMETHOD_VALLEYSEEKING,
    SORTMETHOD_MANUAL,
    SORTMETHOD_NONE
} SortMethod;

enum AlignMode
{
    ALIGNMODE_ORIGINAL,
    //ALIGNMODE_FIRST_LOCAL_MIN,
    //ALIGNMODE_FIRST_LOCAL_MAX,
    //ALIGNMODE_FIRST_LOCAL_EXTREMUM,
    //ALIGNMODE_FIRST_LOCAL_EXTREMUM_AFTER_THR,
    ALIGNMODE_GLOBAL_MIN,
    ALIGNMODE_GLOBAL_MAX
};

enum ChannelState
{
    CHANNELSTATE_INACTIVE,
    CHANNELSTATE_SELECTED,
    CHANNELSTATE_DETECTING,
    CHANNELSTATE_SORTING,
    CHANNELSTATE_ALIGNING,
    CHANNELSTATE_APPLY_OPTIONS
};

enum EditMode
{
    EDITMODE_SELECT,
    EDITMODE_LINES,
    EDITMODE_BOXES
};

enum GLWidgetState
{
    GLWIDGETSTATE_ROTATE = 0,
    GLWIDGETSTATE_EDIT,
    GLWIDGETSTATE_CENTROIDS
};

#define INVALID_WF -1
#define UNINTIALIZED_UNIT -2

enum FeatureID
{
    FEATUREID_PCA1 = 0,
    FEATUREID_PCA2,
    FEATUREID_PCA3,
    FEATUREID_PCA4,
    FEATUREID_PCA5,
    FEATUREID_PCA6,
    FEATUREID_PCA7,
    FEATUREID_PCA8,
    FEATUREID_PCA9,
    FEATUREID_PCA10,
    FEATUREID_PEAK_VALUE,
    FEATUREID_VALLEY_VALUE,
    FEATUREID_TIME_TO_MAX,
    FEATUREID_TIME_TO_MIN,
    FEATUREID_AREA,
    FEATUREID_PEAK_FWHM,
    FEATUREID_VALLEY_FWHM,
    FEATUREID_TIMESTAMP,        // Keep this one always one before the end
    FEATUREID_END
};

enum UnitOperation
{
    UNITOPERATION_SETUNIT = 0,
    UNITOPERATION_DELETE,
    UNITOPERATION_INVALIDATE,
    UNITOPERATION_REVALIDATE
};

// Feature bitmap
const int FEATURE_PCA_1 = 0X01;
const int FEATURE_PCA_2 = 0X02;
const int FEATURE_PCA_3 = 0X04;
const int FEATURE_PCA_4 = 0X08;
const int FEATURE_PCA_5 = 0X010;
const int FEATURE_PCA_6 = 0X020;
const int FEATURE_PCA_7 = 0X040;
const int FEATURE_PCA_8 = 0X080;
const int FEATURE_PCA_9 = 0X0100;
const int FEATURE_PCA_10 = 0X0200;
const int FEATURE_PEAK_VALUE = 0X0400;
const int FEATURE_VALLEY_VALUE = 0X0800;
const int FEATURE_TIME_TO_MAX = 0X01000;
const int FEATURE_TIME_TO_MIN = 0X02000;
const int FEATURE_AREA = 0X04000;
const int FEATURE_PEAK_FWHM = 0X08000;
const int FEATURE_VALLEY_FWHM = 0X010000;

const int FEATURE_PCA_MASK = 0X03F;


enum SliderType
{
    SLIDERTYPE_NA,
    SLIDERTYPE_THRESHOLD_DUAL,   // both SLIDERTYPE_THRESHOLD1 & SLIDERTYPE_THRESHOLD2
    SLIDERTYPE_THRESHOLD1,
    SLIDERTYPE_THRESHOLD2,
    SLIDERTYPE_THRESHOLD_ENERGY,
    SLIDERTYPE_CUTOFF_FREQ,
    SLIDERTYPE_ENDPOINT,
    SLIDERTYPE_SEGMENT,
    SLIDERTYPE_TIME
};

// signals emitted by the Channel Model
enum ChannelModelSignals
{
    SIGNAL_WIZARDPAGE_CHANGED,
    SIGNAL_ENERGY_TYPE_CHANGED,
    SIGNAL_SORT_FEATURES_CHANGED,
    SIGNAL_VIEW_FEATURES_CHANGED,
    SIGNAL_VISIBLE_UNITS_CHANGED,
    SIGNAL_3D_AXIS_CHANGED,
    SIGNAL_2D_AXIS_CHANGED,
    SIGNAL_3D_EDIT_STATE_CHANGED,
    SIGNAL_FILTERING_CHANGED,
    SIGNAL_THRESHOLD_TYPE_CHANGED,
    SIGNAL_SLIDER_CHANGED,
    SIGNAL_INTERVAL_RANGE_CHANGED,
    SIGNAL_DATA_CHANGED,
    SIGNAL_VISIBLE_TIME_RANGE_CHANGED,
    SIGNAL_VISIBLE_SPIKE_TIME_RANGE_CHANGED,
    SIGNAL_VERTICAL_RANGE_CHANGED,
    SIGNAL_RECALC_SPIKE_SLICE,
    SIGNAL_RECALC_TIME_SLICE,
    SIGNAL_RECALC_TIME_STAMP,
    SIGNAL_DETECT,
    SIGNAL_SORT,
    SIGNAL_STATE_CHANGED,
    SIGNAL_SPIKES_INITIALIZED,
    SIGNAL_FEATURES_CHANGED,
    SIGNAL_UNITS_CHANGED,
    SIGNAL_CORRELOGRAMS_CHANGED,
    SIGNAL_CLUSTERING_MATRIX_CHANGED,
    SIGNAL_WAVEFORM_SELECTION_CHANGED,
    SIGNAL_WAVEFORM_EDIT_MODE_CHANGED,
    SIGNAL_ERROR,
    SIGNAL_UPDATE_PCA,
    SIGNAL_CLEAR_SELECTION,
    SIGNAL_CHANNEL_SELECTION_CHANGED,
    SIGNAL_NONE
};

enum SpikeMoveReference
{
    SPIKEMOVE_START,
    SPIKEMOVE_END,
    CONTINUOUSMOVE_START,
    CONTINUOUSMOVE_END
};


class channelItem : public QObject
{
    Q_OBJECT

public:
    channelItem(channelItem *parentItem = 0);
    ~channelItem();

    void appendChild(channelItem *child);

    channelItem *child(int row);
    int childCount() const;
    int row() const;
    channelItem *parentItem();
    

    void removeChild(int row);
    void removeChildren();

    virtual int columnCount() const { return 0; }
	virtual QVariant data(int column, int role) const  { Q_UNUSED(column); Q_UNUSED(role); return QVariant(); }
	virtual bool setData(int column, const QVariant &value, int role = Qt::UserRole) { Q_UNUSED(column); Q_UNUSED(value); Q_UNUSED(role); return false; }
    virtual bool saveToXML(QXmlStreamWriter *writer, bool bPartialSave = false) { Q_UNUSED(writer); Q_UNUSED(bPartialSave);  return false; }
	virtual bool getFromXML(QXmlStreamReader *reader) { Q_UNUSED(reader); return false; }
    virtual bool enabled() { return true; }

protected:
    // General functions for getting and setting a quint64
    QVariant getUInt64(quint64 nValue, int role) const;
    bool setUInt64(quint64 *member, const QVariant &value);

    // General functions for getting and setting a qint64
    QVariant getInt64(qint64 nValue, int role) const;
    bool setInt64(qint64 *member, const QVariant &value);


private:
    QList<channelItem*> m_childItems;
    channelItem *m_parentItem;
};

#endif // CHANNELITEM_H_INCLUDED
