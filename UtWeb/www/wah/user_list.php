<?php
  include("../../lib/connect.php");
  include("../../lib/orm.php");
  header("Content-type: text/plain");

  $trs = Tray::find_all("id >= 4", 'id', '');
  foreach(User::find_all('', '', '') as $u) {
    echo $u->id . " " . $u->name . "\n";
  }
?>
