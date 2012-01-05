// $Id: config_editor.js,v 1.2 2008/10/20 18:17:25 tofp Exp $
// Copyright (C) 2008, Kohei Kajimoto. All rights reserved.

function init_new() {
  var e = document.getElementById("config_type");
  e.addEventListener("change", type_handle, false);

  init_edit();
}

function init_edit() {

  for(var i = 1; i <= 32; i++) {
    var e = document.getElementById("tx" + i);
    e.addEventListener("change", tx_handle, false);
  }

  for(var i = 1; i <= 3; i++) {
    var e = document.getElementById("datime" + i);
    e.addEventListener("change", datime_handle, false);
  }
}

function type_handle(e) {
  var nmod = document.getElementById("n_modules");
  var flw1 = document.getElementById("flow_rate1");
  var gch1 = document.getElementById("gas_channel_1");
  var hvcp = document.getElementById("hv_channel_plus");
  var hvcm = document.getElementById("hv_channel_minus");

  if (e.target.value == 1) {
    nmod.value = 4;
    flw1.value = "25.0";
    gch1.value = "1";
    hvcp.value = "100";
    hvcm.value = "101";
  }
  if (e.target.value == 2) {
    nmod.value = 32;
    flw1.value = "40.0";
    gch1.value = "0";
    hvcp.value = "5";
    hvcm.value = "1";
  }
  if (e.target.value == 3) {
    nmod.value = 96;
    flw1.value = "200.0";
    gch1.value = "1";
    hvcp.value = "4";
    hvcm.value = "0";
  }

  if (e.target.value == 4) {
    nmod.value = 32;
    flw1.value = "200.0";
    gch1.value = "4";
    hvcp.value = "7";
    hvcm.value = "3";
  }
}

function tx_handle(e) {
  var etype = document.getElementById('config_type');
  var eauto = document.getElementById('auto');

  if (etype.value == 1 || etype.value == 2) {
    var idx = -1;

    for (i = 0; i < mod_list.length; i++) {
      if(mod_list[i] == e.target.value.toUpperCase()) idx = i;
    }
    if (idx >= 0) {
      if(eauto.checked) {
        var j = parseInt(e.target.name);
        var n_mods = document.getElementById('n_modules');
        var lim = Math.min(32, n_mods.value);
        for(i = j; i <= lim; i++) {
          et = document.getElementById("tx" + i);
          if (sn  = mod_list[idx + (i - j)])
            et.value = sn;
          else
            break;
        }
      }
    } else if (e.target.value != '') {
      alert(e.target.value + " dose note exist in our list");
      e.target.value = '';
      e.target.select();
    }
  } else { // for trays
    for (i = 0; i < try_list.length; i++) {
      if(try_list[i] == e.target.value) idx = i;
    }
    if (idx >= 0) {
      if(eauto.checked) {
        var j = parseInt(e.target.name);
        var n_mods = document.getElementById('n_modules');
        var lim = Math.min(32, n_mods.value / 32);
        for(i = j; i <= lim; i++) {
          et = document.getElementById("tx" + i);
          if (sn  = try_list[idx + (i - j)])
            et.value = sn;
          else
            break;
        }
      }
    } else if (e.target.value != '') {
      alert(e.target.value + " dose note exist in our list");
      e.target.value = '';
      e.target.select();
    }
  }
}

function datime_handle(e) {
  if(!e.target.value.match(/^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$/)) {
    alert("input is not SQL datetime format!");
    e.target.value = '2000-01-01 12:00:00';
  }
}

function verify() {
  var etype  = document.getElementById('config_type');
  var n_mods = document.getElementById('n_modules');

  var ids = "--- \n";
  if(etype.value == 1 || etype.value == 2) {
    var lim = Math.min(32, n_mods.value);
    for(var i = 1; i <= lim; i++) {
      var e = document.getElementById("tx" + i);
      var mid = mod_dict[e.value];
      if (mid) {
        ids += "- " + mid + "\n";
      } else {
        ids += "- \n";
      }
    }
  } else {
    var lim = Math.min(32, n_mods.value/32);
    for(var i = 1; i <= lim; i++) {
      var e = document.getElementById("tx" + i);
      var tid = try_dict[e.value];
      if (tid) {
        ids += "- " + tid + "\n";
      } else {
        ids += "- \n";
      }
    }
  }
  var e = document.getElementById('module_ids');
  e.value = ids;
  return true;
}

function two_digit(i) {
  var ii = i.toString();
  if(ii.length < 2) ii = '0' + ii;
  return ii;
}

function input_now(name) {
  var e = document.getElementById(name);
  var dt = new Date();
//  dt.setSeconds(0);
  var dy = (1900 + dt.getYear()).toString();
  var dm = two_digit(dt.getMonth() + 1);
  var dd = two_digit(dt.getDate());
  var th = two_digit(dt.getHours());
  var tm = two_digit(dt.getMinutes());
  e.value = dy + '-' + dm + '-' + dd + ' ' + th + ':' + tm + ':00';
}

