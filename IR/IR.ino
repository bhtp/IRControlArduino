#include <IR.cpp>
void setup()
{
  Serial.begin(9600);
  Protocol sony (2400, 1200, 600, 600);
  IRObject defaultObject;
  Sender testSender(10, &sony);
  defaultObject.addSender(testSender);
}

void loop()
{
  defaultObject.run();
}