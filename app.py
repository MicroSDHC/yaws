from flask import Flask, request, render_template, Response
from gevent import monkey
from socketio import socketio_manage
from socketio.namespace import BaseNamespace
from socketio.server import SocketIOServer
import json

app = Flask(__name__)
monkey.patch_all()


class ShoutsNamespace(BaseNamespace):
    sockets = {}

    def recv_connect(self):
        print "Got a socket connection"  # debug
        self.sockets[id(self)] = self

    def disconnect(self, *args, **kwargs):
        print "Got a socket disconnection"  # debug
        if id(self) in self.sockets:
            del self.sockets[id(self)]
        super(ShoutsNamespace, self).disconnect(*args, **kwargs)
    # broadcast to all sockets on this channel!

    @classmethod
    def broadcast(self, event, message):
        for ws in self.sockets.values():
            ws.emit(event, message)


@app.route('/socket.io/<path:rest>')
def push_stream(rest):
    try:
        socketio_manage(request.environ, {'/shouts': ShoutsNamespace}, request)
    except:
        app.logger.error("Exception while handling socketio connection", exc_info=True)
    return Response()


@app.route('/data', methods=['POST'])
def add_data():
    sensors = json.loads(request.data)
    ShoutsNamespace.broadcast('sensors_update', sensors)
    return json.dumps({'result': 'OK'})


@app.route('/', methods=['GET'])
def index():
    return render_template('index.html')


if __name__ == '__main__':
    port = 8080
    SocketIOServer(('', port), app, resource="socket.io").serve_forever()
