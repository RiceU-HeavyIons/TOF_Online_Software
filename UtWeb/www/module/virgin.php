<?php
# $Id: virgin.php,v 1.3 2008/05/29 19:28:14 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
include('../../lib/connect.php');
include('../../lib/orm.php');
include('../../lib/session.php');
include('../../lib/template.php');
tsw_header('Virgin Modules');
tsw_toolbar('module');

print "<div id='main'>\n";
print "<div class='title'>Virgin Modules</div>\n";

print "<div class='section'>\n";
print "<div class='title'>UT Modules</div>\n";
print "<div style='font-family: courier;'>\n";
foreach(Module::find_not_tested("vendor_id=1", "serial asc") as $mod) {
	$link = $mod->p_link();
	print "<div style='float: left;'>{$link}</div>\n";
}
print "</div><div style='clear: both'></div>\n";
print "</div><!-- section -->\n";

print "<div class='section'>\n";
print "<div class='title'>TM Modules</div>\n";
print "<div style='font-family: courier;'>\n";
foreach(Module::find_not_tested("vendor_id=2", "serial asc") as $mod) {
	$link = $mod->p_link();
	print "<div style='float: left;'>{$link}</div>\n";
}
print "</div><div style='clear: both'></div>\n";
print "</div><!-- section -->\n";

print "</div><!-- main -->\n";
tsw_footer();
?>