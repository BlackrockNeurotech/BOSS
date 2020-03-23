#ifndef SPIKEDATA_H
#define SPIKEDATA_H

#include "../backend/dataprocessing/spike/spikeaccumulators.h"
#include "../backend/dataprocessing/spike/clusteringtendencyccumulator.h"




class SpikeData 
{    

public:
    
    SpikeData();
    // GENERAL FUNCTIONS
    void reset();
    void setFeatures(FeaturesAccum);
    void initUnits(int nSamples, int nUnits);
    void setClusteringTendencyMatrix(ClusteringTendencyAccum);

    FeaturesAccum   features() { return m_features; }
    UnitAccum       units() { return m_units; }
    ClusteringTendencyAccum  clusteringTendency() { return m_clusteringMatrix; }

    bool saveToXML(QXmlStreamWriter *writer);
    bool getFromXML(QXmlStreamReader *reader);
     
private:
          
    FeaturesAccum       m_features;
    UnitAccum           m_units;
    ClusteringTendencyAccum      m_clusteringMatrix;

};

#endif // SPIKEDATA_H
