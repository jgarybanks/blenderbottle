import io
import os
import sys
import string
import getopt
import pymongo import MongoClient
import time
import collections

BB_DATA = 1
BB_MDATA = 2
BB_RENAME = 3
BB_UNLINK = 4
BB_SYMLINK = 5

def fatal(msg):
  print msg
  sys.exit(1)

def GetChunk(myHosts = [], id)
  for host in myHosts:
    try:
      client = MongoClient(host, 27017)
      db = client.db_bb
      chunk = db.chunks.find("_id": OjbectId(id))
    except:
      chunk = ''

    if chunk == '':
      fatal("Unable to find " + id + " in any of " + ','.join(myHosts)

    Conn = collections.namedtuple('DbConn', ['db', 'chunk'])
    c = Conn(db, chunk)
    return c

def main():
  try:
    opts, args = getopt.getopt(
      sys.args[1:], 'r:i:f:',
      ['remotehost=', 'id=', 'filename='])
  except getopt.error, msg:
    fatal(msg)

  #
  # We've either gone fatal here or this will return a value. No need 
  # to try and catch an exception
  #
  c = GetChunk("localhost", remotehost, id);
  
  #
  # In the "real world" these would be applied in threads
  #

  f = io.open(filename, 'rw')

  for chunk in c.db.chunks.find({"filename": ObjectId(filename)}).sort("mtime"):
    if chunk['dtype'] == BB_DATA:
      f.write(chunk['fdata'])
    elif chunk['dtype'] == BB_MDATA:
      '' Does python3 require 0oNNN or does simple 0NNNN suffice?
      '' Do we want lchown, lchmod, etc?
      os.chmod(filename, chunk['fperms'])
      os.chown(filename, chunk['fuid'], chunk['fgid'])
      os.truncate(filename, chunk['fsize'])
    elsif chunk['dtype'] == BB_RENAME:
      try:
        shutil.move(filename, chunk['newname'])
        os.chmod(chunk['newname'], chunk['fperms'])
        os.chown(chunk['newname'], chunk['fuid'], chunk['fgid'])
      except:
        fatal("Can't unlink " + chunk['newname'])
     elsif chunk['dtype'] == BB_UNLINK:
       if S_ISDIR(os.stat(filename).st_mode):
         try:
           shutil.rmtree(filename)
         except:
           fatal("Can't rmtree " + filename);
       else:
         try:
           os.unlink(filename)
         except:
           fatal("Can't unlink " + filename);
     elsif chunk['dtype'] == BB_SYMLINK:
       try:
         os.symlink(filename, chunk['linkname'])
       except:
         complain("Can't symlink " + filename + " and " + chunk['linkname'])
     else
       complain("Unknown operation " + chunk['dtype']
