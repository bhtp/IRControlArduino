#ifndef IRLibrary_h
#define IRLibrary_h
#include "Arduino.h"
#include <vector>
#define DEBUGGING

class SpecialTime
{
  public:
   void setTime(unsigned long, unsigned long);
   void addTime(unsigned long);
   bool lessThan(SpecialTime);
   unsigned long timeMillis;
   unsigned long timeMicros;
};

struct sendState
{
   SpecialTime runTime;
   bool state; 
};

class EncodedMSG
{
  private:
     int currentCell;
  public:
     void addAction(SpecialTime, bool); 
     sendState * message;
     int length;
};

class Protocol {
private:
  void addStartPulse(int);
  int startPulseTime;
  int highLength;
  int lowLength;
  int bitSpacing;
public:
    Protocol (int, int, int, int);
    EncodedMSG encode(unsigned long, int);
};

class Sender {
private:
  short pin;
  std::vector <sendState> sendStates;
  Protocol * prot;
public:
  Sender(short,Protocol *);
  void sendMsg(unsigned long, int);
  void run();

};

class IRObject
{
  private:
    std::vector<Sender> sendObjects;
  public:
    void run();
    void runSendObjects();
    int addSender(Sender);
    void removeSender(int);
};
#endif
