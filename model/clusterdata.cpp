#include "../model/clusterdata.h"
#include "../model/channelconfigmodel.h"



ClusterData::ClusterData()
{
}

void ClusterData::clearClusters()
{
	m_clusters.clearData();
}

void ClusterData::addClusters(FeaturesAccum featuresData, QVector<quint32> &centroids)
{
	QVector<WfFeatures> datavec;
	m_clusters.getWfFeatures(false, datavec);
	int size = datavec.size();

	datavec.resize(size + centroids.size());

	for (int i = 0; i < centroids.size(); i++)
		featuresData->getWfFeatureForIndex(centroids[i], false, &datavec[i + size]);

	m_clusters.setData(datavec);
	updateModel();
}

int ClusterData::nbrClusters()
{
	return m_clusters.getWaveformCount();
}

double ClusterData::getFeatureValue(int clusterIndex, FeatureID feature)
{	
	return m_clusters.getFeatureValueForIndex(clusterIndex, feature, false);
}

double ClusterData::getFeatureValueNormalized(FeaturesAccum featuresData, int clusterIndex, FeatureID feature)
{
	QVector<double> maxFeatureValues = featuresData->getFeatureMaxValues();
	double value = m_clusters.getFeatureValueForIndex(clusterIndex, feature, false, false);

    if (feature == FEATUREID_TIMESTAMP)
    {
        quint32 minTs = featuresData->getMinTS();
        value -= minTs;
    }

	value /= maxFeatureValues[feature];
	return value;
}

void ClusterData::updateModel()
{
	ChannelConfigModel *ccm = ChannelConfigModel::instance();
	int channel = ccm->CurrentChannel();
	ccm->setData(ccm->index(channel, CC_NBR_SORT_UNITS), QVariant(nbrClusters()), Qt::UserRole);
}

bool ClusterData::saveToXML(QXmlStreamWriter *writer)
{
	return true;
}
bool ClusterData::getFromXML(QXmlStreamReader *reader)
{
	return true;
}

