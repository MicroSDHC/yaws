import requests
import time
from datetime import datetime
import json
from random import random, randint

while True:
    sensors = {
        'temp': '{0:.1f}'.format(random() * 100.0),
        'humidity': '{0:.1f}'.format(random() * 100.0),
        'pressure': randint(730, 780),
        'time': datetime.utcnow().isoformat()
    }
    requests.post('http://localhost:8080/data', data=json.dumps(sensors))
    time.sleep(3)
