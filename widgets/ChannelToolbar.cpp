#include "ChannelToolBar.h"

ChannelToolBar::ChannelToolBar(QWidget *parent) :
QToolBar(parent)
{
    QStringList items = QStringList()
        << "1" << "2" << "4";
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    m_sSize = ccm->rootData(RC_GROUP_SIZE, Qt::DisplayRole).toString();

    this->setStyleSheet("QToolBar {border-bottom: none; border-top: none;}");
    m_groupButton = new ToolbarWidgetAction(this, "Group", ACTIONWIDGETTYPE_BLACKROCKBUTTON);
    m_nGroupsCombo = new ToolbarWidgetAction(this, items, m_sSize, ACTIONWIDGETTYPE_COMBOBOX);
//    m_unGroupButton = new ToolbarWidgetAction(this, "Ungroup", ACTIONWIDGETTYPE_BLACKROCKBUTTON);

    this->addAction(m_groupButton);
    this->addSeparator();
    this->addAction(m_nGroupsCombo);
    //this->addSeparator();
    //this->addAction(m_unGroupButton);

    this->setContentsMargins(0, 0, 0, 0);

    connect(m_groupButton, SIGNAL(pressed()), this, SLOT(slotGroup()));
    connect(m_nGroupsCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotGroupSizeChanged(QString)));
//    connect(m_unGroupButton, SIGNAL(pressed()), this, SLOT(slotUngroup()));
}

ChannelToolBar::~ChannelToolBar()
{
}

void ChannelToolBar::slotGroup()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    ccm->setRootData(RC_GROUP_SIZE, QVariant(m_sSize.toInt()));
    ccm->changeGroupSize();
}

void ChannelToolBar::slotUngroup()
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();
    ccm->setRootData(RC_GROUP_SIZE, QVariant(1));
    ccm->changeGroupSize();
}

void ChannelToolBar::slotGroupSizeChanged(QString sGroupSize)
{
    m_sSize = sGroupSize;
}
