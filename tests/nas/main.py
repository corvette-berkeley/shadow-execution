#!/usr/bin/env python2

# fs-copy dir

import jobmonitor
import sys

args=['./' + sys.argv[1]]

print jobmonitor.RunJob(args,shell = False)


