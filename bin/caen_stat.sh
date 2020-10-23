#!/bin/sh
caget TOF:Negative:HV:East:2:2:4:vmon.STAT
caget TOF:Negative:HV:East:2:2:4:imon.STAT
caget TOF:Negative:HV:West:3:2:5:vmon.STAT
caget TOF:Negative:HV:West:3:2:5:imon.STAT

caget TOF:Positive:HV:East:2:14:4:vmon.STAT
caget TOF:Positive:HV:East:2:14:4:imon.STAT
caget TOF:Positive:HV:West:3:14:5:vmon.STAT
caget TOF:Positive:HV:West:3:14:5:imon.STAT

echo ""

caget TOF:Negative:HV:East:2:2:4:vmon
caget TOF:Negative:HV:East:2:2:4:imon
caget TOF:Negative:HV:West:3:2:5:vmon
caget TOF:Negative:HV:West:3:2:5:imon
caget TOF:Positive:HV:East:2:14:4:vmon
caget TOF:Positive:HV:East:2:14:4:imon
caget TOF:Positive:HV:West:3:14:5:vmon
caget TOF:Positive:HV:West:3:14:5:imon
