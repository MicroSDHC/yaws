#include <JeeLib.h>
#include <DHT22.h>

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

#define DHT22_POWER_PIN 4
#define DHT22_PIN       7
#define OUTDOOR_NODE_ID 20
#define BASE_STATION_ID 14
#define LIGHT_SENSOR_POWER_PIN 8
#define LIGHT_SENSOR_PIN A2

#define DHT_ERROR_CNT_MAX 10
#define ACK_TIME          10
#define SEND_RETRY_LIMIT 10

struct {
  int temp;
  int humi;
  long vcc;
  int light;
  bool dht_error;
} payload;

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
        if (rf12_recvDone() && rf12_crc == 0 &&
                rf12_hdr == (RF12_HDR_DST | RF12_HDR_CTL | OUTDOOR_NODE_ID))
            return 1;
    }
    return 0;
}

void readLight()
{
  int sensorValue = analogRead(LIGHT_SENSOR_PIN);
  float voltage = sensorValue * (5000 / 1023.0);
  payload.light = (int)voltage;
}

void setup () {
  rf12_initialize(OUTDOOR_NODE_ID, RF12_433MHZ, 1);
  disableDHT22();
  disableLight();
  blinkLed();
}


void readDHT()
{ 
  byte retry_count = 0;
  DHT22_ERROR_t errorCode;
  enableDHT22();  
  errorCode = DHT_ERROR_CHECKSUM;
  while (errorCode != DHT_ERROR_NONE | retry_count < DHT_ERROR_CNT_MAX) {
      errorCode = myDHT22.readData();
      delay(2000);
      retry_count++;
  }
  if (errorCode == DHT_ERROR_NONE)
  {
    payload.temp = myDHT22.getTemperatureCInt();
    payload.humi = myDHT22.getHumidityInt();
    payload.dht_error = false;
  } else {
    payload.dht_error = true;
  }
  disableDHT22();
}

void loop () {
  readDHT();
  readVcc();
  readLight();
  for (byte i = 0; i < SEND_RETRY_LIMIT; i++)
  {
    rf12_sleep(RF12_WAKEUP);
    rf12_sendNow(BASE_STATION_ID, &payload, sizeof(payload));
    rf12_sendWait(2);
    byte acked = waitForAck();  
    rf12_sleep(RF12_SLEEP);
    if (acked)
    {
      return;
    }
    delay(1000);
  }
  // go to sleep for approx 60 * 5 seconds
  Sleepy::loseSomeTime(60000 * 5);
}

