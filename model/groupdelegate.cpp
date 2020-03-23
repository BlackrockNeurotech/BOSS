#include <QtWidgets>
#include <QPainter>

#include "../model/groupdelegate.h"
#include "../model/groupeditor.h"
#include "../model/groupselection.h"
#include "../model/channelconfigmodel.h"


void GroupDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.data().canConvert<GroupSelection>()) 
    {
        GroupSelection groupSelection = qvariant_cast<GroupSelection>(index.data());

        if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());

        bool bEnabled = option.state & QStyle::State_Enabled;
        groupSelection.paint(painter, option.rect, option.palette, GroupSelection::ReadOnly, false, bEnabled);
    } 
    else 
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize GroupDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *GroupDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.data().canConvert<GroupSelection>()) 
    {
        GroupSelection groupSelection = qvariant_cast<GroupSelection>(index.data());

        QPoint globalPos = QCursor::pos();
        QPoint pos = parent->mapFromGlobal(globalPos);
        int y = pos.y() - option.rect.y();
        groupSelection.setOptionSelection(y, option.rect);
        ChannelConfigModel *ccm = ChannelConfigModel::instance();
        ccm->setData(index, QVariant::fromValue(groupSelection), ChannelTableRole);

        GroupEditor *groupEditor = new GroupEditor(parent);
        connect(groupEditor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
        return groupEditor;
    } 
    else 
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void GroupDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.data().canConvert<GroupSelection>()) 
    {
        GroupSelection groupSelection = qvariant_cast<GroupSelection>(index.data());
        GroupEditor *groupEditor = qobject_cast<GroupEditor *>(editor);
        groupEditor->setGroupSelection(groupSelection, index);
    } 
    else 
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void GroupDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.data().canConvert<GroupSelection>()) 
    {
        GroupEditor *groupEditor = qobject_cast<GroupEditor *>(editor);
        ChannelConfigModel *ccm = ChannelConfigModel::instance();
        ccm->setData(index, QVariant::fromValue(groupEditor->groupSelection()), ChannelTableRole);
    } 
    else 
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void GroupDelegate::commitAndCloseEditor()
{
    GroupEditor *groupEditor = qobject_cast<GroupEditor *>(sender());
    emit commitData(groupEditor);
    emit closeEditor(groupEditor);
}
