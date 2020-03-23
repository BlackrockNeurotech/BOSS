#ifndef FEATURE2DTOOLBAR_H
#define FEATURE2DTOOLBAR_H

#include <QToolBar>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QWidgetAction>
#include <QLabel>
#include <QVector>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"
#include "PointSizeWidgetAction.h"
#include "ComboWidgetAction.h"

class Feature2DToolBar : public QToolBar
{
    Q_OBJECT

public:
    Feature2DToolBar(QWidget *parent = 0);
    ~Feature2DToolBar();

private:
    ComboWidgetAction   *m_comboX;
    ComboWidgetAction   *m_comboY;
};


#endif // FEATURE2DTOOLBAR_H
