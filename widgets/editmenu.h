#ifndef EDITMENU_H
#define EDITMENU_H

#include <QMenu>
#include <QLabel>
#include <QVector>
#include "../widgets/MenuItem.h"
#include "../backend/dataprocessing/spike/spikeproxymodel.h"
#include "../model/channelconfigmodel.h"



class EditMenu : public QMenu
{
    Q_OBJECT

public:
    EditMenu(QWidget * parent, EditMode editMode = EDITMODE_SELECT);
    ~EditMenu();

    bool m_actionSelected;

private:
    QWidget                  *m_parent;
    EditMode                  m_editMode;
    QVector<ActionItem *>     m_actions;
    QVector<ActionItem *>     m_deleteMenuActions;
    QVector<MenuPushButton *> m_labels;
    QList<QColor>             m_unitColors;
    QVector<int>              m_usedUnits;
    int                       m_channel;

public slots:
    void slotActionSelected(bool checked, int action_index, QString name);
    void slotDeleteUnit(bool checked, int idx, QString name);


};


#endif // EDITMENU_H
