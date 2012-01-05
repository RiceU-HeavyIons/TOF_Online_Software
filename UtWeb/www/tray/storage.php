<?php
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/session.php");
include("../../lib/template.php");

tsw_header("Trays in Storage");
tsw_toolbar("tray");
?>
<div id="main">
<div class="title">Trays in Storage</div>

<table>
<tr>
 <th>no.</th>
 <th>S/N</th>
 <th>TCPU</th>
 <th>TDIG</th>
 <th>ready to ship</th>
</tr>
<?php
$nready = 0;
$i = 1;
foreach (Tray::find_all("tray_status_id=5") as $t) {
  $ntcpu = Component::count("tray_id=$t->id and component_type_id=1");
  $ntdig = Component::count("tray_id=$t->id and component_type_id=2");
  $tcpu  = Component::find_one("tray_id=$t->id and component_type_id=1");

  if ($t->sn == "118") continue;

  if ($ntcpu > 0)
    if($tcpu->sn >= 18) {
      $str_tcpu = "TCPU-C";
    } else
      $str_tcpu = "TCPU-B";
  else
    $str_tcpu = "N/A";

  $ready = ($ntdig == 8 and $str_tcpu == "TCPU-C");
  if ($ready) $nready += 1;

  $row = $i % 2;
  echo "<tr class='row$row'>\n";
  echo "<th>" . $i . "</th>\n";
  echo "<td>" . $t->p_link() . "</td>\n";
  echo "<td>" . $str_tcpu . "</td>\n";
  echo "<td>" . $ntdig . "</td>\n";
  echo "<td>" . ($ready ? "yes" : "no") . "</td>\n";
  echo "</tr>\n";
  $i += 1;
}
?>
<tr>
  <th>total</th>
  <td></td>
  <td></td>
  <td></td>
  <td><?php echo $nready?></td>
</tr>
</table>
</div><!-- main -->
<?php tsw_footer() ?>