#include  <QSizePolicy>
#include  <QMouseEvent>
#include  <QGuiApplication>


#include "multipleChoiceSlider.h"

MultipleChoiceSlider::MultipleChoiceSlider(QWidget *parent)
    : QSlider(parent)
{
    this->setOrientation(Qt::Horizontal);
    m_bubble = new Bubble();
    m_currentIndex = 0;

    setMouseTracking(true);
}

MultipleChoiceSlider::~MultipleChoiceSlider()
{
    delete m_bubble;
}

int MultipleChoiceSlider::index() const
{
    return m_currentIndex;
}


void MultipleChoiceSlider::setCurrentIndex(int index)
{
    if (!isSliderDown())
    {
        this->setValue(index * m_step);

        if (m_currentIndex != index)
        {
            m_currentIndex = index;
            update();
        }
    }
}

void MultipleChoiceSlider::setOptions(int nOptions, int step, int height)
{
    m_step = step;
    m_width = m_step * nOptions;
    m_height = height;
    m_nOptions = nOptions;
	m_state.clear();

	for (int i = 0; i < nOptions; i++)
		m_state << true;

    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->setMinimumSize(QSize(m_width, m_height));
    this->setMaximumSize(QSize(m_width, m_height));
    this->setRange(0, m_width);
    this->setSingleStep(m_step);
    this->setValue(0);
}

void MultipleChoiceSlider::enable(int option, bool enable)
{
	if (option < m_state.size())
	{
		m_state[option] = enable;
		update();
	}
}

void MultipleChoiceSlider::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

    QPainter painter(this);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect(this->geometry());
    rect.moveTo(0, 0);

    QRect r(rect);

    // draw the buttons
    QLinearGradient gradient(0, 0, 0, r.height());
    gradient.setColorAt(0, ConfigModel::ColorGradientLight);
    gradient.setColorAt(1, ConfigModel::ColorGradientDark);
    QPen pen(QBrush(gradient), 1);
    painter.setPen(pen);
    QRect buttonRect(rect);
    buttonRect.setWidth(m_step);

    for (int i = 0; i < m_nOptions; i++)
    {
        drawButton(&painter, buttonRect, m_state[i]);
        buttonRect.moveLeft(buttonRect.left() + m_step);
    }

    // draw the slider
    int v = this->value();
    rect.setWidth(m_step);
    rect.moveLeft(v);
    m_bubble->drawBubble(&painter, rect, this->isSliderDown(), this->underMouse());

    QPen penBorder(Qt::white);
    penBorder.setWidth(3);
    painter.setPen(penBorder);
    
    QRect border(this->geometry());
    border.moveTo(0, 0);
    border.adjust(-1, -1, 1, 1);
    painter.drawRoundedRect(border, 3, 3);

    QRect borderb(this->geometry());
    borderb.moveTo(0, 0);
    painter.setPen(QPen(Qt::black));
    painter.drawRoundedRect(borderb, 3, 3);

    painter.restore();
    painter.end();
}

void MultipleChoiceSlider::drawButton(QPainter *painter, QRect r, bool bEnabled)
{
    if (bEnabled)
    {
        QLinearGradient downGradient(0, 0, 0, r.height());
        downGradient.setColorAt(0, ConfigModel::ColorGradientLight);
        downGradient.setColorAt(1, ConfigModel::ColorGradientDark);
        painter->fillRect(r, downGradient);
    }
    else
    {
        // if it's disabled, paint it flat gray
        QLinearGradient selectedGradient(0, 0, 0, r.height());
        selectedGradient.setColorAt(0, ConfigModel::ColorDisabled);
        selectedGradient.setColorAt(1, ConfigModel::ColorDisabled);
        painter->fillRect(r, selectedGradient);
    }
    // draw a thin border
    QLinearGradient gradient(0, 0, 0, r.height());
    gradient.setColorAt(0, ConfigModel::ColorGradientBorderLight);
    gradient.setColorAt(1, ConfigModel::ColorGradientBorderDark);
    QPen pen(QBrush(gradient), 1);
    painter->setPen(pen);
    painter->drawRect(r);
}

void MultipleChoiceSlider::mousePressEvent(QMouseEvent *event)
{
    QRectF r = m_bubble->rect();

    if (r.contains(event->pos()))
    {
        m_diff = event->pos().x() - this->value();
        setSliderDown(true);
        update();
    }
    else
        m_diff = -1;

}

void MultipleChoiceSlider::mouseMoveEvent(QMouseEvent *event)
{
    int last = this->maximum() - m_step;

    if (!m_state[m_nOptions - 1])
        last -= m_step;

    if (event->buttons() & Qt::LeftButton)
    {
        if (m_diff != -1)
        {
            QPoint p = event->pos();
            int x = p.x() - m_diff;

            if (x < 0)
                x = 0;
            if (x > last)
                x = last;

			for (int i = 0; i < m_nOptions; i++)
			{
				int left = i * m_step;
				int right = (i + 1) * m_step;

				if ((x >= left) && (x < right) && (m_state[i] == true))
				{
					this->setValue(x);
					break;
				}
			}
        }
    }
    else if (m_tooltips.size() == m_nOptions)
    {
        QPoint p = event->pos();
        int x = p.x();

        for (int i = 0; i < m_nOptions; i++)
        {
            int left = i * m_step;
            int right = (i + 1) * m_step;

            if ((x >= left) && (x < right))
            {
                this->setToolTip(m_tooltips[i]);
                break;
            }
        }
    }
}

void MultipleChoiceSlider::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint p = event->pos();
    int index = 0;

    if (m_diff > 0)
    {
        int x = p.x() - m_diff;

        if (x < 0)
            x = 0;
        if (x > (this->maximum() - m_step))
            x = this->maximum() - m_step;

        for (int i = 0; i < m_nOptions; i++)
        {
            int left = i * m_step;
            int right = (i + 1) * m_step;

			if ((x >= left) && (x < right))
            {
                int leftDelta = x - left;
                int rightDelta = right - x;
                index = i;

                if (leftDelta > rightDelta)
                    index++;

				if (m_state[index] == true)
					this->setValue(index * m_step);
				break;
            }
        }
	}
    else
    {
        int x = p.x();

        for (int i = 0; i < m_nOptions; i++)
        {
			if ((x < ((i + 1) * m_step)) && (m_state[i] == true))
            {
                index = i;
				this->setValue(index * m_step);
				break;
            }
        }
    }
    setSliderDown(false);
    update();
}
