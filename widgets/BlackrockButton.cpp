#include <QPainter>

#include "../widgets/BlackrockButton.h"
#include "../model/configmodel.h"


BlackrockButton::BlackrockButton(QWidget *parent, QString text, QColor color, QColor checked, QColor disabled) :
QPushButton(text, parent)
{
    m_image = 0;
    m_bRadio = false;
    m_bInvert = false;
    m_width = 0;
    m_height = 22;

    m_color = ConfigModel::ColorGradientLight;
    m_colorDark = ConfigModel::ColorGradientDark;
    m_colorHover = ConfigModel::ColorGradientLightHover;
    m_colorHoverDark = ConfigModel::ColorGradientDarkHover;

    m_checkedColor = ConfigModel::ColorGradientSelectedLight;
    m_checkedColorDark = ConfigModel::ColorGradientSelectedDark;
    m_checkedColorHover = ConfigModel::ColorGradientSelectedLightHover;
    m_checkedColorHoverDark = ConfigModel::ColorGradientSelectedDarkHover;

    m_disabledColor = ConfigModel::ColorDisabled;
}

void BlackrockButton::setDimensionsToIcon()
{
    if (m_image != 0)
    {
        QRect rImage = m_image->rect();

        m_width = rImage.width() + 4;
        m_height = rImage.height();
    }
}

QSize BlackrockButton::sizeHint() const
{
    if (m_width > 0)
        return QSize(m_width, m_height);
    else
        return QSize(this->width(), m_height);

    return QPushButton::sizeHint();
}


void BlackrockButton::setAlwaysSelected()
{
    m_color = ConfigModel::ColorGradientSelectedLight;
    m_colorDark = ConfigModel::ColorGradientSelectedDark;
    m_colorHover = ConfigModel::ColorGradientSelectedLightHover;
    m_colorHoverDark = ConfigModel::ColorGradientSelectedDarkHover;
}

void BlackrockButton::setImage(QString path)
{
    m_image = new QImage(path);
}

void BlackrockButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect r(this->geometry());
    r.moveTo(0, 0);
    QLinearGradient gradient(0, 0, 0, r.height());

    bool mouseOver = false;
    QPoint globalPos = QCursor::pos();
    QPoint pos = this->mapFromGlobal(globalPos);

    if (r.contains(pos))
        mouseOver = true;

    if (m_bRadio)
    {
        gradient = radioGradient(mouseOver, r.height());
    }
    else if (m_bInvert)
    {
        gradient = invertGradient(mouseOver, r.height());
    }
    else if (this->isCheckable())
    {
        gradient = checkableGradient(mouseOver, r.height());
    }
    else
    {
        gradient = regularGradient(mouseOver, r.height());
    }
    QBrush brush(gradient);
    painter.setBrush(brush);
    painter.drawRoundedRect(r, 4, 4);

    if (!this->text().isEmpty())
    {
        // draw the button text
        painter.setPen(QPen(Qt::black));
        painter.drawText(r, Qt::AlignHCenter | Qt::AlignVCenter, this->text());
    }
    if (m_image != 0)
    { 
        QRect rButton(this->geometry());
        QRect rImage = m_image->rect();
        rButton.moveTo(0, 0);

        // if there is no text, center the image
        if (this->text().isEmpty())
        {
            rButton.adjust((rButton.width() - rImage.width()) / 2, 0, -(rButton.width() - rImage.width()) / 2, 0);
        }
        // else display the image on the right
        else
        {
            rButton.adjust(rButton.width() - rImage.width(), 0, 0, 0);
        }
        painter.drawImage(rButton, *m_image);
    }
}

QLinearGradient BlackrockButton::radioGradient(bool mouseOver, int height)
{
    QLinearGradient gradient(0, 0, 0, height);

    // initialize to not checked, not hover and not pushed
    gradient.setColorAt(0, m_color);
    gradient.setColorAt(1, m_colorDark);

    // if it is disabled, make it flat
    if (!this->isEnabled())
    {
        gradient.setColorAt(0, m_disabledColor);
        gradient.setColorAt(1, m_disabledColor);
    }
    // If it's checked, it can't be pushed, so just used the checked gradient regardless of other states
    else if (this->isChecked())
    {
        if (this->isDown())  // it's just going into the checked state, reverse the gradient
        {
            gradient.setColorAt(0, m_checkedColorDark);
            gradient.setColorAt(1, m_checkedColor);
        }
        else if (mouseOver) // use the hover color
        {
            gradient.setColorAt(0, m_checkedColorHover);
            gradient.setColorAt(1, m_checkedColorHoverDark);
        }
        else
        {
            gradient.setColorAt(0, m_checkedColor);
            gradient.setColorAt(1, m_checkedColorDark);
        }
    }
    // it's not disabled or checked
    else
    {
        if (this->isDown())  // it's just going into the checked state, reverse the gradient
        {
            gradient.setColorAt(0, m_colorDark);
            gradient.setColorAt(1, m_color);
        }
        else if (mouseOver) // use the hover color
        {
            gradient.setColorAt(0, m_colorHover);
            gradient.setColorAt(1, m_colorHoverDark);
        }
        else
        {
            gradient.setColorAt(0, m_color);
            gradient.setColorAt(1, m_colorDark);
        }
    }
    return gradient;
}

QLinearGradient BlackrockButton::checkableGradient(bool mouseOver, int height)
{
    QLinearGradient gradient(0, 0, 0, height);

    // initialize to not checked, not hover and not pushed
    gradient.setColorAt(0, m_color);
    gradient.setColorAt(1, m_colorDark);

    // if it is disabled, make it flat
    if (!this->isEnabled())
    {
        gradient.setColorAt(0, m_disabledColor);
        gradient.setColorAt(1, m_disabledColor);
    }
    else if (this->isChecked())
    {
        if (this->isDown())  // reverse the gradient and show it unchecked
        {
            gradient.setColorAt(0, m_checkedColorDark);
            gradient.setColorAt(1, m_checkedColor);
        }
        else if (mouseOver) // use the hover color
        {
            gradient.setColorAt(0, m_checkedColorHover);
            gradient.setColorAt(1, m_checkedColorHoverDark);
        }
        else
        {
            gradient.setColorAt(0, m_checkedColor);
            gradient.setColorAt(1, m_checkedColorDark);
        }
    }
    // it's not disabled or checked
    else
    {
        if (this->isDown())  // reverse the gradient and show it checked
        {
            gradient.setColorAt(0, m_colorDark);
            gradient.setColorAt(1, m_color);
        }
        else if (mouseOver) // use the hover color
        {
            gradient.setColorAt(0, m_colorHover);
            gradient.setColorAt(1, m_colorHoverDark);
        }
        else
        {
            gradient.setColorAt(0, m_color);
            gradient.setColorAt(1, m_colorDark);
        }
    }
    return gradient;
}
QLinearGradient BlackrockButton::invertGradient(bool mouseOver, int height)
{
    QLinearGradient gradient(0, 0, 0, height);

    // initialize to not checked, not hover and not pushed
    gradient.setColorAt(0, m_color);
    gradient.setColorAt(1, m_colorDark);

    // if it is disabled, make it flat
    if (!this->isEnabled())
    {
        gradient.setColorAt(0, m_disabledColor);
        gradient.setColorAt(1, m_disabledColor);
    }
    else
    {
        if (mouseOver) // use the hover color
        {
            if (this->isChecked())
            {
                gradient.setColorAt(0, m_colorHover);
                gradient.setColorAt(1, m_colorHoverDark);
            }
            else
            {
                gradient.setColorAt(0, m_colorHoverDark);
                gradient.setColorAt(1, m_colorHover);
            }
        }
        else if (this->isDown() || !this->isChecked())  // reverse the gradient and show it checked
        {
            gradient.setColorAt(0, m_colorDark);
            gradient.setColorAt(1, m_color);
        }
        else
        {
            gradient.setColorAt(0, m_color);
            gradient.setColorAt(1, m_colorDark);
        }
    }
    return gradient;
}

QLinearGradient BlackrockButton::regularGradient(bool mouseOver, int height)
{
    QLinearGradient gradient(0, 0, 0, height);

    // initialize to not hover and not pushed
    gradient.setColorAt(0, m_color);
    gradient.setColorAt(1, m_colorDark);

    // if it is disabled, make it flat
    if (!this->isEnabled())
    {
        gradient.setColorAt(0, m_disabledColor);
        gradient.setColorAt(1, m_disabledColor);
    }
    else
    {
        if (this->isDown())  // reverse the gradient
        {
            gradient.setColorAt(0, m_colorDark);
            gradient.setColorAt(1, m_color);
        }
        else if (mouseOver) // use the hover color
        {
            gradient.setColorAt(0, m_colorHover);
            gradient.setColorAt(1, m_colorHoverDark);
        }
        else
        {
            gradient.setColorAt(0, m_color);
            gradient.setColorAt(1, m_colorDark);
        }
    }
    return gradient;
}

BlackrockButton::~BlackrockButton()
{
}
