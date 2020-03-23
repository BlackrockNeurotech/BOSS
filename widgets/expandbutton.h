#ifndef EXPANDBUTTON_H
#define EXPANDBUTTON_H

#include <QPainter>
#include <QCheckBox>

class ExpandButton : public QCheckBox
{
    Q_OBJECT

public:
    ExpandButton(QWidget *parent);
    ~ExpandButton();

    void setColor(QColor color);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QColor    m_color;
    QColor    m_colorDark;

    void drawShow(QRect r, QPainterPath &path);
    void drawHide(QRect r, QPainterPath &path);
};

#endif // EXPANDBUTTON_H
