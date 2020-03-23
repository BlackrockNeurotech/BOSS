#ifndef SCROLLRANGETOOLBAR_H
#define SCROLLRANGETOOLBAR_H

#include <QToolBar>
#include <QLabel>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"
#include "../graphics/scrollrangegraphicsview.h"
#include "../widgets/arrowbutton.h"

class ScrollRangeToolBar : public QToolBar
{
    Q_OBJECT

public:
    ScrollRangeToolBar(QWidget *parent = 0);
    ~ScrollRangeToolBar();

    public slots:
    // arrow button signals
    void slotIncrement(bool value);
    void slotDecrement(bool value);

    // channel model signals
    void slotDataAvailable(quint32, RawDataAccum);
    void slotFeaturesChanged(int nchannel);
    void slotSliderChanged(int channel, SliderType type);

private:
    QWidget     *m_parent;

    QHBoxLayout             *m_layout;
    QLabel                  *m_labelMin;
    QLabel                  *m_labelMax;
    ArrowButton             *m_arrowLeft;
    ArrowButton             *m_arrowRight;
    ScrollRangeGraphicsView *m_scrollbar;

    int  m_nGeom;
    int  m_nHeight;
    quint64  m_nStartTime;
    quint64  m_nEndTime;
    quint64  m_nStartMs;
    quint64  m_nEndMs;

    void setRange(int nchannel);
    void showhide(int nchannel);
};


#endif // SCROLLRANGETOOLBAR_H
