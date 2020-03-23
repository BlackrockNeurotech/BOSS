#include "ModeWidgetAction.h"


ModeWidgetAction::ModeWidgetAction(QObject *parent, ChannelColumn column, QStringList optionNames, QStringList toolTips, QStringList bitmapNames) :
    QWidgetAction(parent)
{
    if (optionNames.size())
        m_selection = optionNames.at(0);

    m_ChannelColumn = column;
    m_optionNames = optionNames;
    m_bitmapNames = bitmapNames;
    m_toolTips = toolTips;

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setOrientation(Qt::Horizontal);
    m_mapper->setModel(ChannelConfigModel::instance());
    m_mapper->toFirst();

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
}

QWidget* ModeWidgetAction::createWidget(QWidget * parent)
{
    ModeSlider *w = new ModeSlider(parent);    
    setOptions(w);
    MapToModel(w);

    QObject::connect(w, SIGNAL(selectionChanged(QString)), this, SLOT(slotSelectionChanged(QString)));

    w->show();
    return w;
}

void ModeWidgetAction::setOptions(ModeSlider *w, bool selectClusters)
{
    Q_UNUSED(selectClusters);

    w->setOptions(m_optionNames, m_toolTips, m_bitmapNames);
}

void ModeWidgetAction::MapToModel(ModeSlider *w)
{
    m_mapper->addMapping(w, m_ChannelColumn, "currentSelection");
    connect(w, SIGNAL(selectionChanged(QString)), m_mapper, SLOT(submit()));

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    QModelIndex index = ccm->index(ccm->CurrentChannel(), 0);
    m_mapper->setCurrentModelIndex(index);
}

void ModeWidgetAction::enable(int option, bool enable)
{
	QList<QWidget  *> widgets = this->createdWidgets();

	foreach(QWidget *w, widgets)
	{
		ModeSlider *p = (ModeSlider *)w;
		p->enable(option, enable);
	}
}

void ModeWidgetAction::slotSelectionChanged(QString selection)
{
    m_selection = selection;
    QList<QWidget  *> widgets = this->createdWidgets();

    foreach (QWidget *w, widgets)
    {
        ModeSlider *p = (ModeSlider *)w;
        p->setCurrentSelection(m_selection);
    }
}

void ModeWidgetAction::deleteWidget(QWidget *widget)
{
    m_mapper->removeMapping(widget);
    widget->deleteLater();
}

void ModeWidgetAction::slotChannelChanged(int channel)
{
    QModelIndex index = ChannelConfigModel::instance()->index(channel, 0);
    m_mapper->setCurrentModelIndex(index);
}




