#include "UnitsSplitter.h"


UnitsSplitter::UnitsSplitter(QWidget *parent):
    QSplitter(parent)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();

    connect(ccm, SIGNAL(unitsChanged(int)), this, SLOT(slotUnitsChanged(int)));
    connect(ccm, SIGNAL(featuresChanged(int)), this, SLOT(slotUnitsChanged(int)));
    connect(ccm, SIGNAL(currentChannelChanged(int)), this, SLOT(slotChannelChanged(int)));
}


void UnitsSplitter::slotChannelChanged(int channel)
{
    removeUnits(0);
    slotUnitsChanged(channel);
}

void UnitsSplitter::slotUnitsChanged(int channel)
{
    ChannelConfigModel * ccm = ChannelConfigModel::instance();
    UnitAccum unitData = ccm->data(ccm->index(channel, CC_SPIKEDATA_UNITS), Qt::UserRole).value<UnitAccum>();
    
    if (unitData)
    {
        double max = 0;
        double min = 0;
        QVector<int> usedUnits = unitData->getUnitNumbers(false);

        int  nUnits = usedUnits.size();

        if (nUnits != 0)
        {

            // find the max and min values
            UnitData * pUnit;
            for (int i = 0; i < nUnits; i++)
            {
                pUnit = unitData->unit(usedUnits[i]);
                arma::vec avg = pUnit->average;
                arma::vec std = pUnit->SD;
                arma::vec plus = avg + std;
                arma::vec minus = avg - std;

                double mx = plus.max();
                double mn = minus.min();

                if (mx > max)
                    max = mx;
                if (mn < min)
                    min = mn;
            }
            double vRange = (max > abs(min)) ? max : abs(min);
            // add 10% space at top and bottom for the scene rectangle
            vRange += vRange * 0.1;

            QList<int> sizes;
            int size = this->size().width() / nUnits;

            for (int i = 0; i < nUnits; ++i)
            {
                sizes.push_back(size);
                UnitGraphicsView *unit;

                if (i < count())
                {
                    unit = (UnitGraphicsView*)this->widget(i);
                }
                else
                {
                    unit = new UnitGraphicsView(this);
                    this->addWidget(unit);
                }
                pUnit = unitData->unit(usedUnits[i]);
                arma::vec avg = pUnit->average;
                arma::vec std = pUnit->SD;
                int N = pUnit->nWaveforms;
                int U = pUnit->unitNumber;

                unit->setData(avg, std, U, N, vRange);
            }
            removeUnits(nUnits);
            this->setSizes(sizes);
        }
    }
}

// remove the units off the end leaving nUnits displayed
void UnitsSplitter::removeUnits(int nUnits)
{
    while (count() > nUnits)
    {
        UnitGraphicsView *unit = (UnitGraphicsView*)this->widget(nUnits);
        unit->hide();
        delete unit;
    }
}

