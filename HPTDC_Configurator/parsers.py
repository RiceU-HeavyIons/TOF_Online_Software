#Utility functions
#
# $Id: parsers.py,v 1.1 2005-11-04 15:43:34 jschamba Exp $

import string

#### Entry fields return StringVar ####
def parseSV(outvar, first, last, stVar):
    textvar = stVar.get()
    for i in range (first, last+1):
        outvar[i] = textvar[last-i]
    

### option menues return an index ########    
def parseOMIndex(outvar, first, last, oindex):
    for i in range (first, last+1):
        outvar[i] = str(oindex % 2)
        # print i, oindex, outvar[i]
        oindex = int(oindex/2)
        
        
### RadioSelect button groups return a list of names which are indices ####
def parseRSList(outvar, rslist):
    #print rslist
    for i in rslist:
        outvar[int(i)] = '1'
        
### ListBox returns a list of indices ####
def parseLBList(outvar, first, lblist):
    #print lblist
    for i in lblist:
        outvar[int(i)+first] = '1'

### Counter returns an integer that needs to be divided by "factor" first ##
def parseCtr(outvar, first, last, ctrVal, factor):
    ctr = int(ctrVal.get())/factor
    for i in range (first, last+1):
        outvar[i] = str(ctr % 2)
        ctr = int(ctr/2)

def convertIntToStringVar(inInt, outSV, width):
    tempStr = ''
    for i in range(width):
        t = tempStr
        tempStr = str(inInt % 2) + t
        inInt = int(inInt/2)
    outSV.set(tempStr)

def convertToInt(inList, first, last):
    tmpList = inList[first:(last+1)]
    tmpInt = 0
    fac = 1
    for i in range(len(tmpList)):
        tmpInt += int(tmpList[i])*fac
        fac *= 2
    return tmpInt

def convertToString(inList, first, last):
    tmpList = inList[first:(last+1)]
    tmpList.reverse()
    return string.join(tmpList,"")

def convertToCtrString(inList, first, last, factor):
    tmpList = inList[first:(last+1)]
    tmpInt = 0
    fac = 1
    for i in range(len(tmpList)):
        tmpInt += int(tmpList[i])*fac
        fac *= 2
    tmpInt *= factor
    return str(tmpInt)

