/////////////////////////////////////////////////////////////////
///
/// @file layoutItem.cpp
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  July 8, 2015
///
/// @brief layoutItem is the base class for layout tree element.
///
/// @note
/// This class should be subclassed when creating a new layout tree element.
/// It is not abstract because it is used directly for the root item which contains no data.
/// It includes the basic functionallity that layout tree elements must implement.
/// It is used for traversing the layout tree.

#include "../model/layoutitem.h"

/// @note
/// Following are basic methods used for traversing the tree

/// @author Holly Ausbeck
/// @date   July 8, 2015
/// @brief  Constructor for layoutItem class
/// @param [in]  parent tree item
layoutItem::layoutItem(layoutItem *parent)
{
    m_parentItem = parent;
}

/// @author Holly Ausbeck
/// @date   July 8, 2015
/// @brief  Destructor for layoutItem class - deletes all child items
layoutItem::~layoutItem()
{
    qDeleteAll(m_childItems);
}

/// @author Holly Ausbeck
/// @date   July 8, 2015
/// @brief  append a child to this tree item
/// @param [in]  child tree item
void layoutItem::appendChild(layoutItem *item)
{
    m_childItems.append(item);
}

/// @author Holly Ausbeck
/// @date   July 8, 2015
/// @brief  get the child item corresponding to the row
/// @param [in]  row in this tree item
/// @return child item
layoutItem *layoutItem::child(int row)
{
    return m_childItems.value(row);
}

/// @author Holly Ausbeck
/// @date   July 8, 2015
/// @brief  remove the child item corresponding to the row
/// @param [in] row in this tree item
void layoutItem::removeChild(int row)
{
    m_childItems.removeAt(row);
}

/// @author Holly Ausbeck
/// @date   July 8, 2015
/// @brief  remove all children
void layoutItem::removeChildren()
{
    qDeleteAll(m_childItems);
}

/// @author Holly Ausbeck
/// @date   July 8, 2015
/// @brief  get the number of child items belonging to this tree item
/// @return child item count
int layoutItem::childCount() const
{
    return m_childItems.count();
}

/// @author Holly Ausbeck
/// @date   July 8, 2015
/// @brief  get the parent of this tree item
/// @return parent
layoutItem *layoutItem::parentItem()
{
    return m_parentItem;
}

/// @author Holly Ausbeck
/// @date   July 8, 2015
/// @brief  get the row this item corresponds to in it's parent
/// @return row
int layoutItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<layoutItem*>(this));

    return 0;
}

