#include "animatedbutton.h"
#include "../model/configmodel.h"

AnimatedButton::AnimatedButton(QWidget *parent, QString text) :
    QPushButton(text, parent)
{
    setColor(QColor("#009FE3"));
    m_timer.setInterval(250);
    m_timer.setSingleShot(false);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(toggleColor()));

    m_blinkColor = ConfigModel::ColorGradientSelectedLight;
    m_blinkColorDark = ConfigModel::ColorGradientSelectedDark;
    m_blinkColorHover = ConfigModel::ColorGradientSelectedLightHover;
    m_blinkColorHoverDark = ConfigModel::ColorGradientSelectedDarkHover;
}

void AnimatedButton::setColor(QColor color)
{
    m_color = ConfigModel::ColorGradientLight;
    m_colorDark = ConfigModel::ColorGradientDark;
    m_colorHover = ConfigModel::ColorGradientLightHover;
    m_colorHoverDark = ConfigModel::ColorGradientDarkHover;

    m_tempColor = m_color;
    m_tempColorDark = m_colorDark;
    m_tempColorHover = m_colorHover;
    m_tempColorHoverDark = m_colorHoverDark;

    if (color == Qt::gray)
        m_bStatus = false;
    else
        m_bStatus = true;
}


void AnimatedButton::paintEvent (QPaintEvent *event)
{
	Q_UNUSED(event);

    QPainter painter(this);
    QRect r(this->geometry());
    r.moveTo(0, 0);
    QLinearGradient gradient(0, 0, 0, r.height());
    gradient.setColorAt(0, m_tempColor);
    gradient.setColorAt(1, m_tempColorDark);


    if (this->isDown())
    {
        gradient.setColorAt(0, m_tempColorDark);
        gradient.setColorAt(1, m_tempColor);
    }
    else if (this->underMouse())
    {
        gradient.setColorAt(0, m_tempColorHover);
        gradient.setColorAt(1, m_tempColorHoverDark);
    }
    QBrush brush(gradient);
    painter.setBrush(brush);
    painter.drawRoundedRect(r, 4, 4);
    // draw the button text

    painter.setPen(QPen(Qt::black));
    painter.drawText(r, Qt::AlignHCenter | Qt::AlignVCenter, this->text());
}

AnimatedButton::~AnimatedButton()
{

}


void AnimatedButton::toggleColor()
{

    if(m_bStatus)
    {
        m_tempColor = m_blinkColor;
        m_tempColorDark = m_blinkColorDark;
        m_tempColorHover = m_blinkColorHover;
        m_tempColorHoverDark = m_blinkColorHoverDark;
    }
    else
    {
        m_tempColor = m_color;
        m_tempColorDark = m_colorDark;
        m_tempColorHover = m_colorHover;
        m_tempColorHoverDark = m_colorHoverDark;
    }

    update();
    m_bStatus = !m_bStatus;
}

void AnimatedButton::reset()
{
    stopAnimation();
    m_tempColor = m_color;
    m_tempColorDark = m_colorDark;
    m_tempColorHover = m_colorHover;
    m_tempColorHoverDark = m_colorHoverDark;
}
