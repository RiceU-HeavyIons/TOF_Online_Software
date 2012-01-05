<?php
include_once("../../../lib/connect.php");
include_once("../../../lib/orm.php");

echo "<?xml version=\"1.0\" ?>\n";
$query = urldecode($_GET['q']);

echo "<root>\n";
if($query) {
  $search = "note REGEXP \"{$query}\"";
} else {
  $search = "";
}
foreach(TrayNote::find_all($search, "created_on desc", "") as $tn) {
   $ty = new Tray($tn->tray_id);

#  Drew uses <font> tag without end-tag, which is fine as HTML but
#  not as XML so I'll simply clear the tags here.
   $nt = preg_replace('/<\w[^>]*>|<\/\w[^>]*>/', '', $tn->note);
   $nt = preg_replace('/&/', '&amp;', $nt);
   echo " <item>\n";
   echo "  <tray_id>{$tn->tray_id}</tray_id>\n";
   echo "  <tray_sn>{$ty->sn}</tray_sn>\n";
   echo "  <type>{$tn->tray_note_type_id}</type>\n";
   echo "  <date>{$tn->created_on}</date>\n";
   echo "  <note>{$nt}</note>\n";
   echo " </item>\n";
}
echo "</root>\n";
?>
