import io
import os
import sys
import dispatch

import BBDestination

def main():
    try:
        opts, args = getopt.getopt(
            sys.argv[1:], 'r:i:f:', ['remotehost=', 'id=', 'filename='])
    except getopt.error, msg:
        print "Error : " + msg
        sys.exit(1)

  harken = dispatch.Signal(providing_args=['id', 'remotehost', 'filename'])
  harken.connect(Apply);

  while (1):
    time.sleep(0.05);

if __name__ == '__main__':
  main()
  sys.exit(0)
