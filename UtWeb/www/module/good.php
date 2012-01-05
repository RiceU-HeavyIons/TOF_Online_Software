<?php
# $Id: good.php,v 1.3 2008/05/29 19:28:14 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/utility.php");
include("../../lib/session.php");
include("../../lib/template.php");
tsw_header("List of Good Modules");
tsw_toolbar('module');
?>
<div id="main">
<div class="title">List of Good Modules</div>
<div class="section">
<div class="title">Full Test</div>
<div>
<?php
$tr_limit = isset($rvar_tr)? $rvar_tr : 0;
$nr_limit = isset($rvar_nr)? $rvar_nr : 0;
if($tr_limit <= 0) $tr_limit = 120.00;
if($nr_limit <= 0) $nr_limit = 50.00;
# threshold_form($tr_limit, $nr_limit, 'bad.php');
?>
<table>
<tr><th></th><th>SN</th><th>T.R.</th><th>N.R.</th><th>HvTest</th><th>Note</th></tr>
<?php
$cnt = 1;
foreach(Module::find_fully_tested($tr_limit, $nr_limit) as $mod) {
  $tr = $mod->max_tr();
  $nr = $mod->max_nr();
  echo "<tr class='row", ($cnt+1)%2, "'>\n";
  echo "  <td>", $cnt++, ".</td>\n";
  echo "  <td>", $mod->p_link(),"</td>\n";
  printf("  <td style='text-align: right; white-space: nowrap;'>%5.1f ps</td>\n", $tr);
  printf("  <td style='text-align: right; white-space: nowrap;'>%5.1f Hz</td>\n", $nr);
  echo "  <td style='white-space: nowrap;'>", $mod->p_hvtest(),"</td>\n";
  echo "  <td>", $mod->note,"</td>\n";
  echo "</tr>\n";
}
?>
</table></div>
</div> <!-- section -->

<div class="section">
<div class="title">UM Modules</div>
<div>
<table>
<tr><th></th><th>SN</th><th>T.R.</th><th>N.R.</th><th>HvTest</th><th>Note</th></tr>
<?php
$cnt = 1;
foreach(Module::find_minimally_tested($tr_limit, $nr_limit, 1, 40) as $mod) {
  $tr = $mod->max_tr();
  $nr = $mod->max_nr();
  echo "<tr class='row", ($cnt+1)%2, "'>\n";
  echo "  <td>", $cnt++, ".</td>\n";
  echo "  <td>", $mod->p_link(),"</td>\n";
  if($tr > 0.0)
    printf("  <td style='text-align: right; white-space: nowrap;'>%5.1f ps</td>\n", $tr);
  else
    echo "  <td></td>\n";
  if($nr > 0.0)
    printf("  <td style='text-align: right; white-space: nowrap;'>%5.1f Hz</td>\n", $nr);
  else
    echo "  <td></td>\n";
  echo "  <td style='white-space: nowrap;'>", $mod->p_hvtest(),"</td>\n";
  echo "  <td>", $mod->note,"</td>\n";
  echo "</tr>\n";
}
?>
</table></div>
</div> <!-- section -->

<div class="section">
<div class="title">TM Modules</div>
<div>
<table>
<tr><th></th><th>SN</th><th>T.R.</th><th>N.R.</th><th>HvTest</th><th>Note</th></tr>
<?php
$cnt = 1;
foreach(Module::find_minimally_tested($tr_limit, $nr_limit, 2, 40) as $mod) {
  $tr = $mod->max_tr();
  $nr = $mod->max_nr();
  echo "<tr class='row", ($cnt+1)%2, "'>\n";
  echo "  <td>", $cnt++, ".</td>\n";
  echo "  <td>", $mod->p_link(),"</td>\n";
  if($tr > 0.0)
    printf("  <td style='text-align: right; white-space: nowrap;'>%5.1f ps</td>\n", $tr);
  else
    echo "  <td></td>\n";
  if($nr > 0.0)
    printf("  <td style='text-align: right; white-space: nowrap;'>%5.1f Hz</td>\n", $nr);
  else
    echo "  <td></td>\n";
  echo "  <td style='white-space: nowrap;'>", $mod->p_hvtest(),"</td>\n";
  echo "  <td>", $mod->note,"</td>\n";
  echo "</tr>\n";
}
?>
</table></div>
</div> <!-- section -->

</div><!-- main -->
<?php tsw_footer(); ?>
