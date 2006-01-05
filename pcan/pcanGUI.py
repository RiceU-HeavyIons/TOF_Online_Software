#!/usr/bin/env python
#
# $Id: pcanGUI.py,v 1.2 2006-01-05 17:01:59 jschamba Exp $
#

rtitle = 'pcan GUI'
version = '1.1'
cvsVerString = '$Id: pcanGUI.py,v 1.2 2006-01-05 17:01:59 jschamba Exp $'

import os
import sys
import string
from Tkinter import *
#from ScrolledText import ScrolledText
#import signal
import Pmw

BUFSIZE = 512

class PcanGUI:

    def __init__(self, parent, pCmdArgs=None):

        pCmd = '/homes/jschamba/tof/pcan/pcanloop'
        if pCmdArgs:
            shell = pCmd + ' ' + pCmdArgs
        else:
            shell = pCmd
        args = string.split(shell)

        #print "shell", shell, "args", args

        self.tk = parent
        mainFrame = Frame(parent)
        self.cmdEntry = StringVar()
        entryBox = Pmw.EntryField(mainFrame,
                                  labelpos = W,
                                  label_text = "Entry:",
                                  command = self.exeCommand,
                                  entry_textvariable = self.cmdEntry)
        entryBox.pack(fill=X, expand=YES)


        #self.textBox = ScrolledText(mainFrame)
        self.textBox = Pmw.ScrolledText(mainFrame)
        self.textBox.pack(fill=BOTH, expand=YES)
        mainFrame.pack(fill=BOTH, expand=YES)

        self.pid, self.fromchild, self.tochild = spawn(pCmd, args)
        self.tk.createfilehandler(self.fromchild, READABLE,
                                  self.outputhandler)

    def outputhandler(self, file, mask):
        data = os.read(file, BUFSIZE)
        if not data:
            self.tk.deletefilehandler(file)
            pid, sts = os.waitpid(self.pid, 0)
            print 'pid', pid, 'status', sts
            self.pid = None
            detail = sts>>8
            cause = sts & 0xff
            if cause == 0:
                msg = "exit status %d" % detail
            else:
                msg = "killed by signal %d" % (cause & 0x7f)
                if cause & 0x80:
                    msg = msg + " -- core dumped"
            return
        #self.textBox.insert(END,data)
        self.textBox.appendtext(data)

    def exeCommand(self):
        os.system(self.cmdEntry.get())


MAXFD = 100     # Max number of file descriptors (os.getdtablesize()???)

def spawn(prog, args):
    p2cread, p2cwrite = os.pipe()
    c2pread, c2pwrite = os.pipe()
    pid = os.fork()
    if pid == 0:
        # Child
        for i in 0, 1, 2:
            try:
                os.close(i)
            except os.error:
                pass
        if os.dup(p2cread) <> 0:
            sys.stderr.write('popen2: bad read dup\n')
        if os.dup(c2pwrite) <> 1:
            sys.stderr.write('popen2: bad write dup\n')
        if os.dup(c2pwrite) <> 2:
            sys.stderr.write('popen2: bad write dup\n')
        for i in range(3, MAXFD):
            try:
                os.close(i)
            except:
                pass
        try:
            os.execvp(prog, args)
        finally:
            sys.stderr.write('execvp failed\n')
            os._exit(1)
    os.close(p2cread)
    os.close(c2pwrite)
    return pid, c2pread, p2cwrite

def test():
    pCmdArgs = string.join(sys.argv[1:])
    root = Tk()
    Pmw.initialise(root, size = 11, fontScheme = 'pmw1')
    tmpString = rtitle + " " + version
    root.title(tmpString)

    if pCmdArgs:
        w = PcanGUI(root, pCmdArgs=pCmdArgs)
    else:
        w = PcanGUI(root)

    root.mainloop()

if __name__ == '__main__':
    test()
