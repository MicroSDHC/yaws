import os
import fcntl
import struct
import sensor
import time
import select

clsname = 'RFM12Sensor'

DEV_NAME = '/dev/rfm12b.0.1'
NODE_ID = 14
SET_JEE_ID = 0x40047209
BUF_LEN = 128
HDR_SIZE = 2
OUTDOOR_NODE_ADDR = 0x14
INDOOR_NODE_ADDR = 0x15

def parse_outdoor_node(instr):
    rv = {}
    elems = [x.strip() for x in instr.split(',') ]
    for e in elems:
        splt = e.split()
        rv[splt[0]] = splt[1]
    return rv


def open_rf(devname, node_id):
    fd = os.open(devname, os.O_RDWR)
    buf = struct.pack('I', node_id)
    fcntl.ioctl(fd, SET_JEE_ID, buf)
    return fd


class RFM12Sensor(sensor.Sensor):

    def setup(self, **kw):
        dev_name = kw.get('dev_name', DEV_NAME)
        node_id = kw.get('node_id', NODE_ID)
        self._fd = open_rf(dev_name, node_id)
        self._data = {}

    def get_data(self):
        return self._data

    def run(self):
        while not self._stop.isSet():
            reads, _, _ = select.select([self._fd], [], [], 2.0)
            if 0 < len(reads):
                data = os.read(self._fd, BUF_LEN)
                hdr = data[:HDR_SIZE]
                addr = ord(hdr[0]) & 0x1f
                payload = data[HDR_SIZE:]
                res = {}
                if addr == OUTDOOR_NODE_ADDR:
                    self._data['OUTDOOR'] = parse_outdoor_node(payload)
                if addr == INDOOR_NODE_ADDR:
                    self._data['INDOOR'] = parse_outdoor_node(payload)
 

if __name__ == '__main__':
    sobj = RFM12Sensor("rfm12")
    sobj.start()
    while True:
        print sobj.get_data()        
        time.sleep(10)
