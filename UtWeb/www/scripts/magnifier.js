// $Id: magnifier.js,v 1.4 2007/08/21 11:18:11 tofp Exp $
// Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
function magnifier(){ /* create namespace */}

magnifier.over = function(event, name) {
	var e = document.getElementById(name);
//	var e = document.getElementById('mini');
//  e.innerHTML = "<img src='http://www.rhip.utexas.edu/~tofp/var/r/10000250/fig/" + name + "' />"; 
	var x = event.clientX;
	var y = event.clientY;
	var iw = 0;
	var ih = 0;
	
	if( typeof(window.innerWidth) == 'number' ) {//Firefox and Safari
		iw = window.innerWidth;
		ih = window.innerHeight;
	} else if(document.documentElement) { // IE6+
		iw = document.documentElement.clientWidth;
		ih = document.documentElement.clientHeight;
	}
	var w = 640;
	var h = 480;
	var mx = w/20;
	var my = h/20;
	
//	Safari's clientX/Y are page wise, so let's subtract offsets from x/y.
	if(event.pageX == x && event.pageY == y) {
		x -= window.pageXOffset;
		y -= window.pageYOffset;
	}

// from the center of the display
	var dx = x - iw/2;
	var dy = y - ih/2;

// element's position coordinates have to be page wise.
	if(window.pageXOffset || window.pageYOffset) { // Non-IE
		x += window.pageXOffset;
		y += window.pageYOffset;
	} else if(document.documentElement) { // IE6+
		x += document.documentElement.scrollLeft;
		y += document.documentElement.scrollTop;
	}
	
	var gx = 0;
	var gy = 0;
	
	if(dx==0 && dy==0) {
		gx = x+mx;
		gy = y+my;
	} else if(w*Math.abs(dy) < h*Math.abs(dx)) {
		if(dx<=0)
			gx = x + mx;
		else
			gx = x - (w+mx);
		gy = y - (h+my) - (h+2*my)*w/h/2*(dy/Math.abs(dx) - h/w);
	} else if(w*Math.abs(dy) >= h*Math.abs(dx)) {
		gx = x - (w+mx) - (w+2*mx)*h/w/2*(dx/Math.abs(dy) - w/h);
		if(dy<=0)
			gy = y + my;
		else
			gy = y - (h+my);
	}
	e.style.left = parseInt(gx) + "px";
	e.style.top  = parseInt(gy) + "px";
	e.style.visibility = "visible";
}

magnifier.out = function(event, name) {
	var e = document.getElementById(name);
//	var e = document.getElementById('mini');
	e.style.top = "0px";
	e.style.left = "0px";
	e.style.visibility = "hidden";
}
