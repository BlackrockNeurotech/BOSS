#ifndef UNITEDITMENU_H
#define UNITEDITMENU_H

#include <QMenu>
#include <QLabel>
#include <QVector>
#include "../widgets/MenuItem.h"
#include "../backend/dataprocessing/spike/spikeproxymodel.h"
#include "../model/channelconfigmodel.h"


class UnitEditMenu : public QMenu
{
    Q_OBJECT

public:
    UnitEditMenu(QWidget * parent, int selectedUnit);
    ~UnitEditMenu();

private:
    QWidget                  *m_parent;
    QVector<ActionItem *>     m_actions;
    QVector<ActionItem *>     m_combineMenuActions;
    QVector<MenuPushButton *> m_labels;
    QList<QColor>             m_unitColors;
    QVector<int>              m_usedUnits;
    int                       m_channel;
    int                       m_selectedUnit;

public slots:
    void slotActionSelected(bool checked, int action_index, QString name);
    void slotUpdateUnit(bool checked, int idx, QString name);


};


#endif // UNITEDITMENU_H
