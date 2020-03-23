#include <QSpinBox>
#include <QMouseEvent>
#include <QLineEdit>

#include "../model/mapperwidgets/spinBoxGroup.h"


SpinBoxFunction::SpinBoxFunction(functionType fxn, qreal op, ChannelColumn channelColumn, int precision)
{
    m_fxn = fxn;
    m_multiplier = op;
    m_range = op;
    m_sValue = QString("0");
    m_columnMultiplier = channelColumn;
    m_precision = precision;
}

QString SpinBoxFunction::getValue()
{
    return  m_sValue;
}

bool SpinBoxFunction::setValue(QString sValue)
{
    if (m_sValue == sValue)
        return false;

    m_sValue = sValue;
    return true;
}

QString SpinBoxFunction::getBaseValue()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    qreal value = m_sValue.toFloat();

    switch (m_fxn)
    {
    case fxnBase:
        break;

    case fxnMultiply:
        value = value / m_multiplier;
        break;

    case fxnColumnMultiply:
        value = value / m_multiplier;
        value /= ccm->data(ccm->index(ccm->CurrentChannel(), m_columnMultiplier), Qt::UserRole).toInt();
        break;

    case fxnPercent:
        value = value * m_range;
        value /= 100.0;
        break;
    }
    return QString::number(value, 'f', m_precision);
}

void SpinBoxFunction::setBaseValue(QString sValue)
{
    qreal value = sValue.toFloat();

    switch (m_fxn)
    {
    case fxnBase:
        break;
    case fxnMultiply:
        value *= m_multiplier;
        break;
    case fxnColumnMultiply:
    {
        ChannelConfigModel *ccm = ChannelConfigModel::instance();
        int multiplier = ccm->data(ccm->index(ccm->CurrentChannel(), m_columnMultiplier), Qt::UserRole).toInt();
        value *= m_multiplier;
        value *= multiplier;
        break;
    }
    case fxnPercent:
        value = value * 100.0;
        value /= m_range;
        break;
    }
    m_sValue = QString::number(value, 'f', m_precision);
}


SpinBoxGroup::SpinBoxGroup(QWidget *parent, ChannelColumn channelColumn) :
    QWidget(parent), m_currentSelection("")
{
    m_currentSelection = "";
    m_bUpdateOnEditingFinished = false;

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

bool SpinBoxGroup::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (!m_spinMap.isEmpty())
        {
            SpinBoxFunction *fxn = m_spinMap.value(m_spinMap.firstKey());

            if (fxn)
            {
                m_currentSelection = fxn->getBaseValue();
                setValue(m_currentSelection);

                if (!m_bUpdateOnEditingFinished)
                {
                    emit selectionChanged(m_currentSelection);
                }
            }
        }
    }
    return false;
}

SpinBoxGroup::~SpinBoxGroup()
{
    delete m_mapper;
}

QString SpinBoxGroup::currentSelection() const
{
    return m_currentSelection;
}


void SpinBoxGroup::setCurrentSelection(QString selection)
{
    m_currentSelection = selection;

    if (!m_spinMap.isEmpty())
    {
        if (selection.isEmpty())
        {
            m_currentSelection = "";
            QMap<QAbstractSpinBox*, SpinBoxFunction*>::iterator i;

            for (i = m_spinMap.begin(); i != m_spinMap.end(); ++i)
                i.key()->clear();
        }
        else
        {
            SpinBoxFunction *fxn = m_spinMap.value(m_spinMap.firstKey());
            setValue(selection);
        }
    }
}

// set the spinbox and listen for signals from it
void SpinBoxGroup::addSpinBox(QAbstractSpinBox* spinBox, SpinBoxFunction::functionType fxn, qreal op, ChannelColumn channelColumn)
{
    // The spin box updates the model each time the value changes
    // emit the editingFinished signal when one of the spin boxes looses focus
    connect(spinBox, SIGNAL(editingFinished()), this, SLOT(slotEditingFinished()));

    QSpinBox *sb = qobject_cast<QSpinBox*>(spinBox);
    QDoubleSpinBox *dsb = qobject_cast<QDoubleSpinBox*>(spinBox);

    if (sb)
    {
        SpinBoxFunction *sbf = new SpinBoxFunction(fxn, op, channelColumn, 0);
        m_spinMap.insert(spinBox, sbf);

        connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged(int)));
    }
    else if (dsb)
    {
        int precision = dsb->decimals();
        SpinBoxFunction *sbf = new SpinBoxFunction(fxn, op, channelColumn, precision);
        m_spinMap.insert(spinBox, sbf);

        connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(slotValueChanged(double)));
    }
    spinBox->installEventFilter(this);
    QObjectList o_list = spinBox->children();
    for (int i = 0; i < o_list.length(); i++)
    {
        QLineEdit *cast = qobject_cast<QLineEdit*>(o_list[i]);
        if (cast)
            cast->installEventFilter(this);
    }
}

/// @author Holly Ausbeck
/// @date   Dec 29, 2016
/// @brief  don't update the model until Editing is finished
void SpinBoxGroup::updateOnEditingFinished(bool bUpdateOnEditingFinished)
{
    m_bUpdateOnEditingFinished = bUpdateOnEditingFinished;
}

/// @author Holly Ausbeck
/// @date   Feb 11, 2016
/// @brief  enable/disable the spinBoxes
/// @param [in] enable/disable
void SpinBoxGroup::enable(bool bEnable)
{
    QMap<QAbstractSpinBox*, SpinBoxFunction*>::iterator i;

    for (i = m_spinMap.begin(); i != m_spinMap.end(); ++i)
        i.key()->setEnabled(bEnable);
}


void SpinBoxGroup::slotChannelChanged(int channel)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    QModelIndex index = ccm->index(channel, 0);

    QMap<QAbstractSpinBox*, SpinBoxFunction*>::iterator i;

    for (i = m_spinMap.begin(); i != m_spinMap.end(); ++i)
        i.key()->blockSignals(true);
        
    blockSignals(true);

    m_mapper->setCurrentModelIndex(index);

    for (i = m_spinMap.begin(); i != m_spinMap.end(); ++i)
        i.key()->blockSignals(false);

    blockSignals(false);
}

void SpinBoxGroup::slotEditingFinished()
{
    QAbstractSpinBox* spinBox = qobject_cast<QAbstractSpinBox*>(sender());
    Q_ASSERT(spinBox);

    emit editingFinished(m_currentSelection);

    if (m_bUpdateOnEditingFinished)
    {
        emit selectionChanged(m_currentSelection);
    }
}

void SpinBoxGroup::slotValueChanged(double val)
{
    QAbstractSpinBox* spinBox = qobject_cast<QAbstractSpinBox*>(sender());
    Q_ASSERT(spinBox);

    if (updateSpinBoxes(spinBox))
    {
        if (!m_bUpdateOnEditingFinished)
        {
            emit selectionChanged(m_currentSelection);
        }
    }
}

void SpinBoxGroup::slotValueChanged(int val)
{
    QAbstractSpinBox* spinBox = qobject_cast<QAbstractSpinBox*>(sender());
    Q_ASSERT(spinBox);

    if (updateSpinBoxes(spinBox))
    {
        if (!m_bUpdateOnEditingFinished)
        {
            emit selectionChanged(m_currentSelection);
        }
    }
}

bool SpinBoxGroup::updateSpinBoxes(QAbstractSpinBox* spinBox)
{
    QString sValue = spinBox->text();

    SpinBoxFunction *fxn = m_spinMap[spinBox];

    if (fxn)
    {
        if (fxn->setValue(sValue))
        {
            QString sBaseValue = fxn->getBaseValue();

            if (m_currentSelection != sBaseValue)
            {
                m_currentSelection = sBaseValue;
                setValue(sBaseValue, spinBox);
                return true;
            }
        }
    }
    return false;
}

void SpinBoxGroup::setValue(QString sValue, QAbstractSpinBox* sender)
{
    QMap<QAbstractSpinBox*, SpinBoxFunction*>::iterator iter = m_spinMap.begin();

    while (iter != m_spinMap.end())
    {
        QAbstractSpinBox *spinBox = iter.key();

        if ((sender == NULL) || (spinBox != sender))
        {
            SpinBoxFunction *fxn = iter.value();

            fxn->setBaseValue(sValue);
            QString sv = fxn->getValue();

            QSpinBox *sb = qobject_cast<QSpinBox*>(spinBox);
            QDoubleSpinBox *dsb = qobject_cast<QDoubleSpinBox*>(spinBox);

            if (sb)
            {
                sb->blockSignals(true);
                sb->setValue(sv.toInt());
                sb->blockSignals(false);
            }
            else if (dsb)
            {
                dsb->blockSignals(true);
                double dv = sv.toDouble();
                dsb->setValue(dv);
                dsb->blockSignals(false);
            }
        }

        ++iter;
    }
}

void SpinBoxGroup::submit()
{
    m_mapper->submit();
}
