#include  <QSizePolicy>
#include  <QMouseEvent>
#include  <QGuiApplication>


#include "ModeSlider.h"

ModeSlider::ModeSlider(QWidget *parent)
    : MultipleChoiceSlider(parent)
{
    currentSelection_ = "";
    connect(this, SIGNAL(valueChanged (int)), this, SLOT(slotValueChanged(int)));
}

ModeSlider::~ModeSlider()
{
}

QString ModeSlider::currentSelection() const
{
    return currentSelection_;
}


void ModeSlider::setCurrentSelection(QString selection)
{
    int index = m_options.indexOf(selection);

    if (index != -1)
    {
        MultipleChoiceSlider::setCurrentIndex(index);

        if (currentSelection_ != selection)
        {
            currentSelection_ = selection;
            emit selectionChanged(currentSelection_);
            update();
        }
    }
}

void ModeSlider::setOptions(QStringList options, QStringList tooltips, QStringList bitmaps)
{
    m_options = options;
    m_bitmaps = bitmaps;
    m_tooltips = tooltips;

    int width = 0;
    int height = 0;
    this->parentWidget()->resize(this->minimumSize());

    if (!m_bitmaps.isEmpty())
    {
        foreach(QString string, m_bitmaps)
        {
            QImage image(string);
            m_images.push_back(image);

            height = image.height();
            int w = image.width();
            m_optionSize.push_back(w);

            if (width < w)
                width = w;
        }
    }
    else
    {
        QFont font = QGuiApplication::font();
        QFontMetrics fm(font);
        height = fm.height() * 1.7;

        foreach(QString string, m_options)
        {
            int w = fm.width(string);
            m_optionSize.push_back(w);

            if (width < w)
                width = w;
        }
        width *= 1.3;
    }
    // set the dimensions in the base class
    MultipleChoiceSlider::setOptions(m_options.size(), width, height);
}

void ModeSlider::paintEvent(QPaintEvent *event)
{
    // let the base class draw the button
    MultipleChoiceSlider::paintEvent(event);

    QPainter painter(this);
    painter.save();

    // if there are bitmaps, draw them
    if (!m_images.isEmpty())
    {
        QRect rect(this->geometry());
        rect.moveTo(0, 0);
        int x = 0;
        rect.setWidth(m_step);
        int i = 0;

        foreach(QImage image, m_images)
        {
            QRect target(rect);
            QRect source(image.rect());
            painter.drawImage(target, image, source);

            x += m_step;
            rect.moveLeft(x);
            i++;
        }
    }
    else
    {
        // else draw the option text
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(Qt::black));
        QRect rect(this->geometry());
        rect.moveTo(0, 0);
        int x = 0;
        rect.setWidth(m_step);
        int i = 0;

        foreach(QString string, m_options)
        {
            drawText(&painter, rect, string, m_optionSize.at(i), m_state.at(i));
            x += m_step;
            rect.moveLeft(x);
            i++;
        }
    }
    painter.restore();
    painter.end();
}

void ModeSlider::drawText(QPainter *painter, QRect r, QString s, int size, bool enabled)
{
    int offset = (m_step - size) / 2;
    int x = r.left() + offset;
    int y = r.height() - (r.height() / 3);

	if (enabled)
		painter->setPen(QPen(Qt::black));
	else
		painter->setPen(QPen(Qt::gray));
	
	painter->drawText(x, y, s);
}

void ModeSlider::slotValueChanged(int value)
{
    int index = value / m_step;

    if (index < m_options.size())
    {
        currentSelection_ = m_options.at(index);
        emit selectionChanged(currentSelection_);
    }
}
