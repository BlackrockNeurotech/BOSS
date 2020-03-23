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

#include "../model/intervalData.h"
#include "../model/channelconfigmodel.h"

/// @note
/// header string data for each column in a IntervalData tree item
// header string data for each column
/* static */
QStringList IntervalData::ColumnNames = QStringList()
    << "Samples" << "Waveforms"
    << "Valid Waveforms" << "Invalid Waveforms"
    << "Interval Start" << "Interval End";


/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Constructor for IntervalData class - initializes class variables
/// @param [in]  parent tree item
IntervalData::IntervalData(channelItem *parent)
    : channelItem(parent)
{
    m_nSamples = 0;
    m_nWaveforms = 0;
    m_nValidWaveforms = 0;
    m_nInvalidWaveforms = 0;
    m_nIntervalStart = 0;
    m_nIntervalEnd = 2000;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  get the number of columns belonging to this tree item
/// @return column count
int IntervalData::columnCount() const
{
    return IC_END;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  return the QVariant value of the data at the requested column for the role
/// @param [in] column number
/// @param [in] role
/// @return QVariant containing the value
QVariant IntervalData::data(int column, int role) const
{
    switch (column)
    {
    case IC_NSAMPLES:
        return getUInt64(m_nSamples, role);
        break;
    case IC_NWAVEFORMS:
        return getUInt64(m_nWaveforms, role);
        break;
    case IC_VALID_WAVEFORMS:
        return getUInt64(m_nValidWaveforms, role);
        break;
    case IC_INVALID_WAVEFORMS:
        return getUInt64(m_nInvalidWaveforms, role);
        break;
    case IC_INTERVAL_START:
        return getUInt64(m_nIntervalStart, role);
        break;
    case IC_INTERVAL_END:
        return getUInt64(m_nIntervalEnd, role);
        break;
    }
	return QVariant();
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  set the value of the data at the requested column
/// @param [in] column number
/// @param [in] value in the form of a QVariant
/// @param [in] address of the signal type to emit if anyone wants to know about the change
/// @param [in] role
/// @return TRUE if the value changed; FALSE otherwise
bool IntervalData::setData(int column, const QVariant &value, int role)
{
	Q_UNUSED(role);

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    bool changed = false;

    switch (column)
    {
    case IC_NSAMPLES:
        changed = setUInt64(&m_nSamples, value);
        break;
    case IC_NWAVEFORMS:
        changed = setUInt64(&m_nWaveforms, value);
        break;
    case IC_VALID_WAVEFORMS:
        changed = setUInt64(&m_nValidWaveforms, value);
        break;
    case IC_INVALID_WAVEFORMS:
        changed = setUInt64(&m_nInvalidWaveforms, value);
        break;
    case IC_INTERVAL_START:
        changed = setUInt64(&m_nIntervalStart, value);
        ccm->emitSignal(row(), SIGNAL_INTERVAL_RANGE_CHANGED);
        break;
    case IC_INTERVAL_END:
        changed = setUInt64(&m_nIntervalEnd, value);
        ccm->emitSignal(row(), SIGNAL_INTERVAL_RANGE_CHANGED);
        break;
    }
    return changed;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Convert the data in this tree item to XML
/// @param [in] Address of the XML stream to append the XML
/// @return TRUE if successfully writes the XML; FALSE otherwise
bool IntervalData::saveToXML(QXmlStreamWriter *writer, bool bPartialSave)
{
    Q_UNUSED(bPartialSave);

    writer->writeStartElement("IntervalChild");

    writer->writeTextElement(getColumnName(IC_NSAMPLES), QString::number(m_nSamples));
    writer->writeTextElement(getColumnName(IC_NWAVEFORMS), QString::number(m_nWaveforms));

    writer->writeTextElement(getColumnName(IC_VALID_WAVEFORMS), QString::number(m_nValidWaveforms));
    writer->writeTextElement(getColumnName(IC_INVALID_WAVEFORMS), QString::number(m_nInvalidWaveforms));

    writer->writeTextElement(getColumnName(IC_INTERVAL_START), QString::number(m_nIntervalStart));
    writer->writeTextElement(getColumnName(IC_INTERVAL_END), QString::number(m_nIntervalEnd));

    writer->writeEndElement();
    return true;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Populate this tree item with the data saved in the XML stream
/// @param [in] Address of the XML stream to read
/// @return TRUE if successfully writes the XML; FALSE otherwise
bool IntervalData::getFromXML(QXmlStreamReader *reader)
{
    if (reader->name() == "IntervalChild")
    {
        while(reader->readNextStartElement())
        {
            QStringRef name = reader->name();

            if (name == "IntervalChild")
            {
                continue;
            }
            if (name == getColumnName(IC_NSAMPLES))
            {
                m_nSamples = reader->readElementText().toLongLong();
            }
            else if (name == getColumnName(IC_NWAVEFORMS))
            {
                m_nWaveforms = reader->readElementText().toLongLong();
            }
            else if (name == getColumnName(IC_VALID_WAVEFORMS))
            {
                m_nValidWaveforms = reader->readElementText().toLongLong();
            }
            else if (name == getColumnName(IC_INVALID_WAVEFORMS))
            {
                m_nInvalidWaveforms = reader->readElementText().toLongLong();
            }
            else if (name == getColumnName(IC_INTERVAL_START))
            {
                m_nIntervalStart = reader->readElementText().toLongLong();
            }
            else if (name == getColumnName(IC_INTERVAL_END))
            {
                m_nIntervalEnd = reader->readElementText().toLongLong();
            }
            else
            {
                reader->skipCurrentElement();
            }
        }
    }
    return true;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  get the name of the column with no spaces or -'s which are illegal XML characters
/// @param [in] column
/// @return name
QString IntervalData::getColumnName(IntervalColumn column)
{
    QString name = ColumnNames.at(column);
    name = name.replace(QChar(' '), QString("_"));
    name = name.replace(QChar('-'), QString("_"));
    return name;
}
