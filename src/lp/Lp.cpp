#include <iostream>
#include <mpi.h>
#include <assert.h>
#include "Lp.h"
#include "Log.h"

using namespace std;

Lp::Lp() {
  fProcessMutex = Mutex();
  fReceiveMessageQueue = new MessageQueue();
  fSendMessageQueue = new MessageQueue();
  fSendControlMessageQueue = new MessageQueue();

  fReceiveLoadBalancingMessageQueue = new MessageQueue();
  fSendLoadBalancingMessageQueue = new MessageQueue();
}

Lp::~Lp() {
  delete fReceiveMessageQueue;
  delete fSendMessageQueue;
  delete fSendControlMessageQueue;
  delete fReceiveLoadBalancingMessageQueue;
  delete fSendLoadBalancingMessageQueue;
}

void Lp::PreProcessSendMessage(SimulationMessage* pSimulationMessage) {
  pSimulationMessage->SetMatternColour(fGVTCalculator->GetColour());
  //update counters for GVT algorithm
  if (fGVTCalculator->GetColour() == WHITE) {
    assert(pSimulationMessage->GetDestination() != GetRank());
    assert(fGVTCalculator->GetWhiteTransientMessageCounter(GetRank()) <= 0);
    fGVTCalculator->IncrementWhiteTransientMessageCounter(pSimulationMessage->GetDestination());
  } else {
    fGVTCalculator->SetRedTime(min(fGVTCalculator->GetRedTime(), pSimulationMessage->GetTimestamp()));
  }
}

void Lp::PreProcessReceiveMessage(const SimulationMessage* pSimulationMessage) {
  //update counters for GVT algorithm
  if (pSimulationMessage->GetMatternColour() == WHITE) {
    fGVTCalculator->DecrementWhiteTransientMessageCounter(GetRank());
  }
}

void Lp::Run() {
  Initialise();

  while (!TerminationCondition()) {
    fMPIInterface->ReceiveWait();
    /*
     * Lock so we're not processing messages as they
     * are put in the receive queue or receive thread
     */
    Lock();
    /*
     * Receive a message: will either cause the message
     * to be processed here, or forwarded to another LP.
     */
    Receive();
    Unlock();
  }
  Finalise();
  return;

}

void Lp::Lock() {
  fProcessMutex.Lock();
}

void Lp::Unlock() {
  fProcessMutex.Unlock();
}

void Lp::SignalSend() {
  fMPIInterface->SendSignal();
}

bool Lp::AllEndMessagesReceived() const {
  for (unsigned int i = 0; i < GetNumberOfAlps(); ++i) {
    if (fEndMessagesReceived[i] == false) return false;
  }
  return true;
}

unsigned long Lp::GetGvt() const {
  return fGVT;
}

unsigned long Lp::GetEndTime() const {
  return fEndTime;
}

bool Lp::TerminationCondition() const {
  return fGVT >= fEndTime;
}