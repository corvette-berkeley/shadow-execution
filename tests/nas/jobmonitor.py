#!/usr/bin/python
import os
import subprocess
import time
import sys
import time
import signal

## ######################################################
## Run the job [job] and return the timing and memory
## statistics. Simply a handy wrapper.
def RunJob(job,timeout=None,name=None,stdin=None,stdout=None,stderr=None,shell=True):
    jm = JobMonitor(job,timeout,name,stdin,stdout,stderr,shell)
    jm.Run()
    jm.printResult()
    return jm.Result()

## #######################################################
## class MemMonitor
## Given a pid, read /proc/<pid>/status and returns
## memory, resident, or stacksize usage
class MemMonitor:
    _scale = {'kB': 1024.0, 'mB': 1024.0*1024.0,
              'KB': 1024.0, 'MB': 1024.0*1024.0}

    def __init__(self,pid):
        self.pid = pid
        self._proc_status = '/proc/%d/status' % pid

    def _VmB(self,VmKey):
        '''Private.
        '''
        #global _proc_status, _scale
         # get pseudo file  /proc/<pid>/status
        try:
            t = open(self._proc_status)
            v = t.read()
            t.close()
            # print v
            # get VmKey line e.g. 'VmRSS:  9999  kB\n ...'
            i = v.index(VmKey)
            v = v[i:].split(None, 3)  # whitespace
            if len(v) < 3:
                return 0.0  # invalid format?
            # convert Vm value to bytes
            return float(v[1]) * MemMonitor._scale[v[2]]
        except e:
            print('  Error ::> ',e)
            return 0.0  # non-Linux?

    def memory(self,since=0.0):
        '''Return memory usage in bytes.
        '''
        return self._VmB('VmSize:') - since


    def resident(self,since=0.0):
        '''Return resident memory usage in bytes.
        '''
        return self._VmB('VmRSS:') - since


    def stacksize(self,since=0.0):
        '''Return stack size in bytes.
        '''
        return self._VmB('VmStk:') - since

## ########################################################
## Runs a shell command [cmd] and waits for completion.
## Records :
##   maximum memory used
##   user-execution time
##   return code
## @return (cmd, return code , time to execute)
class JobMonitor:
    Debug = False

    def __init__(self,cmd,
        timeout=None,
        name=None,
        stdin=None,
        stdout=None,
        stderr=None,
        shell=True):
        self.cmd     = cmd
        if name == None:
          self.name = cmd
        else:
          self.name = name
        self.rc      = 0
        self.time    = 0
        self.max     = 0
        self.start   = 0
        self.timeout = timeout
        self.pgid    = 0
        self.stdin   = stdin
        self.stdout  = stdout
        self.stderr  = stderr
        self.shell   = shell

    def Result(self):
        return (self.cmd , self.rc , self.time , self.max)

    def printResult(self):
        print('stats, %s' % self.cmd + ', ' + str(self.time) + ', ' + str(self.max))

    def Command(self):
        return self.cmd

    def ReturnCode(self):
        return self.rc

    def Time(self):
        return self.time

    def MaxMemory(self):
        return self.max

    def Run(self):
        if JobMonitor.Debug:
          print('--- Executing "%s"\n' % self.cmd)
        try:
            self.start = time.time()
            p = subprocess.Popen( self.cmd ,
                stdin  = self.stdin,
                stdout = self.stdout,
                stderr = self.stderr,
                shell  = self.shell)
            if JobMonitor.Debug:
                print('  Pid :> ',p.pid)
            monitor = MemMonitor(p.pid)
            self.rc = p.poll()
            while( self.rc == None ):
                mem = monitor.memory()
                if JobMonitor.Debug:
                    print('  Mem :> ',mem)
                if (mem > self.max):
                    self.max = mem
                time.sleep(1)
                self.time = time.time() - self.start
                self.rc = p.poll()
                if (self.timeout != None and self.time >= self.timeout):
                  break
            # Check for timeout
            if (self.rc == None):
              os.kill(p.pid,signal.SIGKILL)
              killedpid,stat = os.waitpid(p.pid,0)#,os.WNOHANG)
              if killedpid == 0:
                print >> sys.stderr, "TODO: FIXME"
            if self.rc == None: #JobMonitor.Debug:
              print(' %s(%d) :> %f s for %s' % (('Completion','Timeout')[self.rc == None],p.pid,self.time,self.name))
        except OSError as e:
            print >>sys.stderr, "  >> Execution failed:",e
            self.rc = self.time = self.max = -60

