/////////////////////////////////////////////////////////////////
///
/// @file RootData.h
/// @attention  (c) Copyright 2016 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  Sept 29, 2016
///
/// @brief RootData contains all the information for a file that is saved in the model.
///
/// @note
/// RootData subclasses channelItem and represents a Channel Model tree root item.
/// Each instance of RootData contains the information for a file that is not channel specific.
/// RootData has children representing each channel.
///

#include "../model/RootData.h"
#include "../model/channelconfigmodel.h"

/// @note
/// header string data for each column in a RootData tree item
/* static */
QStringList RootData::ColumnNames = QStringList()
/* General */
<< "Pauses" << "CDFile Init" << "File Type" << "Time Stamp Resolution" << "Group Size" << "Demo Mode" << "Time Origin";


/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Constructor for RootData class - initializes class variables
/// @param [in]  parent tree item
RootData::RootData()
    : channelItem()
{
    m_bEvFileInit = false;
    m_fileType = none;
    m_timeStampRes = 30000;
    m_nGroupSize = 1;
    m_bDemoMode = false;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Destructor for RootData class
RootData::~RootData()
{
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  set the channel number in the process queue and the waveform characteristics
void RootData::initialize()
{
}


/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  get the number of columns belonging to this tree item
/// @return column count
int RootData::columnCount() const
{
    return RC_END;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  return the QVariant value of the data at the requested column for the role
/// @param [in] column number
/// @param [in] role
/// @return QVariant containing the value
QVariant RootData::data(int column, int role) const
{
    QVariant v = QVariant();

    switch (column)
    {
    case RC_PAUSE_MAP:
        break;
    case RC_EVFILE_INIT:
        v = QVariant(m_bEvFileInit);
        break;
    case RC_TIME_STAMP_RESOLUTION:
        v = QVariant(m_timeStampRes);
        break;
    case RC_FILE_TYPE:
        v = QVariant(m_fileType);
        break;
    case RC_GROUP_SIZE:
        v = QVariant(m_nGroupSize);
        break;
    case RC_DEMO_MODE:
        v = QVariant(m_bDemoMode);
        break;
    case RC_TIME_ORIGIN:
        v = QVariant(m_time_origin);
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
bool RootData::setData(int column, const QVariant &value, int role)
{
    bool changed = false;

    switch (column)
    {
    case RC_PAUSE_MAP:
        break;
    case RC_EVFILE_INIT:
        m_bEvFileInit = value.toBool();
        break;
    case RC_TIME_STAMP_RESOLUTION:
        m_timeStampRes = value.toUInt();
        break;
    case RC_FILE_TYPE:
        m_fileType = (InputFileType)value.toUInt();
        break;
    case RC_GROUP_SIZE:
        m_nGroupSize = value.toUInt();
        break;
    case RC_DEMO_MODE:
        m_bDemoMode = value.toBool();
        break;
    case RC_TIME_ORIGIN:
        m_time_origin = value.toStringList();
        QVariant(m_time_origin);
    }
    return changed;
}

// can't access pause map through data()/setData() because it is not supported by QVariant
QVector<QMultiMap<quint64, quint64>> RootData::getPauseMaps()
{
    return m_pauseMaps;
}

void RootData::addPauseMap(QMultiMap<quint64, quint64> pauseMap)
{
    m_pauseMaps.append(pauseMap);
}

/// @author Holly Ausbeck
/// @date   Sept 29, 2016
/// @brief  Convert the data in this tree item to XML
/// @param [in] Address of the XML stream to append the XML
/// @param [in] If true only detecting and sorting parameters will be saved
/// @return TRUE if successfully writes the XML; FALSE otherwise
bool RootData::saveToXML(QXmlStreamWriter *writer, bool bPartialSave)
{
    writer->writeStartElement("Root");

    writer->writeStartElement(getColumnName(RC_PAUSE_MAP));
    QVector<QMultiMap<quint64, quint64>>::iterator vit = m_pauseMaps.begin();

    while (vit != m_pauseMaps.end())
    {
        writer->writeStartElement("filepause");
        QMultiMap<quint64, quint64>::iterator mit = vit->begin();

        while (mit != vit->end())
        {
            writer->writeStartElement("pause");
            QString sTimestamp = QString::number(mit.key());
            QString sDuration = QString::number(mit.value());
            writer->writeTextElement("ts", sTimestamp);
            writer->writeTextElement("duration", sDuration);
            writer->writeEndElement(); //pause
            mit++;
        }
        writer->writeEndElement(); //filepause
        vit++;
    }
    writer->writeEndElement(); //RC_PAUSE_MAP

    writer->writeTextElement(getColumnName(RC_EVFILE_INIT), QString::number(m_bEvFileInit));
    writer->writeTextElement(getColumnName(RC_TIME_STAMP_RESOLUTION), QString::number(m_timeStampRes));
    writer->writeTextElement(getColumnName(RC_FILE_TYPE), QString::number(m_fileType));
    writer->writeTextElement(getColumnName(RC_GROUP_SIZE), QString::number(m_nGroupSize));
    writer->writeTextElement(getColumnName(RC_DEMO_MODE), QString::number(m_bDemoMode));
    
    writer->writeStartElement(getColumnName(RC_TIME_ORIGIN));

    foreach(QString str, m_time_origin)
    {
        writer->writeTextElement("time", str);
    }
    writer->writeEndElement();

    writer->writeEndElement(); //"Root"
    return true;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Populate this tree item with the data saved in the XML stream
/// @param [in] Address of the XML stream to read
/// @return TRUE if successfully writes the XML; FALSE otherwise
bool RootData::getFromXML(QXmlStreamReader *reader)
{
    m_pauseMaps.clear();

    if (reader->name() == "Root")
    {
        while(reader->readNextStartElement())
        {
            QStringRef name = reader->name();

            if (name == "Root")
            {
                continue;
            }
            else if (name == getColumnName(RC_PAUSE_MAP))
            {
                QVector<QMap<QString, QVariant>> pausemaps;

                while (reader->readNextStartElement())
                {
                    QStringRef vname = reader->name();

                    if (vname == "filepause")
                    {
                        QMap<quint64, quint64> pausemap;

                        while (reader->readNextStartElement())
                        {
                            QStringRef mname = reader->name();

                            if (mname == "pause")
                            {
                                QString sTimestamp = "";
                                QString sDuration = "";

                                while (reader->readNextStartElement())
                                {
                                    QStringRef ename = reader->name();

                                    if (ename == "ts")
                                    {
                                        sTimestamp = reader->readElementText();
                                    }
                                    if (ename == "duration")
                                    {
                                        sDuration = reader->readElementText();
                                    }
                                }
                                pausemap.insert(sTimestamp.toUInt(), sDuration.toUInt());
                            }
                        }
                        addPauseMap(pausemap);
                    }
                }
            }
            else if (name == getColumnName(RC_EVFILE_INIT))
            {
                m_bEvFileInit = reader->readElementText().toInt();
            }
            else if (name == getColumnName(RC_TIME_STAMP_RESOLUTION))
            {
                m_timeStampRes = reader->readElementText().toInt();
            }
            else if (name == getColumnName(RC_FILE_TYPE))
            {
                m_fileType = (InputFileType)reader->readElementText().toInt();
            }
            else if (name == getColumnName(RC_GROUP_SIZE))
            {
                m_nGroupSize = reader->readElementText().toInt();
            }
            else if (name == getColumnName(RC_DEMO_MODE))
            {
                m_bDemoMode = reader->readElementText().toInt();
            }
            else if (name == getColumnName(RC_TIME_ORIGIN))
            {
                m_time_origin.clear();

                while (reader->readNextStartElement())
                {
                    name = reader->name();

                    if (name != "time")
                    {
                        break;
                    }
                    QString value = reader->readElementText();
                    m_time_origin.push_back(value);
                }
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
/// @date   Sept 29, 2016
/// @brief  get the name of the column with no spaces or -'s which are illegal XML characters
/// @param [in] column
/// @return name
QString RootData::getColumnName(RootColumn column)
{
    QString name = ColumnNames.at(column);
    name = name.replace(QChar(' '), QString("_"));
    name = name.replace(QChar('2'), QString("Two"));
    name = name.replace(QChar('3'), QString("Three"));
    return name;
}

