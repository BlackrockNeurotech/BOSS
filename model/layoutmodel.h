/////////////////////////////////////////////////////////////////
///
/// @file LayoutModel.h
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  June 29, 2015
///
/// @brief LayoutModel contains application layout information.
///
/// @note
/// LayoutModel is in a tree format. The top layer is the pages - preprocess, detect, feature, sort.
/// The bottom layer is the application windows with size, position, visible, and float information.
/// Each node subclasses layoutItem.
/// layoutModel is a singleton.
///

#ifndef LAYOUTMODEL_H_INCLUDED
#define LAYOUTMODEL_H_INCLUDED

#include <QItemSelection>
#include <QString>
#include <QDockWidget>


#include "../model/channelconfigmodel.h"
#include "../model/WindowData.h"

/// @note
/// children of the PageData
enum ViewWindows
{
    VIEWWINDOW_FREQSPEC = 0,//0
    VIEWWINDOW_CHANNELS,    //1
    VIEWWINDOW_2DCLUSTER,   //2
    VIEWWINDOW_3DCLUSTER,   //3
    VIEWWINDOW_UNITS,       //4
    VIEWWINDOW_FEATURES,    //5
    VIEWWINDOW_WAVEFORMS,   //6
    VIEWWINDOW_PEAKHEIGHTS, //7
    VIEWWINDOW_OPTIONS,     //8
	VIEWWINDOW_TIMELINE,    //9
	VIEWWINDOW_CORRELOGRAMS,//10
	VIEWWINDOW_MAIN,        //11
    VIEWWINDOW_ALL_COUNT
};

const int VIEWWINDOW_COUNT = VIEWWINDOW_MAIN;


class LayoutModel : public layoutItem
{
    Q_OBJECT
private:
    static LayoutModel *inst;

public:
    static LayoutModel* instance();
    explicit LayoutModel(QObject *parent = 0);
    ~LayoutModel();

    void initialize();

    void saveWindowData(ViewWindows window, QSize size, QPoint pos, bool bFloating, Qt::DockWidgetArea area, QList<QVariant> dockIDs);
    void getWindowData(ViewWindows window, QSize &size, QPoint &pos, bool &bFloating, Qt::DockWidgetArea &area, QList<QVariant> &dockIDs);
    void saveMainWindowData(QSize size, QPoint pos, bool bMaximized);
    void getMainWindowData(QSize &size, QPoint &pos, bool &bMaximized);

    Qt::DockWidgetArea getArea(ViewWindows window);

    bool isWindowAllowed(WizardPage page, ViewWindows window);
    bool isWindowVisible(WizardPage page, ViewWindows window);
    void saveVisible(ViewWindows window, WizardPage page, bool bVisible, bool bSelected, bool allPages = false);
    void updateSizes();
    void updateTopSizes(int diff);

    QList<int> getDockedWindows(Qt::DockWidgetArea &area, WizardPage page);
    QList<int> getDockedWindows(WizardPage page);
    QList<int> getVisibleWindows(WizardPage page);
    int getID(QString objectName);
    bool isSelected(ViewWindows window);

    bool saveToXML(QString path);
    bool getFromXML(QString path);

    // The following string list corresponds to the PageChildren enum
    static QStringList WindowNames;

private:
    static QList<QVariant> AllowedPages[VIEWWINDOW_COUNT];
    layoutItem *m_rootItem;

    void initAllowedPages(ViewWindows window, QList<QVariant> pages);

};

#endif // LAYOUTMODEL_H_INCLUDED
