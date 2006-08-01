#!/usr/bin/env python
#
# $Id: runcontrol.py,v 1.2 2006-08-01 21:22:29 jschamba Exp $
#

rtitle = 'Cosmic Test Stand Run Control'
cvsVerString = '$Id: runcontrol.py,v 1.2 2006-08-01 21:22:29 jschamba Exp $'
version = cvsVerString.split()[2]

import os
import sys
import string
import time
import ConfigParser
import MySQLdb
from Tkinter import *
#from ScrolledText import ScrolledText
#import signal
import Pmw

BUFSIZE = 512
cfg = ConfigParser.ConfigParser()
cfg.read('runcontrol.cfg')

pcCmd = '/homes/jschamba/tof/pcan/pc'
fifofd = 0

class PcanGUI:

    def __init__(self, parent, pCmdArgs=None):

        pCmd = '/homes/jschamba/tof/pcan/pcanloop'
        if pCmdArgs:
            shell = pCmd + ' ' + pCmdArgs
        else:
            shell = pCmd
        args = string.split(shell)

        #print "shell", shell, "args", args

        self.setDefaultVals()
        

        
        self.tk = parent
        mainFrame = Frame(parent)
        run_type = Pmw.OptionMenu(mainFrame,
                                  labelpos = W,
                                  label_text = 'Run Type:',
                                  menubutton_textvariable = self.runTypeNum,
                                  items = self.runTypeOptions
                                  )
        user_name = Pmw.OptionMenu(mainFrame,
                                   labelpos = W,
                                   label_text = 'Operator:',
                                   menubutton_textvariable = self.userNum,
                                   items = self.userOptions
                                  )
        self.current_time = Pmw.EntryField(mainFrame,
                                           labelpos = W,
                                           label_text = "current time:",
                                           entry_state = "readonly",
                                           value = time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime())
                                           )
        self.run_number = Pmw.EntryField(mainFrame,
                                         labelpos = W,
                                         label_text = "run number:",
                                         modifiedcommand = self.fileChanged,
                                         entry_textvariable = self.runNum
                                         )
        
        self.start_time = Pmw.EntryField(mainFrame,
                                         labelpos = W,
                                         label_text = "start time:",
                                         entry_state = "readonly",
                                         value = time.strftime("%a, %d %b %Y %H:%M:%S",
                                                               time.localtime(self.startTime))
                                         )
        
        self.run_time = Pmw.EntryField(mainFrame,
                                       labelpos = W,
                                       label_text = "run time:",
                                       entry_state = "readonly",
                                       )

        if (self.runStatus == 2):
            self.run_time.setvalue(int(time.time()) - self.startTime)
        else:
            self.run_time.setvalue(self.stopTime - self.startTime)

        self.data_dir = Pmw.EntryField(mainFrame,
                                       labelpos = W,
                                       label_text = "data directory:",
                                       modifiedcommand = self.fileChanged,
                                       entry_textvariable = self.dataDir
                                       )
        
        self.data_file = Pmw.EntryField(mainFrame,
                                        labelpos = W,
                                        label_text = "data file:",
                                        entry_state = "readonly",
                                        value = self.dataFile
                                        )
                                                          
        self.log_dir = Pmw.EntryField(mainFrame,
                                      labelpos = W,
                                      label_text = "log directory:",
                                      modifiedcommand = self.fileChanged,
                                      entry_textvariable = self.logDir
                                      )
                                                          
        self.log_file = Pmw.EntryField(mainFrame,
                                       labelpos = W,
                                       label_text = "log file:",
                                       entry_state = "readonly",
                                       value = self.logFile
                                       )
                                                          


        allwdgs = [run_type,
                   user_name,
                   self.current_time,
                   self.run_number,
                   self.start_time,
                   self.run_time,
                   self.data_dir,
                   self.data_file,
                   self.log_dir,
                   self.log_file]
        for wdg in allwdgs:
            wdg.pack(fill=X, expand=1, padx=10, pady=2)

        Pmw.alignlabels(allwdgs)
        
        bb = Pmw.ButtonBox(mainFrame,
                           labelpos = W,
                           label_text = 'Run',
                           frame_borderwidth = 2,
                           frame_relief = 'groove')
        bb.pack(padx = 10, pady = 10)
        bb.add('Start', command = self.runStart)
        bb.add('Stop', command = self.runStop)
        bb.alignbuttons()
        
        self.cmdEntry = StringVar()
        entryBox = Pmw.EntryField(mainFrame,
                                  labelpos = W,
                                  label_text = "CAN Command Entry:",
                                  command = self.exeCommand,
                                  entry_textvariable = self.cmdEntry)
        entryBox.pack(fill=X, expand=YES)


        self.textBox = Pmw.ScrolledText(mainFrame)
        self.textBox.pack(fill=BOTH, expand=YES)
        mainFrame.pack(fill=BOTH, expand=YES)

        self.pid, self.fromchild, self.tochild = spawn(pCmd, args)
        tkinter.createfilehandler(self.fromchild, READABLE,
                                  self.outputhandler)

        parent.after(1000, self.secondElapsed)

    def outputhandler(self, file, mask):
        data = os.read(file, BUFSIZE)
        if not data:
            tkinter.deletefilehandler(file)
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
            sys.exit()
            return
        self.textBox.appendtext(data)

    def exeCommand(self):
        #print "executing ",  self.cmdEntry.get()
        os.system(self.cmdEntry.get())

    def setDefaultVals(self):
        self.db = MySQLdb.connect(user=cfg.get('Main', 'tsSql.User'),
                                  passwd=cfg.get('Main', 'tsSql.Passwd'),
                                  host=cfg.get('Main', 'tsSql.Host'),
                                  db=cfg.get('Main', 'tsSql.Db'))

        self.runTypeOptions = []
        self.userOptions = []
        self.runTypeDict = {}
        self.userDict = {}
        user_id = cfg.getint('Main', 'tsRun.user')
        run_type = cfg.getint('Main', 'tsRun.type')
        self.userNum = StringVar()
        self.runTypeNum = StringVar()

        cursor = self.db.cursor()
        cursor.execute("SELECT * FROM ut_run_type")
        for ut_run_type in cursor.fetchall():
            self.runTypeOptions.append(ut_run_type[1])
            self.runTypeDict[ut_run_type[1]] = ut_run_type[0]
            if ut_run_type[0] == run_type:
                self.runTypeNum.set(ut_run_type[1])

        cursor.execute("SELECT * FROM ut_user")
        for ut_user in cursor.fetchall():
            self.userOptions.append(ut_user[1])
            self.userDict[ut_user[1]] = ut_user[0]
            if ut_user[0] == user_id:
                self.userNum.set(ut_user[1])
        cursor.close()


        self.runStatus = cfg.getint('Main', 'tsRun.Status')
        self.runNumi = cfg.getint('Main', 'tsRun.Number')
        self.runNum = StringVar()
        self.runNum.set(self.runNumi)
        
        self.dataDir = StringVar()
        self.dataDir.set(cfg.get('Main', 'tsData.Directory'))

        self.dataFile = self.dataDir.get() + "/ts" + self.runNum.get() + ".dat"
        
        self.logDir = StringVar()
        self.logDir.set(cfg.get('Main', 'tsLog.Directory'))
        self.logDir.set("/home/data/test_stand")

        self.logFile = self.logDir.get() + "/ts" + self.runNum.get() + ".log"

        self.startTime = cfg.getint('Main', 'tsrun.start')
        self.stopTime = cfg.getint('Main', 'tsrun.stop')
        

    def fileChanged(self):
        self.runNumi = int(self.runNum.get())
        self.dataFile = self.dataDir.get() + "/ts" + self.runNum.get() + ".dat"
        self.data_file.setvalue(self.dataFile)
        self.logFile = self.logDir.get() + "/ts" + self.runNum.get() + ".log"
        self.log_file.setvalue(self.logFile)

    def runStart(self):
        global fifofd
        #print "Run Start pressed"

        if self.runStatus == 3:
            #print "Starting new run"
            self.runStatus = 2
            self.startTime = int(time.time())
            if fifofd == 0:
                fifo_file = "/tmp/pcanfifo-" + str(os.getuid())
                fifofd = os.open(fifo_file, os.O_WRONLY)
            writeStr = "s " + self.dataFile
            os.write(fifofd, writeStr)
            time.sleep(1)
            os.write(fifofd, "m e 0x04000000 2 0x0 0x1")
            self.runNumi += 1
            self.runNum.set(self.runNumi)
            self.fileChanged()
            run_type = self.runTypeDict[self.runTypeNum.get()]
            user_id = self.userDict[self.userNum.get()]
            
            cfg.set('Main', 'tsRun.Number', self.runNumi)
            cfg.set('Main', 'tsRun.user', user_id)
            cfg.set('Main', 'tsRun.type', run_type)
            cfg.set('Main', 'tsRun.Status', 2)
            cfg.set('Main', 'tsrun.start', self.startTime)
            cfgFile = open("runcontrol.cfg", "w")
            cfg.write(cfgFile)
            cfgFile.close()
            self.start_time.setvalue(time.strftime("%a, %d %b %Y %H:%M:%S",
                                                   time.localtime(self.startTime)))
            #self.runStartSQL()


    def runStop(self):
        global fifofd
        #print "Run Stop pressed"
        if self.runStatus != 3:
            #print "Stopping run"
            if fifofd == 0:
                fifo_file = "/tmp/pcanfifo-" + str(os.getuid())
                fifofd = os.open(fifo_file, os.O_WRONLY)
            os.write(fifofd, "m e 0x04000000 2 0x0 0x0")
            time.sleep(1)
            os.write(fifofd, "c")

            self.runStatus = 3
            self.stopTime = int(time.time())
            run_type = self.runTypeDict[self.runTypeNum.get()]
            user_id = self.userDict[self.userNum.get()]
            self.run_time.setvalue(self.stopTime - self.startTime)

            cfg.set('Main', 'tsRun.user', user_id)
            cfg.set('Main', 'tsRun.type', run_type)
            cfg.set('Main', 'tsRun.Status', 3)
            cfg.set('Main', 'tsrun.stop', self.stopTime)
            cfgFile = open("runcontrol.cfg", "w")
            cfg.write(cfgFile)
            cfgFile.close()
            #self.runStopSQL()

    def runStartSQL(self):
        run_type = self.runTypeDict[self.runTypeNum.get()]
        user_id = self.userDict[self.userNum.get()]

        run_config = 1 # What is this?
        level = 5
        data_url = "file://pegasus" + self.dataFile
        logMessage = 'Started run ' + str(self.runNumi)
        
        cursor = self.db.cursor()
        cursor.execute("""INSERT INTO ut_run
        (id, start, stop, events, run_type_id, user_id, run_status_id, config_id, data_uri)
        VALUES (%s %s %s %s %s %s %s %s %s)""",
                       (self.runNumi, self.startTime, self.startTime, 0, run_type, user_id,
                        self.runStatus, run_config, data_url))
        cursor.execute("""INSERT INTO ut_run_log
        (datetime, log_level_id, run_id, user_id, message)
        VALUES (%s %s %s %s %s)""",
                       (self.startTime, level, self.runNumi, user_id, logMessage))

        cursor.close()

    def runStopSQL(self):
        run_type = self.runTypeDict[self.runTypeNum.get()]
        user_id = self.userDict[self.userNum.get()]

        run_config = 1 # What is this?
        level = 5
        data_url = "file://pegasus" + self.dataFile
        logMessage = 'Stopped run ' + str(self.runNumi) + " (" + self.run_time.getvalue() + " sec)"

        cursor = self.db.cursor()
        cursor.execute("""UPDATE ut_run SET
        run_status_id=%s stop=%s run_time=%s WHERE id=%s""",
                       (self.runStatus, self.stopTime, self.run_time.getvalue(), self.runNumi))
        cursor.execute("""INSERT INTO ut_run_log
        (datetime, log_level_id, run_id, user_id, message)
        VALUES (%s %s %s %s %s)""",
                       (self.stopTime, level, self.runNumi, user_id, logMessage))

        cursor.close()

    def secondElapsed(self):
        self.current_time.setvalue(time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime()))
        if self.runStatus == 2:
            self.run_time.setvalue(int(time.time()) - self.startTime)
        self.tk.after(1000, self.secondElapsed)
            

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
    global fifofd
    pCmdArgs = string.join(sys.argv[1:])
    root = Tk()
    Pmw.initialise(root, size = 11, fontScheme = 'pmw1')
    tmpString = rtitle + " " + version
    root.title(tmpString)

    if pCmdArgs:
        w = PcanGUI(root, pCmdArgs=pCmdArgs)
    else:
        w = PcanGUI(root, "1")

    root.mainloop()

    if fifofd == 0:
        fifo_file = "/tmp/pcanfifo-" + str(os.getuid())
        fifofd = os.open(fifo_file, os.O_WRONLY)
    os.write(fifofd, "e")
    os.close(fifofd)
    time.sleep(1)

if __name__ == '__main__':
    test()
