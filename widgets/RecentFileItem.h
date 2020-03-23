#ifndef RECENTFILEITEM_H
#define RECENTFILEITEM_H
#include <QAction>


class RecentFileItem :
    public QAction
{
    Q_OBJECT

public:
    RecentFileItem(const QString & text, QObject * parent);
    ~RecentFileItem();

public slots:
void slotTriggered(bool checked);

signals:
void triggered(QString filename);
};

#endif //RECENTFILEITEM_H