#ifndef UNITSELECTIONACTION_H
#define UNITSELECTIONACTION_H

#include <QWidgetAction>
#include <QDataWidgetMapper>
#include "unitSelection.h"


class UnitSelectionAction : public QWidgetAction
{
    Q_OBJECT

public:
    UnitSelectionAction(QObject *parent, bool bUseVisibleUnits, ChannelColumn modelColumn);

protected:
    virtual QWidget* createWidget (QWidget *parent);
    virtual void deleteWidget(QWidget *widget);

public slots:
    void slotChannelChanged(int channel);

private:
    QDataWidgetMapper *m_mapper;
    bool m_mainWidget;
    bool m_bUseVisibleUnits;
    ChannelColumn m_modelColumn;

    void MapToModel(unitSelection *w);
};


#endif // UNITSELECTIONACTION_H
