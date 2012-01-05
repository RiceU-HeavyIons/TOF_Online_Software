<?php
# $Id: list.php,v 1.1 2009/07/20 22:22:58 tofp Exp $
include("../../lib/orm.php");
include("../../lib/connect.php");
include("../../lib/session.php");
Session::update_ref("../map/list.php");

include("../../lib/template.php");
tsw_header("Map List");
tsw_toolbar("map");

function elm_a($adr, $txt, $atr) {
  $ret = "<a href=\"$adr\"";
  if ($atr) $ret .= " $atr";
  $ret .= ">$txt</a>";
  return $ret;
}

?>
<div id="main">
<div class="title">Map List</div>
<table id="map_list">
<tr>
 <th>ID</th>
 <th>Date</th>
 <th>Name</th>
 <th>Note</th>
</tr>

<?php
$l = 1;
foreach (Map::find_all("", "id desc", "0, 100") as $map) {
   $l += 1;

   $id = $map->id;
   $ll = $l % 2;
   echo " <tr class='row$ll'>";
   echo "  <td>" . elm_a("./show.php?id={$map->id}", $map->id, "") . "</td>\n";
   echo "  <td style='white-space: nowrap;'>", $map->created_at, "</td>\n";
   echo "  <td style='white-space: nowrap;'>", $map->user->name, "</td>\n";
   echo "  <td style='white-space: nowrap;'>", $map->name, "</td>\n";
   echo " </tr>";
 }

?>
</table>
</div><!-- main -->
<?php tsw_footer(false); ?>
<?php mysql_close($con); ?>
