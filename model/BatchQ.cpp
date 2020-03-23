/////////////////////////////////////////////////////////////////
///
/// @file BatchQ.h
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  March 18, 2016
///
/// @brief BatchQ buffers user requests for the backend.
///
/// @note
///

#include "../model/BatchQ.h"
#include "../model/channelconfigmodel.h"

/// @author Holly Ausbeck
/// @date   March 18, 2016
/// @brief  Constructor for BatchQ class
/// @param [in]
BatchQ::BatchQ()
{
    m_bProcessing = false;
}

/// @author Holly Ausbeck
/// @date   March 18, 2016
/// @brief  Destructor for BatchQ class - deletes contents of Queue
BatchQ::~BatchQ()
{   
    while (!m_requests.isEmpty())
        delete m_requests.takeFirst();
}

/// @author Holly Ausbeck
/// @date   March 18, 2016
/// @brief  Add the request
/// @param [in]  nChannel - channel
void BatchQ::enQ(int nChannel, RequestType etype)
{
    m_mutexBatchQ.lock();

    BatchQRequest *pRequest = new BatchQRequest(nChannel, etype);
    m_requests.enqueue(pRequest);

    m_mutexBatchQ.unlock();
}

/// @author Holly Ausbeck
/// @date   March 18, 2016
/// @brief  process the next request in the queue
/// @return request type processed
bool BatchQ::deQ(int &nChannel, RequestType &eType)
{
    m_mutexBatchQ.lock();
    bool status = false;

    if (m_requests.size() > 0)
    {
        BatchQRequest *pRequest = m_requests.dequeue();
        eType = pRequest->m_eType;
        nChannel = pRequest->m_nChannel;
        delete pRequest;
        status = true;
    }
    m_mutexBatchQ.unlock();
    return status;
}

/// @author Holly Ausbeck
/// @date   March 18, 2016
/// @brief  the last request finished, set idle
void BatchQ::finished()
{
    m_mutexBatchQ.lock();
    m_bProcessing = false;
    m_mutexBatchQ.unlock();
}

/// @author Holly Ausbeck
/// @date   March 18, 2016
/// @brief  process the next request in the queue
/// @return request type processed
bool BatchQ::next()
{
    m_mutexBatchQ.lock();
    bool status = false;
    BatchQRequest *pRequest = 0;

    if ((m_requests.size() > 0) && !m_bProcessing)
    {
        pRequest = m_requests.dequeue();
        m_bProcessing = true;
        status = true;
    }
    m_mutexBatchQ.unlock();

    if (pRequest != 0)
    {
        ChannelConfigModel *ccm = ChannelConfigModel::instance();
        ccm->doBatch(pRequest->m_nChannel, pRequest->m_eType);
        delete pRequest;
    }
    return status;
}

/// @author Holly Ausbeck
/// @date   March 18, 2016
/// @brief  file is closing, clear the queue
/// @return request type processed
void BatchQ::exit()
{
    m_mutexBatchQ.lock();
    BatchQRequest *pRequest = 0;

    while (m_requests.size() > 0)
    {
        pRequest = m_requests.dequeue();
        delete pRequest;
    }
    m_bProcessing = false;
    m_mutexBatchQ.unlock();
}

/// @author Holly Ausbeck
/// @date   Oct 11, 2016
/// @brief  remove the requests of eType on channel nChannel from the queue
/// @return #requests removed
int BatchQ::cancelRequest(int nChannel, RequestType eType)
{
    int nCanceled = 0;
    BatchQRequest *pRequest = 0;
    QQueue<BatchQRequest*> tempRequests;

    m_mutexBatchQ.lock();
    m_requests.swap(tempRequests);

    while (tempRequests.size() > 0)
    {
        pRequest = tempRequests.dequeue();

        if ((pRequest->m_nChannel == nChannel) && (pRequest->m_eType == eType))
        {
            delete pRequest;
            nCanceled++;
        }
        else
        {
            m_requests.enqueue(pRequest);
        }
    }
    m_bProcessing = false;
    m_mutexBatchQ.unlock();
    return nCanceled;
}

/// @author Holly Ausbeck
/// @date   Oct 11, 2016
/// @brief  remove all the requests of eType from the queue
/// @return #requests removed
int BatchQ::cancelRequests(RequestType eType)
{
    int nCanceled = 0;
    BatchQRequest *pRequest = 0;
    QQueue<BatchQRequest*> tempRequests;

    m_mutexBatchQ.lock();
    m_requests.swap(tempRequests);

    while (tempRequests.size() > 0)
    {
        pRequest = tempRequests.dequeue();

        if (pRequest->m_eType == eType)
        {
            delete pRequest;
            nCanceled++;
        }
        else
        {
            m_requests.enqueue(pRequest);
        }
    }
    m_bProcessing = false;
    m_mutexBatchQ.unlock();
    return nCanceled;
}

