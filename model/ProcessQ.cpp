/////////////////////////////////////////////////////////////////
///
/// @file ProcessQ.h
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  May 26, 2015
///
/// @brief ProcessQ buffers user requests for the backend.
///
/// @note
///
#include <QThread>

#include "../model/ProcessQ.h"
#include "../model/channelconfigmodel.h"

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  Constructor for ProcessQ class
/// @param [in]
ProcessQ::ProcessQ()
{
    m_pReq = 0;
    m_eType = REQUESTTYPE_NONE;
    m_nValue = 0;

    m_processing = false;
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  Destructor for ProcessQ class - deletes contents of Queue
ProcessQ::~ProcessQ()
{   
    while (!m_requests.isEmpty())
        delete m_requests.takeFirst();

    if (m_pReq != 0)
        delete m_pReq;
}

/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  if there is another move request, replace the request. else add the request
/// @param [in]  RequestType - move request type
/// @param [in]  nStart - beginning continuous data sample
/// @param [in]  nSamples - number of continuous data samples
void ProcessQ::addRecalcRequest(RequestType eType, quint64 nValue, SpikeMoveReference reference)
{
    bool qIt = true;
    m_mutexProcessQ.lock();

    // preprocess queue
    switch (eType)
    {
    case REQUESTTYPE_FILTER:
        // if the queue is not empty the next request will pick up the new filters
        if (m_requests.size())
            qIt = false;
        break;

    case REQUESTTYPE_DETECT:
    case REQUESTTYPE_SORT:
    case REQUESTTYPE_CONTIN_SPIKE_TS:
        break;

    case REQUESTTYPE_CONTIN_MOVE:
    case REQUESTTYPE_SPIKE_MOVE:
    case REQUESTTYPE_UPDATE_SPIKE:
    case REQUESTTYPE_UPDATE_CONTIN:
        // clear everything except this most recent move
        while (!m_requests.isEmpty())
            delete m_requests.takeFirst();
        break;
    }
    if (qIt)
    {
        PQRequest *pRequest = new PQRequest(eType);
        pRequest->nValue = nValue;
        pRequest->nReference = reference;
        m_requests.enqueue(pRequest);
    }
    if ((!m_processing) && (!m_requests.isEmpty()))
    {
        m_processing = true;
        sendRequest(m_channel);
    }
    m_mutexProcessQ.unlock();
}


/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  process the next request in the queue
///         only process a filter request if the queue is empty
/// @return request type processed
void ProcessQ::sendRequest(int channel)
{
    ChannelConfigModel *ccm = ChannelConfigModel::instance();

    if (!m_requests.isEmpty())
    {
        m_pReq = m_requests.dequeue();

        switch (m_pReq->eType)
        {
        case REQUESTTYPE_FILTER:     // a filter change only involves recalculation of continuous because the continuous time line only uses spike times
        case REQUESTTYPE_UPDATE_CONTIN:
        case REQUESTTYPE_CONTIN_MOVE:
			ccm->emitSignal(m_channel, SIGNAL_RECALC_TIME_SLICE, QVariant(m_pReq->nValue));
            break;

        case REQUESTTYPE_UPDATE_SPIKE:
        case REQUESTTYPE_SPIKE_MOVE:
            ccm->emitSignal(m_channel, SIGNAL_RECALC_SPIKE_SLICE, QVariant(m_pReq->nValue), QVariant(m_pReq->nReference));
            break;

        case REQUESTTYPE_CONTIN_SPIKE_TS:
            ccm->emitSignal(m_channel, SIGNAL_RECALC_TIME_STAMP);
            break;

        case REQUESTTYPE_DETECT:
            ccm->emitSignal(m_channel, SIGNAL_DETECT);
            break;

        case REQUESTTYPE_SORT:
            ccm->emitSignal(m_channel, SIGNAL_SORT);
            break;
        case REQUESTTYPE_UPDATE_PCA:
            ccm->emitSignal(m_channel, SIGNAL_UPDATE_PCA);
            break;
        default:
            break;
        }
    }
}


/// @author Holly Ausbeck
/// @date   May 26, 2015
/// @brief  process the next request in the queue
/// @return request type processed
RequestType ProcessQ::processNextRequest()
{
    m_mutexProcessQ.lock();
    m_processing = false;

    if (m_pReq != 0)
    {
        m_eType = m_pReq->eType;
        m_nValue = m_pReq->nValue;
        delete m_pReq;
        m_pReq = 0;

        if ((!m_processing) && (!m_requests.isEmpty()))
        {
            m_processing = true;
            sendRequest(m_channel);
        }
    }
    m_mutexProcessQ.unlock();
    return m_eType;
}

