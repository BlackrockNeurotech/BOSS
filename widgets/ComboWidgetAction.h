#ifndef COMBOWIDGETACTION_H
#define COMBOWIDGETACTION_H

#include <QWidgetAction>
#include <QComboBox>
#include <QDataWidgetMapper>

#include "../model/channelconfigmodel.h"


class ComboWidgetAction : public QWidgetAction
{
    Q_OBJECT

public:
    ComboWidgetAction(QObject *parent, ChannelColumn column, QStringList optionNames, QString toolTip, QString styleSheet);

public slots:
    void slotSelectionChanged(QString selection);

protected:
    virtual QWidget* createWidget (QWidget * parent);
    virtual void deleteWidget(QWidget *widget);

public slots:
    void slotChannelChanged(int channel);

private:
    // model mapping for the vertical graphics item value
    ChannelColumn   m_channelColumn;
    QStringList     m_optionNames;
    QString         m_toolTip;
    QString         m_styleSheet;

    QString         m_selection;
    QDataWidgetMapper *m_mapper;

    void setOptions(QComboBox *w);
    void MapToModel(QComboBox *w);
};


#endif // COMBOWIDGETACTION_H
