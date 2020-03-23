#include "ComboWidgetAction.h"


ComboWidgetAction::ComboWidgetAction(QObject *parent, ChannelColumn column, QStringList optionNames, QString toolTip, QString styleSheet) :
    QWidgetAction(parent)
{
    if (optionNames.size())
        m_selection = optionNames.at(0);
    else
        m_selection = "";

    m_channelColumn = column;
    m_optionNames = optionNames;
    m_toolTip = toolTip;
    m_styleSheet = styleSheet;

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setOrientation(Qt::Horizontal);
    m_mapper->setModel(ChannelConfigModel::instance());
    m_mapper->toFirst();

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
}

QWidget* ComboWidgetAction::createWidget(QWidget * parent)
{
    QComboBox *w = new QComboBox(parent);
    w->setMaximumHeight(22);
    w->setMinimumHeight(22);
    setOptions(w);
    MapToModel(w);

    QObject::connect(w, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotSelectionChanged(QString)));

    w->show();
    return w;
}

void ComboWidgetAction::setOptions(QComboBox *w)
{
    w->setStyleSheet(m_styleSheet);
    w->setToolTip(m_toolTip);
    w->clear();
    w->setMaxVisibleItems(m_optionNames.size());

    for (int i = 0; i < m_optionNames.size(); i++)
    {
        w->addItem(m_optionNames.at(i));
        w->setItemData(i, i);
    }
}

void ComboWidgetAction::MapToModel(QComboBox *w)
{
    m_mapper->addMapping(w, m_channelColumn);
    connect(w, SIGNAL(currentIndexChanged(int)), m_mapper, SLOT(submit()));

    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    QModelIndex index = ccm->index(ccm->CurrentChannel(), 0);
    m_mapper->setCurrentModelIndex(index);
}

void ComboWidgetAction::slotSelectionChanged(QString selection)
{
    m_selection = selection;
    QList<QWidget  *> widgets = this->createdWidgets();

    foreach (QWidget *w, widgets)
    {
        QComboBox *p = (QComboBox *)w;
        p->setCurrentText(m_selection);
    }
}

void ComboWidgetAction::deleteWidget(QWidget *widget)
{
    m_mapper->removeMapping(widget);
    widget->deleteLater();
}

void ComboWidgetAction::slotChannelChanged(int channel)
{
    QModelIndex index = ChannelConfigModel::instance()->index(channel, 0);
    m_mapper->setCurrentModelIndex(index);
}




