/////////////////////////////////////////////////////////////////
///
/// @file layoutItem.h
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  July 8, 2015
///
/// @brief layoutItem is the base class for layout tree element.
///
/// @note
/// This class should be subclassed when creating a new layout tree element.
/// It is not abstract because it is used directly for the root item which contains no data.
/// It includes the basic functionallity that layout tree elements must implement.
/// It is used for traversing the layout tree.
///

#ifndef LAYOUTITEM_H_INCLUDED
#define LAYOUTITEM_H_INCLUDED

#include <QList>
#include <QVariant>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

class layoutItem : public QObject
{
    Q_OBJECT

public:
    layoutItem(layoutItem *parentItem = 0);
    ~layoutItem();

    void appendChild(layoutItem *child);

    layoutItem *child(int row);
    int childCount() const;
    int row() const;
    layoutItem *parentItem();

    void removeChild(int row);
    void removeChildren();

    virtual int columnCount() const { return 0; }
	virtual QVariant data(int column) const  { Q_UNUSED(column); return QVariant(); }
	virtual void setData(int column, const QVariant &value) { Q_UNUSED(column); Q_UNUSED(value); }
	virtual bool saveToXML(QXmlStreamWriter *writer) { Q_UNUSED(writer); return false; }
	virtual bool getFromXML(QXmlStreamReader *reader) { Q_UNUSED(reader); return false; }
    virtual bool enabled() { return true; }

private:
    QList<layoutItem*> m_childItems;
    layoutItem *m_parentItem;
};

#endif // LAYOUTITEM_H_INCLUDED
