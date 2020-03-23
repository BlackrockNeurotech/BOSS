#ifndef DOCKTOOLBAR_H
#define DOCKTOOLBAR_H

#include <QToolBar>
#include <QWidgetAction>
#include <QMenu>
#include <QList>

#include "../widgets/DockWidget.h"
#include "../model/channelItem.h"
#include "../widgets/BlackrockButton.h"

#define DETECT_PAGE 0X01
#define SORT_PAGE 0X02



// note this list must match 	
// static QList<DockWindows> DockWindows::m_dockDescriptors;

enum dockIDs
{
	DOCK_CHANNELS = 0,//0
	DOCK_TIMELINE,    //1
	DOCK_FREQSPEC,    //2
	DOCK_PEAKHEIGHTS, //3
	DOCK_2DCLUSTER,   //4
	DOCK_3DCLUSTER,   //5
	DOCK_UNITS,       //6
	DOCK_FEATURES,    //7
	DOCK_WAVEFORMS,   //8
	DOCK_CORRELOGRAMS,//9
	DOCK_COUNT
};

struct DockWindows
{
	DockWindows(QString sResourceName, QString sTitle, Qt::DockWidgetArea allowedAreas, quint8 allowedPages)
	{
		m_sResourceName = sResourceName;
		m_sTitle = sTitle;
		m_allowedAreas = allowedAreas;
		m_allowedPages = allowedPages;
		m_Action = 0;
		m_DockWidget = 0;
	}
	QString				m_sResourceName;
	QString				m_sTitle;
	Qt::DockWidgetArea	m_allowedAreas;
	quint8				m_allowedPages;
	QAction			   *m_Action;
	DockWidget		   *m_DockWidget;
};


class DockToolbar : public QToolBar
{
    Q_OBJECT

public:
	DockToolbar(QMainWindow *parent);
    ~DockToolbar();

	void initialize();
	DockWidget* getDockWidget(dockIDs dockid);
	void releaseWidget(dockIDs dockid);
	void updateDocks(WizardPage page, bool reset = false);
	void hideDocks();
	void showDocks();
    void raiseDock(dockIDs dockid);
    void createPopupMenu(QMenu *menu, WizardPage page);
    QList<QDockWidget *> getDockList();

private:
	// The resource files for each tool window icon
	static QString     m_iconPath;
	static QString     m_mouseOver;
	static QString     m_pushed;
	static QString     m_extension;
	
	static QList<DockWindows *> m_dockDescriptors;
};


#endif // DOCKTOOLBAR_H
