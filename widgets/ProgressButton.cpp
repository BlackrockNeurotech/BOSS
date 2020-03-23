#include <QPainter>
#include <QStaticText>
#include <QImage>

#include "../widgets/ProgressButton.h"
#include "../model/configmodel.h"


ProgressButton::ProgressButton(QWidget *parent, int nChannel, QString text, bool cancel) :
QPushButton(text, parent)
{
    QString sImage(":/new/Bitmaps/Bitmaps/Cancel.png");
    QString sImageHover(":/new/Bitmaps/Bitmaps/CancelHover.png");
    m_imageCancel = new QImage(sImage);
    m_imageCancelHover = new QImage(sImageHover);
    m_drawImage = m_imageCancel;
    m_nChannel = nChannel;
    m_cancel = cancel;
    m_text = text;
    m_bShowText = false;
    m_percent = 0;

    if (m_cancel)
    {
        setAttribute(Qt::WA_Hover);
        setMouseTracking(true);
        setAutoFillBackground(true);

        connect(this, SIGNAL(clicked(bool)), this, SLOT(slotCancel(bool)));
    }
    else
    {
        setAutoFillBackground(false);
    }
    m_color = ConfigModel::ColorHighlight;
}

QSize ProgressButton::sizeHint() const
{
    return QPushButton::sizeHint();
}

void ProgressButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF r(this->geometry());
    r.moveTo(0, 0);

    if (m_cancel)
    {
        r.adjust(2, 0, -2, 0);
        QRectF rProgress(r);
        rProgress.setTop(rProgress.top() + (rProgress.height() / 3.5));
        rProgress.setBottom(rProgress.bottom() - (rProgress.height() / 3.5));

        QRectF rCancel(rProgress);
        rCancel.setLeft(rProgress.right() - rProgress.height());
        rProgress.setRight(rProgress.right() - rProgress.height() - 4);

        bool mouseOver = false;
        QPoint globalPos = QCursor::pos();
        QPoint pos = this->mapFromGlobal(globalPos);

        if (rCancel.contains(pos))
            mouseOver = true;

        if (m_bShowText)
        {
            QRectF rtext(r);
//            rtext.setRight(r.width() - r.height() - 4);
            painter.setBackgroundMode(Qt::TransparentMode);
            painter.fillRect(rtext, Qt::white);
            painter.drawText(rtext, Qt::AlignLeft | Qt::AlignVCenter, m_text);
        }
        else
        {
            // outline the progress bar
            painter.setPen(QPen(Qt::darkGray));
            painter.drawRect(rProgress);

            // fill the progress
            qreal percentWidth = rProgress.width() * m_percent / 100;
            rProgress.setWidth(percentWidth);
            painter.fillRect(rProgress, m_color);
        }
        // draw the cancel button
        qreal offset = rCancel.width() / 5.0;
        painter.setPen(QPen(Qt::white));
        QRectF rCancelCenter = rCancel.adjusted(offset, 0.0, -offset, -offset);
        if (mouseOver)
            painter.drawImage(rCancel, *m_imageCancelHover);
        else
            painter.drawImage(rCancel, *m_imageCancel);
    }
    else
    {
        qreal percentWidth = r.width() * m_percent / 100;
        QRectF textr = r;
        r.setWidth(percentWidth);
        painter.setBackgroundMode(Qt::TransparentMode);
        painter.fillRect(textr, Qt::white);
        painter.fillRect(r, QColor("#E8F5FF"));
        painter.drawText(textr, Qt::AlignCenter, m_text);
    }
 }

void ProgressButton::mouseMoveEvent(QMouseEvent *event)
{
    update(this->rect());
    QPushButton::mouseMoveEvent(event);
}

void ProgressButton::showText(bool bShow)
{
    m_bShowText = bShow;
    update(this->rect());
}

void ProgressButton::setProgress(int percent)
{
    m_bShowText = false;
    m_percent = percent;
    update(this->rect());
}

ProgressButton::~ProgressButton()
{
}

void ProgressButton::slotCancel(bool)
{
    if (m_cancel)
    {
        QRectF r(this->geometry());
        r.moveTo(0, 0);
        r.setTop(r.top() + (r.height() / 3.5));
        r.setBottom(r.bottom() - (r.height() / 3.5));
        r.adjust(0, 0, -2, 0);
        QRectF rCancel(r);
        rCancel.setLeft(r.width() - r.height());

        QPoint globalPos = QCursor::pos();
        QPoint pos = this->mapFromGlobal(globalPos);

        if (rCancel.contains(pos))
            emit cancelProgress(m_nChannel);
    }
}
