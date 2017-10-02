import io
import os
import sys
import string
import getopt
import pymongo import MongoClient
import time
import BBPy

class BBDestination:

def OpenDb(myHosts = [], id)
  for host in myHosts:
    try:
      client = MongoClient(host, 27017)
      db = client.db_bb
      chunk = db.chunks.find("_id": OjbectId(id))
    except:
      chunk = ''

    if chunk == '':
      fatal("Unable to find " + id + " in any of " + ','.join(myHosts)

    return db

def Apply(client, userdata, message)

  #
  # We've either gone fatal here or this will return a value. No need 
  # to try and catch an exception
  #
  remotehost = client.payload
  id = str(message.payload)
  db = OpenDb("localhost", remotehost, id)
  
  for chunk in db.chunks.find({"_id": ObjectId(id)).sort("mtime"):
    if chunk['dtype'] == BB.BB_DATA:
      f = io.open(filename, rw)
      os.lseek(f, chunk['fpos'], 0)
      os.write(f, chunk['fdata'])
    elif chunk['dtype'] == BB.BB_MDATA:
      # Does python3 require 0oNNN or does simple 0NNNN suffice?
      # Do we want lchown, lchmod, etc?
      os.chmod(chunk['filename'], chunk['fperms'])
      os.chown(chunk['filename'], chunk['fuid'], chunk['fgid'])
      os.truncate(chunk['filename'], chunk['fsize'])
    elsif chunk['dtype'] == BB.BB_RENAME:
      try:
        shutil.move(chunk['filename'], chunk['newname'])
        os.chmod(chunk['newname'], chunk['fperms'])
        os.chown(chunk['newname'], chunk['fuid'], chunk['fgid'])
      except:
        fatal("Can't unlink " + chunk['newname'])
     elsif chunk['dtype'] == BB.BB_UNLINK:
       if S_ISDIR(os.stat(chunk['filename']).st_mode):
         try:
           shutil.rmtree(chunk['filename'])
         except:
           fatal("Can't rmtree " + chunk['filename']);
       else:
         try:
           os.unlink(chunk['filename'])
         except:
           fatal("Can't unlink " + chunk'filename']);
     elsif chunk['dtype'] == BB.BB_SYMLINK:
       try:
         os.symlink(chunk['filename'], chunk['linkname'])
       except:
         complain("Can't symlink " + chunk['filename'] + " and " + chunk['linkname'])
     else
       complain("Unknown operation " + chunk['dtype']
