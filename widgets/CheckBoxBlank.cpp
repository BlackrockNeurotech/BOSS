
#include "../widgets/CheckBoxBlank.h"


CheckBoxBlank::CheckBoxBlank(QWidget *parent) : QCheckBox(parent)
{
    m_currentSelection = "";

    m_mapper = new QDataWidgetMapper(parent);
    m_mapper->setOrientation(Qt::Horizontal);
    m_mapper->setModel(ChannelConfigModel::instance());
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
    connect(this, SIGNAL(stateChanged(int)), this, SLOT(slotValueChanged(int)));
}

CheckBoxBlank::~CheckBoxBlank()
{
    delete m_mapper;
}

void CheckBoxBlank::setChannelColumn(int column)
{
    m_mapper->addMapping(this, column, "currentSelection");
    connect(this, SIGNAL(selectionChanged(QString)), m_mapper, SLOT(submit()));

    blockSignals(true);
    m_mapper->toFirst();
    blockSignals(false);
    setUpdatesEnabled(true);
}

QString CheckBoxBlank::currentSelection() const
{
    return m_currentSelection;
}


void CheckBoxBlank::setCurrentSelection(QString selection)
{
    // if the selection id is blank, this control reflects the state of multiple channels that differ. 
    // Clear the combo box.
    blockSignals(true);

    if (selection.isEmpty())
    {
        m_currentSelection = "";
        QCheckBox::setTristate(true);
        QCheckBox::setCheckState(Qt::PartiallyChecked);
        QCheckBox::update();
    }
    else
    {
        m_currentSelection = selection;
        bool state = (selection == "true");
        QCheckBox::setChecked(state);
        QCheckBox::setTristate(false);
        QCheckBox::update();
    }
    blockSignals(false);
    update();
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  the current channel reflected by this control has changed. 
/// @param [in] new channel
void CheckBoxBlank::slotChannelChanged(int channel)
{
    blockSignals(true);
    m_mapper->setCurrentIndex(channel);
    blockSignals(false);
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  the value in this control has changed. 
/// @param [in] new value
void CheckBoxBlank::slotValueChanged(int value)
{
    m_currentSelection = QString::number(value);
    emit selectionChanged(m_currentSelection);
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  update the model with the current selection - used by apply options to all selected channels
void CheckBoxBlank::submit()
{
    m_mapper->submit();
}
