#include "verticallabel.h"
#include <QPainter>

VerticalLabel::VerticalLabel(QWidget *parent)
    : QPushButton(parent)
{

}

VerticalLabel::VerticalLabel(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
}


VerticalLabel::~VerticalLabel()
{

}

void VerticalLabel::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::Dense1Pattern);
    
    painter.translate(sizeHint().width(), sizeHint().height());
    painter.rotate(270);

    painter.drawText(0, 0, text());
}

QSize VerticalLabel::minimumSizeHint() const
{
    QSize s = QPushButton::minimumSizeHint();
    return QSize(s.height(), s.width());
}

QSize VerticalLabel::sizeHint() const
{
    QSize s = QPushButton::sizeHint();
    return QSize(s.height() * 2, s.width());
}