#ifndef MODEWIDGETACTION_H
#define MODEWIDGETACTION_H

#include <QWidgetAction>
#include <QDataWidgetMapper>
#include "ModeSlider.h"


class ModeWidgetAction : public QWidgetAction
{
    Q_OBJECT

public:
    ModeWidgetAction(QObject *parent, ChannelColumn column, QStringList optionNames, QStringList toolTips = QStringList(), QStringList bitmapNames = QStringList());
	void enable(int option, bool enable);

public slots:
    void slotSelectionChanged(QString selection);

protected:
    virtual QWidget* createWidget (QWidget * parent);
    virtual void deleteWidget(QWidget *widget);

public slots:
    void slotChannelChanged(int channel);

private:
    // model mapping for the vertical graphics item value
    ChannelColumn  m_ChannelColumn;
    QStringList m_optionNames;
    QStringList m_bitmapNames;
    QStringList m_toolTips;

    QString m_selection;
    QDataWidgetMapper *m_mapper;

    void setOptions(ModeSlider *w, bool selectClusters = false);
    void MapToModel(ModeSlider *w);
};


#endif // MODEWIDGETACTION_H
