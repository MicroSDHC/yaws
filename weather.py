#!/usr/bin/python
import sys
import time
from datetime import datetime
import requests
import argparse
import logging
import logging.config
import json
import sensors


def load_log_config(filename='weather_log.json'):
    with open(filename) as fin:
        logging.config.dictConfig(json.loads(fin.read()))


class WeatherStation(object):

    def __init__(self, cfg):
        self._sensors = []
        for sensor in cfg['sensors']:
            sm = __import__('sensors.{}'.format(sensor['class']))
            m = getattr(sm, sensor['class'])
            sobj = getattr(m, m.clsname)(sensor['name'], **sensor['kw'])
            sobj.start()
            self._sensors.append(sobj)
        self.log = logging.getLogger('main')
        self._host = cfg['http']['host']
        self._port = cfg['http']['port']
        self.log.info('initialize WeatherStation')

    def update(self):
        payload = {}
        for sensor in self._sensors:
            payload[sensor.name] = sensor.get_data()
        dt = datetime.utcnow()
        try:
            self.log.debug('payload: {}'.format(payload))
            r = requests.post('http://{}:{}/sensors'.format(
                self._host, self._port), data=json.dumps(payload), timeout=10.0)
        except Exception:
            self.log.error('Can not connect to remote host')

    def stop(self):
        self.log.info('Try to stop sensors')
        for sensor in self._sensors:
            sensor.stop()

if __name__ == '__main__':
    load_log_config()
    cfg = json.loads(open('weather.json', 'r').read())
    ws = WeatherStation(cfg)

    while True:
        try:
            ws.update()
            time.sleep(cfg.get('interval', 60))
        except (KeyboardInterrupt, SystemExit):
            ws.stop()
            sys.exit(0)

