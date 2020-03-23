/////////////////////////////////////////////////////////////////
///
/// @file EditLines.h
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  July 28, 2015
///
/// @brief EditLines contains information for the edit EditLines
///
/// @note
/// Edit EditLines are created in WaveformGraphicsView. They are a way of defining a unit - all waveforms that intersect all the EditLines.
///

#ifndef EDITLINES_H_INCLUDED
#define EDITLINES_H_INCLUDED

#include <QSharedData>
#include <QVariant>
#include <QLineF>
#include <QXmlStreamWriter>


struct LineData
{
public:
    int addLine(QLineF line)
    {
        m_editLines.append(line);
        return m_editLines.count() - 1;
    }
    QLineF getLine(int index)
    {
        return m_editLines.at(index);
    }
    bool removeLine(int index)
    {
        if (index < count())
        {
            m_editLines.removeAt(index);
            return true;
        }
        return false;
    }
    bool removeLine(QLineF line)
    {
        return m_editLines.removeOne(line);
    }
    bool lineIndex(QLineF line)
    {
        return m_editLines.indexOf(line);
    }
    void clear()
    {
        m_editLines.clear();
    }
    int count()
    {
        return m_editLines.count();
    }
    QPointF getPoint(int lineIndex, int pointIndex)
    {
        if ((m_editLines.count() > lineIndex) && (2 > pointIndex))
        {
            QLineF line = m_editLines[lineIndex];

            if (pointIndex == 0)
                return line.p1();
            else
                return line.p2();
        }
        return QPointF();
    }
    void setPoint(int lineIndex, int pointIndex, QPointF point)
    {
        if ((m_editLines.count() > lineIndex) && (2 > pointIndex))
        {
            if (pointIndex == 0)
                m_editLines[lineIndex].setP1(point);
            else
                m_editLines[lineIndex].setP2(point);
        }
    }
    QVector<QLineF> getLines()
    {
        QVector<QLineF> linesVector;

        for (int i = 0; i < m_editLines.count(); i++)
            linesVector.append(m_editLines.at(i));

        return linesVector;
    }
    bool saveToXML(QXmlStreamWriter *writer)
    {
        QList<QLineF>::iterator itLine = m_editLines.begin();

        while (itLine != m_editLines.end())
        {
            writer->writeStartElement("Line");

            writer->writeTextElement("x1", QString::number(itLine->x1()));
            writer->writeTextElement("y1", QString::number(itLine->y1()));
            writer->writeTextElement("x2", QString::number(itLine->x2()));
            writer->writeTextElement("y2", QString::number(itLine->y2()));

            writer->writeEndElement();
            itLine++;
        }
        return true;
    }
    bool getFromXML(QXmlStreamReader *reader)
    {
        clear();

        while (reader->readNextStartElement())
        {
            QStringRef n = reader->name();
            qreal x1 = 0;
            qreal y1 = 0;
            qreal x2 = 0;
            qreal y2 = 0;

            if (n == "Line")
            {
                while (reader->readNextStartElement())
                {
                    QStringRef n3 = reader->name();

                    if (n3 == "x1")
                    {
                        x1 = reader->readElementText().toFloat();
                    }
                    else if (n3 == "y1")
                    {
                        y1 = reader->readElementText().toFloat();
                    }
                    else if (n3 == "x2")
                    {
                        x2 = reader->readElementText().toFloat();
                    }
                    else if (n3 == "y2")
                    {
                        y2 = reader->readElementText().toFloat();
                    }
                }
            }
            QLineF line(x1, y1, x2, y2);
            m_editLines.append(line);
        }
        return true;
    }

private:
    QList<QLineF> m_editLines;
};

// Shared Data Pointer for Lines
class EditLines : public QSharedData
{
public:
    EditLines();
    EditLines(const EditLines &other);
    ~EditLines();

    LineData m_data;
};
typedef QExplicitlySharedDataPointer<EditLines> EditLinesData;


#endif // EDITLINES_H_INCLUDED
