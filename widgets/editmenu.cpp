#include "editmenu.h"
#include "../model/configmodel.h"
#include "../backend/dataprocessing/common/unitassignment.h"




EditMenu::EditMenu(QWidget *parent, EditMode editMode) :
    QMenu(parent)
{
    m_parent = parent;
    m_editMode = editMode;
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    m_usedUnits.clear();

    int  nUnits = 0;
    m_actionSelected = false;

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
    setTitle(QString("Select Unit"));

    QList<QColor> colors = ConfigModel::instance()->getColors();
    for (int i = 0; i < colors.size(); ++i)
    {
        if( (i == 0) || m_usedUnits.contains(i) )
            m_unitColors.push_back(colors[i]);
    }

    QColor color;
    int k = 0;

	FeaturesAccum featuresData = ccm->data(ccm->index(m_channel, CC_SPIKEDATA_FEATURES), Qt::UserRole).value<FeaturesAccum>();
	QVector<quint32> wfID = featuresData->getSelectionWfID();

    if (!wfID.isEmpty())
    {
        if (!m_usedUnits.isEmpty())
        {
            // "Unsorted" button *********************************
            color = colors[k];
            m_actions.push_back(new ActionItem(parent, k, "Unsorted", "unsorted", color));
            k++;
            //****************************************************************

        }
        // "Unit X" buttons **********************************************

        for (int i(0); i < nUnits; ++i)
        {
            if (m_usedUnits[i] < colors.size())
                color = colors[m_usedUnits[i]];
            else
                color.setNamedColor("darkgray");

            m_actions.push_back(new ActionItem(parent, k + i, "Unit " + QString::number(m_usedUnits[i]), "unit" + QString::number(m_usedUnits[i]), color));
        }
        k += nUnits;

        if (nUnits < colors.size())
        {
            // "Add unit" button *********************************************
            color.setNamedColor("#FFFFFF");
            m_actions.push_back(new ActionItem(parent, k, "Add Unit", "addunit", color));
            k++;
            //****************************************************************
        }

        // "Invalidate waveforms" button *********************************
        color.setNamedColor("#FFFFFF");
        m_actions.push_back(new ActionItem(parent, k, "Invalidate", "invalidate", color));
        k++;
        //****************************************************************

    }

    


    // "Delete unit" menu ********************************************* Not visible on Lines Mode on the Waveform View
    if (m_editMode == EDITMODE_SELECT)
    {
        color.setNamedColor("#FFFFFF");
        if (!m_usedUnits.isEmpty())
        {
            QMenu * deleteMenu = new QMenu(this);
            deleteMenu->setStyleSheet("QMenu{border:transparent;}");
            deleteMenu->setMouseTracking(true);
            deleteMenu->setFocusPolicy(Qt::StrongFocus);


            ActionItem *deleteAction = new ActionItem(parent, 0, "Delete Unit", "deleteunit", color);

            for (int i(0); i < nUnits; ++i)
            {
                if (m_usedUnits[i] < colors.size())
                    color = colors[m_usedUnits[i]];
                else
                    color.setNamedColor("darkgray");

                m_deleteMenuActions.push_back(new ActionItem(deleteMenu, i, "Unit " + QString::number(m_usedUnits[i]), "deleteunit" + QString::number(m_usedUnits[i]), color));
            }
            //****************************************************************
            //        deleteMenu->setDefaultAction(deleteAction);
            for (int i(0); i < m_deleteMenuActions.size(); ++i)
            {
                deleteMenu->addAction(m_deleteMenuActions[i]);
                QObject::connect(m_deleteMenuActions[i], SIGNAL(triggered(bool, int, QString)), this, SLOT(slotDeleteUnit(bool, int, QString)));
            }
            deleteAction->setMenu(deleteMenu);
            this->addAction(deleteAction);
        }

        // "Revalidate waveforms" button *********************************
        int nInvalidWaveforms = ccm->data(ccm->index(m_channel, CC_INVALID_WAVEFORMS), Qt::UserRole).toInt(); 
        if (nInvalidWaveforms != 0)
        {
            color.setNamedColor("#FFFFFF");
            m_actions.push_back(new ActionItem(parent, k, "Revalidate", "revalidate", color));
            k++;
        }
        //****************************************************************
    }

    for (int i(0); i < m_actions.size(); ++i)
    {
        this->addAction(m_actions[i]);
        QObject::connect(m_actions[i], SIGNAL(triggered(bool, int, QString)), this, SLOT(slotActionSelected(bool, int, QString)));
    }

}

EditMenu::~EditMenu()
{
    foreach (ActionItem *action, m_actions)
        delete action;
    foreach (ActionItem *action, m_deleteMenuActions)
        delete action;
}


void EditMenu::slotActionSelected(bool checked, int action_index, QString name)
{
    Q_UNUSED(checked);

    int unit = 0;
    ChannelConfigModel * ccm = ChannelConfigModel::instance();    
    UnitOperation op = UNITOPERATION_SETUNIT;

    if(name == "addunit")
    {
        QVector<int> usedUnits(m_usedUnits);
        qSort(usedUnits);
        unit = 1;
        if (!usedUnits.isEmpty())
        {
            for(int i(1); i <= usedUnits.last(); ++i )
            {
                if (!usedUnits.contains(i))
                {
                    unit = i;
                    break;
                }
                if(i == usedUnits.last())
                    unit = usedUnits.last() + 1;

            }
        }
    }
    else if (name == "invalidate")
    {
        unit = INVALID_WF;
        op = UNITOPERATION_INVALIDATE;
    }
    else if (name == "revalidate")
    {
        unit = 0;
        op = UNITOPERATION_REVALIDATE;
    }
    else if (action_index > 0)
    {
        // User is changing the unit assignment
        unit = m_usedUnits[action_index - 1];
    }

    UnitAssignment unitAssignment;
    unitAssignment.assign(unit, m_editMode, op);    

    this->hide();

	FeaturesAccum featuresData = ccm->data(ccm->index(m_channel, CC_SPIKEDATA_FEATURES), Qt::UserRole).value<FeaturesAccum>();
	featuresData->clearSelection();
	m_actionSelected = true;
}


void EditMenu::slotDeleteUnit(bool checked, int idx, QString name)
{
    Q_UNUSED(checked);
    int unit = 0;
    int oldUnit = m_usedUnits[idx];       

    UnitAssignment unitAssignment;
    unitAssignment.assign(unit, m_editMode, UNITOPERATION_DELETE, oldUnit);

    this->hide();

	ChannelConfigModel *ccm = ChannelConfigModel::instance();
	FeaturesAccum featuresData = ccm->data(ccm->index(m_channel, CC_SPIKEDATA_FEATURES), Qt::UserRole).value<FeaturesAccum>();
	featuresData->clearSelection();
	m_actionSelected = true;

    close();
}


