<?php
# $Id: template.php,v 1.34 2009/08/17 17:11:07 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.

function tsw_header($title) {
  $tsw = new Tsw($title);
  $tsw->header();
}

function tsw_header_upload($title) {
  echo "<?xml version='1.0' encoding='iso-8859-1'?>\n";
  echo "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>\n";
  echo "<html xmlns='http://www.w3.org/1999/xhtml' xml:lang='en' lang='en'>\n";
  echo "<head>\n";
  echo "<title>$title</title>";
  echo "<link rel='SHORTCUT ICON' href='../style/favicon.ico' />\n";
  echo "<link rel='stylesheet' type='text/css' href='../style/default.css' />\n";
  echo "<script src='../scripts/toolbar.js' type='text/javascript'></script>\n";
  echo "<script src='../scripts/magnifier.js' type='text/javascript'></script>\n";
  echo "<script src='http://www.google-analytics.com/urchin.js' type='text/javascript'></script>\n";
  echo "<script type='text/javascript'>\n";
  echo "<!--\n";
  echo "  _uacct = 'UA-1852182-1';\n";
  echo "  urchinTracker();\n";
  echo "var sURL = unescape(window.location.pathname);\n";
  echo "function doLoad() { setTimeout('refresh()', 10*60*1000); } // wait 10 minutes\n";
  echo "function refresh() { window.location.href = sURL; } // reload\n";
  echo "//-->\n";
  echo "</script>\n"; 
  echo "</head>\n";
  echo "<body onload='doLoad()'>\n";
}

function tsw_footer() {
  $tsw = new Tsw("");
  $tsw->footer();
}

function tsw_toolbar($current) {
  $tsw = new Tsw("");
  $tsw->toolbar($current);
}

class Tsw {
  var $title;
  var $body_opt;
  var $opt;
  var $top;
  var $dtop = 'http://www.rhip.utexas.edu';

  function Tsw($title) {
    $this->title = $title;
    $this->top = "..";
  }

  function set_opt($opt) {
    $this->opt = $opt;
  }
  
  function set_body_opt($opt) {
    $this->body_opt = $opt;
  }
  function set_top($depth) {
     $this->top = $depth;
  }

  function header() {
    echo "<?xml version='1.0' encoding='iso-8859-1'?>\n";
    echo "<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Strict//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'>\n";
    echo "<html xmlns='http://www.w3.org/1999/xhtml' xml:lang='en' lang='en'>\n";
    echo "<head>\n";
    echo "<title>{$this->title}</title>";
    echo "<link rel='SHORTCUT ICON' href='{$this->top}/style/favicon.ico' />\n";
    echo "<link rel='stylesheet' type='text/css' href='{$this->top}/style/default.css' />\n";
    echo "<script src='{$this->top}/scripts/toolbar.js' type='text/javascript'></script>\n";
    echo "<script src='{$this->top}/scripts/magnifier.js' type='text/javascript'></script>\n";
    echo "<script src='http://www.google-analytics.com/urchin.js' type='text/javascript'></script>\n";
    echo "<script type='text/javascript'>\n";
    echo "<!--\n";
    echo "  _uacct = 'UA-1852182-1';\n";
    echo "  urchinTracker();\n";
    echo "//-->\n";
    echo "</script>\n";
    if ($this->opt) { echo $this->opt; }
    echo "</head>\n";
    echo "<body{$this->body_opt}>\n";
  }

  function toolbar($current) {
    $mlst = array(
      array('back', 'javascript:history.back();', null),
      array('top', "{$this->dtop}/drupal/TOF",null),
      array('xoop', "{$this->top}", null), 
      array('tray', "{$this->top}/tray/index.php",
        array(
          array('status', "{$this->top}/tray/status.php"),
          array('status_in_xls', "{$this->top}/tray/status_in_xls.php"),
          array('storage', "{$this->top}/tray/storage.php"),
          array('note', "{$this->top}/tray/note/")
          )),
      array('mtd_tray', "{$this->top}/mtd_tray/index.php",
        array(
          array('status', "{$this->top}/mtd_tray/status.php"),
          array('status_in_xls', "{$this->top}/mtd_tray/status_in_xls.php"),
          array('storage', "{$this->top}/mtd_tray/storage.php"),
          array('note', "{$this->top}/mtd_tray/note/")
          )),
      array('module', "{$this->top}/module/index.php",
        array(
          array('search', "{$this->top}/module/search.php"),
          array('summary', "{$this->top}/module/summary.php"),
          array('statistics', "{$this->top}/module/stat.php"),
          array('size measurements', "{$this->top}/module/size.php"),
          array('bad modules', "{$this->top}/module/bad.php"),
          array('good modules', "{$this->top}/module/good.php"),
          array('virgin modules', "{$this->top}/module/virgin.php"),
          array('check database', "{$this->top}/module/check.php")
      )),
      array('component', "{$this->top}/component/index.php", 
        array(
          array('summary', "{$this->top}/component/summary.php"),
          array('search', "{$this->top}/component/search.php"),
          array('tcpu', "{$this->top}/component/tcpu_list.php"),
          array('tdig', "{$this->top}/component/tdig_list.php"),
          array('tino', "{$this->top}/component/tino_list.php")
      )),
      array('hvtest', "{$this->top}/hvtest/index.php",
        array(
          array('list all', "{$this->top}/hvtest/list.php"),
          array('entry page', "{$this->top}/hvtest/notes.php")
      )),
      array('run', "{$this->top}/run/index.php",
        array(
          array('summary', "{$this->top}/run/summary.php"),
      )),
      array('config', "{$this->top}/config/list.php", null),
      array('system', "{$this->top}/system/index.php", null),
      array('map', "{$this->top}/map/list.php", null),
#        array(
#          array('tot', '../system/tot.php'),
#          array('paddle', '../system/paddle.php')
#      )),
      array('wah', "{$this->top}/wah/",
        array(
          array('tray list', "{$this->top}/wah/tray_list.php"),
          array('user list', "{$this->top}/wah/user_list.php"),
          array('help', "{$this->top}/wah/help.php")
      )),
      Session::loggedin() ? 
        array('logout', "{$this->top}/account/logout.php", null) :
        array('login', "{$this->top}/account/login.php", null)
    );

    echo "<div id='toolbar' onmouseover='tbon()' onmouseout='tboff()'>\n";
    $cnt = 0;
    foreach ($mlst as $m) {
      $cur = ($m[0] == $current) ? " id='current'" : '';
      echo "<div class='tmenu' onmouseover=\"topen(event, 'tsub$cnt');\" onmouseout=\"tclose(event, 'tsub$cnt');\">\n";
      echo "<div class='tmenut'><a href='$m[1]'$cur>$m[0]</a></div>\n";
      echo "<div class='tsub' id='tsub$cnt'>\n";
      if($m[2]) {
        foreach($m[2] as $sm) {
          echo "<div class='tsubi'><a href='$sm[1]'>$sm[0]</a></div>\n";
        }
      }
      echo "</div>\n";
      echo "</div>\n";
      $cnt += 1;
    }
    echo "</div>\n";
    echo "<div style='clear: right; margin: 0px; padding: 0px;'></div>\n";
  }

  function footer($debug) {
    if(isset($debug)) {
      echo "<address>\n";
      echo " <span style='float: right;'>\n";
      echo "  <a href='http://validator.w3.org/check?uri=referer'>\n";
      echo "   <img style='border:0;width:88px;height:31px;' src='http://www.w3.org/Icons/valid-xhtml10' alt='Valid XHTML 1.0!' />\n";
      echo "  </a>\n";
      echo " </span>\n";
      echo "</address>\n";
      echo "<div style='clear: right;'></div>\n";
    }
    echo "</body>\n";
    echo "</html>\n";
  }
}

?>
