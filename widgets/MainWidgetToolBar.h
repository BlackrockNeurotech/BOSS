#ifndef MAINWIDGETTOOLBAR_H
#define MAINWIDGETTOOLBAR_H

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

#include "../widgets/UnitSelectionAction.h"
#include "../widgets/PointSizeWidgetAction.h"
#include "toolbarWidgetAction.h"

class MainWidgetToolBar : public QToolBar
{
    Q_OBJECT

public:
    MainWidgetToolBar(QWidget *parent);
    ~MainWidgetToolBar();

    void initialize();

private:
    UnitSelectionAction   *m_actionUnits;
    PointSizeWidgetAction *m_actionPointSize;
    ToolbarWidgetAction   *m_actionUpdatePCA;

    QList<QColor>      m_unitColors;
    QDataWidgetMapper *m_mapper;
};


#endif // MAINWIDGETTOOLBAR_H
