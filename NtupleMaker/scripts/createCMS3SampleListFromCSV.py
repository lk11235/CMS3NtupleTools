#!/bin/env python

import sys
import imp
import copy
import os
import filecmp
import shutil
import pickle
import math
import pprint
import subprocess
import csv
from datetime import date
from optparse import OptionParser
from CMSDataTools.AnalysisTree.TranslateStringBetweenPythonAndShell import *
from CMSDataTools.AnalysisTree.eostools import listFiles


class BatchManager:
   def __init__(self):
      # define options and arguments ====================================
      self.parser = OptionParser()

      self.parser.add_option("--csv", type="string", help="CSV file to expand")
      self.parser.add_option("--outfile", type="string", help="Output file to write")
      self.parser.add_option("--method", type="string", default="dbs", help="Method to list the data files")
      self.parser.add_option("--options", type="string", default=None, help="Other options specific to each method")
      self.parser.add_option("--nfiles", type="int", default=-1, help="Limit on the number of files per process")

      (self.opt,self.args) = self.parser.parse_args()

      optchecks=[
         "csv",
         "outfile"
         ]
      for theOpt in optchecks:
         if not hasattr(self.opt, theOpt) or getattr(self.opt, theOpt) is None:
            sys.exit("Need to set --{} option".format(theOpt))

      self.infile = self.opt.csv
      self.outfile = self.opt.outfile

      self.run()


   def run(self):
      firstLine=True
      indices = []
      with open(self.outfile,"wb") as outfile:
         with open(self.infile,"rb") as csvfile:
            csvreader = csv.reader(csvfile)
            for row in csvreader:
               rowstr = ','.join(row)
               rowstr.lstrip()
               if firstLine:
                  for el in row:
                     el=el.lstrip()
                     el=el.rstrip()
                     el=el.replace('#','')
                     indices.append(el)
                  firstLine=False
               elif rowstr.startswith('#'):
                  continue
               elif (len(row)==0):
                  continue
               else:
                  slines = []
                  strsample=row[0]
                  strsample = strsample.lstrip()
                  strsample = strsample.rstrip()
                  row[-1] = row[-1].split('#')[0].rstrip()

                  ffoutcore = strsample
                  ffoutcore = ffoutcore.replace('/MINIAODSIM','')
                  ffoutcore = ffoutcore.replace('/MINIAOD','')
                  ffoutcore = ffoutcore.lstrip('/')
                  condorffout = ffoutcore
                  ffoutcore = ffoutcore.replace('/','_')

                  print "Checking {}".format(strsample)
                  filelist = listFiles(
                     sample = strsample,
                     path = self.opt.method,
                     rec = True,
                     other_options = self.opt.options
                     )
                  index_ff=0
                  for ff in filelist:
                     stroutlist=[]
                     ffout = "{}_{}.root".format(ffoutcore,index_ff)
                     for ix in range(len(row)):
                        if ix == 0:
                           stroutlist.append('inputs={}'.format(ff))
                           stroutlist.append('output={}'.format(ffout))
                        elif ix == len(row)-1:
                           stroutlist.append(row[ix])
                        elif indices[ix] == "condoroutdir":
                           stroutlist.append('{}={}/{}'.format(indices[ix],row[ix],condorffout))
                        else:
                           stroutlist.append('{}={}'.format(indices[ix],row[ix]))
                     strout = " ".join(stroutlist)
                     print strout
                     outfile.write(strout+'\n')
                     index_ff = index_ff+1
                     if self.opt.nfiles>0 and index_ff == self.opt.nfiles:
                        break




if __name__ == '__main__':
   batchManager = BatchManager()
