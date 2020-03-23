#include "expandbutton.h"


// Author & Date:       Holly Ausbeck        10 Jan 2015
// Purpose: left or right arrow button for scrolling
// Inputs:
//          parent - parent widget
//          color - unit color

ExpandButton::ExpandButton(QWidget *parent) : QCheckBox(parent)
{
    setColor("#009FE3");
}

void ExpandButton::setColor(QColor color)
{
    m_color = color;
    m_colorDark = color.darker(600);
}

void ExpandButton::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    QRect r(this->geometry());
	int textX = r.height();
	r.moveTo(0, 0);
	r.setWidth(r.height());
    QLinearGradient gradient(0, 0, 0, r.height());
    gradient.setColorAt(0, m_color);
    gradient.setColorAt(1, m_colorDark);

    if (this->isDown())
    {
        gradient.setColorAt(0, m_colorDark);
        gradient.setColorAt(1, m_color);
    }
    else if (this->underMouse())
    {
        gradient.setColorAt(0, m_color.lighter());
        gradient.setColorAt(1, m_colorDark.lighter());
    }
    painter.setPen(QPen(Qt::black));
    QPainterPath path;
	int offset = r.height() / 4;

	if (this->isChecked())
	{
		r.adjust(0, offset, -offset, -offset);
		drawShow(r, path);
	}
	else
	{
		r.adjust(0, offset, -offset * 2, -offset);
		drawHide(r, path);
	}

	painter.fillPath(path, QBrush(Qt::black));
	QRect tr(this->geometry());
	tr.moveTo(0, 0);
	tr.moveLeft(textX);
	QString s = text();
	painter.drawText(tr, s);
}

ExpandButton::~ExpandButton()
{
}

void ExpandButton::drawShow(QRect r, QPainterPath &path)
{
	path.moveTo(r.topLeft());
	path.lineTo(r.center());
	path.lineTo(r.bottomLeft());
	path.closeSubpath();
}

void ExpandButton::drawHide(QRect r, QPainterPath &path)
{
	QPoint bottom = r.center();
	bottom.setY(r.bottom());
	path.moveTo(r.topRight());
	path.lineTo(r.bottomRight());
	path.lineTo(r.bottomLeft());
	path.closeSubpath();
}

