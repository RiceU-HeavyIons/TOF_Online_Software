<?php
include("../../../lib/connect.php");
include("../../../lib/orm.php");
include("../../../lib/session.php");
include("../../../lib/template.php");


$tsw = new Tsw("Tray Note");
$tsw->set_top("../..");
$opt =<<< EOF
<link rel="alternate" type="application/rss+xml" title="tray note feed" href="./rss.php" />

<script type="text/javascript" language="jaavscript">
function makeRequest(url) {


  var e = document.getElementById('query');
  var full_url = url + "?q=" + escape(e.value);

  var httpRequest;
  if (window.XMLHttpRequest) { // Mozilla, Safari, ...
    httpRequest = new XMLHttpRequest();
    if (httpRequest.overrideMimeType) {
      httpRequest.overrideMimeType('text/xml');
    }
  }

  httpRequest.onreadystatechange = function() { alertContents(httpRequest); };
  httpRequest.open('GET', full_url, true);
  httpRequest.send('');

  return false;
}

function alertContents(httpRequest) {
  if (httpRequest.readyState == 4) {
    if (httpRequest.status == 200) {
      var e = document.getElementById("result");
      var xmldoc = httpRequest.responseXML;

      var root = xmldoc.getElementsByTagName('root').item(0);
      var items = root.getElementsByTagName('item');
      var table = '<table>\\n';
      table += "<tr><th>tray</th><th>date</th><th>note</th></tr>\\n";
      for(var i = 0; i < items.length; i++) {
        var tid = items[i].getElementsByTagName('tray_id')[0].childNodes[0].nodeValue;
        var tsn = items[i].getElementsByTagName('tray_sn')[0].childNodes[0].nodeValue;
        var dat = items[i].getElementsByTagName('date')[0].childNodes[0].nodeValue;
        var txt = items[i].getElementsByTagName('note')[0].childNodes[0].nodeValue;
        var typ = items[i].getElementsByTagName('type')[0].childNodes[0].nodeValue;
        var sty = ""
        switch(parseInt(typ)) {
          case 0:
            sty = 'style="color: black;"';
            break;
          case 1:
            sty = 'style="color: red;"';
            break;
          case 2:
            sty = 'style="color: blue;"';
            break;
        }
        table += " <tr>\\n";
        table += "  <td><a href=\\"../view.php?id=" + tid + "\\">" + tsn + "</a></td>\\n";
        table += "  <td>" + dat + "</td>\\n";
        table += "  <td " + sty + ">" + txt + "</td>\\n";
        var tid = items[i].getElementsByTagName('tray_id')[0].childNodes[0].nodeValue;
        table += " </tr>\\n";
      }
      table += "</table>\\n";

      e.innerHTML = table;
    } else {
      alert('There was a problem with the request.');
    }
  }
}
</script>

EOF;

$tsw->set_opt($opt);
$tsw->header();
$tsw->toolbar('tray');
?>
<div id='main'><!--  -->
<div class='title'>Tray Note</div>
<div style='padding: 20px;'>
<form action="" onsubmit="return makeRequest('search.php');">
<input id='query' type='text' size='25' value='^\\[bad channel\\]' />
<input class='round-btn' type='submit' name='submit' value='search' />
</form>
</div>

<div id="result">
<table>
 <tr><th>tray</th><th>date</th><th>note</th></tr>
<?php
foreach(TrayNote::find_all("", "created_on desc", "15") as $tn) {
   $ty = new Tray($tn->tray_id);
   echo " <tr>\n";
   echo "  <td><a href=\"../view.php?id={$ty->id}\">{$ty->sn}</a></td>\n";
   echo "  <td>{$tn->created_on}</td>\n";
   echo "  <td>{$tn->note}</td>\n";
   echo " </tr>\n";
}
?>
</table>
</div><!-- result -->
</div><!-- main -->
<?php
$tsw->fotter(1);
?>