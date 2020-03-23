#include "EditAction.h"
#include "channelconfigmodel.h"
#include "../backend/dataprocessing/spike/spikeproxymodel.h"

//************ EDIT ACTION ******************//
EditAction::EditAction(int channel, void * params)
{
    m_type = EDITACTIONTYPE_NONE;
    m_channel = channel;
    m_params = params;
}

EditAction::~EditAction()
{

}

void EditAction::undo()
{
}

void EditAction::redo()
{

}
//*******************************************//

//************ EDIT ACTION ALIGN*************//
EditActionAlign::EditActionAlign(int channel, AlignParams * params) :
    EditAction(channel, (void *) params)
{
    m_type = EDITACTIONTYPE_ALIGN;
}

EditActionAlign::~EditActionAlign()
{
    AlignParams * params = (AlignParams *)m_params;
    delete params;
}

void EditActionAlign::undo()
{
    AlignParams * params = (AlignParams *) m_params;

    // Set model with last parameters
    ChannelConfigModel * ccm = ChannelConfigModel::instance();
    ccm->setData(ccm->index(m_channel, CC_WAVEFORM_ALIGNMENT), (AlignMode)(params->prevAlignTo), Qt::UserRole);
    ccm->setData(ccm->index(m_channel, CC_MAX_SHIFT), params->maxShift, Qt::UserRole);
    
    QStringList unitsString;
    foreach(int u, params->unitsToAlign)
        unitsString.push_back(QString::number(u));
    ccm->setData(ccm->index(m_channel, CC_UNITS_ALIGN), unitsString);

    // Call the appropriate slot in the backend
    SpikeProxyModel::instance()->slotAlignWaveforms(m_channel);
}

void EditActionAlign::redo()
{
    AlignParams * params = (AlignParams *)m_params;

    // Set model with last parameters
    ChannelConfigModel * ccm = ChannelConfigModel::instance();
    ccm->setData(ccm->index(m_channel, CC_WAVEFORM_ALIGNMENT), (AlignMode)(params->alignTo));
    ccm->setData(ccm->index(m_channel, CC_MAX_SHIFT), params->maxShift);

    QStringList unitsString;
    foreach(int u, params->unitsToAlign)
        unitsString.push_back(QString::number(u));
    ccm->setData(ccm->index(m_channel, CC_UNITS_ALIGN), unitsString);

    // Call the appropriate slot in the backend
    SpikeProxyModel::instance()->slotAlignWaveforms(m_channel);
}
//**********************************************//

//************ EDIT ACTION SET UNIT*************//
EditActionSetUnit::EditActionSetUnit(int channel, void * params) :
EditAction(channel, params)
{
    m_type = EDITACTIONTYPE_SETUNIT;
}

EditActionSetUnit::~EditActionSetUnit()
{

}

void EditActionSetUnit::undo()
{
    SpikeProxyModel::instance()->slotUndoSetUnit(m_channel);
}

void EditActionSetUnit::redo()
{
    // Undo and redo have the same functionality for setting the unit
    undo();
}
//*********************************************//

//************ EDIT ACTION MANAGER*************//

EditActionManager::EditActionManager()
{

}

EditActionManager::~EditActionManager()
{
    foreach(EditAction * action, undoStack)
        delete action;

    foreach(EditAction * action, redoStack)
        delete action;
}

void EditActionManager::addAction(EditAction * action)
{
    if (action->type() == EDITACTIONTYPE_SETUNIT)
        removeElementsFromStack();
    
    undoStack.push_back(action);
    
    foreach(EditAction * action, redoStack)
        delete action;
    redoStack.clear();
}

void EditActionManager::undo()
{
    if (undoStack.isEmpty() == false)
    {
        EditAction * action = undoStack.takeLast();
        action->undo();
        redoStack.push_back(action);
    }
}

void EditActionManager::redo()
{
    if (redoStack.isEmpty() == false)
    {
        EditAction * action = redoStack.takeLast();
        action->redo();
        undoStack.push_back(action);
    }
}

void EditActionManager::removeElementsFromStack()
{
    int index = 0;
    bool bSetUnitFound = false;
    for (index; index < undoStack.size(); ++index)
    {
        EditAction * action = undoStack[index];
        if (action->type() == EDITACTIONTYPE_SETUNIT)
        {
            bSetUnitFound = true;
            break;
        }
    }

    if (bSetUnitFound)
    {
        for (int i(0); i <= index; ++i)
            delete undoStack[i];
        
        undoStack.remove(0, index + 1);
    }
    
}