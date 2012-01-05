<html>
<head>
<title>Noise Test at WAH Procedure</title>

<style type="text/css">
div.header {
	font-size: 110%;
	text-align: center;	
}
div.subsection {
	font-size: 110%;
}
div.block {
	margin: 20px;
}
div.command {
	margin: 40px;
	padding: 10px;
	border: 1px solid black;
	font-family: sans-serif;
	
}
div.cent_img {
	text-align: center;
}
</style>

</head>
<body>
	<div class="header">
		Noise Test at WAH Procedure<br />
		Kohei Kajimoto (koheik@ph.utexas.edu)<br />
		July 15, 2009
	</div>
	<hr />
	
	<div class="subsection">Power Supplies</div>
	<div class="block">
		To control the HV &amp LV supplies, you need to login to <span style='font-style: italic'>tofp</span> first (remember to use the -X or -Y option when ssh'ing in) and bring up the GUI interface by the following command:
	</div>
	<div class="command">
		[tof@tofp ~] TestStand_GUI
	</div>
	<div class="cent_img">
		<img src="img/TestStand_GUI.png" width=600px>
	</div>
<!--
	<div class="command">
		[tof@tofp ~] tof-lv
	</div>
	<div class="cent_img">
		<img src="img/TOF_LV.png" width=600px>
	</div>
-->
	<div class="block">
		While the HV can be turn on/off at any time, turning on or off the LV requires a certain order (see below).
	</div>

	<div class="subsection">CANBus Frontend</div>
	<div class="block">
		In another terminal, login to <span style='font-style: italic'>tofcontrol</span>, start 'pcanloop' and wait until it shows driver version at end.
	</div>
	<div class="command">
		[tof@tofcontrol ~]$ cd koheik <br />
		[tof@tofcontrol koheik]$ ../tof/pcan/pcanloop <br />
		Trying to open PCAN devices with BTR0BTR1=0x0014 <br />
		        Device at /dev/pcan32: Hardware ID = 0xfe <br />
		        Device at /dev/pcan33: Hardware ID = 0xff <br />
		        Device at /dev/pcan34: Hardware ID = 0xfd <br />
		        Device at /dev/pcan35: Hardware ID = 0xfc <br />
		pcanloop: driver version = Release_20080220_n
	</div>
	
	<div class="block">
		At this point, you can power up LV U0 (THUB) and U1 (tray).
		Please note that THUB provides clock to tray electronics and has to be turn on before tray.
		Greeting messages from TCPU and TDIGs should appear in the pcanloop window.
	</div>
	<div class="command">
		pcanloop: message received : 252 m s 0x00000207 4  0xff 0x00 0x00 0x00 <br /> 
		pcanloop: message received : 252 m e 0x041c0020 4  0xff 0x00 0x00 0x00 <br />
		pcanloop: message received : 252 m e 0x045c0020 4  0xff 0x00 0x00 0x00 <br />
		pcanloop: message received : 252 m e 0x049c0020 4  0xff 0x00 0x00 0x00 <br />
		pcanloop: message received : 252 m e 0x04dc0020 4  0xff 0x00 0x00 0x00 <br />
		pcanloop: message received : 252 m e 0x051c0020 4  0xff 0x00 0x00 0x00 <br />
		pcanloop: message received : 252 m e 0x055c0020 4  0xff 0x00 0x00 0x00 <br />
		pcanloop: message received : 252 m e 0x059c0020 4  0xff 0x00 0x00 0x00 <br />
		pcanloop: message received : 252 m e 0x05dc0020 4  0xff 0x00 0x00 0x00
	</div>

	<div class="block">
		Press Ctrl-C to quit pcanloop, start GUI for tray, select your name, CANBus device (252) and a tray you want to test.
	</div>
	<div class="command">
		[tof@tofcontrol koheik]$ ./test_stand.sh
	</div>

	<div class="cent_img">
		<img src="img/anaconda.png" width="300pt">
	</div>

	<div class="block">
		Although it should be ready to take noise rates at this point,
		you may want to press 'diag' button (shows firmware versions) and/or
		'read temp' button (shows temperature readings from all TDIGs) to make sure
		CANBus connection is working.
	</div
	
	<div class="cent_img">
		<img src="img/anaconda_diag.png">		
	</div>

	<div class="block">Press 'start / stop' to start and stop noise test run.</div>
	
	<div class="command">
		start noise run for tray 11 -- 2008-07-11 12:24:52 <br />
		... snip ... <br />
		253 events -- saved in 20080711_122452.tgz
	</div>

	<div class="subsection">Web Interface</div>
	<div class="block">
		Data you obtained can be uploaded into UT database via web interface.
		Select tarball which is shown at the end of previous step from
		the directory /home/data/wah and submit it.
	</div>
	<div class="cent_img">
		<img src="img/web.png" width=600px>
	</div>
	
	<div class="subsection">Trouble Shootings</div>
	<div class="block">
		Please contact me at <a href="mailto:koheik@ph.utexas.edu">koheik@ph.utexas.edu</a> or 512-471-0695 (office).
	</div>
</body>
</html>
