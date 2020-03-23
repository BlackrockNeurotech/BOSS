#ifndef POINTSIZEWIDGETACTION_H
#define POINTSIZEWIDGETACTION_H

#include <QWidgetAction>
#include <QDataWidgetMapper>

#include "PointSizeSlider.h"


class PointSizeWidgetAction : public QWidgetAction
{
    Q_OBJECT

public:
    PointSizeWidgetAction(QObject *parent);

protected:
    virtual QWidget* createWidget (QWidget * parent);

private:
    QDataWidgetMapper *m_mapper;
    void MapToModel(PointSizeSlider *w);
};


#endif // POINTSIZEWIDGETACTION_H
