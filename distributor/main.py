import io
import os
import sys
import paho.mqtt.client as mqtt
import BBPy
import BBDestination

def on_connect():
  pass

def main():
  try:
    opts, args = getopt.getopt(
      sys.argv[1:], 'r:i:f:', ['remotehost=', 'id=', 'filename='])
  except getopt.error, msg:
      print "Error : " + msg
      sys.exit(1)

  for opt, arg in opts:
    if opt in ('-r', '--remotehost'):
      remotehost = arg
    elif opt in ('-i', '--id'):
      id = arg
    elif opt in ('-f', '--filename'):
      filename = arg
    else
      fatal("Unknown argument " + arg)

  #
  # This is a subscriber (destination) host. There is an individual
  # instance of each of these proesses for each source host, and we
  # care about all bbfsevent events
  #
  client = mqtt.Client()
  client.reinitialise(client_id="", clean_session=True, userdata=None)
  client.connect(remotehost)
  client.subscribe("bb/fsevent")
  client.loop_start()
  
  while (1):
    time.sleep(0.05);

if __name__ == '__main__':
  main()
  sys.exit(0)
