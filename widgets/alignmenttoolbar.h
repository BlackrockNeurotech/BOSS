#ifndef ALIGNMENTTOOLBAR_H
#define ALIGNMENTTOOLBAR_H

#include <QToolBar>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QWidgetAction>
#include <QLabel>
#include <QVector>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"
#include "../backend/dataprocessing/spike/waveformalignment.h"
#include "toolbarWidgetAction.h"
#include "UnitSelectionAction.h"
#include "ModeWidgetAction.h"
#include "ComboWidgetAction.h"
#include "SpinWidgetAction.h"

class AlignmentToolBar : public QToolBar
{
    Q_OBJECT

public:
    AlignmentToolBar(QWidget *parent = 0);
    ~AlignmentToolBar();

private:
    QWidget             *m_parent;
    QStringList          m_alignmentOptions;

    ComboWidgetAction   *m_comboAlignTo;
    SpinWidgetAction    *m_spinMaxShift;

    ToolbarWidgetAction *m_alignButton;

    ToolbarWidgetAction *m_labelUnits;
    UnitSelectionAction *m_widgetUnits;
    ModeWidgetAction    *m_actionMode;

    QList<QColor>        m_unitColors;

    void populate();
    void setUnits();

public slots:
    void slotFeaturesChanged(int channel);
    void slotChannelChanged(int channel);
    void slotAlign();
    QVector<int> getAlignUnits();
};


#endif // ALIGNMENTTOOLBAR_H
