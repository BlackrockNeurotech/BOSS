#include <QCheckBox>
#include "../model/mapperwidgets/checkboxgroup.h"

CheckBoxGroup::CheckBoxGroup(QWidget *parent, ChannelColumn channelColumn) :
    QWidget(parent)
{
    m_mapper = new QDataWidgetMapper(parent);
    m_mapper->setOrientation(Qt::Horizontal);
    m_mapper->setModel(ChannelConfigModel::instance());
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    m_mapper->addMapping(this, channelColumn, "currentSelection");
    connect(this, SIGNAL(selectionChanged(QList<QVariant>)), m_mapper, SLOT(submit()));

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));

    blockSignals(true);
    m_mapper->toFirst();
    blockSignals(false);
}

CheckBoxGroup::~CheckBoxGroup()
{
    delete m_mapper;
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  This is the READ accessor function for the currentSelection property. 
/// Return the current selection
QList<QVariant> CheckBoxGroup::currentSelection() const
{
    return m_currentSelection;
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  This is the WRITE accessor function for the currentSelection property. 
/// Change the current selection
void CheckBoxGroup::setCurrentSelection(QList<QVariant> selection)
{
    // If the specified selection id is not in our button map,
    // then it is invalid, return.  Otherwise,
    // add the selection to the ccurrentSelection string

    if (selection.size() == 2)
    {
        QMap<uint, QCheckBox*>::iterator i = m_buttonMap.begin();
        uint andMask = selection.at(0).toUInt();
        uint orMask = selection.at(1).toUInt();

        while (i != m_buttonMap.end())
        {
            uint key = i.key();

            if (andMask & key)
            {
                i.value()->setTristate(false);
                i.value()->setChecked(true);
                i.value()->update();
            }
            else if (orMask & key)
            {
                i.value()->setTristate(true);
                i.value()->setCheckState(Qt::PartiallyChecked);
                i.value()->update();
            }
            else
            {
                i.value()->setTristate(false);
                i.value()->setChecked(false);
                i.value()->update();
            }
            ++i;
        }
        m_currentSelection.clear();
        m_currentSelection.push_back(QVariant(andMask));
        m_currentSelection.push_back(QVariant(orMask));
    }
}

void CheckBoxGroup::setSelectionId(QCheckBox* button, uint id)
{
    // Make sure we got a valid Id (non-negative)
    // Also then listen for signals from this button
    if (id > 0)
    {
        m_buttonMap[id] = button;
        m_revButtonMap[button] = id;
        connect(button, SIGNAL(clicked(bool)), this, SLOT(slotClicked(bool)));
    }
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  the current channel reflected by this control has changed. 
/// @param [in] new channel
void CheckBoxGroup::slotChannelChanged(int channel)
{
    m_mapper->setCurrentIndex(channel);
}

void CheckBoxGroup::slotClicked(bool checked)
{
    QCheckBox* btn = qobject_cast<QCheckBox*>(sender());
    Q_ASSERT(btn);
    uint selection = m_revButtonMap[btn];

    if (checked == true)
    {
        addSelection(selection);
    }
    else
    {
        removeSelection(selection);
    }
    emit selectionChanged(m_currentSelection);
}

void CheckBoxGroup::addSelection(uint selection)
{
    if (m_currentSelection.size() == 2)
    {
        uint andMask = m_currentSelection.at(0).toUInt();
        uint orMask = m_currentSelection.at(1).toUInt();

        andMask |= selection;
        orMask |= selection;

        m_currentSelection.clear();
        m_currentSelection.push_back(QVariant(andMask));
        m_currentSelection.push_back(QVariant(orMask));
    }
}

void CheckBoxGroup::removeSelection(uint selection)
{
    if (m_currentSelection.size() == 2)
    {
        uint andMask = m_currentSelection.at(0).toUInt();
        uint orMask = m_currentSelection.at(1).toUInt();

        andMask &= ~selection;
        orMask &= ~selection;

        m_currentSelection.clear();
        m_currentSelection.push_back(QVariant(andMask));
        m_currentSelection.push_back(QVariant(orMask));
    }
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  update the model with the current selection - used by apply options to all selected channels
void CheckBoxGroup::submit()
{
    m_mapper->submit();
}
