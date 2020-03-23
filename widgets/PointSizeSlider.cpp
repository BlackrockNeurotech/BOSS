#include  <QSizePolicy>
#include  <QMouseEvent>
#include  <QGuiApplication>


#include "PointSizeSlider.h"

PointSizeSlider::PointSizeSlider(QWidget *parent)
    : MultipleChoiceSlider(parent)
{
    currentSelection_ = 0;
    connect(this, SIGNAL(valueChanged (int)), this, SLOT(slotValueChanged(int)));
}

PointSizeSlider::~PointSizeSlider()
{
}

int PointSizeSlider::currentSelection() const
{
    return currentSelection_;
}


void PointSizeSlider::setCurrentSelection(int selection)
{
    if (selection < m_images.size())
    {
        MultipleChoiceSlider::setCurrentIndex(selection);

        if (currentSelection_ != selection)
        {
            currentSelection_ = selection;
            emit selectionChanged(currentSelection_);
            update();
        }
    }
}

void PointSizeSlider::setOptions()
{
    QStringList images = QStringList() << ":/new/Bitmaps/Bitmaps/Point0.png" << ":/new/Bitmaps/Bitmaps/Point1.png" << ":/new/Bitmaps/Bitmaps/Point2.png" << ":/new/Bitmaps/Bitmaps/Point3.png";
    int h = 0;
    int w = 0;

    foreach(QString string, images)
    {
        QImage image(string);
        m_images.push_back(image);

        h = image.height();
        w = image.width();
    }
    MultipleChoiceSlider::setOptions(m_images.size(), w, h);
}

void PointSizeSlider::paintEvent(QPaintEvent *event)
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
    painter.restore();
    painter.end();
}

void PointSizeSlider::slotValueChanged(int value)
{
    int index = value / m_step;

    if (index < m_images.size())
    {
        currentSelection_ = index;
        emit selectionChanged(currentSelection_);
    }
}
