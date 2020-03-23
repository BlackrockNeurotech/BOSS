/////////////////////////////////////////////////////////////////
///
/// @file WindowData.h
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  June 29, 2015
///
/// @brief WindowData contains window position data for each window
///
/// @note
/// WindowData subclasses layoutItem and represents a Channel Model tree item.
/// Each instance of WindowData contains the information for positioning a window.
///

#ifndef WINDOWDATA_H_INCLUDED
#define WINDOWDATA_H_INCLUDED

#include <QString>
#include <QSize>
#include <QPoint>

#include "../model/layoutitem.h"


/// @note
/// columns in a WindowData tree item
enum WindowColumn
{
    // General
    WC_NAME = 0,           // Name of the window
    WC_SIZE,               // QSize  window size in pixels
    WC_POS,                // QPoint window position
    WC_FLOATING,           // bool true if window is floating
    WC_DOCKAREA,           // int dock area
    WC_DOCKIDS,            // list of dock windows this window is tabbed with
    WC_VISIBLE,            // bool true if window is visible - contains state for each page
    WC_ALLOWED,            // bool true if window is allowed - contains state for each page
    WC_SELECTED,           // bool true if window is tabbed and selected
    WC_END
};

class WindowData : public layoutItem
{
    Q_OBJECT

public:
    WindowData(layoutItem *parent = 0);
    WindowData(QString name, layoutItem *parent = 0);

    virtual int columnCount() const;
    virtual QVariant data(int column) const;
    virtual void setData(int column, const QVariant &value);
    virtual bool saveToXML(QXmlStreamWriter *writer);
    virtual bool getFromXML(QXmlStreamReader *reader);

private:
    // The following string list corresponds to the WindowColumn enum
    static QStringList ColumnNames;

    // Each of the following attributes correspond to a model column
    QString m_sName;

    QSize           m_size;       //WC_SIZE
    QPoint          m_pos;        //WC_POS
    bool            m_bFloating;  //WC_FLOATING
    bool            m_bSelected; //WC_SELECTED
    int             m_nDockArea;  //WC_DOCKAREA
    QList<QVariant> m_dockIDs;    //WC_DOCKIDS
    QList<QVariant> m_visible;    //WC_VISIBLE
    QList<QVariant> m_allowed;    //WC_ALLOWED

    QString getColumnName(WindowColumn column);
};

#endif // WINDOWDATA_H_INCLUDED
