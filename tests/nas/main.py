#!/usr/bin/env python2

# fs-copy dir

import jobmonitor
import subprocess
import sys

args=['valgrind', '--soname-synonyms=somalloc=*tcmalloc*', './' + sys.argv[1]]
f = open('last_run_log.txt', 'w')
result = jobmonitor.RunJob(args,shell = False, stdout=f, stderr=subprocess.STDOUT)
f.close()

f = open('last_run_log.txt', 'r')
for a in f:
	print(a[0:-1])
print(result)

