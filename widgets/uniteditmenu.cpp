#include "uniteditmenu.h"
#include "../model/configmodel.h"
#include "../backend/dataprocessing/common/unitassignment.h"



UnitEditMenu::UnitEditMenu(QWidget *parent, int selectedUnit) :
    QMenu(parent)
{
    m_parent = parent;
    m_selectedUnit = selectedUnit;
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    m_usedUnits.clear();

    int  nUnits = 0;
  

    ChannelConfigModel * ccm = ChannelConfigModel::instance();
    m_channel = ccm->CurrentChannel();
    UnitAccum unitData = ccm->data(ccm->index(m_channel, CC_SPIKEDATA_UNITS), Qt::UserRole).value<UnitAccum>();
    
    if (unitData)
    {
        QVector<int> usedUnits = unitData->getUnitNumbers();
        foreach(int u, usedUnits)
            if (u != 0)
                m_usedUnits.push_back(u);

        nUnits = m_usedUnits.size();
    }
    
    setStyleSheet("QMenu {margin: 0px}");

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    QList<QColor> colors = ConfigModel::instance()->getColors();
    for (int i = 0; i < colors.size(); ++i)
    {
        if ((i == 0) || m_usedUnits.contains(i))
            m_unitColors.push_back(colors[i]);
    }

    QColor color;
    int k = 0;


    // "Invalidate" button *****************************************
    color.setNamedColor("#FFFFFF");
    m_actions.push_back(new ActionItem(parent, k, "Invalidate", "invalidate", color));
    k++;
    //****************************************************************

    // "Delete" button *****************************************
    color.setNamedColor("#FFFFFF");
    m_actions.push_back(new ActionItem(parent, k, "Delete", "delete", color));
    k++;
    //****************************************************************

    for(int i(0); i < m_actions.size(); ++i)
    {
        this->addAction(m_actions[i]);
        QObject::connect(m_actions[i], SIGNAL(triggered(bool, int, QString)), this, SLOT(slotActionSelected(bool, int, QString)));
    }


    // "Combine with..." menu *********************************************
    color.setNamedColor("#FFFFFF");
    if (!m_usedUnits.isEmpty())
    {
        QMenu * combineMenu = new QMenu(this);
        combineMenu->setStyleSheet("QMenu{border:transparent;}");
        combineMenu->setMouseTracking(true);
        combineMenu->setFocusPolicy(Qt::StrongFocus);


        ActionItem *combineAction = new ActionItem(parent, 0, "Combine With...", "combinewith", color);
        for (int i(0); i < nUnits; ++i)
        {
            if (m_usedUnits[i] != selectedUnit)
            {
                if (m_usedUnits[i] < colors.size())
                    color = colors[m_usedUnits[i]];
                else
                    color.setNamedColor("darkgray");

                m_combineMenuActions.push_back(new ActionItem(combineMenu, i, "Unit " + QString::number(m_usedUnits[i]), "combinewith" + QString::number(m_usedUnits[i]), color));
            }
        }
        //****************************************************************
        for (int i(0); i < m_combineMenuActions.size(); ++i)
        {
            combineMenu->addAction(m_combineMenuActions[i]);
            QObject::connect(m_combineMenuActions[i], SIGNAL(triggered(bool, int, QString)), this, SLOT(slotUpdateUnit(bool, int, QString)));
        }
        combineAction->setMenu(combineMenu);
        this->addAction(combineAction);
    }

}

UnitEditMenu::~UnitEditMenu()
{
    foreach (ActionItem *action, m_actions)
        delete action;
    foreach(ActionItem *action, m_combineMenuActions)
        delete action;
}


void UnitEditMenu::slotActionSelected(bool checked, int action_index, QString name)
{
    Q_UNUSED(checked);
    UnitAssignment unitAssignment;

    if (name == "invalidate")
    {
        unitAssignment.assign(m_selectedUnit, EDITMODE_SELECT, UNITOPERATION_INVALIDATE, m_selectedUnit);
    }
    else if (name == "delete")
    {
        unitAssignment.assign(0, EDITMODE_SELECT, UNITOPERATION_DELETE, m_selectedUnit);
    }
    this->hide();
}

void UnitEditMenu::slotUpdateUnit(bool checked, int idx, QString name)
{
    Q_UNUSED(checked);
    int firstUnit = m_selectedUnit;
    int secondUnit = m_usedUnits[idx]; 
    int keepUnit, deleteUnit;
    

    if (firstUnit < secondUnit && (firstUnit != 0))
    {
        keepUnit = firstUnit;
        deleteUnit = secondUnit;
    }
    else
    {
        keepUnit = secondUnit;
        deleteUnit = firstUnit;
    }

    UnitAssignment unitAssignment;
    unitAssignment.assign(keepUnit, EDITMODE_SELECT, UNITOPERATION_DELETE, deleteUnit);
    close();
}


