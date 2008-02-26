#!/usr/bin/env python
#
# $Id: config.py,v 1.6 2008-02-26 20:52:32 jschamba Exp $

rtitle = 'HPTDC Configurator'
version = '1.6'

import sys
from Tkinter import *
import tkFileDialog
import tkMessageBox
import Pmw

# import the utility functions
from parsers import *

class Configurator:
    def __init__(self, parent):
        # Create and set the default values
        self.setDefaultVals()
        
	# Create the Balloon.
	self.balloon = Pmw.Balloon(parent)
        
	# Create and pack the MenuBar.
	menuBar = Pmw.MenuBar(parent,
		hull_relief = 'raised',
		hull_borderwidth = 1,
		balloon = self.balloon)
	menuBar.pack(fill = 'x')
	self.menuBar = menuBar

	# Add some buttons to the MenuBar.
	menuBar.addmenu('File', 'Close this window or exit')
	menuBar.addmenuitem('File', 'command', 'Load values from file',
		#command = PrintOne('Action: load preset'),
		command = self.readFile,
		label = 'Load Preset')
	menuBar.addmenuitem('File', 'command', 'Write to Configuration File',
		#command = PrintOne('Action: go button pressed'),
		command = self.writeFile,
		label = 'Write Config File')
	menuBar.addmenuitem('File', 'command', 'Write to Include File',
		#command = PrintOne('Action: go button pressed'),
		command = self.writeInclude,
		label = 'Write Include File')
	#menuBar.addmenuitem('File', 'command', 'Test this window',
	#	command = self.print_testSelect,
	#	label = 'Test')
	menuBar.addmenuitem('File', 'separator')
	menuBar.addmenuitem('File', 'command', 'Exit the application',
                            command = parent.destroy,
                            label = 'Exit')


	menuBar.addmenu('Help', 'User manuals', side = 'right')
	menuBar.addmenuitem('Help', 'command', 'About this application',
		command = PrintOne('Action: about'),
		label = 'About...')

        notebook = Pmw.NoteBook(parent)
        notebook.pack(fill='both', expand = 1, padx=10, pady=10)

        # Add first page to notebook
        page = notebook.add('General')
        notebook.tab('General').focus_set()

        self.createPage1(page)

        page = notebook.add('Options')
        self.createPage2(page)
        page = notebook.add('Offsets')
        self.createPage3(page)
        page = notebook.add('Calibrations')
        self.createPage4(page)

        notebook.setnaturalsize()
        

    ########### PAGE 1 CREATION ####################################
    def createPage1(self,parent):
        #### MAIN AREA #####
        mainArea = Frame(parent,
                         borderwidth = 3,
                         relief = RIDGE)
        mainArea.pack(side = TOP,
                      fill = X)

        ###### UPPER LEFT ##########
        ts = Frame(mainArea,
                   borderwidth = 3,
                   relief = RIDGE)
        ts.grid(column = 0, row = 0)
        self.create_ts(ts)

        ###### UPPER RIGHT #########
        drs = Frame(mainArea,
                    borderwidth = 3,
                    relief = RIDGE)
        drs.grid(column = 1, row = 0, sticky=N+S+E+W)
        self.create_drs(drs)
        
        ###### LOWER LEFT ##########
        os = Frame(mainArea,
                   borderwidth = 3,
                   relief = RIDGE)
        os.grid(column = 0, row = 1, sticky=N+S+E+W)
        self.create_os(os)

        ###### LOWER RIGHT ##########
        tms = Frame(mainArea,
                   borderwidth = 3,
                   relief = RIDGE)
        tms.grid(column = 1, row = 1)
        self.create_tms(tms)




    ##### TROUBLE SHOOTING PARAMETERS ####################
    def create_ts(self, parent):
        title = Label(parent,
                      text = 'Troubleshooting Features',
                      font = ("Arial", 12, "bold"),
                      padx = 1,
                      relief = RAISED)
        title.pack(fill=X, expand=1, pady=1)


        testSelect = Pmw.OptionMenu(parent,
                                    labelpos = W,
                                    label_text = 'Test Select:',
                                    #label_font = ("Arial", 12),
                                    menubutton_textvariable = self.testSelectNum,
                                    items = self.testSelectOptions)


        fixedPattern = Pmw.EntryField(parent,
                                      labelpos = W,
                                      label_text = "Fixed Pattern (28bit binary): ",
                                      #label_font = ("Arial", 12),
                                      validate = {'max' : 28, 'min' : 28, 'minstrict': 0},
                                      entry_textvariable = self.fixedPatternNum,
                                      entry_width = 28,
                                      entry_relief = SUNKEN)

        self.enableError = Pmw.ScrolledListBox(parent,
                                               labelpos = W,
                                               label_text = "Enable Error:",
                                               #label_font = ("Arial", 12),
                                               items = ("Vernier Error", "Coarse Error",
                                                        "Channel Select Error", "L1 Buffer Parity Error",
                                                        "Trigger FIFO Parity Error",
                                                        "Trigger Matching Error", "Read-out FIFO Parity Error",
                                                        "Read-out State Error", "Setup Parity Error",
                                                        "Control Parity Error", "JTAG Instruction Parity Error"),
                                               listbox_selectmode = EXTENDED,
                                               listbox_exportselection = 0,
                                               listbox_height = 4,
                                               hscrollmode = 'none')
        # enable them all by default:
        self.enableError.selection_set(0, self.enableError.size())
        
        self.tscheckb = Pmw.RadioSelect(parent,
                                        buttontype = 'checkbutton',
                                        orient = 'vertical',
                                        hull_borderwidth = 1,
                                        hull_relief = RIDGE,
                                        labelpos = W,
                                        label_text = 'Enable/Disable:',
                                        labelmargin = 10,
                                        #label_font = ("Arial", 12),
                                        pady=0)
        self.tscheckb.add('115', text='Fixed Pattern')
        self.tscheckb.add('122', text='Readout Separator')
        self.tscheckb.add('152', text='Master Reset on  Event Reset')
        self.tscheckb.add('153', text='Reset Channel Buffers when Separator')
        self.tscheckb.add('154', text='Generate Separator on Event Reset')
        self.tscheckb.add('155', text='Generate Separator on Bunch Reset')
                                       
        serialClockSource = Pmw.OptionMenu(parent,
                                           labelpos = W,
                                           label_text = 'Serial Clock Source:',
                                           #label_font = ("Arial", 12),
                                           menubutton_textvariable = self.serialCSNum,
                                           items = self.serialCSOptions)

        ioClockSource = Pmw.OptionMenu(parent,
                                           labelpos = W,
                                           label_text = 'IO Clock Source:',
                                           #label_font = ("Arial", 12),
                                           menubutton_textvariable = self.ioCSNum,
                                           items = self.ioCSOptions)

        coreClockSource = Pmw.OptionMenu(parent,
                                         labelpos = W,
                                         label_text = 'Core Clock Source:',
                                         #label_font = ("Arial", 12),
                                         menubutton_textvariable = self.coreCSNum,
                                         items = self.coreCSOptions)

        dllClockSource = Pmw.OptionMenu(parent,
                                        labelpos = W,
                                        label_text = 'DLL Clock Source:',
                                        #label_font = ("Arial", 12),
                                        menubutton_textvariable = self.dllCSNum,
                                        items = self.dllCSOptions)



        # first to the top few widgets...
        allwdgs = [testSelect, fixedPattern, self.enableError]
        for wdg in allwdgs:
            wdg.pack(fill=X, expand=1, padx=10, pady=2)

        # ... then the checkboxes
        self.tscheckb.pack(fill=X, expand=1, padx=10, pady=2)

        # ... and finally the option menues at the end
        allwdgs2 = [serialClockSource, ioClockSource, coreClockSource,
                    dllClockSource]
        for wdg in allwdgs2:
            wdg.pack(fill=X, expand=1, padx=10, pady=2)

        # now combine them and align them all
        allwdgs += allwdgs2
        Pmw.alignlabels(allwdgs)


    ##### DATA READOUT SECTION ####################
    def create_drs(self, parent):
        title = Label(parent,
                      text = 'Data Readout Features',
                      font = ("Arial", 12, "bold"),
                      padx = 1,
                      relief = RAISED)
        title.pack(fill=X, expand=1, pady=1)

        tdcIdent = Pmw.EntryField(parent,
                                  labelpos = W,
                                  label_text = "TDC ID (4bit binary): ",
                                  #label_font = ("Arial", 12),
                                  validate = {'max' : 4, 'min' : 4, 'minstrict': 0},
                                  entry_textvariable = self.tdcIdentNum,
                                  entry_width = 4,
                                  entry_relief = SUNKEN)

        roFifoSize = Pmw.OptionMenu(parent,
                                    labelpos = W,
                                    label_text = 'Readout FIFO Size:',
                                    #label_font = ("Arial", 12),
                                    menubutton_textvariable = self.roFifoSizeNum,
                                    items = self.roFifoSizeOptions)

        roType = Pmw.OptionMenu(parent,
                                labelpos = W,
                                label_text = 'Readout Type:',
                                #label_font = ("Arial", 12),
                                menubutton_textvariable = self.roTypeNum,
                                items = self.roTypeOptions)

        roSpeedSelect = Pmw.OptionMenu(parent,
                                       labelpos = W,
                                       label_text = 'Readout Speed Select:',
                                       #label_font = ("Arial", 12),
                                       menubutton_textvariable = self.roSpeedSelNum,
                                       items = self.roSpeedSelOptions)

        readSingleCycleSpeed = Pmw.OptionMenu(parent,
                                              labelpos = W,
                                              label_text = 'Read Single Cycle Speed:',
                                              #label_font = ("Arial", 12),
                                              menubutton_textvariable = self.readSingleCycleSpeedNum,
                                              items = self.readSingleCycleSpeedOptions)

        strobeSel = Pmw.OptionMenu(parent,
                                   labelpos = W,
                                   label_text = 'Strobe Select:',
                                   #label_font = ("Arial", 12),
                                   menubutton_textvariable = self.strobeSelNum,
                                   items = self.strobeSelOptions)
        
        self.drscheckb = Pmw.RadioSelect(parent,
                                         buttontype = 'checkbutton',
                                         orient = 'vertical',
                                         hull_borderwidth = 1,
                                         hull_relief = RIDGE,
                                         labelpos = W,
                                         label_text = 'Enable/Disable:',
                                         labelmargin = 40,
                                         #label_font = ("Arial", 12),
                                         pady=0)
        self.drscheckb.add(  '4', text='Error Mark')
        self.drscheckb.add( '31', text='Local Trailer')
        self.drscheckb.add( '32', text='Local Header')
        self.drscheckb.add( '33', text='Global Trailer')
        self.drscheckb.add( '34', text='Glocal Header')
        self.drscheckb.add( '35', text='Keep Token')
        self.drscheckb.add( '36', text='Master Chip')
        self.drscheckb.add('120', text='Reject Readout FIFO Full')

        self.drscheckb.invoke('35')

        allwdgs = (tdcIdent, roFifoSize, roType, roSpeedSelect, readSingleCycleSpeed, strobeSel )
        for wdg in allwdgs:
            wdg.pack(fill=X, expand=1, padx=10, pady=2)

        Pmw.alignlabels(allwdgs)

        self.drscheckb.pack(fill=X, expand=1, padx=10, pady=2)


    ##### OPERATING SECTION ####################
    def create_os(self, parent):
        title = Label(parent,
                      text = 'Operating Features',
                      font = ("Arial", 12, "bold"),
                      padx = 1,
                      relief = RAISED)
        title.pack(fill=X, expand=1, pady=1)

        self.oscheckb = Pmw.RadioSelect(parent,
                                        buttontype = 'checkbutton',
                                        orient = 'vertical',
                                        hull_borderwidth = 1,
                                        hull_relief = RIDGE,
                                        labelpos = W,
                                        label_text = 'Enable/Disable:',
                                        labelmargin = 5,
                                        #label_font = ("Arial", 12),
                                        pady=0)
        self.oscheckb.add('588', text='Trailing Edge Measurement')
        self.oscheckb.add('589', text='Leading Edge Measurement')
        self.oscheckb.add('640', text='Paired Readout (overrides last 2 options)')
        self.oscheckb.add('590', text='Perform 25ps interpolation on-chip')
        self.oscheckb.add('591', text='Very High Res. Mode (25ps)')
        self.oscheckb.invoke('589')
        self.oscheckb.invoke('590')
        self.oscheckb.invoke('591')

        dllMode = Pmw.OptionMenu(parent,
                                 labelpos = W,
                                 label_text = 'DLL Mode:',
                                 #label_font = ("Arial", 12),
                                 menubutton_textvariable = self.dllModeNum,
                                 items = self.dllModeOptions)
        
        leadingRes = Pmw.OptionMenu(parent,
                                    labelpos = W,
                                    label_text = 'Leading Resolution:',
                                    #label_font = ("Arial", 12),
                                    menubutton_textvariable = self.leadingResNum,
                                    items = self.leadingResOptions)
        
        deadTime = Pmw.OptionMenu(parent,
                                  labelpos = W,
                                  label_text = 'Dead Time:',
                                  #label_font = ("Arial", 12),
                                  menubutton_textvariable = self.deadTimeNum,
                                  items = self.deadTimeOptions)

        rolloverCtr = Pmw.Counter(parent,
                                  labelpos = W,
                                  label_text = 'Rollover Count (ns): ',
                                  datatype = 'numeric',
                                  increment = 25,
                                  entryfield_validate = {'validator' : self.__custom_validate,
                                                         'min' : 0,
                                                         'max' : 102375,
                                                         'stringtovalue' : 'numeric'},
                                  entry_textvariable = self.rolloverCtrNum
                                  #entryfield_value = 102375
                                  )
        
        self.oscheckb.pack(fill=X, expand=1, padx=10, pady=2)

        allwdgs = (dllMode, leadingRes, deadTime, rolloverCtr)
        for wdg in allwdgs:
            wdg.pack(fill=X, expand=1, padx=10, pady=2)

        Pmw.alignlabels(allwdgs)

    def __custom_validate(self,text):
        #print 'text:', text
        if (int(text) % 25 == 0):
            return Pmw.OK
        else:
            return Pmw.PARTIAL
        

    ##### TRIGGER MATCHING SECTION ####################
    def create_tms(self, parent):
        title = Label(parent,
                      text = 'Trigger Matching Features',
                      font = ("Arial", 12, "bold"),
                      padx = 1,
                      relief = RAISED)
        title.pack(fill=X, expand=1, pady=1)

        self.tmscheckb = Pmw.RadioSelect(parent,
                                         buttontype = 'checkbutton',
                                         orient = 'vertical',
                                         hull_borderwidth = 1,
                                         hull_relief = RIDGE,
                                         labelpos = W,
                                         label_text = 'Enable/Disable:',
                                         labelmargin = 40,
                                         #label_font = ("Arial", 12),
                                         pady=0)
        self.tmscheckb.add('639', text='Trigger Matching')
        self.tmscheckb.add('124', text='Relative Time')
        self.tmscheckb.add('125', text='Automatic Reject')
        self.tmscheckb.add('121', text='Readout Occupancy')
        self.tmscheckb.invoke('639')
        self.tmscheckb.invoke('125')

        maxEventSize = Pmw.OptionMenu(parent,
                                      labelpos = W,
                                      label_text = 'Max Event Size:',
                                      #label_font = ("Arial", 12),
                                      menubutton_textvariable = self.maxEventSizeNum,
                                      items = self.maxEventSizeOptions)
        
        trgCtrOffset = Pmw.Counter(parent,
                                   labelpos = W,
                                   label_text = 'Trigger Count Offset (ns): ',
                                   datatype = 'numeric',
                                   increment = 25,
                                   entryfield_validate = {'validator' : self.__custom_validate,
                                                          'min' : 0,
                                                          'max' : 51200,
                                                          'stringtovalue' : 'numeric'},
                                   #entryfield_modifiedcommand = self.trgCtrOffsetChanged,
                                   entry_textvariable = self.trgCtrOffsetNum
                                   )
        
        searchWindowSize = Pmw.Counter(parent,
                                       labelpos = W,
                                       label_text = 'Search Window Size (ns): ',
                                       datatype = 'numeric',
                                       increment = 25,
                                       entryfield_validate = {'validator' : self.__custom_validate,
                                                              'min' : 0,
                                                              'max' : 51200,
                                                              'stringtovalue' : 'numeric'},
                                       entry_textvariable = self.searchWindowSizeNum
                                       )
        
        matchWindowSize = Pmw.Counter(parent,
                                      labelpos = W,
                                      label_text = 'Match Window Size (ns): ',
                                      datatype = 'numeric',
                                      increment = 25,
                                      entryfield_validate = {'validator' : self.__custom_validate,
                                                             'min' : 0,
                                                             'max' : 51200,
                                                             'stringtovalue' : 'numeric'},
                                      entry_textvariable = self.matchWindowSizeNum
                                      )
        
        rejectCtrOffset = Pmw.Counter(parent,
                                      labelpos = W,
                                      label_text = 'Reject Count Offset (ns): ',
                                      datatype = 'numeric',
                                      increment = 25,
                                      entryfield_validate = {'validator' : self.__custom_validate,
                                                             'min' : 0,
                                                             'max' : 51300,
                                                             'stringtovalue' : 'numeric'},
                                      entry_textvariable = self.rejectCtrOffsetNum
                                      )
        

        self.tmscheckb.pack(fill=X, expand=1, padx=10, pady=2)

        allwdgs = (maxEventSize, trgCtrOffset, searchWindowSize, matchWindowSize, rejectCtrOffset)
        for wdg in allwdgs:
            wdg.pack(fill=X, expand=1, padx=10, pady=2)

        Pmw.alignlabels(allwdgs)

    def trgCtrOffsetChanged(self):
        print "trgCtrOffset changed:", self.trgCtrOffsetNum.get()



    ########### PAGE 2 WIDGETS ####################################
    def createPage2(self,parent):
        #### Options #####
        mainArea = Frame(parent,
                         borderwidth = 3,
                         relief = RIDGE)
        #mainArea.pack(side = TOP,
        #              fill = X)
        mainArea.pack(side = TOP)
        title = Label(mainArea,
                      text = 'OPTIONS',
                      font = ("Arial", 12, "bold"),
                      padx = 1,
                      relief = RAISED)
        title.pack(fill=X, expand=1, pady=1)
        Label(mainArea,
              text = 'These Options are mainly for production testing\nand should be left alone!',
              font = ("Arial", 12, "bold"),
              padx = 1).pack(fill=X, expand=1, pady=5)

        self.optionscheckb = Pmw.RadioSelect(mainArea,
                                             buttontype = 'checkbutton',
                                             orient = 'vertical',
                                             hull_borderwidth = 1,
                                             hull_relief = RIDGE,
                                             labelpos = W,
                                             label_text = 'Enable/Disable:',
                                             labelmargin = 50,
                                             #label_font = ("Arial", 12),
                                             pady=0)
        self.optionscheckb.add(  '5', text='Error Bypass')
        self.optionscheckb.add( '44', text='Serial In and Token in from Bypass Inputs')
        self.optionscheckb.add('123', text='Overflow Detect')
        self.optionscheckb.add('150', text='Set All Counters on Bunch Reset')
        self.optionscheckb.add('151', text='Master Reset Code on encoded_control')
        self.optionscheckb.add('156', text='Direct Event Reset (off = from encoded control)')
        self.optionscheckb.add('157', text='Direct Bunch Reset (off = from encoded control)')
        self.optionscheckb.add('158', text='Direct Trigger (off = from encoded control)')
        self.optionscheckb.add('570', text='Low Power Mode')
        self.optionscheckb.add('586', text='Test Invert')
        self.optionscheckb.add('587', text='Test Mode')
        self.optionscheckb.add('641', text='TTL Serial (off=LVDS)')
        self.optionscheckb.add('642', text='TTL Control (off=LVDS)')
        self.optionscheckb.add('643', text='TTL Reset (off=LVDS)')
        self.optionscheckb.add('644', text='TTL Clock (off=LVDS)')
        self.optionscheckb.add('645', text='TTL Hit (off=LVDS)')
        self.optionscheckb.invoke('123')
        self.optionscheckb.invoke('150')
        self.optionscheckb.invoke('156')
        self.optionscheckb.invoke('157')
        self.optionscheckb.invoke('158')
        self.optionscheckb.invoke('570')
        self.optionscheckb.invoke('641')
        self.optionscheckb.invoke('642')
        self.optionscheckb.invoke('643')
        
        serialDelay = Pmw.Counter(mainArea,
                                  labelpos = W,
                                  label_text = 'Serial Delay (ns): ',
                                  datatype = 'numeric',
                                  #increment = 25,
                                  entryfield_validate = {'validator' : 'numeric',
                                                         'min' : 0,
                                                         'max' : 15},
                                  entry_textvariable = self.serialDelayNum
                                  )
        
        tokenDelay = Pmw.Counter(mainArea,
                                 labelpos = W,
                                 label_text = 'Token Delay (ns): ',
                                 datatype = 'numeric',
                                 #increment = 25,
                                 entryfield_validate = {'validator' : 'numeric',
                                                        'min' : 0,
                                                        'max' : 15},
                                 entry_textvariable = self.tokenDelayNum
                                 )
        
        eventCtrOffset = Pmw.Counter(mainArea,
                                     labelpos = W,
                                     label_text = 'Event Number Offset: ',
                                     datatype = 'numeric',
                                     increment = 25,
                                     entryfield_validate = {'validator' : 'numeric',
                                                            'min' : 0,
                                                            'max' : 4095},
                                     entry_textvariable = self.eventCtrOffsetNum
                                     )
        
        widthSelect = Pmw.OptionMenu(mainArea,
                                     labelpos = W,
                                     label_text = 'Width Select:',
                                     #label_font = ("Arial", 12),
                                     menubutton_textvariable = self.widthSelectNum,
                                     items = self.widthSelectOptions)
        
        dllControl = Pmw.EntryField(mainArea,
                                    labelpos = W,
                                    label_text = "DLL Control (4bit binary): ",
                                    #label_font = ("Arial", 12),
                                    validate = {'max' : 4, 'min' : 4, 'minstrict': 0},
                                    entry_textvariable = self.dllControlNum,
                                    entry_width = 4,
                                    entry_relief = SUNKEN)

        pllControl = Pmw.EntryField(mainArea,
                                    labelpos = W,
                                    label_text = "PLL Control (8bit binary): ",
                                    #label_font = ("Arial", 12),
                                    validate = {'max' : 8, 'min' : 8, 'minstrict': 0},
                                    entry_textvariable = self.pllControlNum,
                                    entry_width = 8,
                                    entry_relief = SUNKEN)

        vernierOffset = Pmw.EntryField(mainArea,
                                       labelpos = W,
                                       label_text = "Vernier Offset (5bit binary):        ",
                                       #label_font = ("Arial", 12),
                                       validate = {'max' : 5, 'min' : 5, 'minstrict': 0},
                                       entry_textvariable = self.vernierOffsetNum,
                                       entry_width = 5,
                                       entry_relief = SUNKEN)
        
        self.optionscheckb.pack(fill=X, expand=1, padx=10, pady=2)

        allwdgs = (serialDelay,tokenDelay, eventCtrOffset, widthSelect, dllControl,
                   pllControl, vernierOffset)
        for wdg in allwdgs:
            wdg.pack(fill=X, expand=1, padx=10, pady=2)

        Pmw.alignlabels(allwdgs)

    ########### PAGE 3 WIDGETS ####################################
    def createPage3(self,parent):
        #### Offsets #####
        mainArea = Frame(parent,
                         borderwidth = 3,
                         relief = RIDGE)
        #mainArea.pack(side = TOP,
        #              fill = X)
        mainArea.pack(side = TOP)

        chOffsets = Frame(mainArea,
                    borderwidth = 3,
                    relief = RIDGE)
        chOffsets.pack(side=LEFT)
        Label(chOffsets,
              text = 'Channel Offsets',
              font = ("Arial", 12, "bold"),
              padx = 1,
              relief = RAISED).pack(fill=X, expand=1, pady=5)

        allwdgs1 = []
        for i in range(24):
            wdg = Pmw.EntryField(chOffsets,
                                 labelpos = W,
                                 label_text = "Channel Offset %d (9-bit binary):  " % i,
                                 #label_font = ("Arial", 12),
                                 validate = {'max' : 9, 'min' : 9, 'minstrict': 0},
                                 entry_textvariable = self.chOffset[i],
                                 entry_width = 9,
                                 entry_relief = SUNKEN)
            allwdgs1.append(wdg)
            
        for wdg in allwdgs1:
            wdg.pack(fill=X, expand=1, padx=10, pady=0)

        Pmw.alignlabels(allwdgs1)
        
        rc = Frame(mainArea,
                   borderwidth = 3,
                   relief = RIDGE)
        rc.pack(side=LEFT)

        allwdgs1 = []
        for i in range(24,32):
            wdg = Pmw.EntryField(rc,
                                 labelpos = W,
                                 label_text = "Channel Offset %d (9-bit binary):  " % i,
                                 #label_font = ("Arial", 12),
                                 validate = {'max' : 9, 'min' : 9, 'minstrict': 0},
                                 entry_textvariable = self.chOffset[i],
                                 entry_width = 9,
                                 entry_relief = SUNKEN)
            allwdgs1.append(wdg)
            
        for wdg in allwdgs1:
            wdg.pack(padx=10, pady=0)

        Pmw.alignlabels(allwdgs1)
        
        Label(rc,
              text = '',
              padx = 1).pack(fill=X, expand=1, pady=20)

        Label(rc,
              text = 'Offsets',
              font = ("Arial", 12, "bold"),
              padx = 1,
              relief = RAISED).pack(fill=X, expand=1, pady=0)

        coarseCount = Pmw.EntryField(rc,
                                     labelpos = W,
                                     label_text = "Coarse Count Offset (12-bit binary):  ",
                                     #label_font = ("Arial", 12),
                                     validate = {'max' : 12, 'min' : 12, 'minstrict': 0},
                                     entry_textvariable = self.coarseCountNum,
                                     entry_width = 12,
                                     entry_relief = SUNKEN)

        serialClock = Pmw.EntryField(rc,
                                     labelpos = W,
                                     label_text = "Serial Clock Delay (4-bit binary):  ",
                                     #label_font = ("Arial", 12),
                                     validate = {'max' : 4, 'min' : 4, 'minstrict': 0},
                                     entry_textvariable = self.serialClockNum,
                                     entry_width = 4,
                                     entry_relief = SUNKEN)

        ioClock = Pmw.EntryField(rc,
                                 labelpos = W,
                                 label_text = "I/O Clock Delay (4-bit binary):  ",
                                 #label_font = ("Arial", 12),
                                 validate = {'max' : 4, 'min' : 4, 'minstrict': 0},
                                 entry_textvariable = self.ioClockNum,
                                 entry_width = 4,
                                 entry_relief = SUNKEN)
        
        coreClock = Pmw.EntryField(rc,
                                   labelpos = W,
                                   label_text = "Core Clock Delay (4-bit binary):  ",
                                   #label_font = ("Arial", 12),
                                   validate = {'max' : 4, 'min' : 4, 'minstrict': 0},
                                   entry_textvariable = self.coreClockNum,
                                   entry_width = 4,
                                   entry_relief = SUNKEN)
        
        dllClock = Pmw.EntryField(rc,
                                  labelpos = W,
                                  label_text = "DLL Clock Delay (4-bit binary):  ",
                                  #label_font = ("Arial", 12),
                                  validate = {'max' : 4, 'min' : 4, 'minstrict': 0},
                                  entry_textvariable = self.dllClockNum,
                                  entry_width = 4,
                                  entry_relief = SUNKEN)
        
        allwdgs2 = (coarseCount, serialClock, ioClock, coreClock, dllClock)
            
        for wdg in allwdgs2:
            wdg.pack(fill=X, expand=1, padx=10, pady=2)

        Pmw.alignlabels(allwdgs2)
        

    ########### PAGE 4 WIDGETS ####################################
    def createPage4(self,parent):
        #### Calibrations #####
        mainArea = Frame(parent,
                         borderwidth = 3,
                         relief = RIDGE)
        #mainArea.pack(side = TOP,
        #              fill = X)
        mainArea.pack(side = TOP)

        dll = Frame(mainArea,
                    borderwidth = 3,
                    relief = RIDGE)
        dll.pack(side=LEFT)
        Label(dll,
              text = 'DLL ADJUSTS',
              font = ("Arial", 12, "bold"),
              padx = 1,
              relief = RAISED).pack(fill=X, expand=1, pady=0)

        allwdgs1 = []
        for i in range(24):
            wdg = Pmw.EntryField(dll,
                                 labelpos = W,
                                 label_text = "DLL Adjust Tap %d (3-bit binary):  " % i,
                                 #label_font = ("Arial", 12),
                                 validate = {'max' : 3, 'min' : 3, 'minstrict': 0},
                                 entry_textvariable = self.dllAdjust[i],
                                 entry_width = 3,
                                 entry_relief = SUNKEN)
            allwdgs1.append(wdg)
            
        for wdg in allwdgs1:
            wdg.pack(fill=X, expand=1, padx=10, pady=0)

        Pmw.alignlabels(allwdgs1)
        
        rc = Frame(mainArea,
                   borderwidth = 3,
                   relief = RIDGE)
        rc.pack(side=LEFT)

        allwdgs1 = []
        for i in range(24,32):
            wdg = Pmw.EntryField(rc,
                                 labelpos = W,
                                 label_text = "DLL Adjust Tap %d (3-bit binary):  " % i,
                                 #label_font = ("Arial", 12),
                                 validate = {'max' : 3, 'min' : 3, 'minstrict': 0},
                                 entry_textvariable = self.dllAdjust[i],
                                 entry_width = 3,
                                 entry_relief = SUNKEN)
            allwdgs1.append(wdg)
            
        for wdg in allwdgs1:
            wdg.pack(padx=10, pady=0)

        Pmw.alignlabels(allwdgs1)
        
        Label(rc,
              text = '',
              padx = 1).pack(fill=X, expand=1, pady=20)

        Label(rc,
              text = 'RC ADJUSTS',
              font = ("Arial", 12, "bold"),
              padx = 1,
              relief = RAISED).pack(fill=X, expand=1, pady=5)

        allwdgs2 = []
        for i in range(4):
            wdg = Pmw.EntryField(rc,
                                 labelpos = W,
                                 labelmargin = 15,
                                 label_text = "RC Adjust Tap %d (3-bit binary): " % (i+1),
                                 #label_font = ("Arial", 12),
                                 validate = {'max' : 3, 'min' : 3, 'minstrict': 0},
                                 entry_textvariable = self.rcAdjust[i],
                                 entry_width = 3,
                                 entry_relief = SUNKEN)
            allwdgs2.append(wdg)
            
        for wdg in allwdgs2:
            wdg.pack(fill=X, expand=1, padx=10, pady=2)

        Pmw.alignlabels(allwdgs2)
        
    ############## set default values ###################
    def setDefaultVals(self):
        self.output = ['0' for x in range(647)]

        #### Troubleshooting Features
        self.testSelectOptions = [
                   "Clock_40",
                   "PLL Clock_40",
                   "PLL Clock_80",
                   "PLL Clock_160",
                   "PLL Phase Detector",
                   "PLL Frequency Detector",
                   "PLL Lock",
                   "DLL End Inverted",
                   "DLL Phase Detector",
                   "DLL Lock",
                   "DLL Clock",
                   "Core Clock",
                   "IO Clock",
                   "Serial Clock",
                   "Constant Low",
                   "Constant High"]
        self.testSelectNum = StringVar()
        self.testSelectNum.set(self.testSelectOptions[14])

        self.fixedPatternNum = StringVar()
        self.fixedPatternNum.set("0100111100001111000011110000")

        self.serialCSOptions = [
            "PLL Clock_80",
            "PLL Clock_160 (not valid)",
            "PLL CLock_40 (not valid)",
            "Aux Clock"]
        self.serialCSNum = StringVar()
        self.serialCSNum.set(self.serialCSOptions[0])

        self.ioCSOptions = [
            "PLL CLock_40",
            "PLL Clock_80 (not valid)",
            "PLL Clock_160 (not valid)",
            "Aux Clock"]
        self.ioCSNum = StringVar()
        self.ioCSNum.set(self.ioCSOptions[0])

        self.coreCSOptions = [
            "PLL CLock_40",
            "PLL Clock_80",
            "PLL Clock_160",
            "Aux Clock"]
        self.coreCSNum = StringVar()
        self.coreCSNum.set(self.ioCSOptions[0])

        self.dllCSOptions = [
            "CLock_40",
            "PLL CLock_40",
            "PLL Clock_160",
            "PLL Clock_320",
            "Aux Clock"]
        self.dllCSNum = StringVar()
        self.dllCSNum.set(self.dllCSOptions[3])

        #### Data Readout Features
        self.tdcIdentNum = StringVar()
        self.tdcIdentNum.set("0000")

        self.roSpeedSelOptions = [
            "Single Cycle",
            "80 Mbits/s (PLL lock required)"]
        self.roSpeedSelNum = StringVar()
        self.roSpeedSelNum.set(self.roSpeedSelOptions[0])

        self.readSingleCycleSpeedOptions = [
            "40 Mbits/s",
            "20 Mbits/s",
            "10 Mbits/s",
            "5 Mbits/s",
            "2.5 Mbits/s",
            "1.25 Mbits/s",
            "0.625 Mbits/s",
            "0.3125 Mbits/s"]
        self.readSingleCycleSpeedNum = StringVar()
        self.readSingleCycleSpeedNum.set(self.readSingleCycleSpeedOptions[0])

        self.roTypeOptions = [
            "32bit Parallel",
            "Bytewise Parallel",
            "Serial",
            "illegal",
            "JTAG"]
        self.roTypeNum = StringVar()
        self.roTypeNum.set(self.roTypeOptions[2])

        self.strobeSelOptions = [
            "No Strobe",
            "DS Strobe",
            "Leading and Trailing Edge",
            "Leading Edge"]
        self.strobeSelNum = StringVar()
        self.strobeSelNum.set(self.strobeSelOptions[3])

        self.roFifoSizeOptions = [
            "2",
            "4",
            "8",
            "16",
            "32",
            "64",
            "128",
            "256"]
        self.roFifoSizeNum = StringVar()
        self.roFifoSizeNum.set(self.roFifoSizeOptions[7])

        ##### Operating Features
        self.dllModeOptions = [
            "40 MHz",
            "160 MHz",
            "320 MHz"]
        self.dllModeNum = StringVar()
        self.dllModeNum.set(self.dllModeOptions[2])

        self.leadingResOptions = [
            "100 ps",
            "200 ps",
            "400 ps (pair mode)",
            "800 ps (pair mode)",
            "1.6 ns (pair mode)",
            "3.12 ns (pair mode)",
            "6.25 ns (pair mode)",
            "12.5 ns (pair mode)"]
        self.leadingResNum = StringVar()
        self.leadingResNum.set(self.leadingResOptions[0])

        self.deadTimeOptions = [
            "5 ns",
            "10 ns",
            "30 ns",
            "100 ns"]
        self.deadTimeNum = StringVar()
        self.deadTimeNum.set(self.deadTimeOptions[0])

        self.rolloverCtrNum = StringVar()
        self.rolloverCtrNum.set('102375')

        ##### Trigger Matching Features
        self.maxEventSizeOptions = [
            	"0", "1", "2", "4", "8", "16", "32", "64", "128",
                "no limit"]
        self.maxEventSizeNum = StringVar()
        self.maxEventSizeNum.set(self.maxEventSizeOptions[9])
        
        self.trgCtrOffsetNum = StringVar()
        self.trgCtrOffsetNum.set('3000')

        self.searchWindowSizeNum = StringVar()
        self.searchWindowSizeNum.set('2975')

        self.matchWindowSizeNum = StringVar()
        self.matchWindowSizeNum.set('2775')

        self.rejectCtrOffsetNum = StringVar()
        self.rejectCtrOffsetNum.set('3100')

        #### Options Page (page 2)
        self.serialDelayNum = StringVar()
        self.serialDelayNum.set('0')
        
        self.tokenDelayNum = StringVar()
        self.tokenDelayNum.set('0')
        
        self.eventCtrOffsetNum = StringVar()
        self.eventCtrOffsetNum.set('0')

        self.widthSelectOptions = [
            "100 ps", "200 ps", "400 ps", "800 ps", "1.6 ns",
            "3.2 ns", "6.25 ns", "12.5 ns", "25 ns", "50 ns",
            "100 ns", "200 ns", "400 ns", "800 ns"]
        self.widthSelectNum = StringVar()
        self.widthSelectNum.set(self.widthSelectOptions[0])
            
        self.dllControlNum = StringVar()
        self.dllControlNum.set('0001')
        
        self.pllControlNum = StringVar()
        self.pllControlNum.set('00000100')
        
        self.vernierOffsetNum = StringVar()
        self.vernierOffsetNum.set('00000')

        #### Offsets Page (page 3)
        self.chOffset = []
        for i in range(32):
            sv = StringVar()
            sv.set('000000000')
            self.chOffset.append(sv)

        self.coarseCountNum = StringVar()
        self.coarseCountNum.set('000000000000')

        self.serialClockNum = StringVar()
        self.serialClockNum.set('0000')

        self.ioClockNum = StringVar()
        self.ioClockNum.set('0000')

        self.coreClockNum = StringVar()
        self.coreClockNum.set('0000')

        self.dllClockNum = StringVar()
        self.dllClockNum.set('0000')

        #### Calibrations Page (page 4)
        self.dllAdjust = []
        for i in range(32):
            sv = StringVar()
            convertIntToStringVar(i/4, sv, 3)
            self.dllAdjust.append(sv)

        self.rcAdjust = []
        for i in range(4):
            sv = StringVar()
            sv.set('001')
            self.rcAdjust.append(sv)
        self.rcAdjust[0].set('111')
        self.rcAdjust[1].set('111')


    ################# WRITE CONFIG FILE ##########################
    def writeFile(self):
        configFileName = tkFileDialog.asksaveasfilename(
            filetypes=[("text files", "*.txt"),
                       ("all files", "*")])
        if (configFileName == None or configFileName == ""):
            return
        try:
            File = open(configFileName, "w")
            self.saveChanges()
            # append a '0' to the end of the array (highest bit 647)
            tmp = self.output + ['0']
            for i in range(81):
                for j in range(8):
                    File.write("%c"% tmp[(i+1)*8-1-j])
                #File.write("\r\n") # windows like end-of-line
                File.write("\n")
            File.close()
        except IOError:
            tkMessageBox.showerror("Save error...",
                                   "Could not save to '%s'"%configFileName)
            return
            
    ################# WRITE INCLUDE FILE ##########################
    def writeInclude(self):
        includeFileName = tkFileDialog.asksaveasfilename(
            filetypes=[("include files", "*.inc"),
                       ("all files", "*")])
        if (includeFileName == None or includeFileName == ""):
            return
        try:
            File = open(includeFileName, "w")
            self.saveChanges()
            # append a '0' to the end of the array (highest bit 647)
            tmp = self.output + ['0']
            for i in range(40):
                File.write("0b")
                for j in range(8):
                    File.write("%c"% tmp[(i+1)*16-9-j])
                File.write(", 0b")
                for j in range(8):
                    File.write("%c"% tmp[(i+1)*16-1-j])
                #File.write("\r\n") # windows like end-of-line
                File.write("\n")
            File.write("0b")
            for  j in  range(8):
                File.write("%c"% tmp[647-j])
            File.write("\n")
            File.close()
        except IOError:
            tkMessageBox.showerror("Save error...",
                                   "Could not save to '%s'"%includeFileName)
            return
            
    ################### READ CONFIG FILE #########################
    def readFile(self):
        configFileName = tkFileDialog.askopenfilename(
            filetypes=[("text files", "*.txt"),
                       ("all files", "*")])
        if (configFileName == None or configFileName == ""):
            return
        try:
            #File = open(configFileName, "r")
            tmp = [x.strip() for x in open(configFileName).readlines()]
            #File.close()
            if(len(tmp) != 81):
                tkMessageBox.showerror("File format error...",
                                       "%s' contains %d lines of data"%(configFileName, len(tmp)))
            self.output = ['0' for x in range(647)]
            for i in range(80):
                tmpList = list(tmp[i])
                tmpList.reverse()
                self.output[(i*8):(i*8+8)] = tmpList
            # the last line contains an extra '0' at the msb, so handle separate
            tmpList = list(tmp[80])
            tmpList.reverse()
            self.output[640:647] = tmpList[0:7]
            self.loadPreset()
        except IOError:
            tkMessageBox.showerror("Read error...",
                                   "Could not read from '%s'"%configFileName)
            return


    ######## SAVE CHANGES #################################
    def saveChanges(self):
        self.output = ['0' for x in range(len(self.output))]

        #### Trouble shooting parameters
        parseOMIndex(self.output, 0, 3, self.testSelectOptions.index(self.testSelectNum.get()))
        parseSV(self.output, 87, 114, self.fixedPatternNum)
        parseLBList(self.output, 6, self.enableError.curselection())
        parseRSList(self.output, self.tscheckb.getcurselection())
        parseOMIndex(self.output, 618, 619, self.serialCSOptions.index(self.serialCSNum.get()))
        parseOMIndex(self.output, 620, 621, self.ioCSOptions.index(self.ioCSNum.get()))
        parseOMIndex(self.output, 622, 623, self.coreCSOptions.index(self.coreCSNum.get()))
        parseOMIndex(self.output, 624, 626, self.dllCSOptions.index(self.dllCSNum.get()))

        #### Data Readout parameters
        parseSV(self.output, 40, 43, self.tdcIdentNum)
        parseOMIndex(self.output, 45, 47, self.roFifoSizeOptions.index(self.roFifoSizeNum.get()))
        parseOMIndex(self.output, 37, 39, self.roTypeOptions.index(self.roTypeNum.get()))
        self.output[26] = str(self.roSpeedSelOptions.index(self.roSpeedSelNum.get()))
        parseOMIndex(self.output, 17, 19,
                     self.readSingleCycleSpeedOptions.index(self.readSingleCycleSpeedNum.get()))
        parseOMIndex(self.output, 24, 25, self.strobeSelOptions.index(self.strobeSelNum.get()))
        parseRSList(self.output, self.drscheckb.getcurselection())
        
        #### Operating parameters
        parseRSList(self.output, self.oscheckb.getcurselection())
        parseOMIndex(self.output, 592, 593, self.dllModeOptions.index(self.dllModeNum.get()))
        parseOMIndex(self.output, 84, 86, self.leadingResOptions.index(self.leadingResNum.get()))
        parseOMIndex(self.output, 584, 585, self.deadTimeOptions.index(self.deadTimeNum.get()))
        parseCtr(self.output, 627, 638, self.rolloverCtrNum, 25)
            
        #### Trigger matching parameters
        parseRSList(self.output, self.tmscheckb.getcurselection())
        parseOMIndex(self.output, 116, 119, self.maxEventSizeOptions.index(self.maxEventSizeNum.get()))
        tmpSV = StringVar()
        tmpSV.set(str((102400-int(self.trgCtrOffsetNum.get()))))
        parseCtr(self.output, 138, 149, tmpSV, 25)
        tmpSV.set(str((int(self.searchWindowSizeNum.get()) - 25)))
        parseCtr(self.output, 60, 71, tmpSV, 25)
        tmpSV.set(str((int(self.matchWindowSizeNum.get()) - 25)))
        parseCtr(self.output, 72, 83, tmpSV, 25)
        tmpSV.set(str((102400-int(self.rejectCtrOffsetNum.get()))))
        parseCtr(self.output, 48, 59, tmpSV, 25)

        #### Options page
        parseRSList(self.output, self.optionscheckb.getcurselection())
        parseCtr(self.output, 20, 23, self.serialDelayNum, 1)
        parseCtr(self.output, 27, 30, self.tokenDelayNum, 1)
        parseCtr(self.output, 126, 137, self.eventCtrOffsetNum, 1)
        parseOMIndex(self.output, 571, 574, self.widthSelectOptions.index(self.widthSelectNum.get()))
        parseSV(self.output, 580, 583, self.dllControlNum)
        parseSV(self.output, 594, 601, self.pllControlNum)
        parseSV(self.output, 575, 579, self.vernierOffsetNum)
        
        #### Offsets page
        for i in range(32):
            parseSV(self.output, 438-i*9, 446-i*9, self.chOffset[i])
        parseSV(self.output, 447, 458, self.coarseCountNum)
        parseSV(self.output, 602, 605, self.serialClockNum)
        parseSV(self.output, 606, 609, self.ioClockNum)
        parseSV(self.output, 610, 613, self.coreClockNum)
        parseSV(self.output, 614, 617, self.dllClockNum)
        
        #### Calibration page
        for i in range(32):
            parseSV(self.output, 459+i*3, 461+i*3, self.dllAdjust[i])
        for i in range(4):
            parseSV(self.output, 555+i*3, 557+i*3, self.rcAdjust[i])

        ### calculate parity
        parity = 0
        for i in range(len(self.output)):
            parity += int(self.output[i])
        self.output[646] = str(parity % 2)

    ############### LOAD PRESET ######################################################
    def loadPreset(self):
        #### Trouble shooting parameters
        self.testSelectNum.set(self.testSelectOptions[convertToInt(self.output, 0, 3)])
        self.fixedPatternNum.set(convertToString(self.output, 87,114))
        self.enableError.setvalue([]) # clear all first
        for i in range(6,(16+1)):
            if (self.output[i] == '1'):
                self.enableError.selection_set(i-6)
        self.tscheckb.setvalue([]) # clear all checkbuttons first
        for text in ('115', '122', '152', '153', '154', '155'):
            if (self.output[int(text)] == '1'):
                self.tscheckb.invoke(text)
        self.serialCSNum.set(self.serialCSOptions[convertToInt(self.output, 618, 619)])
        self.ioCSNum.set(self.ioCSOptions[convertToInt(self.output, 620, 621)])
        self.coreCSNum.set(self.coreCSOptions[convertToInt(self.output, 622, 623)])
        self.dllCSNum.set(self.dllCSOptions[convertToInt(self.output, 624, 626)])

        #### Data Readout parameters
        self.tdcIdentNum.set(convertToString(self.output, 40, 43))
        self.roFifoSizeNum.set(self.roFifoSizeOptions[convertToInt(self.output, 45, 47)])
        self.roTypeNum.set(self.roTypeOptions[convertToInt(self.output, 37, 39)])
        self.roSpeedSelNum.set(self.roSpeedSelOptions[int(self.output[26])])
        self.readSingleCycleSpeedNum.set(self.readSingleCycleSpeedOptions[convertToInt(self.output, 17, 19)])
        self.strobeSelNum.set(self.strobeSelOptions[convertToInt(self.output, 24, 25)])
        self.drscheckb.setvalue([]) # clear all checkbuttons first
        for text in ('4', '31', '32', '33', '34', '35', '36', '120'):
            if (self.output[int(text)] == '1'):
                self.drscheckb.invoke(text)
        
        #### Operating parameters
        self.oscheckb.setvalue([]) # clear all checkbuttons first
        for text in ('588', '589', '640', '590', '591'):
            if (self.output[int(text)] == '1'):
                self.oscheckb.invoke(text)
        self.dllModeNum.set(self.dllModeOptions[convertToInt(self.output, 592, 593)])
        self.leadingResNum.set(self.leadingResOptions[convertToInt(self.output, 84, 86)])
        self.deadTimeNum.set(self.deadTimeOptions[convertToInt(self.output, 584, 585)])
        self.rolloverCtrNum.set(convertToCtrString(self.output, 627, 638, 25))
        
        #### Trigger matching parameters
        self.tmscheckb.setvalue([]) # clear all checkbuttons first
        for text in ('639', '124', '125', '121'): 
            if (self.output[int(text)] == '1'):
                self.tmscheckb.invoke(text)
        self.maxEventSizeNum.set(self.maxEventSizeOptions[convertToInt(self.output, 116, 119)])
        self.trgCtrOffsetNum.set(str(102400 - 25*convertToInt(self.output, 138, 149)))
        self.searchWindowSizeNum.set(str(25 + 25*convertToInt(self.output, 60, 71)))
        self.matchWindowSizeNum.set(str(25 + 25*convertToInt(self.output, 72, 83)))
        self.rejectCtrOffsetNum.set(str(102400 - 25*convertToInt(self.output, 48, 59)))

        #### Options page
        self.optionscheckb.setvalue([]) # clear all checkbuttons first
        for text in ('5','44','123','150','151','156','157','158',
                     '570','586','587','641','642','643','644','645'):
            if (self.output[int(text)] == '1'):
                self.optionscheckb.invoke(text)
        self.serialDelayNum.set(convertToCtrString(self.output, 20, 23, 1))
        self.tokenDelayNum.set(convertToCtrString(self.output, 27, 30, 1))
        self.eventCtrOffsetNum.set(convertToCtrString(self.output, 126, 137, 1))
        self.widthSelectNum.set(self.widthSelectOptions[convertToInt(self.output, 571, 574)])
        self.dllControlNum.set(convertToString(self.output, 580, 583))
        self.pllControlNum.set(convertToString(self.output, 594, 601))
        self.vernierOffsetNum.set(convertToString(self.output, 575, 579))
        
        #### Offsets page
        for i in range(32):
            self.chOffset[i].set(convertToString(self.output, 438-i*9, 446-i*9))
        self.coarseCountNum.set(convertToString(self.output, 447, 458))
        self.serialClockNum.set(convertToString(self.output, 602, 605))
        self.ioClockNum.set(convertToString(self.output, 606, 609))
        self.coreClockNum.set(convertToString(self.output, 610, 613))
        self.dllClockNum.set(convertToString(self.output, 614, 617))

        #### Calibration page
        for i in range(32):
            self.dllAdjust[i].set(convertToString(self.output, 459+i*3, 461+i*3))
        for i in range(4):
            self.rcAdjust[i].set(convertToString(self.output, 555+i*3, 557+i*3))

    ###################### just for test purposes ################## 
    def print_testSelect(self):
        print self.testSelectNum.get()
        print "Index: ", self.testSelectOptions.index(self.testSelectNum.get())
        print "fixed patter = ", self.fixedPatternNum.get()
        print "checkbutton selection: ", self.tscheckb.getcurselection()
        print "enable error selection: ", self.enableError.curselection()

        # now change it
        #tsoIndex = self.testSelectOptions.index(self.testSelectNum.get())
        #self.output[tsoIndex] = '1'
        self.saveChanges()

        for x in range(0,8):
            sys.stdout.softspace = 0
            print self.output[x],
        print
        for x in range(8,16):
            sys.stdout.softspace = 0
            print self.output[x],
        print
        for x in range(631,639):
            sys.stdout.softspace = 0
            print self.output[x],
        print
        for x in range(639,647):
            sys.stdout.softspace = 0
            print self.output[x],
        print
        

##### This one for testing only ##########
class PrintOne:
    def __init__(self, text):
        self.text = text

    def __call__(self):
        print self.text

######################################################################


# Create demo in root window for testing.
if __name__ == '__main__':
    root = Tk()
    Pmw.initialise(root, size = 11, fontScheme = 'pmw1')
    #Pmw.initialise(root, size = 12, fontScheme = 'pmw2')
    #Pmw.initialise(root, size = 12, fontScheme = 'default')
    #Pmw.initialise(root, size = 12, fontScheme = None)
    #Pmw.initialise(root)
    tmpString = rtitle + " " + version
    root.title(tmpString)

    widget = Configurator(root)
    root.mainloop()
