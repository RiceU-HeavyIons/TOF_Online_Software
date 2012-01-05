<?php
   include("../../../lib/connect.php");
   include("../../../lib/orm.php");

   echo "<?xml version=\"1.0\"?>\n";
   echo "<rss version=\"2.0\">\n";
   echo "  <channel>\n";
   echo "    <title>Tray Notes Feed</title>\n";
   echo "    <language>en-US</language>\n";
#   echo "    <ttl>5</ttl>\n";

   foreach(TrayNote::find_all("","created_on desc","10") as $tn) {
      $user = new User($tn->user_id);
      $tray = new Tray($tn->tray_id);
      $hs = md5($tn->creaetd_on . "+" . $tn->tray_id . "+" . $tn->note);
      echo "    <item>\n";
      echo "      <title>" . $user->name . " on tray " . $tray->sn . "</title>\n";
      echo "      <link>http://www.rhip.utexas.edu/~tofp/tray/view.php?id={$tray->id}</link>\n";
      echo "      <description>" . $tn->note. "</description>\n";
      echo "      <pubDate>" . $tn->created_on . " 00:00:00</pubDate>\n";
      echo "      <guid>http://www.rhip.utexas.edu/~tofp/tray/view.php?id={$tray->id}#tn{$tn->id}</guid>\n";
      echo "    </item>\n";
   }

   echo "  </channel>\n";
   echo "</rss>\n";

?>