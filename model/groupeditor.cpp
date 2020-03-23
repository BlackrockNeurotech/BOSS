#include <QtWidgets>

#include "groupeditor.h"
#include "../model/channelconfigmodel.h"


GroupEditor::GroupEditor(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setAutoFillBackground(true);
//    setFocusPolicy(Qt::StrongFocus);
}

void GroupEditor::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    m_groupSelection.paint(&painter, rect(), this->palette(), GroupSelection::Editable);
}

void GroupEditor::setGroupSelection(const GroupSelection &groupSelection, QModelIndex index) 
{
    m_groupSelection = groupSelection; 
    m_modelIndex = index; 
}

void GroupEditor::mousePressEvent(QMouseEvent *event)
{
    if (m_groupSelection.setOptionSelection(event->y(), rect()))
    {
        ChannelConfigModel *ccm = ChannelConfigModel::instance();
        ccm->setData(m_modelIndex, QVariant::fromValue(groupSelection()), ChannelTableRole);
    }
    else
    {
        groupSelection().setApplyOptions(-1);
        ChannelConfigModel *ccm = ChannelConfigModel::instance();
        ccm->setData(m_modelIndex, QVariant::fromValue(groupSelection()), ChannelTableRole);
        emit editingFinished();
        update();
    }
}

