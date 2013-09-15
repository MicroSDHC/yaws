#include <JeeLib.h>
#include <DHT22.h>
#include <stdio.h>

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

#define DHT22_POWER_PIN 4
#define DHT22_PIN       7
#define OUTDOOR_NODE_ID 20
#define BASE_STATION_ID 14

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
  blinkLed();
}

int readDHT(char* buf)
{ 
  DHT22_ERROR_t errorCode;
  enableDHT22();  
  errorCode = myDHT22.readData();
  switch(errorCode)
  {
    case DHT_ERROR_NONE:
      sprintf(buf, "TEMP %hi.%01hi C, HUM %i.%01i %% RH, VCC %i mv\n",
                   myDHT22.getTemperatureCInt()/10, abs(myDHT22.getTemperatureCInt()%10),
                   myDHT22.getHumidityInt()/10, myDHT22.getHumidityInt()%10, readVcc());
      Serial.println(buf);
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
  return errorCode;
}

void loop () {
  rf12_sleep(RF12_WAKEUP);
  char buf[128];
  readDHT(buf);
  rf12_sendNow(BASE_STATION_ID, buf, strlen(buf));
  rf12_sendWait(1);
  rf12_sleep(RF12_SLEEP);
  // go to sleep for approx 60 seconds
  Sleepy::loseSomeTime(60000);
}
