#include <QRadioButton>

#include "../model/mapperwidgets/radiogroup.h"

RadioGroup::RadioGroup(QWidget *parent, ChannelColumn channelColumn) :
    QWidget(parent)
{
    m_currentSelection = "";
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

RadioGroup::~RadioGroup()
{
    delete m_mapper;
}

QString RadioGroup::currentSelection() const
{
    return m_currentSelection;
}


void RadioGroup::setCurrentSelection(QString selection)
{
    // if the selection id is blank, this control reflects the state of multiple channels that differ. 
    // Set the check box to partially checked, and clear the radio buttons.
    if (selection.isEmpty())
    {
        setChannelsDiffer();
    }
    else
    {
        // if the selection id is not in our button map, it's the no selection state

        QMap<QString, QRadioButton*>::iterator iter = m_buttonMap.find(selection);
        if (iter == m_buttonMap.end())
        {
            m_currentSelection = "";
            for (iter = m_buttonMap.begin(); iter != m_buttonMap.end(); ++iter)
                iter.value()->setChecked(false);
        }
        else
        {
            m_currentSelection = selection;
            iter.value()->setChecked(true);
            iter.value()->update();
        }
    }
}


void RadioGroup::addRadioButton(QRadioButton* button, QString id)
{
    // Make sure we got a valid Id (non-negative)
    // Also then listen for signals from this button
    if (id.size() != 0)
    {
        m_buttonMap[id] = button;
        connect(button, SIGNAL(clicked(bool)), this, SLOT(slotClicked(bool)));
    }
}

/// @author Holly Ausbeck
/// @date   Feb 11, 2016
/// @brief  enable/disable one radio button and disable/enable the rest
/// @param [in] radio button to enable/disale
/// @param [in] enable/disable
void RadioGroup::enableOne(QRadioButton *button, bool bEnable)
{
    QMap<QString, QRadioButton*>::iterator i;

    for (i = m_buttonMap.begin(); i != m_buttonMap.end(); ++i)
    {
        QRadioButton* rb = i.value();

        if (rb == button)
            rb->setEnabled(bEnable);
        else
            rb->setEnabled(!bEnable);
    }
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  the current channel reflected by this control has changed. 
/// @param [in] new channel
void RadioGroup::slotChannelChanged(int channel)
{
    m_mapper->setCurrentIndex(channel);
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  the user clicked one of the radio buttons
/// @param [in] check state = true/false
void RadioGroup::slotClicked(bool checked)
{
    // two signals come in, one for the old button to false, and one for the new button to true
    // only process the true case
    QString lastSelection = m_currentSelection;

    if (checked == true)
    {
        QRadioButton* btn = qobject_cast<QRadioButton*>(sender());
        Q_ASSERT(btn);

        m_currentSelection = m_buttonMap.key(btn);

        if (lastSelection != m_currentSelection)
            emit selectionChanged(m_currentSelection);
    }
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  Set the check box to partially checked, and clear the radio buttons to reflect the state of multiple channels that differ.
void RadioGroup::setChannelsDiffer()
{
    m_currentSelection = m_noSelection;
    clearRadioButtons();
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  clear the radio buttons to reflect the state of multiple channels that differ
void RadioGroup::clearRadioButtons()
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
void RadioGroup::submit()
{
    m_mapper->submit();
}
