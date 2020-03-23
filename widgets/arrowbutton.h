#ifndef ARROWBUTTON_H
#define ARROWBUTTON_H

#include <QPainter>
#include <QToolButton>

class ArrowButton : public QToolButton
{
    Q_OBJECT

public:
    enum direction {left, right, vzoomin, vzoomout, hzoomin, hzoomout, fittoview};

    ArrowButton(QWidget *parent, ArrowButton::direction eDirection, int max);
    ~ArrowButton();

    void setColor(QColor color);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    direction m_eDirection;
    QColor m_color;
    QColor m_colorDark;
    QColor m_colorHover;
    QColor m_colorHoverDark;

    void drawLeft(QRect r, QPainterPath &path);
    void drawRight(QRect r, QPainterPath &path);
    void drawHZoomIn(QRect r, QPainterPath &path);
    void drawHZoomOut(QRect r, QPainterPath &path);
    void drawVZoomIn(QRect r, QPainterPath &path);
    void drawVZoomOut(QRect r, QPainterPath &path);
    void drawFitToView(QRect r, QPainterPath &path);

    void drawVerticalArrow(QRect r, QPainterPath &path);
    void drawHorizontalArrow(QRect r, QPainterPath &path);
    void drawLeftArrow(QRect r, QPainterPath &path);
    void drawRightArrow(QRect r, QPainterPath &path);
    void drawDiamond(QRect r, QPainterPath &path);
};

#endif // ARROWBUTTON_H
