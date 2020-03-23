#include "PointSizeWidgetAction.h"
#include "PointSizeSlider.h"
#include "../model/configmodel.h"


PointSizeWidgetAction::PointSizeWidgetAction(QObject *parent):
    QWidgetAction(parent)
{
    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setOrientation(Qt::Horizontal);
    m_mapper->setModel(ConfigModel::instance());
    m_mapper->toFirst();
}


QWidget* PointSizeWidgetAction::createWidget(QWidget * parent)
{
    PointSizeSlider *w = new PointSizeSlider(parent);
    w->setOptions();
    MapToModel(w);
	w->setToolTip(toolTip());
    w->show();
    return w;
}

void PointSizeWidgetAction::MapToModel(PointSizeSlider *w)
{
    m_mapper->addMapping(w, OFSMODEL_COLUMN_POINT_SIZE);
    connect(w, SIGNAL(selectionChanged(int)), m_mapper, SLOT(submit()));

    w->blockSignals(true);
    m_mapper->toFirst();
    w->blockSignals(false);

//    ConfigModel *cm = ConfigModel::instance();
//    QModelIndex index = cm->index(0, 0);
//    w->blockSignals(true);
//    m_mapper->setCurrentModelIndex(index);
//    w->blockSignals(false);
}


