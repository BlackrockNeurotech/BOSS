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

#ifndef DOCKLAYOUT_H_INCLUDED
#define DOCKLAYOUT_H_INCLUDED

#include <QVariant>
#include <QList>

#include "../ProjectFiles/mainwindow.h"
#include "../model/channelconfigmodel.h"
#include "../model/layoutmodel.h"
#include "../ProjectFiles/dockMap.h"


class DockLayout
{
public:
    DockLayout(MainWindow *mainWindow);
    ~DockLayout();

	void createToolButton(DockWidget *dockWidget, ViewWindows window);

    void saveWindowLayout(WizardPage page);
    void resetLayout();
    void restoreMainWindow();
    void restoreWindowLayout(WizardPage thisPage, WizardPage lastPage, bool bForce = false);
    void selectTopTabs(WizardPage page);
    void resizeDocks(Qt::DockWidgetArea area);

	void saveState();
	void restoreState();

private:
	// The resource files for each tool window icon
	static QString     m_iconPath;
	static QString     m_mouseOver;
	static QString     m_pushed;
	static QStringList m_resourceNames;
	static QStringList m_dockTitles;
	static QList<Qt::DockWidgetArea> allowedAreas;

	QAction* ActionList[VIEWWINDOW_COUNT];
	DockWidget* m_dockList[VIEWWINDOW_COUNT];

	
	MainWindow *m_mainWindow;
    bool m_mainWindowLayoutInitialized;
    bool m_topContinuousDockAreaInitialized;
    bool m_topSpikeDockAreaInitialized;


    void restoreDocks(WizardPage page);
    void restoreDockWidgetArea(Qt::DockWidgetArea area, WizardPage page);
    void tabDocks(WizardPage page);
    void doSplit(DockMap &dockMap);
};


#endif // DOCKLAYOUT_H_INCLUDED  

