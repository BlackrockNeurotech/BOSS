#include "arrowbutton.h"
#include "../model/configmodel.h"


// Author & Date:       Holly Ausbeck        10 Jan 2015
// Purpose: left or right arrow button for scrolling
// Inputs:
//          parent - parent widget
//          color - unit color

ArrowButton::ArrowButton(QWidget *parent, ArrowButton::direction eDirection, int max) : QToolButton(parent)
{
	Q_UNUSED(max);

    m_eDirection = eDirection;

    m_color = ConfigModel::ColorGradientSelectedLight;
    m_colorDark = ConfigModel::ColorGradientSelectedDark;
    m_colorHover = ConfigModel::ColorGradientSelectedLightHover;
    m_colorHoverDark = ConfigModel::ColorGradientSelectedDarkHover;
}

void ArrowButton::setColor(QColor color)
{
    m_color = ConfigModel::ColorGradientSelectedLight;
    m_colorDark = ConfigModel::ColorGradientSelectedDark;
    m_colorHover = ConfigModel::ColorGradientSelectedLightHover;
    m_colorHoverDark = ConfigModel::ColorGradientSelectedDarkHover;
}

void ArrowButton::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QPainter painter(this);
    QRect rBorder(this->geometry());
    rBorder.moveTo(0, 0);
    painter.fillRect(rBorder, QBrush(Qt::lightGray));

    painter.setRenderHint(QPainter::Antialiasing);
    QRect r(this->geometry());
    r.moveTo(1, 1);
    r.setWidth(r.width() - 2);
    r.setHeight(r.height() - 2);
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
        gradient.setColorAt(0, m_colorHover);
        gradient.setColorAt(1, m_colorHoverDark);
    }
    painter.fillRect(r, gradient);
    painter.setPen(QPen(Qt::black));
    QPainterPath path;

    switch (m_eDirection)
    {
    case left:
        r.adjust(1, 1, -1, -1);
        drawLeft(r, path);
        break;
    case right:
        r.adjust(1, 1, -1, -1);
        drawRight(r, path);
        break;
    case hzoomin:
        drawHZoomIn(r, path);
        break;
    case hzoomout:
        drawHZoomOut(r, path);
        break;
    case vzoomin:
        r.adjust(-1, -1, 1, 1);
        drawVZoomIn(r, path);
        break;
    case vzoomout:
        drawVZoomOut(r, path);
        break;
    case fittoview:
        drawFitToView(r, path);
        break;
    }
    painter.fillPath(path, QBrush(Qt::black));
}

ArrowButton::~ArrowButton()
{
}

void ArrowButton::drawLeft(QRect r, QPainterPath &path)
{
    path.setFillRule(Qt::WindingFill);
    int offset = r.height() / 3;

    QRect arrowRect(r.x() + offset, r.y() + offset, r.height() - offset, r.width() - offset);
    drawLeftArrow(arrowRect, path);
}

void ArrowButton::drawRight(QRect r, QPainterPath &path)
{
    path.setFillRule(Qt::WindingFill);
    int offset = r.height() / 3;

    QRect arrowRect(r.x(), r.y() + offset, r.height() - offset, r.width() - offset);
    drawRightArrow(arrowRect, path);
}

void ArrowButton::drawHZoomIn(QRect r, QPainterPath &path)
{
    path.setFillRule(Qt::WindingFill);
    int offset = r.height() / 6;

    QRect arrowRect(r.x(), r.y() + offset, r.height(), r.width() - (offset * 2));
    drawHorizontalArrow(arrowRect, path);
}

void ArrowButton::drawHZoomOut(QRect r, QPainterPath &path)
{
    path.setFillRule(Qt::WindingFill);
    int yOffset = r.height() / 4;
    int xOffset = r.height() / 6;

    QRect arrowRect(r.x() + xOffset, r.y() + yOffset, r.height() - (xOffset * 2), r.width() - (yOffset * 2));
    drawHorizontalArrow(arrowRect, path);
}

void ArrowButton::drawVZoomIn(QRect r, QPainterPath &path)
{
    path.setFillRule(Qt::WindingFill);
    int offset = r.width() / 6;

    QRect arrowRect(r.x() + offset, r.y(), r.width() - (offset * 2), r.height());
    drawVerticalArrow(arrowRect, path);
}

void ArrowButton::drawVZoomOut(QRect r, QPainterPath &path)
{
    path.setFillRule(Qt::WindingFill);
    int xOffset = r.width() / 4;
    int yOffset = r.width() / 6;

    QRect arrowRect(r.x() + xOffset, r.y() + yOffset, r.width() - (xOffset * 2), r.height() - (yOffset * 2));
    drawVerticalArrow(arrowRect, path);
}

void ArrowButton::drawFitToView(QRect r, QPainterPath &path)
{
    path.setFillRule(Qt::WindingFill);
    int offset = r.width() / 6;

    QRect arrowRect(r.x() + offset, r.y() + offset, r.width() - (offset * 2), r.height() - (offset * 2));
    drawVerticalArrow(arrowRect, path);
}

void ArrowButton::drawVerticalArrow(QRect r, QPainterPath &path)
{
    int arrowSize = r.width() / 2;

    // draw top arrow
    QPoint top = r.center();
    top.setY(r.top());
    QPoint topLeft(r.left(), top.y() + arrowSize);
    QPoint topRight(r.right(), top.y() + arrowSize);
    path.moveTo(top);
    path.lineTo(topLeft);
    path.lineTo(topRight);
    path.closeSubpath();

    // draw bottom arrow
    QPoint bottom = r.center();
    bottom.setY(r.bottom());
    QPoint bottomLeft(r.left(), bottom.y() - arrowSize);
    QPoint bottomRight(r.right(), bottom.y() - arrowSize);
    path.moveTo(bottom);
    path.lineTo(bottomLeft);
    path.lineTo(bottomRight);
    path.closeSubpath();

    // draw center line
    top.setX(top.x() - 1);
    top.setY(top.y() + arrowSize);
    bottom.setX(bottom.x() - 1);
    bottom.setY(bottom.y() - arrowSize);
    path.moveTo(top);
    path.lineTo(bottom);
    top.setX(top.x() + 2);
    bottom.setX(bottom.x() + 2);
    path.lineTo(bottom);
    path.lineTo(top);
    path.closeSubpath();
}

void ArrowButton::drawHorizontalArrow(QRect r, QPainterPath &path)
{
    int arrowSize = r.height() / 2;

    // draw left arrow
    QPoint left = r.center();
    left.setX(r.left());
    QPoint leftTop(r.left() + arrowSize, r.top());
    QPoint leftBottom(r.left() + arrowSize, r.bottom());
    path.moveTo(left);
    path.lineTo(leftTop);
    path.lineTo(leftBottom);
    path.closeSubpath();

    // draw right arrow
    QPoint right = r.center();
    right.setX(r.right());
    QPoint rightTop(r.right() - arrowSize, r.top());
    QPoint rightBottom(r.right() - arrowSize, r.bottom());
    path.moveTo(right);
    path.lineTo(rightTop);
    path.lineTo(rightBottom);
    path.closeSubpath();

    // draw center line
    left.setX(left.x() + arrowSize);
    left.setY(left.y() - 1);
    right.setX(right.x() - arrowSize);
    right.setY(right.y() - 1);
    path.moveTo(left);
    path.lineTo(right);
    left.setY(left.y() + 2);
    right.setY(right.y() + 2);
    path.lineTo(right);
    path.lineTo(left);
    path.closeSubpath();
}

void ArrowButton::drawLeftArrow(QRect r, QPainterPath &path)
{
    int arrowSize = r.height() / 2;

    // draw left arrow
    QPoint left = r.center();
    left.setX(r.left());
    QPoint leftTop(r.left() + arrowSize, r.top());
    QPoint leftBottom(r.left() + arrowSize, r.bottom());
    path.moveTo(left);
    path.lineTo(leftTop);
    path.lineTo(leftBottom);
    path.closeSubpath();
}

void ArrowButton::drawRightArrow(QRect r, QPainterPath &path)
{
    int arrowSize = r.height() / 2;

    // draw right arrow
    QPoint right = r.center();
    right.setX(r.right());
    QPoint rightTop(r.right() - arrowSize, r.top());
    QPoint rightBottom(r.right() - arrowSize, r.bottom());
    path.moveTo(right);
    path.lineTo(rightTop);
    path.lineTo(rightBottom);
    path.closeSubpath();
}

void ArrowButton::drawDiamond(QRect r, QPainterPath &path)
{
    QPoint left = r.center();
    left.setX(r.left());
    QPoint top = r.center();
    top.setY(r.top());
    QPoint right = r.center();
    right.setX(r.right());
    QPoint bottom = r.center();
    bottom.setY(r.bottom());
    path.moveTo(left);
    path.lineTo(top);
    path.lineTo(right);
    path.lineTo(bottom);
    path.closeSubpath();
}

