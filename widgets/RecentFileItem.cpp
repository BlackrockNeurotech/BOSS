#include "RecentFileItem.h"


RecentFileItem::RecentFileItem(const QString & text, QObject * parent) :
QAction(text, parent)
{
    connect(this, SIGNAL(triggered(bool)), this, SLOT(slotTriggered(bool)));
}


RecentFileItem::~RecentFileItem()
{
}


void RecentFileItem::slotTriggered(bool checked)
{
    Q_UNUSED(checked);
    QString str = text();
    emit triggered(str);
}