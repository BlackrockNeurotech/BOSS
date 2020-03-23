#include "SpinWidgetAction.h"


SpinWidgetAction::SpinWidgetAction(QObject *parent, ChannelColumn column, int min, int max, QString toolTip) :
    QWidgetAction(parent)
{
    m_channelColumn = column;
    m_min = min;
    m_max = max;
    m_toolTip = toolTip;

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setOrientation(Qt::Horizontal);
    m_mapper->setModel(ChannelConfigModel::instance());
    m_mapper->toFirst();

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
}

void SpinWidgetAction::setRange(int min, int max)
{
    m_min = min;
    m_max = max;

    QList<QWidget  *> widgets = this->createdWidgets();

    foreach(QWidget *w, widgets)
    {
        QSpinBox *p = (QSpinBox *)w;
        p->setMinimum(m_min);
        p->setMaximum(m_max);
    }
}

QWidget* SpinWidgetAction::createWidget(QWidget * parent)
{
    QSpinBox *w = new QSpinBox(parent);
    setOptions(w);
    MapToModel(w);

    QObject::connect(w, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged(int)));

    w->setMaximumHeight(22);
    w->show();
    return w;
}

void SpinWidgetAction::setOptions(QSpinBox *w)
{
    w->setToolTip(m_toolTip);
    w->setMinimum(m_min);
    w->setMaximum(m_max);
}

void SpinWidgetAction::MapToModel(QSpinBox *w)
{
    m_mapper->addMapping(w, m_channelColumn);
    connect(w, SIGNAL(valueChanged(int)), m_mapper, SLOT(submit()));

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    QModelIndex index = ccm->index(ccm->CurrentChannel(), 0);
    m_mapper->setCurrentModelIndex(index);
}

void SpinWidgetAction::slotValueChanged(int value)
{
    m_value = value;
    QList<QWidget  *> widgets = this->createdWidgets();

    foreach (QWidget *w, widgets)
    {
        QSpinBox *p = (QSpinBox *)w;
        p->setValue(m_value);
    }
}

void SpinWidgetAction::deleteWidget(QWidget *widget)
{
    m_mapper->removeMapping(widget);
    widget->deleteLater();
}

void SpinWidgetAction::slotChannelChanged(int channel)
{
    QModelIndex index = ChannelConfigModel::instance()->index(channel, 0);
    m_mapper->setCurrentModelIndex(index);
}




