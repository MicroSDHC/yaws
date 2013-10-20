#include <JeeLib.h>
#include <DHT22.h>
#include <PortsBMP085.h>

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

#define DHT22_POWER_PIN 4
#define DHT22_PIN       7
#define INDOOR_NODE_ID  21
#define BASE_STATION_ID 14

#define DHT_ERROR_CNT_MAX 10
#define ACK_TIME          1000

#define SEND_RETRY_LIMIT 10

DHT22 myDHT22(DHT22_PIN);
PortI2C i2cport (2);
BMP085 psensor (i2cport, 3);

struct {
  int temp;
  int humi;
  int32_t pressure;  
  long vcc;
  bool dht_error;
} payload;

void disableDHT22() {
  pinMode(DHT22_POWER_PIN, OUTPUT);
  digitalWrite(DHT22_POWER_PIN, LOW);
  pinMode(DHT22_POWER_PIN, INPUT);
}

void enableDHT22() {
  pinMode(DHT22_POWER_PIN, OUTPUT);
  digitalWrite(DHT22_POWER_PIN, HIGH);
  Sleepy::loseSomeTime(2000);
}

void blinkLed() {
  digitalWrite(9, HIGH);
  Sleepy::loseSomeTime(100);
  digitalWrite(9, LOW);
}


void readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  payload.vcc = result;
}


static byte waitForAck() {
    MilliTimer ackTimer;
    while (!ackTimer.poll(ACK_TIME)) {
              if (rf12_recvDone() && rf12_crc == 0 && rf12_hdr == INDOOR_NODE_ID)
            return 1;
    }
    return 0;
}

void setup () {
  rf12_initialize(INDOOR_NODE_ID, RF12_433MHZ, 1);
  psensor.getCalibData();
  disableDHT22();
  blinkLed();
}

void readDHT()
{ 
  byte retry_count = 0;
  DHT22_ERROR_t errorCode;
  enableDHT22();  
  errorCode = DHT_ERROR_CHECKSUM;
  while (retry_count < DHT_ERROR_CNT_MAX) {
      errorCode = myDHT22.readData();
      Sleepy::loseSomeTime(2000);
      retry_count++;
      Serial.println(errorCode);
      Serial.println(DHT_ERROR_NONE);
      if (errorCode == DHT_ERROR_NONE)
      {
        break;
      }
  }
  if (errorCode == DHT_ERROR_NONE)
  {
    payload.temp = myDHT22.getTemperatureCInt();
    payload.humi = myDHT22.getHumidityInt();
    Serial.println(payload.temp);
    Serial.println(payload.humi);
    payload.dht_error = false;
  } else {
    payload.dht_error = true;
  }
  disableDHT22();
}

void readPres()
{
    psensor.startMeas(BMP085::PRES);
    Sleepy::loseSomeTime(32);
    payload.pressure = psensor.getResult(BMP085::PRES);
}

void loop () {
  readDHT();
  readVcc();
  for (byte i = 0; i < SEND_RETRY_LIMIT; i++)
  {
    rf12_sleep(RF12_WAKEUP);
    rf12_sendNow(BASE_STATION_ID, &payload, sizeof(payload));
    rf12_sendWait(1);
    byte acked = waitForAck(); 
    rf12_sleep(RF12_SLEEP);
    if (acked)
    {
      break;
    }
    Sleepy::loseSomeTime(1000);
  }
  // go to sleep for approx 60
  for (int idx = 0; idx < 5; idx++)
    Sleepy::loseSomeTime(60000);
}
