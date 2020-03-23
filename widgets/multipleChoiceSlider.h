#ifndef MULTIPLECHOICESLIDER_H
#define MULTIPLECHOICESLIDER_H

#include <QSlider>
#include <QLabel>
#include <QPainter>
#include <QList>

#include "../model/configmodel.h"


class Bubble
{
public: 
    Bubble() {}

    void drawBubble(QPainter *painter, QRect r, bool bDown, bool bHover)
    {
        painter->save();
        m_rect = r;
        QLinearGradient gradient(0, 0, 0, r.height());
        gradient.setColorAt(0, ConfigModel::ColorGradientBorderLight);
        gradient.setColorAt(1, ConfigModel::ColorGradientBorderDark);

        if (bDown)
        {
            //QLinearGradient downGradient(0, 0, 0, r.height());
            //downGradient.setColorAt(0, QColor(Qt::green).darker());
            //downGradient.setColorAt(1, QColor(Qt::green));
            //painter->fillRect(r, downGradient);
        }
        else if (bHover)
        {
            QColor c = QColor("#009FE3");
            c = c.darker();
            c = c.darker();
            c.setAlpha(100);
            QLinearGradient hoverGradient(0, 0, 0, r.height());
            hoverGradient.setColorAt(0, ConfigModel::ColorGradientSelectedLightHover);
            hoverGradient.setColorAt(1, ConfigModel::ColorGradientSelectedDarkHover);
            painter->fillRect(r, hoverGradient);
        }
        else
        {
            QColor c = QColor("#009FE3");
            c = c.darker();
            c = c.darker();
            QLinearGradient selectedGradient(0, 0, 0, r.height());
            selectedGradient.setColorAt(0, ConfigModel::ColorGradientSelectedLight);
            selectedGradient.setColorAt(1, ConfigModel::ColorGradientSelectedDark);
            painter->fillRect(r, selectedGradient);
        }
        r.adjust(1, 1, -1, -1);
        QPen pen(QBrush(gradient), 2);
        painter->setPen(pen);
        painter->drawRect(r);

        painter->restore();
    }
    QRectF rect() { return m_rect; }

private:
    QRect  m_rect;
};



class MultipleChoiceSlider : public QSlider
{
    Q_OBJECT

public:
    MultipleChoiceSlider(QWidget *parent = 0);
    ~MultipleChoiceSlider();

	void setOptions(int nOptions, int step, int height);
	void enable(int option, bool enable);

    int index() const;
    void setCurrentIndex(int index);

protected:
    virtual void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    int  m_currentIndex;
    int  m_step;
	QList<bool>	m_state;
    QStringList m_tooltips;

private:
    int     m_diff;
    int     m_width;
    int     m_height;
    int     m_nOptions;
    Bubble *m_bubble;

    void drawButton(QPainter *painter, QRect r, bool bEnabled);
};


#endif // MULTIPLECHOICESLIDER_H
