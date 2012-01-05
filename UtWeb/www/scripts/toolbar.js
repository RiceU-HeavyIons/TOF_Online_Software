// $Id: toolbar.js,v 1.3 2007/08/21 11:18:11 tofp Exp $
// Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
function tbon(){
	var e = document.getElementById('toolbar');
	e.style.opacity = "0.99";
}

function tboff(){
	var e = document.getElementById('toolbar');
	e.style.opacity = "0.2";
}

function topen(e, name) {
	document.getElementById(name).style.display = 'inline';
	if(e && e.currentTarget) {
		e.currentTarget.style.background = "#e0e0e0";
	}
}
function tclose(e, name) {
	document.getElementById(name).style.display = 'none';
	if(e && e.currentTarget) {
		e.currentTarget.style.background = "#ffffff";
	}
}
