#include "Arduino.h"
#include "IR.h"
#define DEBUGGING
//****SpecialTime Class Begins
void SpecialTime::setTime(unsigned long millisIn, unsigned long microsIn)
{
	timeMillis = millisIn;
	timeMicros = microsIn;
}

void SpecialTime::addTime(unsigned long microsIn)
{
	timeMillis += microsIn / 1000;
	timeMicros += microsIn;
}

bool SpecialTime::lessThan(SpecialTime timeIn)
{
	return (timeIn.timeMillis > timeMillis && timeIn.timeMicros > timeMicros); 
}
//**SpecialTime Class Ends
//**EncodedMSG Class Begins
void EncodedMSG::addAction(SpecialTime timeIn, bool stateIn)
{
	message [currentCell].runTime.setTime(timeIn.timeMillis, timeIn.timeMicros);
	message [currentCell].state = stateIn;
	currentCell ++; 
}
//**EncodedMSG Class Ends
//*****Protocol Class Begins***
Protocol::Protocol(int startPulseTimeIn, int highLengthIn, int lowLengthIn, int bitSpacingIn)
{
	startPulseTime = startPulseTimeIn; 
	highLength = highLengthIn;
	lowLength = lowLengthIn;
	bitSpacing = bitSpacingIn;
}

EncodedMSG Protocol::encode(unsigned long message, int msgLength)
{
	EncodedMSG returnMessage;
	SpecialTime currentTime;
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
Sender::Sender (short pinIn, Protocol * protIn){
	pin = pinIn;
	prot = protIn;
	pinMode(pin,OUTPUT);
}
void Sender::sendMsg(unsigned long msg, int msgLength)
{
	EncodedMSG msgStates = prot -> encode(msg, msgLength);
		for (int i=0;i<msgStates.length;i++)
		{
			sendStates.push_back(msgStates.message[i]);
		}
}

void Sender::run ()
{
	SpecialTime currentTime;
	currentTime.setTime(millis(), micros());
	for(int i = 0; i < sendStates.size(); i++)
	{
		if(currentTime.lessThan(sendStates.at(i).runTime))
		{
			digitalWrite(pin, sendStates.at(i).state);
			sendStates.erase(sendStates.begin() + i);
			#ifdef DEBUGGING
				Serial.println(sendStates.at(i).state);
				Serial.println(micros());
				Serial.println(millis());
			#endif
		}
	}
}
//*****Sender Class Ends*****
//*******IR Object Class Begins*****
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