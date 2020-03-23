#ifndef FEATURE3DTOOLBAR_H
#define FEATURE3DTOOLBAR_H

#include <QToolBar>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QWidgetAction>
#include <QLabel>
#include <QVector>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"
#include "ModeSlider.h"
#include "toolbarWidgetAction.h"
#include "PointSizeWidgetAction.h"
#include "ModeWidgetAction.h"
#include "ComboWidgetAction.h"

class Feature3DToolBar : public QToolBar
{
    Q_OBJECT

public:
    Feature3DToolBar(QWidget *parent = 0);
    ~Feature3DToolBar();

    ToolbarWidgetAction   *m_actionReset;

public slots:
    void slotChannelChanged(int channel);
    void slotGlEditStateChanged(int channel, GLWidgetState editState);

private:
    ComboWidgetAction *m_comboX;
    ComboWidgetAction *m_comboY;
    ComboWidgetAction *m_comboZ;
    ModeWidgetAction  *m_actionMode;

    void setOptions(bool selectClusters = false);
};


#endif // FEATURE3DTOOLBAR_H
