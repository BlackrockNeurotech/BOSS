/////////////////////////////////////////////////////////////////
///
/// @file dockMap.h
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  Aug 26, 2015
///
/// @brief DockMap is responsible for saving and restoring the layout of the dock widgets
///
/// @note
///

#include "../ProjectFiles/dockMap.h"

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  Default constructor for DockMap class
/// @param [in]
DockMap::DockMap()
{
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  Constructor for DockMap class
/// @param [in]
DockMap::DockMap(QMap<int, QRect> rects)
{
    setRects(rects);
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  Destructor for DockMap class - deletes contents of Queue
DockMap::~DockMap()
{   
}

void DockMap::setRects(QMap<int, QRect> rects)
{
    m_rects = rects;
    m_bounds = findBoundingRectangle(m_rects);
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  if there is a right split, split the docks into two maps
/// @param [in]  map of dockIDs and corresponding rectangles
/// @param [out] list of docks in right split
int DockMap::topLeftDockID()
{
    return findDockID(m_bounds.topLeft());
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  find the top left dock widget
/// @param [in]  map of dockIDs and corresponding rectangles
/// @param [out] dockID of the top left dock widget
int DockMap::findDockID(QPoint topLeft)
{
    QMap<int, QRect>::const_iterator it = m_rects.constBegin();

    while (it != m_rects.constEnd())
    {
        QRect r = it.value();

        if ((r.left() == topLeft.x()) && (r.top() == topLeft.y()))
        {
            return it.key();
        }
        it++;
    }
    return -1;
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  if there is a right split, split the docks into two maps
/// @param [in]  map of dockIDs and corresponding rectangles
/// @param [out] list of docks in right split
void DockMap::splitHorizontal(DockMap &leftMap, DockMap &rightMap, int split)
{
    QMap <int, QRect> left;
    QMap <int, QRect> right;
    QMap<int, QRect>::const_iterator it = m_rects.constBegin();

    while (it != m_rects.constEnd())
    {
        QRect rect = it.value();

        if (rect.left() >= split)
        {
            right.insert(it.key(), it.value());
        }
        else
        {
            left.insert(it.key(), it.value());
        }
        it++;
    }
    leftMap.setRects(left);
    rightMap.setRects(right);
}

// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  if there is a right split, split the docks into two maps
/// @param [in]  map of dockIDs and corresponding rectangles
/// @param [out] list of docks in right split
void DockMap::splitVertical(DockMap &topMap, DockMap &bottomMap, int split)
{
    QMap <int, QRect> top;
    QMap <int, QRect> bottom;
    QMap<int, QRect>::const_iterator it = m_rects.constBegin();

    while (it != m_rects.constEnd())
    {
        QRect rect = it.value();

        if (rect.top() >= split)
        {
            bottom.insert(it.key(), it.value());
        }
        else
        {
            top.insert(it.key(), it.value());
        }
        it++;
    }
    topMap.setRects(top);
    bottomMap.setRects(bottom);
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  if there is a right split, split the docks into two maps
/// @param [in]  map of dockIDs and corresponding rectangles
/// @param [out] list of docks in right split
int DockMap::findRightSplit()
{
    QPoint topLeft = m_bounds.topLeft();
    QRect rect;

    while (findRect(topLeft, rect))
    {
        int right = rect.right();

        // if the top left rectangle has the same top right point as the bounding rectangle, then there is no right split
        if (right >= m_bounds.right())
            break;

        if (isRightSplit(right, m_bounds.height()))
            return right;

        topLeft = rect.topRight();
    }
    return -1;
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  if there is a right split, split the docks into two maps
/// @param [in]  map of dockIDs and corresponding rectangles
/// @param [out] list of docks in right split
int DockMap::findTopSplit()
{
    QPoint topLeft = m_bounds.topLeft();
    QRect rect;

    while (findRect(topLeft, rect))
    {
        int bottom = rect.bottom();

        // if the top left rectangle has the same top right point as the bounding rectangle, then there is no right split
        if (bottom >= m_bounds.bottom())
            break;

        if (isTopSplit(bottom, m_bounds.width()))
            return bottom;

        topLeft = rect.bottomLeft();
    }
    return -1;
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  if there is a right split, split the docks into two maps
/// @param [in]  map of dockIDs and corresponding rectangles
/// @param [out] list of docks in right split
bool DockMap::isRightSplit(int split, int height)
{
    QMap<int, QRect>::const_iterator it = m_rects.constBegin();
    QList <QRect> rightAtSplit;

    // find all the rectangles that have right() == split
    while (it != m_rects.constEnd())
    {
        QRect rect = it.value();

        if (split == rect.right())
        {
            rightAtSplit.push_back(rect);
        }
        it++;
    }
    if (!rightAtSplit.isEmpty())
    {
        QRect splitBounds = findBoundingRectangle(rightAtSplit);

        if (splitBounds.height() == height)
            return true;
    }
    return false;
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  if there is a right split, split the docks into two maps
/// @param [in]  map of dockIDs and corresponding rectangles
/// @param [out] list of docks in right split
bool DockMap::isTopSplit(int split, int width)
{
    QMap<int, QRect>::const_iterator it = m_rects.constBegin();
    QList <QRect> bottomAtSplit;

    // find all the rectangles that have right() == split
    while (it != m_rects.constEnd())
    {
        QRect rect = it.value();

        if (split == rect.bottom())
        {
            bottomAtSplit.push_back(rect);
        }
        it++;
    }
    if (!bottomAtSplit.isEmpty())
    {
        QRect splitBounds = findBoundingRectangle(bottomAtSplit);

        if (splitBounds.width() == width)
            return true;
    }
    return false;
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  find the top left dock widget
/// @param [in]  map of dockIDs and corresponding rectangles
/// @param [out] dockID of the top left dock widget
bool DockMap::findRect(QPoint topLeft, QRect &rect)
{
    QMap<int, QRect>::const_iterator it = m_rects.constBegin();

    while (it != m_rects.constEnd())
    {
        QRect r = it.value();

        if ((r.left() == topLeft.x()) && (r.top() == topLeft.y()))
        {
            rect = r;
            return true;
        }
        it++;
    }
    return false;
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  find rectangle bounding the given rectangles.
/// @param [in]  map of dockIDs and corresponding rectangles
/// @param [out] QRect bounding rectangle
QRect DockMap::findBoundingRectangle(QMap <int, QRect> &rects)
{
    if (rects.isEmpty())
        return QRect();

    QMap<int, QRect>::const_iterator it = rects.constBegin();
    QRect boundingRect = it.value();
    it++;

    while (it != rects.constEnd())
    {
        QRect rect = it.value();
        boundingRect = rect.united(boundingRect);
        it++;
    }
    return boundingRect;
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  find rectangle bounding the given rectangles. Note: this function assumes rectangles is not empty
/// @param [in]  map of dockIDs and corresponding rectangles
/// @param [out] QRect bounding rectangle
QRect DockMap::findBoundingRectangle(QList <QRect> &rects)
{
    QList<QRect>::const_iterator it = rects.constBegin();
    QRect boundingRect = *it;
    it++;

    while (it != rects.constEnd())
    {
        QRect rect = *it;
        boundingRect = rect.united(boundingRect);
        it++;
    }
    return boundingRect;
}

