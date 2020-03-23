#include "UnitSelectionAction.h"


UnitSelectionAction::UnitSelectionAction(QObject *parent, bool bUseVisibleUnits, ChannelColumn modelColumn):
    QWidgetAction(parent)
{
    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setOrientation(Qt::Horizontal);
    m_mapper->setModel(ChannelConfigModel::instance());
    m_mapper->toFirst();
    m_mainWidget = true;
    m_bUseVisibleUnits = bUseVisibleUnits;
    m_modelColumn = modelColumn;

    connect(ChannelConfigModel::instance(), SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
}


QWidget* UnitSelectionAction::createWidget(QWidget * parent)
{
    unitSelection *w = new unitSelection(parent, m_bUseVisibleUnits, m_mainWidget);
    m_mainWidget = false;
    MapToModel(w);
	w->setToolTip(toolTip());
    w->show();
    return w;
}

void UnitSelectionAction::MapToModel(unitSelection *w)
{
    m_mapper->addMapping(w, m_modelColumn);
    connect(w, SIGNAL(selectionChanged(QStringList)), m_mapper, SLOT(submit()));

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    QModelIndex index = ccm->index(ccm->CurrentChannel(), 0);
    w->blockSignals(true);
    m_mapper->setCurrentModelIndex(index);
    w->blockSignals(false);
}

void UnitSelectionAction::deleteWidget(QWidget *widget)
{
    m_mapper->removeMapping(widget);
    widget->deleteLater();
}

void UnitSelectionAction::slotChannelChanged(int channel)
{
    QModelIndex index = ChannelConfigModel::instance()->index(channel, 0);
    m_mapper->setCurrentModelIndex(index);
}






