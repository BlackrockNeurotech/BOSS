#include <QSizePolicy>

#include "correlograms.h"


Correlograms::Correlograms(QWidget *parent):
    QWidget(parent)
{
    m_layout = new QGridLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(2);
    this->setLayout(m_layout);

	m_nUnits = 0;
	
	ChannelConfigModel *ccm = ChannelConfigModel::instance();
    connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
	connect(ccm, SIGNAL(correlogramsChanged(int)), this, SLOT(slotUnitsChanged(int)));

    // put in an empty widget so we don't just display a blank screen before sorting
    m_widgetUnitZero = new CorrelogramGraphicsView(0, 0, 0, 0, this);
    layoutWidgets();
}

void Correlograms::layoutWidgets()
{
    foreach(QWidget *widget, m_widgets)
        m_layout->removeWidget(widget);

    m_layout->removeWidget(m_widgetUnitZero);
    m_widgetUnitZero->hide();

    delete m_layout;
    m_layout = new QGridLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(2);
    this->setLayout(m_layout);

	foreach(CorrelogramGraphicsView *widget, m_widgets)
	{
		m_layout->addWidget(widget, widget->getRow(), widget->getCol(), 1, 1);
    }
    for (int i = 0; i < m_layout->columnCount(); i++)
    {
        m_layout->setColumnStretch(i, 5);
    }
    for (int i = 0; i < m_layout->rowCount(); i++)
    {
        m_layout->setRowStretch(i, 5);
    }
    if (m_widgets.size() == 0)
    {
        m_layout->addWidget(m_widgetUnitZero, 0, 0, 1, 1);
        m_widgetUnitZero->show();

        m_layout->setColumnStretch(0, 5);
        m_layout->setRowStretch(0, 5);
    }
}

CorrelogramGraphicsView* Correlograms::findWidget(int row, int column)
{
	CorrelogramGraphicsView *view = 0;

	foreach(CorrelogramGraphicsView *widget, m_widgets)
	{
        if ((widget->getRow() == row) && (widget->getCol() == column))
		{
			view = widget;
			break;
		}
	}
	return view;
}

void Correlograms::removeUnit(int unit)
{
	foreach(CorrelogramGraphicsView *widget, m_widgets)
	{
		if (widget->UnitMatches(unit))
		{
			m_widgets.removeOne(widget);
			delete widget;
		}
	}
}

void Correlograms::removeUnits()
{
	foreach(CorrelogramGraphicsView *widget, m_widgets)
	{
		m_widgets.removeOne(widget);
		delete widget;
	}
}


// Author & Date:       Holly Ausbeck        10 Dec 2015
// Purpose:  this slot is called when the average units for the file changed
//           update the display.

void Correlograms::slotUnitsChanged(int channel)
{
	updateUnits(channel);
}

void Correlograms::updateUnits(int channel)
{
	ChannelConfigModel * ccm = ChannelConfigModel::instance();
	UnitAccum unitData = ccm->data(ccm->index(channel, CC_SPIKEDATA_UNITS), Qt::UserRole).value<UnitAccum>();

	if (unitData)
	{
		QVector<correlogram> &correlograms = unitData->getCorrelograms();
		QVector<int> units = unitData->getUnitNumbers(true);

        // the units aren't ordered, and remove unit zero which is unsorted if there are any
        if (units.size() > 0)
        {
            std::sort(units.begin(), units.end());

            if (units[0] == 0)
                units.remove(0);
        }
        if (units.size() > 0)
        {
            // the number of units shrunk, remove extra widgets
            if (units.size() < m_nUnits)
            {
                foreach(CorrelogramGraphicsView *widget, m_widgets)
                {
                    if (widget->getRow() >= units.size())
                    {
                        m_widgets.removeOne(widget);
                        delete widget;
                    }
                }
            }
            m_nUnits = units.size();

            foreach(correlogram c, correlograms)
            {
                int row = units.indexOf(c.referenceUnit);
                int col = units.indexOf(c.secondUnit);

                if ((row != -1) && (col != -1))
                {
                    CorrelogramGraphicsView *widget = findWidget(row, col);

                    if (widget != 0)
                    {
                        widget->setUnits(c.referenceUnit, c.secondUnit);
                    }
                    else
                    {
                        CorrelogramGraphicsView *widget = new CorrelogramGraphicsView(c.referenceUnit, c.secondUnit, row, col, this);
                        m_widgets.push_back(widget);
                    }
                }
            }
            if (correlograms.size())
                layoutWidgets();
        }
        else
        {
            removeUnits();
            layoutWidgets();
        }
	}
    else
    {
        removeUnits();
        layoutWidgets();
    }
}


void Correlograms::slotChannelChanged(int channel)
{
	updateUnits(channel);
}

