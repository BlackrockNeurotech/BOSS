#include "spikedata.h"



// Author & Date:       Sylvana Alpert         3 Dec 2014
// Purpose: Default constructor
SpikeData::SpikeData() :
    m_features(0),
    m_units(0),
    m_clusteringMatrix(0)
{
}

void SpikeData::reset()
{
    m_features.reset();
    m_clusteringMatrix.reset();
}

void SpikeData::setFeatures(FeaturesAccum features)
{
    m_features = features;
}

void SpikeData::initUnits(int nSamples, int nUnits)
{
    if (m_units)
    {
        QVector<int> usedUnits = m_units->getUnitNumbers(true);
        int numUnits = usedUnits.size();
        if (numUnits != nUnits || m_units->numSamples() != nSamples)
            m_units.reset(); // deletes the shared object and sets the pointer to NULL
        else
            m_units->init();
    }        

    if (!m_units && (nUnits != 0))
    {
        m_units = new AccumulatorUnitData(nSamples, nUnits);
    }
}

void SpikeData::setClusteringTendencyMatrix(ClusteringTendencyAccum matrix)
{
    m_clusteringMatrix = matrix;
}

bool SpikeData::saveToXML(QXmlStreamWriter *writer)
{
    if (m_units)    
        return m_units->saveToXML(writer);    
    
    return true;
}
bool SpikeData::getFromXML(QXmlStreamReader *reader)
{
    int nSamples = -1;
    int nUnits = -1;

    while (reader->readNextStartElement())
    { 
        QStringRef n1 = reader->name();

        if (n1 == "numSamples")
        {
            nSamples = reader->readElementText().toInt();            
        }
        else if (n1 == "numUnits")
        {
            nUnits = reader->readElementText().toInt();            
        }          

        if ((nSamples != -1) && (nUnits != -1))
        {
            initUnits(nSamples, nUnits);
            break;
        }
    }
    return m_units->getFromXML(reader);
}

