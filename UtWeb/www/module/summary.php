<?php
# $Id: summary.php,v 1.6 2008/05/29 19:28:14 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/utility.php");
include("../../lib/session.php");
include("../../lib/template.php");
tsw_header("Module Summary");
tsw_toolbar('module');
?>
<div id="main">
<div class="title">Module Summary</div>
<form  action="show.php" method="get">
Find Module by Serial Number: <input name="sn" type="text" />
<input name="commit" type="submit" value="find" />
</form>
<?php
$lim_tr = isset($rvar_tr)? $rvar_tr : 0;
$lim_nr = isset($rvar_nr)? $rvar_nr : 0;
if($lim_tr <= 0) $lim_tr = 120.00;
if($lim_nr <= 0) $lim_nr = 50.00;
threshold_form($lim_tr, $lim_nr, 'summary.php');
$st = Module::stats($lim_tr, $lim_nr);
#$st = ModuleStats($lim_tr, $lim_nr);
?>

<p>We have tested <?php echo $st["tested"] ?> of <?php echo $st["total"] ?> modules since 8/1/06.</p>

<p><?php echo $st["passed"]?> of them passed and <?php echo $st["failed"]?> of them failed the condition
(timing resolution &lt; <?php echo $lim_tr ?> ps and noise rate &lt; <?php echo $lim_nr ?> Hz).</p>
</div><!-- main -->
<?php
  tsw_footer();
  mysql_close($con);
?>
