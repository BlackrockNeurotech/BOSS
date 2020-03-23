#ifndef CLUSTERDATA_H
#define CLUSTERDATA_H

#include "../backend/dataprocessing/spike/spikeaccumulators.h"




class ClusterData 
{    

public:
    
	ClusterData();
    // GENERAL FUNCTIONS
    void clearClusters();
	void addClusters(FeaturesAccum featuresData, QVector<quint32> &centroids);
	int  nbrClusters();
	double getFeatureValue(int clusterIndex, FeatureID feature);
	double getFeatureValueNormalized(FeaturesAccum featuresData, int clusterIndex, FeatureID feature);

    bool saveToXML(QXmlStreamWriter *writer);
    bool getFromXML(QXmlStreamReader *reader);
     
private:
	WfFeaturesVec m_clusters;

	void updateModel();
};

typedef ClusterData* ClusterDataPtr;

#endif // CLUSTERDATA_H
