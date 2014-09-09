#!/usr/bin/env python2

# fs-copy dir

import jobmonitor
import sys

args=['valgrind', './' + sys.argv[1]]

print jobmonitor.RunJob(args,shell = False)


