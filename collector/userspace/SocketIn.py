import sys
import socket
import SocketServer
import threading
import BBPy

class MessageHandler(SocketServer.BaseRequestHandler):
  def handle(self):
    data = self.request.recv(4096)
    print "I need to put " + data + " into mongo"
    self.request.close()

class Server(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
  daemon_threads = True
  allow_reuse_address = True

  def __init__(self, addr, RequestHandlerClass):
    SocketServer.TCPServer.__init__(self, addr, RequestHandlerClass)

server = Server(("localhost", 6667), MessageHandler)
server.serve_forever()

