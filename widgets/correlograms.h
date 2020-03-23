#ifndef CORRELOGRAMS_H
#define CORRELOGRAMS_H

#include <QWidget>
#include <QGridLayout>

#include "../model/channelconfigmodel.h"
#include "../graphics/correlogramgraphicsview.h"

class Correlograms : public QWidget
{
    Q_OBJECT

public:
	Correlograms(QWidget *parent = 0);
	QSize sizeHint() const { return this->size(); }

public slots:
	void slotUnitsChanged(int channel);
	void slotChannelChanged(int channel);

private:
    QGridLayout                      *m_layout;
	QList<CorrelogramGraphicsView *>  m_widgets;
    CorrelogramGraphicsView          *m_widgetUnitZero;
	int                               m_nUnits;

    void layoutWidgets();
	void updateUnits(int channel);
	void removeUnit(int unit);
	void removeUnits();
	CorrelogramGraphicsView* findWidget(int referenceUnit, int secondUnit);
};


#endif // CORRELOGRAMS_H
