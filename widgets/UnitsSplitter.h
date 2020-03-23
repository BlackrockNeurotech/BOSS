#ifndef UNITS_SPLITTER_H
#define UNITS_SPLITTER_H

#include <QSplitter>

#include "../model/channelconfigmodel.h"
#include "../graphics/unitgraphicsview.h"


class UnitsSplitter : public QSplitter
{
    Q_OBJECT

public:
    UnitsSplitter(QWidget *parent = 0);

public slots:
    void slotChannelChanged(int channel);
    void slotUnitsChanged(int channel);

private:
    void removeUnits(int nUnits);
};


#endif // UNITS_SPLITTER_H
