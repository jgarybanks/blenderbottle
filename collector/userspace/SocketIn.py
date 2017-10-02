import sys
import socket
import SocketServer
import threading
import paho.mqtt.client as mqtt
import BBPy

class MessageHandler(SocketServer.BaseRequestHandler):
  def Publish(id)
    # Just send it to the broker on our local host
    single("bb/fsevent", payload=id)
    
  def handle(self):
    data = self.request.recv(4096)
    id = BBStore(data)
    Publish(id)
    # self.request.close()

class Server(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
  daemon_threads = True
  allow_reuse_address = True

  def __init__(self, addr, RequestHandlerClass):
    SocketServer.TCPServer.__init__(self, addr, RequestHandlerClass)

server = Server(("localhost", 6667), MessageHandler)
server.serve_forever()

