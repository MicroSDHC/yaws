#include <JeeLib.h>
#include <DHT22.h>
#include <stdio.h>

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

#define DHT22_POWER_PIN 4
#define DHT22_PIN       7
#define OUTDOOR_NODE_ID 20
#define BASE_STATION_ID 14
#define LIGHT_SENSOR_POWER_PIN 8
#define LIGHT_SENSOR_PIN A2

DHT22 myDHT22(DHT22_PIN);

void disableDHT22() {
  pinMode(DHT22_POWER_PIN, OUTPUT);
  digitalWrite(DHT22_POWER_PIN, LOW);
  pinMode(DHT22_POWER_PIN, INPUT);
}

void enableDHT22() {
  pinMode(DHT22_POWER_PIN, OUTPUT);
  digitalWrite(DHT22_POWER_PIN, HIGH);
  delay(2000);
}

void disableLight()
{
  pinMode(LIGHT_SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(LIGHT_SENSOR_POWER_PIN, LOW);
  pinMode(LIGHT_SENSOR_POWER_PIN, INPUT);  
}

void enableLight()
{
  pinMode(LIGHT_SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(LIGHT_SENSOR_POWER_PIN, HIGH);
  delay(2000);
}

void blinkLed() {
  digitalWrite(9, HIGH);
  delay(100);
  digitalWrite(9, LOW);
}

long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

void setup () {
  rf12_initialize(OUTDOOR_NODE_ID, RF12_433MHZ, 1);
  disableDHT22();
  disableLight();
  blinkLed();
}

int readLight()
{
  int sensorValue = analogRead(LIGHT_SENSOR_PIN);
  float voltage = sensorValue * (5000 / 1023.0);
  return (int)voltage;
}

int readDHT(char* buf)
{ 
  int l;
  DHT22_ERROR_t errorCode;
  enableDHT22();
  enableLight();  
  errorCode = myDHT22.readData();
  switch(errorCode)
  {
    case DHT_ERROR_NONE:
      sprintf(buf, "TEMP %hi.%01hi C, HUM %i.%01i %% RH, LUM %i mv, VCC %i mv\n",
                   myDHT22.getTemperatureCInt()/10, abs(myDHT22.getTemperatureCInt()%10),
                   myDHT22.getHumidityInt()/10, myDHT22.getHumidityInt()%10, readLight(), readVcc());
      break;
    case DHT_ERROR_CHECKSUM:
      break;
    case DHT_BUS_HUNG:
      break;
    case DHT_ERROR_NOT_PRESENT:
      break;
    case DHT_ERROR_ACK_TOO_LONG:
      break;
    case DHT_ERROR_SYNC_TIMEOUT:
      break;
    case DHT_ERROR_DATA_TIMEOUT:
      break;
    case DHT_ERROR_TOOQUICK:
      break;
  }
  disableDHT22();
  disableLight();
  return errorCode;
}


void loop () {
  rf12_sleep(RF12_WAKEUP);
  char buf[160];
  readDHT(buf);
  rf12_sendNow(BASE_STATION_ID, buf, strlen(buf));
  rf12_sendWait(1);
  rf12_sleep(RF12_SLEEP);
  // go to sleep for approx 60 seconds
  Sleepy::loseSomeTime(300000);
}
