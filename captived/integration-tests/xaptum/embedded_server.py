import atexit
import os
import subprocess

CWD = os.path.abspath(os.path.join(os.pardir, os.pardir))
BIN  = os.path.join(CWD, 'captived')

DEVNULL = open(os.devnull, 'wb')
atexit.register(lambda: DEVNULL.close())

class Embedded_Server(object):

    def __init__(self, cwd=CWD, bin=BIN, args=None, quiet=False):
        self.cwd = cwd
        self.bin  = bin
        self.args = args if args else []
        self.quiet = quiet

        self.process = None

    def start(self):
        args   = [self.bin] + self.args
        stdout = DEVNULL if self.quiet else None
        stderr = DEVNULL if self.quiet else None
        self.process = subprocess.Popen(args, cwd=self.cwd, stdout=stdout, stderr=stderr)

    def terminate(self):
        self.process.terminate()
        outs, errs = self.process.communicate()

    def kill(self):
        self.process.kill()
        outs, errs = self.process.communicate()

    def is_stopped(self):
        return self.process.poll() != None

    def pid(self):
        return self.process.pid
