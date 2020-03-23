#ifndef GROUPEDITOR_H
#define GROUPEDITOR_H

#include <QWidget>
#include <QModelIndex>

#include "groupselection.h"


class GroupEditor : public QWidget
{
    Q_OBJECT

public:
    GroupEditor(QWidget *parent = 0);

    void setGroupSelection(const GroupSelection &groupSelection, QModelIndex index);
    GroupSelection groupSelection() { return m_groupSelection; }

signals:
    void editingFinished();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    GroupSelection m_groupSelection;
    QModelIndex m_modelIndex;
};


#endif   // GROUPEDITOR_H
