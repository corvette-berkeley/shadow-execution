#!/usr/bin/env python

# fs-copy dir

import jobmonitor
import sys

args=['./' + sys.argv[1]]

print jobmonitor.RunJob(args,shell = False)


