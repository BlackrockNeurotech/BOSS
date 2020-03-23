#include <QApplication>
#include <QStyle>


#include "../widgets/DockWidgetTitleBar.h"
#include "../model/configmodel.h"


DockWidgetTitleBar::DockWidgetTitleBar(QDockWidget *parent, bool bAutohide) : QWidget(parent)
{
    m_dockfloatToolButton = 0;
    m_autohideToolButton = 0;

    m_bAutohide = bAutohide;

    this->setGeometry(QRect(0, 0, 355, 20));
    this->setAutoFillBackground(true);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 1, 0, 0);

    QFrame *frame = new QFrame(this);
    frame->setAutoFillBackground(true);
    frame->setFrameShape(QFrame::NoFrame);
    frame->setFrameShadow(QFrame::Plain);

    QHBoxLayout *layout2 = new QHBoxLayout;
    layout2->setSpacing(1);
    layout2->setContentsMargins(4, 1, 0, 0);

    m_title = new QLabel(parent->windowTitle());
    m_title->setIndent(2);

    if (m_bAutohide)
    {
        m_autohideToolButton = new QToolButton();
        m_autohideToolButton->setMaximumSize(16, 16);
        m_autohideToolButton->setMinimumSize(16, 16);
        m_autohideToolButton->setAutoRaise(true);
        m_autohideToolButton->setIcon(QApplication::style()->standardPixmap(QStyle::SP_TitleBarNormalButton));
    }
    else
    {
        m_dockfloatToolButton = new QToolButton();
        m_dockfloatToolButton->setMaximumSize(16, 16);
        m_dockfloatToolButton->setMinimumSize(16, 16);
        m_dockfloatToolButton->setAutoRaise(true);
        m_dockfloatToolButton->setIcon(QApplication::style()->standardPixmap(QStyle::SP_TitleBarNormalButton));
    }
    m_closeToolButton = new QToolButton();
    m_closeToolButton->setMaximumSize(16, 16);
    m_closeToolButton->setMinimumSize(16, 16);
    m_closeToolButton->setAutoRaise(true);

    m_closeToolButton->setIcon(QApplication::style()->standardPixmap(QStyle::SP_TitleBarCloseButton));

    layout2->addWidget(m_title);

    if (m_bAutohide)
        layout2->addWidget(m_autohideToolButton);
    else
        layout2->addWidget(m_dockfloatToolButton);

    layout2->addWidget(m_closeToolButton);

    frame->setLayout(layout2);
    layout->addWidget(frame);
    this->setLayout(layout);

    QPalette p(frame->palette());
    p.setColor(QPalette::Window, ConfigModel::ColorDockTitlebar);
    p.setColor(QPalette::Foreground, ConfigModel::ColorDockTitlebarForeground);
    frame->setPalette(p);

    connect(m_closeToolButton, SIGNAL(clicked(bool)), parent, SLOT(close()));
    
    if (m_bAutohide)
        connect(m_autohideToolButton, SIGNAL(clicked(bool)), this, SLOT(slotAutohideClicked(bool)));
    else
        connect(m_dockfloatToolButton, SIGNAL(clicked(bool)), this, SLOT(slotDockClicked(bool)));
}

DockWidgetTitleBar::~DockWidgetTitleBar()
{
    disconnect(m_closeToolButton, SIGNAL(clicked(bool)), parent(), SLOT(close()));
}

void DockWidgetTitleBar::setWindowTitle(const QString &title)
{
    m_title->setText(title);
    QWidget::setWindowTitle(title);
}


void DockWidgetTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit dock();
}

void DockWidgetTitleBar::slotDockClicked(bool state)
{
    Q_UNUSED(state);
    emit dock();
}

void DockWidgetTitleBar::slotAutohideClicked(bool state)
{
    Q_UNUSED(state);
    emit autohide();
}

void DockWidgetTitleBar::setDocked(bool dock)
{
    if (m_dockfloatToolButton != 0)
    {
        if (dock)
            m_dockfloatToolButton->setIcon(QApplication::style()->standardPixmap(QStyle::SP_TitleBarNormalButton));
        else
            m_dockfloatToolButton->setIcon(QIcon(":/new/Bitmaps/Bitmaps/float_16x16.png"));
    }
}

void DockWidgetTitleBar::setAutohide(bool autohide)
{
    if (autohide)
        m_autohideToolButton->setIcon(QApplication::style()->standardPixmap(QStyle::SP_ToolBarVerticalExtensionButton));
    else
        m_autohideToolButton->setIcon(QIcon(":/new/Bitmaps/Bitmaps/float_16x16.png"));
}
