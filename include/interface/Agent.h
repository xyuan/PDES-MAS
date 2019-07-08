/*
 * Agent.h
 *
 *  Created on: 4 May 2019
 *      Author: CubicPill
 */

#ifndef IAGENT_H_
#define IAGENT_H_

#include "Alp.h"
#include "IdentifierHandler.h"
#include "Thread.h"
#include "HasIDLVTMap.h"
#include "HasMPIInfo.h"
#include "HasResponseMessage.h"
#include "EndMessage.h"
#include "PrivateVariableStorage.h"
#include <csignal>

namespace pdesmas {
  class Agent : public Thread {

  private:
    void WaitUntilMessageArrive();

    const AbstractMessage *SendReadMessageAndGetResponse(unsigned long variable_id, unsigned long timestamp);

    const AbstractMessage *SendWriteIntMessageAndGetResponse(unsigned long, int, unsigned long);

    const AbstractMessage *SendWriteDoubleMessageAndGetResponse(unsigned long, double, unsigned long);

    const AbstractMessage *SendWritePointMessageAndGetResponse(unsigned long, const Point, unsigned long);

    const AbstractMessage *SendWriteStringMessageAndGetResponse(unsigned long, const string, unsigned long);

    const AbstractMessage *SendRangeQueryPointMessageAndGetResponse(unsigned long, const Point, const Point);


    bool SetLVT(unsigned long lvt);

    Alp *attached_alp_;
    LpId agent_identifier_;
    unsigned long start_time_;
    unsigned long end_time_;
    PrivateVariableStorage *private_variable_storage_;

    void *MyThread(void *) final;

  protected:
    void SendGVTMessage();

    void SendEndMessage();

    unsigned long GetLVT();

    const int ReadInt(unsigned long variable_id, unsigned long timestamp) const;

    const double ReadDouble(unsigned long variable_id, unsigned long timestamp) const;

    const Point ReadPoint(unsigned long variable_id, unsigned long timestamp) const;

    const string ReadString(unsigned long variable_id, unsigned long timestamp) const;

    const int ReadPrivateInt(unsigned long variable_id) const;

    const double ReadPrivateDouble(unsigned long variable_id) const;

    const Point ReadPrivatePoint(unsigned long variable_id) const;

    const string ReadPrivateString(unsigned long variable_id) const;

    bool WritePrivateInt(unsigned long variable_id);

    bool WritePrivateDouble(unsigned long variable_id);

    bool WritePrivatePoint(unsigned long variable_id);

    bool WritePrivateString(unsigned long variable_id);

    bool WriteInt(unsigned long variable_id, unsigned long timestamp);

    bool WriteDouble(unsigned long variable_id, unsigned long timestamp);

    bool WritePoint(unsigned long variable_id, unsigned long timestamp);

    bool WriteString(unsigned long variable_id, unsigned long timestamp);

    const SerialisableMap<SsvId, Value<Point> >
    RangeQueryPoint(const Point start, const Point end, unsigned long timestamp);


  public:
    Agent(unsigned long const start_time, unsigned long const end_time, Alp *parent_alp, unsigned long agent_id);

    // agent's main loop, must be overridden
    virtual void Cycle() = 0;

    void Finalise();
  };
}

#endif /* IAGENT_H_ */
