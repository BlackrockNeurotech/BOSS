#include <QPainter>
#include <QStaticText>
#include <QImage>

#include "../widgets/GroupButton.h"
#include "../model/configmodel.h"


GroupButton::GroupButton(QWidget *parent, int nGroup, QList<int> nChannels, QString text) :
QPushButton(text, parent)
{
    m_nGroup = nGroup;
    m_nChannels = nChannels;
    m_text = text;
    m_bSelected = false;
    m_bDisplayed = false;

    setAutoFillBackground(false);
    m_color = ConfigModel::ColorHighlight;

    connect(this, SIGNAL(clicked(bool)), this, SLOT(slotClicked(bool)));
}

QSize GroupButton::sizeHint() const
{
    return QPushButton::sizeHint();
}

void GroupButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    QRectF r(this->geometry());
    r.moveTo(0, 0);

    bool mouseOver = false;
    QPoint globalPos = QCursor::pos();
    QPoint pos = this->mapFromGlobal(globalPos);

    if (r.contains(pos))
        mouseOver = true;

    QRectF textr = r;
    
    if (m_bDisplayed)
    {
        QPen pen(Qt::black);
        pen.setWidth(0.5);
        painter.setPen(pen);
        r.adjust(2, 2, -4, -4);
        painter.fillRect(r, QColor("#EAF6FF"));
        painter.drawRect(r);
    }
    else
    {
        painter.fillRect(textr, Qt::white);
    }
    painter.drawText(textr, Qt::AlignCenter, m_text);
}

void GroupButton::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    update(this->rect());
}

GroupButton::~GroupButton()
{
}

void GroupButton::setSelected(bool bSelected)
{
    m_bSelected = bSelected;
    update(this->rect());
}

void GroupButton::setDisplayed(bool bDisplayed)
{
    m_bDisplayed = bDisplayed;
    update(this->rect());
}


void GroupButton::slotClicked(bool)
{
    emit selected(m_nGroup);
}
