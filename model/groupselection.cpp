#include <QtWidgets>
#include <math.h>

#include "../model/groupselection.h"

bool GroupSelection::m_bImagesSet = false;

QImage *GroupSelection::m_displayImage;
QImage *GroupSelection::m_displaySortImage;
QImage *GroupSelection::m_displayDetectImage;
QImage *GroupSelection::m_displayAlignImage;
QImage *GroupSelection::m_selectImage;
QImage *GroupSelection::m_detectImage;
QImage *GroupSelection::m_sortImage;
QImage *GroupSelection::m_alignImage;
QImage *GroupSelection::m_blankImage;


GroupSelection::GroupSelection()
{
    m_eState = CHANNELSTATE_INACTIVE;
    m_groupSize = 1;
    m_optionSelection = 0;
    m_bDisplayed = false;
    m_sText = "";

    if (!m_bImagesSet)
    {
        setImages();
    }
}


void GroupSelection::setImages()
{
    m_blankImage = new QImage(":/new/Bitmaps/Bitmaps/ChannelBlank.png");
    m_displayImage = new QImage(":/new/Bitmaps/Bitmaps/ChannelView.png");
    m_displaySortImage = new QImage(":/new/Bitmaps/Bitmaps/ChannelViewSort.png");
    m_displayDetectImage = new QImage(":/new/Bitmaps/Bitmaps/ChannelViewDetect.png");
    m_displayAlignImage = new QImage(":/new/Bitmaps/Bitmaps/ChannelViewAlign.png");
    m_selectImage = new QImage(":/new/Bitmaps/Bitmaps/ChannelSelect.png");
    m_detectImage = new QImage(":/new/Bitmaps/Bitmaps/ChannelDetect.png");
    m_sortImage = new QImage(":/new/Bitmaps/Bitmaps/ChannelSort.png");
    m_alignImage = new QImage(":/new/Bitmaps/Bitmaps/ChannelAlign.png");

    m_bImagesSet = true;
}

void GroupSelection::setApplyOptions(int optionSelection)
{
    m_optionSelection = optionSelection; 
}

bool GroupSelection::setOptionSelection(int y, QRect rect)
{
    int optionSelection = y * m_groupSize / rect.height();
    
    if (optionSelection != m_optionSelection)
    {
        m_optionSelection = optionSelection;
        return true;
    }
    return false;
}

void GroupSelection::paint(QPainter *painter, const QRect &rect, const QPalette &palette, EditMode mode, bool bMouseOver, bool bEnabled) const
{
    painter->save();

    //if (mode == Editable)
    //{
    //    painter->drawRect(rect);
    //}

    // if the channels are displayed, draw a black outline around the outside
    if (m_eState == CHANNELSTATE_SELECTED)
    {
        painter->fillRect(rect, palette.highlight());
    }
    else
    {
        painter->fillRect(rect, Qt::white);
    }
    if (m_bDisplayed)
    {
        QPen pen(Qt::black);
        pen.setWidth(0.5);
        painter->setPen(pen);
        QRect rOutline(rect);
        rOutline.adjust(1, 1, -2, -1);
        painter->fillRect(rOutline, QColor("#EAF6FF"));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(rOutline);

        if ((m_optionSelection != -1) && (m_groupSize > 1)) // && m_bDisplayed
        {
            QPen pen(Qt::black);
            pen.setWidth(0.5);
            painter->setPen(pen);
            QRect rOutline(rect);
            int groupHeight = rect.height() / m_groupSize;
            rOutline.setHeight(groupHeight);
            rOutline.moveTo(0, groupHeight * m_optionSelection);
            rOutline.adjust(3, 3, -4, 0);
            painter->fillRect(rOutline, QColor(Qt::white));// "#EAF6FF"));
            painter->setBrush(Qt::NoBrush);
            painter->drawRect(rOutline);
        }
    }
    // either draw the image or the text
    if (!drawImage(painter, rect))
    {
        if (!bEnabled)
        {
            QPen pen(Qt::gray);
            pen.setWidth(0.5);
            painter->setPen(pen);
        }
        QRectF textr = rect;
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->drawText(textr, Qt::AlignCenter, m_sText);
    }
    painter->restore();
}

bool GroupSelection::drawImage(QPainter *painter, const QRect &rect) const
{
    QImage *image = 0;
    painter->setRenderHint(QPainter::Antialiasing, true);

    switch (m_eState)
    {
    case CHANNELSTATE_DETECTING:
        image = m_detectImage;
        break;
    case CHANNELSTATE_SORTING:
        image = m_sortImage;
        break;
    case CHANNELSTATE_ALIGNING:
        image = m_alignImage;
        break;
    case CHANNELSTATE_SELECTED:
    case CHANNELSTATE_APPLY_OPTIONS:
    case CHANNELSTATE_INACTIVE:
        break;
    }
    if (image != 0)
    {
        QRect rImage = image->rect();
        QRect rBounds(rect);
        int xAdjustment = 0;
        int yAdjustment = 0;

        if (rBounds.width() > rImage.width())
        {
            xAdjustment = (rBounds.width() - rImage.width()) / 2;
        }
        if (rBounds.width() > rImage.width())
        {
            yAdjustment = (rBounds.height() - rImage.height()) / 2;
        }
        rBounds.adjust(xAdjustment, yAdjustment, -xAdjustment, -yAdjustment);
        painter->drawImage(rBounds, *image);
        return true;
    }
    return false;
}
