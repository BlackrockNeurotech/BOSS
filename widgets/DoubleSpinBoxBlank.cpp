
#include "../widgets/DoubleSpinBoxBlank.h"


DoubleSpinBoxBlank::DoubleSpinBoxBlank(QWidget *parent) : QDoubleSpinBox(parent)
{
    m_currentSelection = "";

    m_mapper = new QDataWidgetMapper(parent);
    m_mapper->setOrientation(Qt::Horizontal);
    m_mapper->setModel(ChannelConfigModel::instance());
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
    connect(this, SIGNAL(valueChanged(const QString)), this, SLOT(slotValueChanged(const QString)));
    connect(this, SIGNAL(editingFinished()), this, SLOT(slotEditingFinished()));
}

DoubleSpinBoxBlank::~DoubleSpinBoxBlank()
{
    delete m_mapper;
}

void DoubleSpinBoxBlank::setChannelColumn(int column)
{
    m_mapper->addMapping(this, column, "currentSelection");
    connect(this, SIGNAL(selectionChanged(QString)), m_mapper, SLOT(submit()));

    blockSignals(true);
    m_mapper->toFirst();
    blockSignals(false);
    setUpdatesEnabled(true);
}

QString DoubleSpinBoxBlank::currentSelection() const
{
    return m_currentSelection;
}


void DoubleSpinBoxBlank::setCurrentSelection(QString selection)
{
    // if the selection id is blank, this control reflects the state of multiple channels that differ. 
    // Clear the combo box.
    setUpdatesEnabled(true);
    blockSignals(true);

    if (selection.isEmpty())
    {
        m_currentSelection = "";
        clear();
    }
    else
    {
        m_currentSelection = selection;
        QDoubleSpinBox::setValue(selection.toDouble());
    }
    blockSignals(false);
    update();
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  the current channel reflected by this control has changed. 
/// @param [in] new channel
void DoubleSpinBoxBlank::slotChannelChanged(int channel)
{
    blockSignals(true);
    m_mapper->setCurrentIndex(channel);
    blockSignals(false);
}

void DoubleSpinBoxBlank::slotEditingFinished()
{
    m_currentSelection = QString::number(this->value());
    emit selectionChanged(m_currentSelection);
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  the value in this control has changed. 
/// @param [in] new value
void DoubleSpinBoxBlank::slotValueChanged(const QString &text)
{
    m_currentSelection = text;
    emit selectionChanged(m_currentSelection);
}

/// @author Holly Ausbeck
/// @date   March 25, 2016
/// @brief  update the model with the current selection - used by apply options to all selected channels
void DoubleSpinBoxBlank::submit()
{
    m_mapper->submit();
}
