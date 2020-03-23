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

#ifndef ROOTDATA_H_INCLUDED
#define ROOTDATA_H_INCLUDED

#include <QItemSelection>
#include <QAbstractTableModel>
#include <QString>
#include <QVariant>
#include <QLineF>
#include <QXmlStreamWriter>


#include "../model/channelItem.h"
#include "../backend/file/fileinterface.h"


/// @note
/// columns in a RootData tree item
enum RootColumn
{
    RC_PAUSE_MAP,              // QMap<int, QVariant>
    RC_EVFILE_INIT,            // event file has been saved
    RC_FILE_TYPE,              // current file type
    RC_TIME_STAMP_RESOLUTION,  // 
    RC_GROUP_SIZE,             // channel group size
    RC_DEMO_MODE,              // session saved in demo mode with only first 12 channels
    RC_TIME_ORIGIN,
    RC_END
};


class RootData : public channelItem
{
    Q_OBJECT

public:
    RootData();
    ~RootData();
    void initialize();

    virtual int columnCount() const;
    virtual QVariant data(int column, int role) const;
    virtual bool setData(int column, const QVariant &value, int role = Qt::UserRole);

    // to/from xml
    virtual bool saveToXML(QXmlStreamWriter *writer, bool bPartialSave = false);
    virtual bool getFromXML(QXmlStreamReader *reader);

    // can't access pause map through data()/setData() because it is not supported by QVariant
    QVector<QMultiMap<quint64, quint64>> getPauseMaps();
    void addPauseMap(QMultiMap<quint64, quint64> pauseMap);

private:
    static QStringList ColumnNames;

    // Each of the following attributes correspond to a model root column
    // General 
    QVector<QMultiMap<quint64, quint64>> m_pauseMaps;  //RC_PAUSE_MAPS
    bool                            m_bEvFileInit;     //RC_EVFILE_INIT
    quint32                         m_timeStampRes;    //RC_TIME_STAMP_RESOLUTION
    InputFileType                   m_fileType;        //RC_FILE_TYPE
    quint32                         m_nGroupSize;      //RC_GROUP_SIZE
    bool                            m_bDemoMode;       //RC_DEMO_MODE
    QStringList                     m_time_origin;     //RC_TIME_ORIGIN

    QString getColumnName(RootColumn column);
};

#endif // ROOTDATA_H_INCLUDED


