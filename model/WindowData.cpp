/////////////////////////////////////////////////////////////////
///
/// @file WindowData.h
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  April 29, 2015
///
/// @brief WindowData contains window position data for each window
///
/// @note
/// WindowData subclasses layoutItem and represents a Channel Model tree item.
/// Each instance of WindowData contains the information for positioning a window.
///

#include "../model/WindowData.h"
#include "../model/channelconfigmodel.h"

/// @note
/// header string data for each column in a WindowData tree item
/* static */
QStringList WindowData::ColumnNames = QStringList()
<< "name" << "size" << "pos" << "floating" << "area" << "docks" << "visible" << "allowed" << "selected";


/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Constructor for WindowData class - initializes class variables
/// @param [in]  parent tree item
WindowData::WindowData(QString name, layoutItem *parent)
    : layoutItem(parent)
{
    m_sName = name;
    m_size = QSize(100, 100);
    m_pos = QPoint(0, 0);
    m_bFloating = false;
    m_nDockArea = (int)Qt::LeftDockWidgetArea;
    m_dockIDs.empty();
    m_visible.empty();
    m_allowed.empty();
    m_bSelected = false;
}

WindowData::WindowData(layoutItem *parent)
    : layoutItem(parent)
{
    m_sName = "";
    m_size = QSize(100, 100);
    m_pos = QPoint(0, 0);
    m_bFloating = false;
    m_nDockArea = (int)Qt::LeftDockWidgetArea;
    m_dockIDs.empty();
    m_visible.empty();
    m_allowed.empty();
    m_bSelected = false;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  get the number of columns belonging to this tree item
/// @return column count
int WindowData::columnCount() const
{
    return WC_END;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  return the QVariant value of the data at the requested column for the role
/// @param [in] column number
/// @param [in] role
/// @return QVariant containing the value
QVariant WindowData::data(int column) const
{
	switch (column)
    {
    case WC_NAME:
        return QVariant(m_sName);
        break;
    case WC_SIZE:
        return QVariant(m_size);
        break;
    case WC_POS:
        return QVariant(m_pos);
        break;
    case WC_FLOATING:
        return QVariant(m_bFloating);
        break;
    case WC_DOCKAREA:
        return QVariant(m_nDockArea);
        break;
    case WC_DOCKIDS:
        return QVariant(m_dockIDs);
        break;
    case WC_VISIBLE:
        return QVariant(m_visible);
        break;
    case WC_ALLOWED:
        return QVariant(m_allowed);
        break;
    case WC_SELECTED:
        return QVariant(m_bSelected);
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
void WindowData::setData(int column, const QVariant &value)
{
    switch (column)
    {
    case WC_NAME:
        m_sName = value.toString();
        break;
    case WC_SIZE:
        m_size = value.toSize();
        break;
    case WC_POS:
        m_pos = value.toPoint();
        break;
    case WC_FLOATING:
        m_bFloating = value.toBool();
        break;
    case WC_DOCKAREA:
        m_nDockArea = value.toInt();
        break;
    case WC_DOCKIDS:
        m_dockIDs = value.toList();
        break;
    case WC_VISIBLE:
        m_visible = value.toList();
        break;
    case WC_ALLOWED:
        m_allowed = value.toList();
        break;
    case WC_SELECTED:
        m_bSelected = value.toBool();
        break;
    }
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Convert the data in this tree item to XML
/// @param [in] Address of the XML stream to append the XML
/// @return TRUE if successfully writes the XML; FALSE otherwise
bool WindowData::saveToXML(QXmlStreamWriter *writer)
{
    writer->writeStartElement("WindowData");

    writer->writeTextElement(getColumnName(WC_NAME), m_sName);
    writer->writeTextElement(getColumnName(WC_SIZE) + QString("-x"), QString::number(m_size.width()));
    writer->writeTextElement(getColumnName(WC_SIZE) + QString("-y"), QString::number(m_size.height()));
    writer->writeTextElement(getColumnName(WC_POS) + QString("-x"), QString::number(m_pos.x()));
    writer->writeTextElement(getColumnName(WC_POS) + QString("-y"), QString::number(m_pos.y()));
    writer->writeTextElement(getColumnName(WC_FLOATING), QString::number(m_bFloating));
    writer->writeTextElement(getColumnName(WC_DOCKAREA), QString::number(m_nDockArea));

    QString name = getColumnName(WC_DOCKIDS);

    foreach(QVariant id, m_dockIDs)
    {
        writer->writeTextElement(name, QString::number(id.toInt()));
    }
    name = getColumnName(WC_VISIBLE);

    foreach(QVariant id, m_visible)
    {
        writer->writeTextElement(name, QString::number(id.toInt()));
    }
    name = getColumnName(WC_ALLOWED);

    foreach(QVariant id, m_allowed)
    {
        writer->writeTextElement(name, QString::number(id.toInt()));
    }
    writer->writeTextElement(getColumnName(WC_SELECTED), QString::number(m_bSelected));
    writer->writeEndElement();
    return true;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Populate this tree item with the data saved in the XML stream
/// @param [in] Address of the XML stream to read
/// @return TRUE if successfully writes the XML; FALSE otherwise
bool WindowData::getFromXML(QXmlStreamReader *reader)
{
    if (reader->name() == "WindowData")
    {
        m_dockIDs.empty();

        while(reader->readNextStartElement())
        {
            QStringRef name = reader->name();

            if (name == "WindowData")
            {
                continue;
            }
            else if (name == getColumnName(WC_NAME))
            {
                m_sName = reader->readElementText();
            }
            else if (name == getColumnName(WC_SIZE) + QString("-x"))
            {
                m_size.setWidth(reader->readElementText().toInt());
            }
            else if (name == getColumnName(WC_SIZE) + QString("-y"))
            {
                m_size.setHeight(reader->readElementText().toInt());
            }
            else if (name == getColumnName(WC_POS) + QString("-x"))
            {
                m_pos.setX(reader->readElementText().toInt());
            }
            else if (name == getColumnName(WC_POS) + QString("-y"))
            {
                m_pos.setY(reader->readElementText().toInt());
            }
            else if (name == getColumnName(WC_FLOATING))
            {
                m_bFloating = reader->readElementText().toInt();
            }
            else if (name == getColumnName(WC_DOCKAREA))
            {
                m_nDockArea = reader->readElementText().toInt();
            }
            else if (name == getColumnName(WC_DOCKIDS))
            {
                m_dockIDs.append(reader->readElementText().toInt());
            }
            else if (name == getColumnName(WC_VISIBLE))
            {
                m_visible.append(reader->readElementText().toInt());
            }
            else if (name == getColumnName(WC_ALLOWED))
            {
                m_allowed.append(reader->readElementText().toInt());
            }
            else if (name == getColumnName(WC_SELECTED))
            {
                m_bSelected = reader->readElementText().toInt();
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
QString WindowData::getColumnName(WindowColumn column)
{
    QString name = ColumnNames.at(column);
    name = name.replace(QChar(' '), QString("_"));
    name = name.replace(QChar('-'), QString("_"));
    return name;
}
