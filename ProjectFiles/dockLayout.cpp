/////////////////////////////////////////////////////////////////
///
/// @file dockLayout.h
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  Aug 26, 2015
///
/// @brief DockLayout is responsible for saving and restoring the layout of the dock widgets
///
/// @note
///
#include <QTabBar>

#include "../ProjectFiles/dockLayout.h"
#include "../widgets/DockWidget.h"


/// @note
/// icon resource path
/* static */
QString DockLayout::m_iconPath = ":/new/Bitmaps/Bitmaps/";
QString DockLayout::m_mouseOver = "MouseOver";
QString DockLayout::m_pushed = "Pushed";

/// @note
/// name for each dock window icon resource
/* static */
QStringList DockLayout::m_dockTitles = QStringList()
<< "Continuous Frequency Sp" << "Channels" << "2D Clusters" << "3D Clusters" << "Units" << "Feature vs Feature" << "Waveforms" << "Peak Heights Histogram" << "Timeline" << "Cross Correlograms";

/// @note
/// name for each dock window icon resource
/* static */
QStringList DockLayout::m_resourceNames = QStringList()
<< "FFT" << "Channels" << "2DClusters" << "3DClusters" << "Units" << "Features" << "Waveforms" << "PeakHeightsHistogram" << "Timeline" << "Timeline";

/// @note
/// allowed areas for each dock window
/* static */
QList<Qt::DockWidgetArea> DockLayout::allowedAreas = QList<Qt::DockWidgetArea>()
<< Qt::TopDockWidgetArea << Qt::RightDockWidgetArea << Qt::TopDockWidgetArea << Qt::TopDockWidgetArea << Qt::TopDockWidgetArea << Qt::TopDockWidgetArea << Qt::TopDockWidgetArea << Qt::TopDockWidgetArea << Qt::TopDockWidgetArea << Qt::BottomDockWidgetArea;

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  Constructor for DockLayout class
/// @param [in]
DockLayout::DockLayout(MainWindow *pMainWindow)
{
	for (int i = 0; i < m_resourceNames.size(); i++)
	{
		QString sName = m_resourceNames[i];

		QString sNormal = m_iconPath + sName;
		QString sSelected = m_iconPath + sName + m_pushed;
		QString sActive = m_iconPath + sName + m_mouseOver;
		QString sDisabled = m_iconPath + sName;

		QIcon icon(sNormal);
		icon.addFile(sSelected, QSize(), QIcon::Normal, QIcon::On);
		icon.addFile(sActive, QSize(), QIcon::Active, QIcon::On);
		icon.addFile(sDisabled, QSize(), QIcon::Active, QIcon::Off);

		DockWidget *dock = new DockWidget(pMainWindow, m_dockTitles[i]);
		dock->setObjectName(sName);
		m_dockList[i] = dock;
		ActionList[i] = dock->toggleViewAction();
		ActionList[i]->setIcon(icon);
		pMainWindow->addDockWidget(allowedAreas[i], dock);
	}
	m_mainWindow = pMainWindow;
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  Destructor for DockLayout class - deletes contents of Queue
DockLayout::~DockLayout()
{   
}

void DockLayout::saveWindowLayout(WizardPage page)
{
    //// save the layout for this page
    //if (page != WIZARDPAGE_END)
    //{
    //    LayoutModel *lm = LayoutModel::instance();
    //    lm->saveMainWindowData(m_mainWindow->size(), m_mainWindow->pos(), m_mainWindow->isMaximized());

    //    //QList<QMainWindowLayout *> mwl = m_mainWindow->findChildren<QMainWindowLayout *>();

    //    ////QList<QLayout *> layout = m_mainWindow->findChildren<QLayout *>();
    //    //QRect r;
    //    //QString s;

    //    //foreach(QMainWindowLayout *l, mwl)
    //    //{
    //    //    r = l->geometry();
    //    //    s = l->objectName();

    //    //    if (s == "_layout")
    //    //    {
    //    //        l->setSizeConstraint(QLayout::SetMaximumSize);
    //    //    }
    //    //}

    //    // get a list of the dock widgets that are tabbed and selected
    //    QList<QTabBar *> tabBars = m_mainWindow->findChildren<QTabBar *>();
    //    QList<QVariant> currentTabIDs;

    //    foreach(QTabBar *tb, tabBars)
    //    {
    //        int id = lm->getID(tb->tabText(tb->currentIndex()));

    //        if (id > 0)
    //            currentTabIDs.append(id);
    //    }
    //    // save the layout data for each view window
    //    for (int i = 0; i < VIEWWINDOW_COUNT; ++i)
    //    {
    //        DockWidget *w = m_mainWindow->m_dockList[i];
    //        // get a list of the dock widgets that are tabbed with this view window
    //        QList<QDockWidget *> dl = m_mainWindow->tabifiedDockWidgets(w);
    //        QList<QVariant> dockIDs;

    //        foreach(QDockWidget *dw, dl)
    //        {
    //            int id = lm->getID(dw->objectName());

    //            if (id > 0)
    //                dockIDs.append(id);
    //        }
    //        // if the view window is allowed on this page
    //        if (lm->isWindowAllowed(page, (ViewWindows)i))
    //        {
    //            bool bSelected = false;
    //            bool bAllPages = false;

    //            if (currentTabIDs.contains(i))
    //                bSelected = true;

    //            if (((ViewWindows)i == VIEWWINDOW_CHANNELS) || ((ViewWindows)i == VIEWWINDOW_OPTIONS))
    //                bAllPages = true;

    //            lm->saveWindowData((ViewWindows)i, w->size(), w->pos(), w->isFloating(), m_mainWindow->dockWidgetArea(w), dockIDs);
    //            lm->saveVisible((ViewWindows)i, page, w->isVisible(), bSelected, bAllPages);
    //        }
    //    }
    //    // set all tabbed docks that are tabbed together to the right geometry (only the selected dock widget has the correct geometry)
    //    lm->updateSizes();
    //}
}

void DockLayout::resetLayout()
{
    m_mainWindowLayoutInitialized = false;
    m_topContinuousDockAreaInitialized = false;
    m_topSpikeDockAreaInitialized = false;
}

void DockLayout::restoreWindowLayout(WizardPage page, WizardPage lastPage, bool bForce)
{
	return;
    bool initialized = m_mainWindowLayoutInitialized;
    restoreMainWindow();

    if (bForce)
    {
        restoreDocks(page);
        restoreDockWidgetArea(Qt::TopDockWidgetArea, page);
        restoreDockWidgetArea(Qt::BottomDockWidgetArea, page);
        restoreDockWidgetArea(Qt::RightDockWidgetArea, page);
        restoreDockWidgetArea(Qt::LeftDockWidgetArea, page);
        tabDocks(page);
        selectTopTabs(page);
    }
	else if ((page == WIZARDPAGE_DETECT) && !m_topContinuousDockAreaInitialized)
    {
        restoreDocks(page);
        restoreDockWidgetArea(Qt::TopDockWidgetArea, page);
        restoreDockWidgetArea(Qt::BottomDockWidgetArea, page);
        restoreDockWidgetArea(Qt::RightDockWidgetArea, page);
        restoreDockWidgetArea(Qt::LeftDockWidgetArea, page);
        tabDocks(page);
        selectTopTabs(page);
        m_topContinuousDockAreaInitialized = true;
    }
	else if ((page != WIZARDPAGE_DETECT) && !m_topSpikeDockAreaInitialized)
    {
        restoreDocks(page);
        restoreDockWidgetArea(Qt::TopDockWidgetArea, page);
        restoreDockWidgetArea(Qt::BottomDockWidgetArea, page);
        restoreDockWidgetArea(Qt::RightDockWidgetArea, page);
        restoreDockWidgetArea(Qt::LeftDockWidgetArea, page);
        tabDocks(page);
        selectTopTabs(page);
        m_topSpikeDockAreaInitialized = true;
    }
	else if ((page != lastPage) && !m_mainWindow->isMaximized())
    {
        restoreDocks(page);
        restoreDockWidgetArea(Qt::TopDockWidgetArea, page);
        restoreDockWidgetArea(Qt::BottomDockWidgetArea, page);
        restoreDockWidgetArea(Qt::RightDockWidgetArea, page);
        restoreDockWidgetArea(Qt::LeftDockWidgetArea, page);
        tabDocks(page);
        selectTopTabs(page);
    }
    if (!initialized)
    {
		resizeDocks(Qt::TopDockWidgetArea);
		restoreDockWidgetArea(Qt::RightDockWidgetArea, page);
        restoreDockWidgetArea(Qt::LeftDockWidgetArea, page);
        tabDocks(page);
        selectTopTabs(page);
    }
}

void DockLayout::restoreMainWindow()
{
	m_mainWindow->show();
	return;

	if (m_mainWindowLayoutInitialized == false)
    {
        m_mainWindowLayoutInitialized = true;
        LayoutModel *lm = LayoutModel::instance();
        QSize size;
        QPoint pos;
        bool bMaximized;

        lm->getMainWindowData(size, pos, bMaximized);

        if (bMaximized)
        {
            m_mainWindow->showMaximized();
        }
        else
        {
            m_mainWindow->resize(size);
            m_mainWindow->move(pos);
            m_mainWindow->show();
        }
    }
}

void DockLayout::restoreDocks(WizardPage page)
{
	return;
//    LayoutModel *lm = LayoutModel::instance();
//
//    for (int i = 0; i < VIEWWINDOW_COUNT; ++i)
//    {
//        QSize size;
//        QPoint pos;
//        bool bFloating;
//        Qt::DockWidgetArea area;
//        QList<QVariant> tabIDs;
//        DockWidget *w = m_mainWindow->m_dockList[i];
//
//        lm->getWindowData((ViewWindows)i, size, pos, bFloating, area, tabIDs);
//
//        if (lm->isWindowAllowed(page, (ViewWindows)i))
//        {
//            // remove the dock widget and re-add it with the new area
//            // the entire layout is resized if you don't do this
////            m_mainWindow->removeDockWidget(w);
//            // reset the geometry
//            w->resize(size);
//            w->move(pos);
////            w->updateGeometry();
////            m_mainWindow->addDockWidget(area, w);
//            w->setFloating(bFloating);
//        }
//    }
}

void DockLayout::resizeDocks(Qt::DockWidgetArea area)
{
	return;
//	LayoutModel *lm = LayoutModel::instance();
//
//    for (int i = 0; i < VIEWWINDOW_COUNT; ++i)
//    {
//        QSize size;
//        QPoint pos;
//        bool bFloating;
//        Qt::DockWidgetArea a;
//        QList<QVariant> tabIDs;
//        DockWidget *w = m_mainWindow->m_dockList[i];
//
//        lm->getWindowData((ViewWindows)i, size, pos, bFloating, a, tabIDs);
//
//        if (area == a)
//        {
//            // remove the dock widget and re-add it with the new area
//            // the entire layout is resized if you don't do this
//            m_mainWindow->removeDockWidget(w);
//            w->setFloating(bFloating);
//            w->resize(size);
//			w->move(pos);
////			w->updateGeometry();
//            m_mainWindow->addDockWidget(area, w);
//        }
//    }
}

void DockLayout::tabDocks(WizardPage page)
{
	return;
	//LayoutModel *lm = LayoutModel::instance();
 //   QList <int> dockedWidgetIDs;
 //   dockedWidgetIDs = lm->getDockedWindows(page);
 //   QMap <int, QList<QVariant>> tabbedDocks;
 //   tabbedDocks.clear();

 //   // first collect the list of tabbed docks
 //   for (int i = 0; i < dockedWidgetIDs.count(); i++)
 //   {
 //       QSize size;
 //       QPoint point;
 //       bool bFloating;
 //       Qt::DockWidgetArea a;
 //       QList<QVariant> tabIDs;

 //       ViewWindows dockID = (ViewWindows)dockedWidgetIDs[i];

 //       if (lm->isWindowAllowed(page, (ViewWindows)dockID))
 //       {
 //           lm->getWindowData(dockID, size, point, bFloating, a, tabIDs);

 //           if (!tabIDs.isEmpty())
 //               tabbedDocks.insert(dockID, tabIDs);
 //       }
 //   }
 //   // tabify each of the tabbed docks
 //   QMap<int, QList<QVariant>>::const_iterator it = tabbedDocks.constBegin();

 //   while (it != tabbedDocks.constEnd())
 //   {
 //       int dockID = it.key();
 //       DockWidget *w = m_mainWindow->m_dockList[dockID];

 //       QList<QVariant> tabIDs = it.value();

 //       foreach(QVariant id, tabIDs)
 //       {
 //           ViewWindows tabID = (ViewWindows)id.toInt();
 //           DockWidget *wt = m_mainWindow->m_dockList[tabID];

 //           m_mainWindow->tabifyDockWidget(w, wt);
 //       }
 //       it++;
 //   }
 //   selectTopTabs(page);
}

void DockLayout::selectTopTabs(WizardPage page)
{
	return;
	//LayoutModel *lm = LayoutModel::instance();

 //   for (int i = 0; i < VIEWWINDOW_COUNT; ++i)
 //   {
 //       if (lm->isWindowAllowed(page, (ViewWindows)i))
 //       {
 //           if (lm->isSelected((ViewWindows)i))
 //           {
 //               m_mainWindow->m_dockList[i]->raise();
 //           }
 //       }
 //   }
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  create multi maps for the positions, sizes and tabbed docks
void DockLayout::restoreDockWidgetArea(Qt::DockWidgetArea area, WizardPage page)
{
	return;
	//LayoutModel *lm = LayoutModel::instance();
 //   QList <int> dockedWidgetIDs;
 //   dockedWidgetIDs = lm->getDockedWindows(area, page);
 //   QMap <int, QRect>  rects;

 //   for (int i = 0; i < dockedWidgetIDs.count(); i++)
 //   {
 //       QSize size;
 //       QPoint point;
 //       bool bFloating;
 //       Qt::DockWidgetArea a;
 //       QList<QVariant> tabIDs;

 //       ViewWindows dockID = (ViewWindows)dockedWidgetIDs[i];

 //       if (lm->isWindowAllowed(page, (ViewWindows)dockID))
 //       {
 //           lm->getWindowData(dockID, size, point, bFloating, a, tabIDs);

 //           QRect rect(point, size);
 //           rects.insert(dockID, rect);
 //       }
 //   }
 //   DockMap dockMap(rects);
 //   doSplit(dockMap);
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  if there is another move request, replace the request. else add the request
/// @param [in]  area - top, left, bottom
/// @param [in]  page - BOSS wizard page
void DockLayout::doSplit(DockMap &dockMap)
{
	return;
	//if (dockMap.count() > 1)
 //   {
 //       int split = dockMap.findRightSplit();

 //       if (split != -1)
 //       {
 //           DockMap leftMap;
 //           DockMap rightMap;

 //           dockMap.splitHorizontal(leftMap, rightMap, split);
 //           int leftDockID = leftMap.topLeftDockID();
 //           int rightDockID = rightMap.topLeftDockID();

 //           if ((leftDockID != -1) && (rightDockID != -1))
 //           {
 //               m_mainWindow->splitDockWidget(m_mainWindow->m_dockList[leftDockID], m_mainWindow->m_dockList[rightDockID], Qt::Horizontal);
 //               doSplit(leftMap);
 //               doSplit(rightMap);
 //           }
 //       }
 //       else
 //       {
 //           split = dockMap.findTopSplit();

 //           if (split != -1)
 //           {
 //               DockMap topMap;
 //               DockMap bottomMap;

 //               dockMap.splitVertical(topMap, bottomMap, split);
 //               int topDockID = topMap.topLeftDockID();
 //               int bottomDockID = bottomMap.topLeftDockID();

 //               if ((topDockID != -1) && (bottomDockID != -1))
 //               {
 //                   m_mainWindow->splitDockWidget(m_mainWindow->m_dockList[topDockID], m_mainWindow->m_dockList[bottomDockID], Qt::Vertical);
 //                   doSplit(topMap);
 //                   doSplit(bottomMap);
 //               }
 //           }
 //       }
 //   }
}


