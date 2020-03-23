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

#ifndef DOCKMAP_H_INCLUDED
#define DOCKMAP_H_INCLUDED

#include <QMap>
#include <QList>
#include <QRect>
#include <QPoint>

class DockMap 
{
public:
    DockMap();
    DockMap(QMap<int, QRect> rects);
    ~DockMap();

    void setRects(QMap<int, QRect> rects);
    int count() { return m_rects.count();  }
    int topLeftDockID();

    int findRightSplit();
    int findTopSplit();

    void splitHorizontal(DockMap &leftMap, DockMap &rightMap, int split);
    void splitVertical(DockMap &topMap, DockMap &bottomMap, int split);

private:
    // variables initialized in constructor
    QMap<int, QRect> m_rects;
    QRect m_bounds;

    int findDockID(QPoint topLeft);

    bool isRightSplit(int split, int height);
    bool isTopSplit(int split, int width);
    bool findRect(QPoint topLeft, QRect &rect);
    QRect findBoundingRectangle(QMap <int, QRect> &rects);
    QRect findBoundingRectangle(QList <QRect> &rects);
};

#endif // DOCKMAP_H_INCLUDED  
