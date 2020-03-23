/////////////////////////////////////////////////////////////////
///
/// @file ProcessQ.h
/// @attention  (c) Copyright 2015 Blackrock Microsystems, LLC
///
/// @author  Holly Ausbeck
/// @date  May 26, 2015
///
/// @brief BatchQ queues user batch requests for the backend.
///
/// @note
///

#ifndef BATCHQ_H_INCLUDED
#define BATCHQ_H_INCLUDED

#include <QQueue>
#include <QVariant>
#include <QMutex>

#include "../model/ProcessQ.h"

// Request for backend processing
struct BatchQRequest
{
    int         m_nChannel;
    RequestType m_eType;

    BatchQRequest(int nChannel, RequestType eType)
    {
        m_nChannel = nChannel;
        m_eType    = eType;
    }
};

class BatchQ : public QObject
{
    Q_OBJECT

public:
    BatchQ();
    ~BatchQ();

    void enQ(int nChannel, RequestType eType);
    bool deQ(int &nChannel, RequestType &eType);
    bool next();
    void finished();
    int count() { return m_requests.count(); }
    int cancelRequest(int nChannel, RequestType eType);
    int cancelRequests(RequestType eType);
    void exit();

    quint64 getCount() { return m_requests.size(); }

private:
    QQueue<BatchQRequest*> m_requests;
    QMutex  m_mutexBatchQ;
    bool    m_bProcessing;
};

#endif // BATCHQ_H_INCLUDED
