#include <QMainWindow>
#include <QApplication>
#include <QHideEvent>

#include "DockWidget.h"


DockWidget::DockWidget(QWidget *parent, QString name, bool bAutohide) :
QDockWidget(name, parent)
{
    m_titleBar = new DockWidgetTitleBar(this, bAutohide);
	m_titleBar->setWindowTitle(name);
	setTitleBarWidget(m_titleBar);

    connect(m_titleBar, SIGNAL(dock()), this, SLOT(slotToggleDock()));
    connect(m_titleBar, SIGNAL(autohide()), this, SLOT(slotToggleAutoHide()));
    connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(dockStatusChanged()));
}

DockWidget::~DockWidget()
{
    delete m_titleBar;
}

void DockWidget::hideEvent(QHideEvent *event)
{
	if (!event->spontaneous())
	{
        emit dockVisibilityChanged(false);
	}
}

void DockWidget::showEvent(QShowEvent *event)
{
	if (!event->spontaneous())
	{
        emit dockVisibilityChanged(true);
        raise();
    }
}


void DockWidget::resetWindowTitle()
{
    m_titleBar->setWindowTitle(this->windowTitle());
}

void DockWidget::slotToggleDock()
{
    setFloating(!isFloating());
    m_titleBar->setDocked(!isFloating());
    raise();
}

void DockWidget::dockStatusChanged()
{
    m_titleBar->setDocked(!isFloating());
}

void DockWidget::slotToggleAutoHide()
{
    bool bOptions = false;
    
    if (this->windowTitle() == "Options")
    {
        bOptions = true;
    }
    QMainWindow *w = qobject_cast<QMainWindow*>(parent());

    if (isFloating())
    {
        if (bOptions)
            w->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

        setFloating(false);
        m_titleBar->setAutohide(false);
    }
    else
    {
        if (bOptions)
            w->setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);

        setFloating(true);
        m_titleBar->setAutohide(true);
        emit autohide();
    }
}



