#include <QRadioButton>
#include <QCheckBox>
#include "../model/mapperwidgets/checkradiogroup.h"

CheckRadioGroup::CheckRadioGroup(QWidget *parent, ChannelColumn channelColumn) :
    QWidget(parent)
{
    m_currentSelection = "";
    m_checkbox = 0;
    m_defaultSelection = "";
    m_noSelection = "";

    m_mapper = new QDataWidgetMapper(parent);
    m_mapper->setOrientation(Qt::Horizontal);
    m_mapper->setModel(ChannelConfigModel::instance());
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    m_mapper->addMapping(this, channelColumn, "currentSelection");
    connect(this, SIGNAL(selectionChanged(QString)), m_mapper, SLOT(submit()));

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));

    blockSignals(true);
    m_mapper->toFirst();
    blockSignals(false);
}

CheckRadioGroup::~CheckRadioGroup()
{
    delete m_mapper;
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  This is the READ accessor function for the currentSelection property. 
/// Return the current selection
QString CheckRadioGroup::currentSelection() const
{
    return m_currentSelection;
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  This is the WRITE accessor function for the currentSelection property. 
/// Change the current selection
void CheckRadioGroup::setCurrentSelection(QString selection)
{
    // if the selection id is blank, this control reflects the state of multiple channels that differ. 
    // Set the check box to partially checked, and clear the radio buttons.
    if (selection.isEmpty())    
    {
        setChannelsDiffer();
    }
    else
    {
        QMap<QString, QRadioButton*>::iterator i = m_buttonMap.find(selection);

        // if the selection id is not in our button map, it's the no selection state
        // clear the checkbox
        if (i == m_buttonMap.end())
        {
            if (selection == m_noSelection)
            {
                m_currentSelection = selection;
                m_checkbox->setChecked(false);
                m_checkbox->setTristate(false);
                m_checkbox->update();
                enableGroup(false);
            }
        }
        else
        {
            // this control reflects the state of one or more channels that are the same. 
            // set the current state.
            m_currentSelection = selection;
            i.value()->setChecked(true);
            m_checkbox->setChecked(true);
            m_checkbox->setTristate(false);
            m_checkbox->update();
            enableGroup(true);
        }
    }
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  Set the checkbox for our checkbox/radio control
/// @param [in] pointer to the check box control
/// @param [in] string value of the default radio id to set when the checkbox is checked
/// @param [in] string value representing the unchecked state of the checkbox
void CheckRadioGroup::setCheckBox(QCheckBox *button, QString checked, QString unchecked)
{
    m_checkbox = button;
    m_defaultSelection = checked;
    m_noSelection = unchecked;
    // listen for signals from the checkbox
    connect(button, SIGNAL(clicked(bool)), this, SLOT(slotGroupClicked(bool)));
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  Add a radio button to our checkbox/radio control
/// @param [in] pointer to the radio button control
/// @param [in] string value the radio button represents
void CheckRadioGroup::addRadioButton(QRadioButton* button, QString id)
{
   if (id.size() != 0)
    {
        // Add the radio button to our button map
        m_buttonMap[id] = button;
        // listen for signals from this radio button
        connect(button, SIGNAL(clicked(bool)), this, SLOT(slotClicked(bool)));
    }
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  the current channel reflected by this control has changed. 
/// @param [in] new channel
void CheckRadioGroup::slotChannelChanged(int channel)
{
    m_mapper->setCurrentIndex(channel);
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  the user clicked one of the radio buttons
/// @param [in] check state = true/false
void CheckRadioGroup::slotClicked(bool checked)
{
    // two signals come in, one for the old button to false, and one for the new button to true
    // only process the true case
    if (checked == true)
    {
        QRadioButton* btn = qobject_cast<QRadioButton*>(sender());
        Q_ASSERT(btn);

        QString selection = m_buttonMap.key(btn);

        if (m_currentSelection != selection)
        {
            m_checkbox->setTristate(false);
            m_checkbox->setChecked(true);
            m_currentSelection = selection;
            emit selectionChanged(m_currentSelection);
        }
    }
    else
    {
        QRadioButton* btn = qobject_cast<QRadioButton*>(sender());
        Q_ASSERT(btn);
        btn->setChecked(false);
    }
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  the user changed the state of the checkbox 
/// @param [in] check state = true/false
void CheckRadioGroup::slotGroupClicked(bool checked)
{
    QCheckBox* btn = qobject_cast<QCheckBox*>(sender());
    Q_ASSERT(btn);
    m_checkbox->setTristate(false);
    m_checkbox->setChecked(checked);
    m_currentSelection = enableGroup(checked);
    emit selectionChanged(m_currentSelection);
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  the user clicked the checkbox, set current selection and enable/disable the radio buttons
QString CheckRadioGroup::enableGroup(bool enable)
{
    QString currentSelection = m_currentSelection;

    if (enable == true)
    {
        enableRadioButtons(true);

        if (currentSelection.isEmpty() || (currentSelection == m_noSelection))
            currentSelection = m_defaultSelection;
    }
    else
    {
        enableRadioButtons(false);
        if (currentSelection != m_noSelection)
            m_defaultSelection = currentSelection;
        currentSelection = m_noSelection;
    }
    return currentSelection;
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  Set the check box to partially checked, and clear the radio buttons to reflect the state of multiple channels that differ.
void CheckRadioGroup::setChannelsDiffer()
{
    m_currentSelection = m_noSelection;

    if (m_checkbox != 0)
    {
        m_checkbox->setTristate(true);
        m_checkbox->setCheckState(Qt::PartiallyChecked);
        m_checkbox->update();
    }
    enableRadioButtons(false);
    clearRadioButtons();
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  enable/disable the radio buttons
void CheckRadioGroup::enableRadioButtons(bool enable)
{
    QMap<QString, QRadioButton*>::iterator i;

    for (i = m_buttonMap.begin(); i != m_buttonMap.end(); ++i)
    {
        i.value()->setEnabled(enable);
        i.value()->update();
    }
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  clear the radio buttons to reflect the state of multiple channels that differ
void CheckRadioGroup::clearRadioButtons()
{
    QMap<QString, QRadioButton*>::iterator i;

    for (i = m_buttonMap.begin(); i != m_buttonMap.end(); ++i)
    {
        i.value()->setAutoExclusive(false);
        i.value()->setChecked(false);
        i.value()->setAutoExclusive(true);
        i.value()->update();
    }
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  update the model with the current selection - used by apply options to all selected channels
void CheckRadioGroup::submit()
{
    m_mapper->submit();
}
