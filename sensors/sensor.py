import threading

class Sensor(threading.Thread):

    def __init__(self, name, **kw):
        threading.Thread.__init__(self)
        self.name = name
        self._stop = threading.Event()
        self.setup(**kw)

    def setup(self, **kw):
        raise NotImplementedError

    def get_data(self):
        raise NotImplementedError

    def run(self):
        raise NotImplementedError

    def stop(self):
        self._stop.set()
        self.join()
