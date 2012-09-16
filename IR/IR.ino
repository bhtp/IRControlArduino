#include <StandardCplusplus.h>
#include <system_configuration.h>
#include <unwind-cxx.h>
#include <utility.h>
#include <vector>
using namespace std;

class specialTime
{
  public:
   void setTime(unsigned long, unsigned long);
   void addTime(unsigned long);
   bool lessThan(specialTime);
   unsigned long timeMillis;
   unsigned long timeMicros;
};

void specialTime::setTime(unsigned long millisIn, unsigned long microsIn)
{
  timeMillis = millisIn;
  timeMicros = microsIn;
}

void specialTime::addTime(unsigned long microsIn)
{
   timeMillis += microsIn / 1000;
   timeMicros += microsIn;
}

bool specialTime::lessThan(specialTime timeIn)
{
   return (timeIn.timeMillis > timeMillis && timeIn.timeMicros > timeMicros); 
}

struct sendState
{
   specialTime runTime;
   bool state; 
};

class encodedMSG
{
  private:
     int currentCell;
  public:
     void addAction(specialTime, bool); 
     sendState * message;
     int length;
};

void encodedMSG::addAction(specialTime timeIn, bool stateIn)
{
   message [currentCell].runTime.setTime(timeIn.timeMillis, timeIn.timeMicros);
   message [currentCell].state = stateIn;
   currentCell ++; 
}

//*****Protocol Class Begins***
class Protocol {
private:
  void addStartPulse(int);
  int startPulseTime;
  int highLength;
  int lowLength;
  int bitSpacing;
public:
    Protocol (int, int, int, int);
    encodedMSG encode(unsigned long, int);
};

Protocol::Protocol(int startPulseTimeIn, int highLengthIn, int lowLengthIn, int bitSpacingIn)
{
   startPulseTime = startPulseTimeIn; 
   highLength = highLengthIn;
   lowLength = lowLengthIn;
   bitSpacing = bitSpacingIn;
}

encodedMSG Protocol::encode(unsigned long message, int msgLength)
{
   encodedMSG returnMessage;
   specialTime currentTime;
   currentTime.setTime(millis(), micros());
   int stateChanges = (msgLength + 1) * 2;
   returnMessage.message = new sendState [stateChanges];
   returnMessage.length = stateChanges;
   returnMessage.addAction(currentTime, 1);
   currentTime.addTime(startPulseTime);
   returnMessage.addAction(currentTime, 0);
   for(int i = 0; i<msgLength; i++)
   {
     currentTime.addTime(bitSpacing);
     returnMessage.addAction(currentTime, 1);
     currentTime.addTime(((message & i) ? highLength : lowLength));
     returnMessage.addAction(currentTime, 0);
     
   }
   return returnMessage;
}

//*****Protocol Class Ends***

//*****Sender Class Begins*****
class Sender {
private:
  short pin;
  vector <sendState> sendStates;
  Protocol * prot;
public:
  Sender(short,Protocol *);
  void sendMsg(unsigned long, int);
  void run();

};

Sender::Sender (short pinIn,Protocol * protIn){
  pin=pinIn;
  prot=protIn;
  pinMode(pin,OUTPUT);

}
void Sender::sendMsg(unsigned long msg, int msgLength)
{

  encodedMSG msgStates = prot -> encode(msg, msgLength);

    for (int i=0;i<msgStates.length;i++){
      sendStates.push_back(msgStates.message[i]);
      }

}

void Sender::run ()
{
  specialTime currentTime;
  currentTime.setTime(millis(), micros());
  for(int i = 0; i < sendStates.size(); i++)
  {
    if(currentTime.lessThan(sendStates.at(i).runTime))
    {
        digitalWrite(pin, sendStates.at(i).state);
        sendStates.erase(sendStates.begin() + i);
    }
  }
}
  //*****Sender Class Ends*****
//*******IR Object Class Begins*****

class IRObject
{
  private:
    vector<Sender> sendObjects;
  public:
    void run();
    void runSendObjects();
    int addSender(Sender);
    void removeSender(int);
};

void IRObject::run()
{
  runSendObjects();
}

void IRObject::runSendObjects()
{
  for(int i = 0; i<sendObjects.size(); i++)
  {
     sendObjects.at(1).run();
  }
}

int IRObject::addSender(Sender senderIn)
{
  sendObjects.push_back(senderIn);
  return sendObjects.size() - 1;
}

void IRObject::removeSender(int idIn)
{
  sendObjects.erase(sendObjects.begin() + idIn);
}
//******IR Object Class Ends******

void loop()
{
  
}
void setup()
{
  
}
