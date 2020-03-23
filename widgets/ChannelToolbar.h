#ifndef CHANNELTOOLBAR_H
#define CHANNELTOOLBAR_H

#include <QToolBar>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QWidgetAction>
#include <QLabel>
#include <QVector>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"
#include "toolbarWidgetAction.h"
#include "ComboWidgetAction.h"

class ChannelToolBar : public QToolBar
{
    Q_OBJECT

public:
    ChannelToolBar(QWidget *parent = 0);
    ~ChannelToolBar();

private:
    ToolbarWidgetAction *m_groupButton;
    ToolbarWidgetAction *m_nGroupsCombo;
    ToolbarWidgetAction *m_unGroupButton;
    QString m_sSize;

public slots:
    void slotGroup();
    void slotUngroup();
    void slotGroupSizeChanged(QString sGroupSize);
};


#endif // CHANNELTOOLBAR_H
