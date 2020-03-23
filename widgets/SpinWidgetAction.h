#ifndef SPINWIDGETACTION_H
#define SPINWIDGETACTION_H

#include <QWidgetAction>
#include <QSpinBox>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"


class SpinWidgetAction : public QWidgetAction
{
    Q_OBJECT

public:
    SpinWidgetAction(QObject *parent, ChannelColumn column, int min, int max, QString toolTip);

    void setRange(int min, int max);

public slots:
    void slotValueChanged(int value);
    void slotChannelChanged(int channel);

protected:
    virtual QWidget* createWidget (QWidget * parent);
    virtual void deleteWidget(QWidget *widget);

private:
    // model mapping for the vertical graphics item value
    ChannelColumn      m_channelColumn;
    int                m_max;
    int                m_min;
    QString            m_toolTip;
    int                m_value;
    QDataWidgetMapper *m_mapper;

    void setOptions(QSpinBox *w);
    void MapToModel(QSpinBox *w);
};


#endif // SPINWIDGETACTION_H
