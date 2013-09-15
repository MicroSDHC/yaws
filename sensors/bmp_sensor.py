import sensor
from Adafruit_BMP085 import BMP085
import time

clsname = 'BMPSensor'

class BMPSensor(sensor.Sensor):

    def setup(self, **kw):
        addr = kw.get('addr', 0x77)
        self._interval = kw.get('interval', 60)
        self._bmp = BMP085(addr)
        self._data = {}

    def get_data(self):
        return self._data

    def run(self):
        while not self._stop.isSet():
            self._data['TEMP'] = self._bmp.readTemperature()
            self._data['PRESSURE'] = int(self._bmp.readPressure() / 133.322)
            self._stop.wait(self._interval)

