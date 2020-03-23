/////////////////////////////////////////////////////////////////
///
/// @file LayoutModel.cpp
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

#include <QSize>
#include <QPoint>
#include <QFile>

#include "../model/layoutmodel.h"

/// @note
/// name for each child in a PageData tree item
/* static */
QStringList LayoutModel::WindowNames = QStringList()
<< "Continuous Frequency Spectrum" << "Channels" << "2D Clusters" << "3D Clusters" << "Units" << "Feature vs Feature" << "Waveforms" << "Peak Heights Histogram" << "Options" << "Timeline" << "main";


/* static */
QList<QVariant> LayoutModel::AllowedPages[VIEWWINDOW_COUNT] =
{
    /* VIEWWINDOW_FREQSPEC */
	QList < QVariant > { WIZARDPAGE_DETECT },
    /* VIEWWINDOW_CHANNELS */
    QList < QVariant > {  WIZARDPAGE_DETECT, WIZARDPAGE_SORT, WIZARDPAGE_EVALUATE },
    /* VIEWWINDOW_2DCLUSTER */
    QList < QVariant > { WIZARDPAGE_SORT },
    /* VIEWWINDOW_3DCLUSTER */
    QList < QVariant > { WIZARDPAGE_SORT },
    /* VIEWWINDOW_UNITS */
    QList < QVariant > { WIZARDPAGE_SORT },
    /* VIEWWINDOW_FEATURES */
    QList < QVariant > { WIZARDPAGE_SORT },
    /* VIEWWINDOW_WAVEFORMS */
    QList < QVariant > { WIZARDPAGE_SORT },
    /* VIEWWINDOW_PEAKHEIGHTS */
    QList < QVariant > { WIZARDPAGE_DETECT },
    /* VIEWWINDOW_OPTIONS */
    QList < QVariant > { WIZARDPAGE_DETECT, WIZARDPAGE_SORT, WIZARDPAGE_EVALUATE },
	/* VIEWWINDOW_TIMELINE */
	QList < QVariant > { WIZARDPAGE_DETECT, WIZARDPAGE_SORT },
	/* VIEWWINDOW_CORRELOGRAMS */
	QList < QVariant > { WIZARDPAGE_SORT }
};

/* static */
LayoutModel *LayoutModel::inst = 0;

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  LayoutModel is a singleton. Return the instance or create it if it doesn't exist.
/// @return one and only instance
/* static */
LayoutModel *LayoutModel::instance()
{
    if (!inst)
        inst = new LayoutModel();
    return inst;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Constructor for LayoutModel class - initializes the tree
/// @param [in]  parent object
LayoutModel::LayoutModel(QObject *parent)
{
    Q_UNUSED(parent);

    m_rootItem = 0;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Destructor for LayoutModel class - deletes the tree
/// @param [in]  parent object
LayoutModel::~LayoutModel()
{
    if (m_rootItem != 0)
        delete m_rootItem;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  create the tree with initial data
/// @param [in] number of rows (channels)
void LayoutModel::initialize()
{
    if (m_rootItem != 0)
        delete m_rootItem;
    m_rootItem = new layoutItem();

    for (int i = 0; i < VIEWWINDOW_COUNT; i++)
    {
        WindowData *wd = new WindowData(WindowNames[i], m_rootItem);
        wd->setData(WC_ALLOWED, QVariant(AllowedPages[i]));
        wd->setData(WC_VISIBLE, QVariant(AllowedPages[i]));
        m_rootItem->appendChild(wd);
    }
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Write the contents of the tree to XML
/// @param [in] path of the file that will hold the XML
/// @return TRUE if success; FALSE otherwise
bool LayoutModel::saveToXML(QString path)
{
    if (m_rootItem == 0)
        return false;

    QFile file(path);

    if (!file.open(QFile::WriteOnly|QIODevice::Text))
        return false;

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("BOSS");

    for (int i = 0; i < m_rootItem->childCount(); i++)
    {
        layoutItem *item = m_rootItem->child(i);
        item->saveToXML(&writer);
    }
    writer.writeEndElement();
    writer.writeEndDocument();
    return true;
}

/// @author Holly Ausbeck
/// @date   April 29, 2015
/// @brief  Create the model tree with the contents of the XML
/// @param [in] path of the XML file to read
/// @return TRUE if success; FALSE otherwise
bool LayoutModel::getFromXML(QString path)
{
    if (m_rootItem != 0)
        delete m_rootItem;
    m_rootItem = new layoutItem();

    bool status = false;
    QFile file(path);

    if (!file.open(QFile::ReadOnly | QIODevice::Text))
    {
        initialize();
        return status;
    }
    QXmlStreamReader reader(&file);

    QStringRef name = reader.name();
    if (name != "BOSS")
        reader.readNextStartElement();

    while(!reader.atEnd())
    {
        QStringRef name = reader.name();
        reader.readNextStartElement();
        name = reader.name();

        if (name == "WindowData")
        {
            WindowData *wd = new WindowData("WindowData", m_rootItem);
            wd->getFromXML(&reader);
            m_rootItem->appendChild(wd);
            status = true;
        }
    }
    return status;
}

/// @author Holly Ausbeck
/// @date   June 29, 2015
/// @brief  save position data for the given window
/// @param [in] window - particular window to save
/// @param [in] size - window width and height
/// @param [in] pos - window position relative to the main window
/// @param [in] bFloating - true if the window is floating
/// @param [in] area - dock area
/// @param [in] dockIDs - other window IDs that are tabbed with window
void LayoutModel::saveWindowData(ViewWindows window, QSize size, QPoint pos, bool bFloating, Qt::DockWidgetArea area, QList<QVariant> dockIDs)
{
    if (window < m_rootItem->childCount())
    {
        layoutItem* windowItem = m_rootItem->child(window);

        windowItem->setData(WC_SIZE, QVariant(size));
        windowItem->setData(WC_POS, QVariant(pos));
        windowItem->setData(WC_FLOATING, QVariant(bFloating));
        windowItem->setData(WC_DOCKAREA, QVariant((int)area));
        windowItem->setData(WC_DOCKIDS, QVariant(dockIDs));
    }
}

/// @author Holly Ausbeck
/// @date   June 29, 2015
/// @brief  retrieve position data for the given window
/// @param [in] window - particular window to save
/// @param [out] size - window width and height
/// @param [out] pos - window position relative to the main window
/// @param [out] bFloating - true if the window is floating
/// @param [in] area - dock area
/// @param [in] dockIDs - other window IDs that are tabbed with window
void LayoutModel::getWindowData(ViewWindows window, QSize &size, QPoint &pos, bool &bFloating, Qt::DockWidgetArea &area, QList<QVariant> &dockIDs)
{
    if (window < m_rootItem->childCount())
    {
        layoutItem* windowItem = m_rootItem->child(window);

        size = windowItem->data(WC_SIZE).toSize();
        pos = windowItem->data(WC_POS).toPoint();
        bFloating = windowItem->data(WC_FLOATING).toBool();
        area = (Qt::DockWidgetArea)windowItem->data(WC_DOCKAREA).toInt();
        dockIDs = windowItem->data(WC_DOCKIDS).toList();
    }
}

/// @author Holly Ausbeck
/// @date   June 29, 2015
/// @brief  save position data for the main window
/// @param [in] size - window width and height
/// @param [in] pos - window position relative to the main window
/// @param [in] bMaximized - true if the window is maximized
void LayoutModel::saveMainWindowData(QSize size, QPoint pos, bool bMaximized)
{
    if (VIEWWINDOW_MAIN < m_rootItem->childCount())
    {
        layoutItem* windowItem = m_rootItem->child(VIEWWINDOW_MAIN);

        windowItem->setData(WC_SIZE, QVariant(size));
        windowItem->setData(WC_POS, QVariant(pos));
        windowItem->setData(WC_VISIBLE, QVariant(bMaximized));
    }
}

/// @author Holly Ausbeck
/// @date   June 29, 2015
/// @brief  retrieve position data for the main window
/// @param [out] size - window width and height
/// @param [out] pos - window position relative to the main window
/// @param [out] bVisible - true if the window is visible
/// @param [out] bFloating - true if the window is floating
void LayoutModel::getMainWindowData(QSize &size, QPoint &pos, bool &bMaximized)
{
    if (VIEWWINDOW_MAIN < m_rootItem->childCount())
    {
        layoutItem* windowItem = m_rootItem->child(VIEWWINDOW_MAIN);

        size = windowItem->data(WC_SIZE).toSize();
        pos = windowItem->data(WC_POS).toPoint();
        bMaximized = windowItem->data(WC_VISIBLE).toBool();
    }
}

/// @author Holly Ausbeck
/// @date   July 10, 2015
/// @brief  retrieve all the dock widgets docked in dock area
/// @param [in] dock area = Qt::LeftDockWidgetArea, Qt::RightDockWidgetArea, Qt::BottomDockWidgetArea
/// @param [out] pages - dock widget ID list of dock widgets allowed on page
QList<int> LayoutModel::getDockedWindows(Qt::DockWidgetArea &area, WizardPage page)
{
    QList<int> dockedWindows;

    for (int i = 0; i < VIEWWINDOW_COUNT; i++)
    {
        if (isWindowAllowed(page, (ViewWindows)i))
        {
            layoutItem* windowItem = m_rootItem->child(i);

            Qt::DockWidgetArea a = (Qt::DockWidgetArea)windowItem->data(WC_DOCKAREA).toInt();

            if (a == area)
            {
                QList<QVariant> dockIDs = windowItem->data(WC_DOCKIDS).toList();

                if ((dockIDs.count() == 0) || isSelected((ViewWindows)i))
                    dockedWindows.append(i);
            }
        }
    }
    return dockedWindows;
}

/// @author Holly Ausbeck
/// @date   July 10, 2015
/// @brief  retrieve all the dock widgets. Don't include tabbed widgets that aren't selected
/// @param [in] page - wizard page
/// @param [out] dockedWindows - dock widget ID list of dock widgets allowed on page
QList<int> LayoutModel::getDockedWindows(WizardPage page)
{
    QList<int> dockedWindows;

    for (int i = 0; i < VIEWWINDOW_COUNT; i++)
    {
        if (isWindowAllowed(page, (ViewWindows)i))
        {
            layoutItem* windowItem = m_rootItem->child(i);
            QList<QVariant> dockIDs = windowItem->data(WC_DOCKIDS).toList();

            if ((dockIDs.count() == 0) || isSelected((ViewWindows)i))
                dockedWindows.append(i);
        }
    }
    return dockedWindows;
}

/// @author Holly Ausbeck
/// @date   July 10, 2015
/// @brief  retrieve visible dock widgets for page
/// @param [in] WizardPage
/// @param [out] pages - dock widget ID list of dock widgets visible on page
QList<int> LayoutModel::getVisibleWindows(WizardPage page)
{
    QList<int> visibleWindows;

    for (int i = 0; i < VIEWWINDOW_COUNT; i++)
    {
        layoutItem* windowItem = m_rootItem->child(i);

        QList<QVariant> visiblePages = windowItem->data(WC_ALLOWED).toList();

        if (visiblePages.contains(QVariant(page)))
        {
            visibleWindows.append(i);
        }
    }
    return visibleWindows;
}

/// @author Holly Ausbeck
/// @date   Aug 26, 2015
/// @brief  return the ID of the given dock widget
/// @param [in] objectName is the name of the QDockWidgt
/// @param [out] ID
int LayoutModel::getID(QString objectName)
{
    for (int i = 0; i < VIEWWINDOW_COUNT; ++i)
    {
        if (objectName == WindowNames[i])
            return i;
    }
    return -1;
}

/// @author Holly Ausbeck
/// @date   July 10, 2015
/// @brief  return true if the window is selected
/// @param [in] window
/// @param [out] true if seleced
bool LayoutModel::isSelected(ViewWindows window)
{
    if (window < m_rootItem->childCount())
    {
        layoutItem* windowItem = m_rootItem->child(window);

        return windowItem->data(WC_SELECTED).toBool();
    }
    return false;
}


/// @author Holly Ausbeck
/// @date   July 10, 2015
/// @brief  return the area the window is docked on
/// @param [in] window
Qt::DockWidgetArea LayoutModel::getArea(ViewWindows window)
{
    Qt::DockWidgetArea area = Qt::LeftDockWidgetArea;

    if (window < m_rootItem->childCount())
    {
        layoutItem* windowItem = m_rootItem->child(window);

        area = (Qt::DockWidgetArea)windowItem->data(WC_DOCKAREA).toInt();
    }
    return area;
}


/// @author Holly Ausbeck
/// @date   July 10, 2015
/// @brief  return true if the window is allowed on the page
/// @param [in] page
/// @param [in] window
bool LayoutModel::isWindowAllowed(WizardPage page, ViewWindows window)
{
    if (window < m_rootItem->childCount())
    {
        layoutItem* windowItem = m_rootItem->child(window);

        QList<QVariant> allowedPages = windowItem->data(WC_ALLOWED).toList();

        if (allowedPages.contains(QVariant(page)))
        {
            return true;
        }
    }
    return false;
}

/// @author Holly Ausbeck
/// @date   July 10, 2015
/// @brief  return true if the window is visible on the page
/// @param [in] page
/// @param [in] window
bool LayoutModel::isWindowVisible(WizardPage page, ViewWindows window)
{
    if (window < m_rootItem->childCount())
    {
        layoutItem* windowItem = m_rootItem->child(window);

        QList<QVariant> visiblePages = windowItem->data(WC_VISIBLE).toList();

        if (visiblePages.contains(QVariant(page)))
        {
            return true;
        }
    }
    return false;
}

/// @author Holly Ausbeck
/// @date   July 10, 2015
/// @brief  save the visiblity status of the window on the page
/// @param [in] window
/// @param [in] page
/// @param [in] bVisible
void LayoutModel::saveVisible(ViewWindows window, WizardPage page, bool bVisible, bool bSelected, bool allPages)
{
    if (window < m_rootItem->childCount())
    {
        layoutItem* windowItem = m_rootItem->child(window);
        QList<QVariant> visiblePages;

        if (allPages)
        {
            visiblePages = windowItem->data(WC_ALLOWED).toList();

            if (bVisible)
                visiblePages = windowItem->data(WC_ALLOWED).toList();
            else
                visiblePages.clear();
        }
        else
        {
            visiblePages = windowItem->data(WC_VISIBLE).toList();

            if (visiblePages.contains(QVariant(page)))
            {
                if (!bVisible)
                    visiblePages.removeAll(QVariant(page));
            }
            else
            {
                if (bVisible)
                    visiblePages.append(QVariant(page));
            }
        }
        windowItem->setData(WC_VISIBLE, QVariant(visiblePages));
        windowItem->setData(WC_SELECTED, QVariant(bSelected));
    }
}

/// @author Holly Ausbeck
/// @date   Aug 26, 2015
/// @brief  Set the geometry on all the windows in dockIDs to that of the selected dock
void LayoutModel::updateSizes()
{
    for (int i = 0; i < VIEWWINDOW_COUNT; ++i)
    {
        if (isSelected((ViewWindows)i))
        {
            layoutItem* selectedItem = m_rootItem->child(i);
            QSize size = selectedItem->data(WC_SIZE).toSize();
            QPoint pos = selectedItem->data(WC_POS).toPoint();
            QList<QVariant> dockIDs = selectedItem->data(WC_DOCKIDS).toList();

            foreach(QVariant v, dockIDs)
            {
                int id = v.toInt();

                if (id < m_rootItem->childCount())
                {
                    layoutItem* item = m_rootItem->child(id);

                    item->setData(WC_SIZE, QVariant(size));
                    item->setData(WC_POS, QVariant(pos));
                }
            }
        }
    }
}

/// @author Holly Ausbeck
/// @date   Aug 26, 2015
/// @brief  Set the geometry on all the windows in dockIDs to that of the selected dock
void LayoutModel::updateTopSizes(int diff)
{
    for (int i = 0; i < VIEWWINDOW_COUNT; ++i)
    {
        if ((getArea((ViewWindows)i) == Qt::TopDockWidgetArea) || (getArea((ViewWindows)i) == Qt::RightDockWidgetArea))
        {
            layoutItem* windowItem = m_rootItem->child(i);
            QSize size = windowItem->data(WC_SIZE).toSize();
            size.setHeight(size.height() + diff);
            windowItem->setData(WC_SIZE, QVariant(size));
        }
    }
}

/// @author Holly Ausbeck
/// @date   July 10, 2015
/// @brief  initialize allowed pages and initialize visible pages for the given dock widget
/// @param [in] window - dock widget ID
/// @param [in] pages - bool QVariant list for all WizardPages. true = window allowed on page
void LayoutModel::initAllowedPages(ViewWindows window, QList<QVariant> pages)
{
    if (window < m_rootItem->childCount())
    {
        layoutItem* windowItem = m_rootItem->child(window);

        windowItem->setData(WC_ALLOWED, QVariant(pages));
        windowItem->setData(WC_VISIBLE, QVariant(pages));
    }
}

