<?php
include("../../lib/connect.php");
include("../../lib/orm.php");
include("../../lib/session.php");
include("../../lib/template.php");
include("../../lib/utility.php");

tsw_header('Backleg List');
tsw_toolbar('mtd_tray');

if(Session::loggedin()) {
  $action = './edit.php';
} else {
  $action = './show.php';
}

echo "<div id='main'>\n";
echo "<div class='title'>Component List</a></div>\n";

if(Session::loggedin()) {
  echo "<div><a href='./create.php'>new component</a></div>\n";
}
echo "<table style='border: 1px solid black;'>\n";
echo " <tr>\n";
echo "  <th>id</th>\n";
echo "  <th>type</th>\n";
echo "  <th>sn</th>\n";
echo "  <th>date</th>\n";
echo "  <th>chip sn</th>\n";
echo "  <th>tray</th>\n";
echo "  <th>slot</th>\n";


$ln = 0;
foreach(Component::find_all("","id desc", "$off, $lim", "") as $c) {
  $ll = $ln % 2;
  $try = new Tray($c->tray_id);
  echo " <tr class='row$ll'>\n";
  echo "  <td><a href='{$action}?id={$c->id}'>{$c->id}</a></td>\n";
  echo "  <td>" . $c->str_type() . "</td>\n";
  echo "  <td>{$c->serial}</td>\n";
  echo "  <td>{$c->date}</td>\n";
  echo "  <td>{$c->chip_sn}</td>\n";
  echo "  <td>" . $try->p_link() . "</td>\n";
  echo "  <td>{$c->slot}</td>\n";
  echo " </tr>\n";
  $ln += 1;
}
echo "</table>\n";
echo "</div><!-- main -->\n";
tsw_footer();
?>
