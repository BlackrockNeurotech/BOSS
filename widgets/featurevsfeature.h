#ifndef FEATUREVSFEATURE_H
#define FEATUREVSFEATURE_H

#include <QWidget>
#include <QGridLayout>

#include "../model/channelconfigmodel.h"
#include "../graphics/featuregraphicsview.h"

class FeatureVSFeature : public QWidget
{
    Q_OBJECT

public:
    FeatureVSFeature(QWidget *parent = 0);

public slots:
    void slotViewFeaturesChanged(int channel);
    void slotChannelChanged(int channel);
    void slotFeaturesChanged(int channel);

    QSize sizeHint() const { return this->size(); }

private:
    QGridLayout                  *m_layout;
    QList<FeatureGraphicsView *>  m_widgets;
    bool                          m_channelChange;

    void layoutWidgets();
    FeatureGraphicsView* findWidget(FeatureID idX, FeatureID idY);
    void AddFeatures(FeatureID idX, FeatureID idY);
    void RemoveFeatures(FeatureID idX, FeatureID idY);
    void removeFeature(FeatureID id);
    void updateFeatures(int channel);
    void newFeatures(int channel);
};


#endif // FEATUREVSFEATURE_H
