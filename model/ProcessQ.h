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

#ifndef PROCESSQ_H_INCLUDED
#define PROCESSQ_H_INCLUDED

#include <QQueue>
#include <QVariant>
#include <QMutex>
#include "../model/channelItem.h"

enum RequestType
{
    REQUESTTYPE_FILTER,
    REQUESTTYPE_CONTIN_MOVE,
    REQUESTTYPE_SPIKE_MOVE,
    REQUESTTYPE_CONTIN_SPIKE_TS,
    REQUESTTYPE_DETECT,
    REQUESTTYPE_SORT,
    REQUESTTYPE_UPDATE_PCA,
    REQUESTTYPE_UPDATE_SPIKE,
    REQUESTTYPE_UPDATE_CONTIN,
    REQUESTTYPE_NONE
};

// Request for backend processing
struct PQRequest
{
    RequestType eType;
    quint64     nValue;
    SpikeMoveReference nReference;

    PQRequest(RequestType type)
    {
        eType = type;
        nValue = 0;
    }
    bool operator==(const PQRequest &other) const { return eType == other.eType; }
};

class ProcessQ : public QObject
{
    Q_OBJECT

public:
    ProcessQ();
    ~ProcessQ();
    void setChannel(int channel) { m_channel = channel; }

    void addRecalcRequest(RequestType eType, quint64 nValue = 0, SpikeMoveReference reference = SPIKEMOVE_START);
    RequestType processNextRequest();

    RequestType getType() { return m_eType; }
    quint64 getValue() { return m_nValue; }

    void sendRequest(int channel);

private:
    bool bFirstRequest;
    QQueue<PQRequest*> m_requests;
    PQRequest *m_pReq;

    RequestType m_eType;
    quint64     m_nValue;

    int         m_channel;
    bool        m_processing;

    QMutex  m_mutexProcessQ;
};

#endif // PROCESSQ_H_INCLUDED
