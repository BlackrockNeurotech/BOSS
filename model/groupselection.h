
#ifndef GROUPSELECTION_H
#define GROUPSELECTION_H

#include <QMetaType>
#include <QPointF>
#include <QVector>
#include <QImage>

#include "../model/channelItem.h"


class GroupSelection
{
public:
    enum EditMode { Editable, ReadOnly };

    explicit GroupSelection();

    void setGroupSize(int groupSize) { m_groupSize = groupSize; }
    void setState(ChannelState state) { m_eState = state; }
    ChannelState getState() { return m_eState; }
    void setDisplayed(bool bDisplayed) { m_bDisplayed = bDisplayed; }
    bool getDisplayed() { return m_bDisplayed; }
    void setApplyOptions(int optionSelection);
    int  getOptionSelection() { return m_optionSelection; }
    void setText(QString sText) { m_sText = sText; }

    void paint(QPainter *painter, const QRect &rect, const QPalette &palette, EditMode mode, bool bMouseOver = false, bool bEnabled = true) const;
    bool drawImage(QPainter *painter, const QRect &rect) const;
    bool setOptionSelection(int y, QRect rect);

    GroupSelection& operator =(const GroupSelection& other)
    {
        m_eState = other.m_eState;
        m_bDisplayed = other.m_bDisplayed;
        m_groupSize = other.m_groupSize;
        m_optionSelection = other.m_optionSelection;
        m_sText = other.m_sText;
        return *this;
    }

private:
    static bool m_bImagesSet;
    // images used for selection
    static QImage *m_displayImage;       // the channel is displayed
    static QImage *m_displaySortImage;   // the channel is displayed and sorting
    static QImage *m_displayDetectImage; // the channel is displayed and detecting
    static QImage *m_displayAlignImage;  // the channel is displayed and aligning
    static QImage *m_selectImage;        // the channel is selected - not used
    static QImage *m_detectImage;        // the channel is detecting
    static QImage *m_sortImage;          // the channel is sorting
    static QImage *m_alignImage;         // the channel is aligning
    static QImage *m_blankImage;         // the channel is inactive

    ChannelState m_eState;
    bool         m_bDisplayed;
    int          m_groupSize;
    int          m_optionSelection;
    QString      m_sText;

    void setImages();
};

Q_DECLARE_METATYPE(GroupSelection)

#endif  //GROUPSELECTION_H
