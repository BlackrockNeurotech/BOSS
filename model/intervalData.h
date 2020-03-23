/////////////////////////////////////////////////////////////////
///
/// @file intervalData.h
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  April 29, 2015
///
/// @brief IntervalData contains segment information for each channel
///
/// @note
/// IntervalData subclasses channelItem and represents a Channel Model tree item.
/// Each instance of IntervalData contains the information for a time segment.
///

#ifndef INTERVALDATA_H_INCLUDED
#define INTERVALDATA_H_INCLUDED

#include <QItemSelection>
#include <QAbstractTableModel>
#include <QString>

#include "../model/channelItem.h"


/// @note
/// columns in a IntervalData tree item
enum IntervalColumn
{
    // General
    IC_NSAMPLES = 0,              // int  number of samples
    IC_NWAVEFORMS,                // int  number of waveforms

    // Nev values
    IC_VALID_WAVEFORMS,           // int  number of valid waveforms
    IC_INVALID_WAVEFORMS,         // int  number of invalid waveforms

    // interval range
    IC_INTERVAL_START,            // int  Start of nsx interval
    IC_INTERVAL_END,              // int  End of nsx interval

    IC_END
};

class IntervalData : public channelItem
{
    Q_OBJECT

public:
    IntervalData(channelItem *parent = 0);

    virtual int columnCount() const;
    virtual QVariant data(int column, int role = Qt::UserRole) const;
    virtual bool setData(int column, const QVariant &value, int role = Qt::UserRole);
    virtual bool saveToXML(QXmlStreamWriter *writer, bool bPartialSave = false);
    virtual bool getFromXML(QXmlStreamReader *reader);

private:
    // The following string list corresponds to the IntervalColumn enum
    static QStringList ColumnNames;

    // Each of the following attributes correspond to a model column
    quint64 m_nSamples;          //IC_NSAMPLES
    quint64 m_nWaveforms;        //IC_NWAVEFORMS

    quint64 m_nValidWaveforms;   //IC_VALID_WAVEFORMS
    quint64 m_nInvalidWaveforms; //IC_INVALID_WAVEFORMS

    quint64 m_nIntervalStart;    //IC_INTERVAL_START
    quint64 m_nIntervalEnd;      //IC_INTERVAL_END

    QString getColumnName(IntervalColumn column);
};

#endif // INTERVALDATA_H_INCLUDED
