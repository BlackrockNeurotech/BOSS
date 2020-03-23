#include <QMainWindow>

#include "../widgets/DockToolbar.h"

/// @note
/// icon resource path
/* static */
QString DockToolbar::m_iconPath = ":/new/Bitmaps/Bitmaps/";
QString DockToolbar::m_mouseOver = "MouseOver";
QString DockToolbar::m_pushed = "Pushed";
QString DockToolbar::m_extension = ".png";

/// @note
/// information for each dock window
///  this list must match enum dockIDs
/* static */
QList<DockWindows*> DockToolbar::m_dockDescriptors = QList<DockWindows*>()
<< new DockWindows("Channels", "Channels", Qt::RightDockWidgetArea, DETECT_PAGE | SORT_PAGE)
<< new DockWindows("TimeLine", "Timeline", Qt::BottomDockWidgetArea, DETECT_PAGE | SORT_PAGE)
<< new DockWindows("Frequency", "Frequency Spectrum", Qt::TopDockWidgetArea, DETECT_PAGE)
<< new DockWindows("PeakHeightsHistogram", "Histograms", Qt::TopDockWidgetArea, DETECT_PAGE)
<< new DockWindows("2DClusters", "2D Clusters", Qt::TopDockWidgetArea, SORT_PAGE)
<< new DockWindows("3DClusters", "3D Clusters", Qt::TopDockWidgetArea, SORT_PAGE)
<< new DockWindows("Units", "Units", Qt::TopDockWidgetArea, SORT_PAGE)
<< new DockWindows("Features", "Feature vs Feature", Qt::TopDockWidgetArea, SORT_PAGE)
<< new DockWindows("WaveForms", "Waveforms", Qt::TopDockWidgetArea, SORT_PAGE)
<< new DockWindows("CrossCorrelograms", "Cross Correlograms", Qt::TopDockWidgetArea, SORT_PAGE);


DockToolbar::DockToolbar(QMainWindow *parent) :
    QToolBar((QWidget*)parent)
{
    setStyleSheet("QToolBar { border: 0px; background-color: #FFFFFF}");

    foreach(DockWindows *dockWindow, m_dockDescriptors)
	{
        QString sName = dockWindow->m_sResourceName;
        QString sNormal = m_iconPath + sName + m_extension;
        QString sTooltip("Show or Hide ");
        sTooltip += dockWindow->m_sTitle;

        // create the dock widget
        DockWidget *dock = new DockWidget(parent, dockWindow->m_sTitle);
        dock->setObjectName(dockWindow->m_sTitle);
        dock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
        dock->setContentsMargins(0, 0, 0, 0);
        dockWindow->m_DockWidget = dock;

        // create the tool button
        BlackrockButton *pb = new BlackrockButton(parent, "");

        pb->setImage(sNormal);
        pb->setToolTip(sTooltip);
        pb->setCheckable(true);
//        pb->setInvert(true);
        pb->setDimensionsToIcon();
        
        dockWindow->m_Action = this->addWidget(pb);
        QObject::connect(pb, SIGNAL(clicked(bool)), dock, SLOT(setVisible(bool)));
        QObject::connect(dock, SIGNAL(dockVisibilityChanged(bool)), pb, SLOT(setChecked(bool)));

        parent->addDockWidget(dockWindow->m_allowedAreas, dock);
    }
    this->setContentsMargins(0, 0, 0, 0);
}

DockToolbar::~DockToolbar()
{
}

void DockToolbar::initialize()
{
    foreach(DockWindows *dockWindow, m_dockDescriptors)
    {
        if (dockWindow->m_Action->isVisible())
            dockWindow->m_Action->setChecked(true);
        else
            dockWindow->m_Action->setChecked(false);
    }
}


void DockToolbar::hideDocks()
{
	foreach(DockWindows *dockWindow, m_dockDescriptors)
	{
        dockWindow->m_Action->setVisible(false);
        dockWindow->m_Action->setChecked(false);
		dockWindow->m_DockWidget->setVisible(false);
	}
}

void DockToolbar::showDocks()
{
    foreach(DockWindows *dockWindow, m_dockDescriptors)
    {
        dockWindow->m_DockWidget->show();
    }
}
QList<QDockWidget *> DockToolbar::getDockList()
{
    QList<QDockWidget *> docks;

    foreach(DockWindows *dockWindow, m_dockDescriptors)
    {
        docks.push_back(dockWindow->m_DockWidget);
    }
    return docks;
}

void DockToolbar::raiseDock(dockIDs dockid)
{
    if ((dockid >= 0) && (dockid < DOCK_COUNT))
    {
        getDockWidget(dockid)->raise();
    }
}

void DockToolbar::updateDocks(WizardPage page, bool reset)
{
	quint8 pageBitMask = 0;

	if (page == WIZARDPAGE_DETECT)
		pageBitMask = DETECT_PAGE;
	else if (page == WIZARDPAGE_SORT)
		pageBitMask = SORT_PAGE;

	foreach(DockWindows *dockWindow, m_dockDescriptors)
	{
		if (dockWindow->m_allowedPages & pageBitMask)
		{
			if (reset || !dockWindow->m_DockWidget->isVisible())
			{
                dockWindow->m_Action->setVisible(true);
                dockWindow->m_Action->setChecked(true);
				dockWindow->m_DockWidget->setVisible(true);
			}
		}
		else
		{
            dockWindow->m_Action->setVisible(false);
            dockWindow->m_Action->setChecked(false);
			dockWindow->m_DockWidget->setVisible(false);
		}
	}
}

void DockToolbar::createPopupMenu(QMenu *menu, WizardPage page)
{
    QList<QAction *> list = menu->actions();
    quint8 pageBitMask = 0;

    if (page == WIZARDPAGE_DETECT)
        pageBitMask = DETECT_PAGE;
    else if (page == WIZARDPAGE_SORT)
        pageBitMask = SORT_PAGE;

    // enable/disable each dock window depending on the page
    foreach(DockWindows *dockWindow, m_dockDescriptors)
    {
        QString sTitle = dockWindow->m_sTitle;
        QAction *action = 0;

        // find the corresponding action in the menu
        for (int i(0); i < list.size(); ++i)
        {
            QAction *act = list[i];
            QString text = act->text();

            if (text == sTitle)
            {
                action = act;
                break;
            }
        }
        if (dockWindow->m_allowedPages & pageBitMask)
        {
            if (action != 0)
                action->setEnabled(true);

            //if (!dockWindow->m_DockWidget->isVisible())
            //{
            //    dockWindow->m_Action->setVisible(true);
            //    dockWindow->m_Action->setChecked(true);
            //    dockWindow->m_DockWidget->setVisible(true);
            //}
        }
        else
        {
            if (action != 0)
                action->setDisabled(true);

            dockWindow->m_Action->setVisible(false);
            dockWindow->m_Action->setChecked(false);
            dockWindow->m_DockWidget->setVisible(false);
        }
    }
}

void DockToolbar::releaseWidget(dockIDs dockid)
{
	if ((dockid >= 0) && (dockid < DOCK_COUNT))
	{
		DockWidget* dock = getDockWidget(dockid);
		dock->setWidget(0);
	}
}

DockWidget* DockToolbar::getDockWidget(dockIDs dockid)
{
	if ((dockid >= 0) && (dockid < DOCK_COUNT))
		return m_dockDescriptors[dockid]->m_DockWidget;

	return NULL;
}



