<?php
# $Id: utility.php,v 1.15 2008/10/10 17:01:16 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto. All rights reserved.
function chinese_link($vendor, $id)
{
  if($vendor == 1) {
    return("<a href='http://mrpc.ustc.edu.cn/readustc.asp?id=". $id . "'>" . $id ."<a/>");
  } else if($vendor == 2) {
    return("<a href='http://mrpc.ustc.edu.cn/readtemp.asp?id=" . $id . "'>" . $id . "</a>");
  }
}

function fig_magnifier_link($fig_name, $base) {
  $a = "<a href=\"" . $base . "/fig/" . $fig_name . "\" onmousemove='magnifier.over(event, \"$fig_name\")' onmouseout='magnifier.out(event, \"$fig_name\")'>";
  $s = "<img src=\"". $base . "/fig/thumb/" . $fig_name . "\" />";
  $f = "<img src=\"" . $base . "/fig/" . $fig_name . "\" />";
  $x = "<div class=\"fullimg\" id=\"$fig_name\" style=\"visibility: hidden;\">" . $f . "</div>";
  return $a . $s . "</a>" . $x;
#  return $a . $s . "</a>";
}

function fig_link($fig_name, $base) {
  $a = "<a href='" . $base . "/fig/" . $fig_name ."'>";
  $s = "<img src='". $base . "/fig/thumb/" . $fig_name . "' />";
  $f = "<img src='" . $base . "/fig/" . $fig_name . "' />";
  return $a . $s . "</a>";
}

function mk_link($txt, $ref) {
  return "<a href='$ref'>$txt</a>";
}

function threshold_form($tr, $nr, $act) {
  echo "<form action='{$act}' method='get'>\n";
  echo " <table>\n";
  echo "  <tr>\n";
  echo "   <th>timing resolution</th>\n";
  echo "   <th>noise rate</th>\n";
  echo "  </tr>\n";
  echo "  <tr>\n";
  echo "   <td><input name='tr' type='text' value='${tr}' /></td>\n";
  echo "   <td><input name='nr' type='text' value='${nr}' /></td>\n";
  echo "   <td><input type='submit' value='change bars' /></td>\n";
  echo "  </tr>\n";
  echo " </table>\n";
  echo "</form>\n";
}

function list_pagenate($page, $total, $lines, $opt) {

  $tpg = round($total / $lines);

  if ( ($total % $lines) > 0) { $tpg += 1; }

  echo "<div class='pagenate'>page: ";
  $prev = $page - 1;
  $next = $page + 1;

  if($page > 1) {
    echo "<a href=\"list.php?page=$previous\">previous</a>";
  }

  for($i = 1; $i <= $tpg; $i++) {
    if($i == $page)
      echo " <font style='color: red; background-color: white;'>$i</font>";
    else
      echo " <a href='list.php?page=$i'>$i</a>";
  }

  if($page < $tpg) {
    echo " <a href='list.php?page=$next'>next</a>";
  }
  echo "</div>\n";
}

function pagenate($action, $page, $total, $lines, $opt) {

  $tpg = round($total / $lines);

  if ( ($total % $lines) > 0) { $tpg += 1; }

  echo "<div class='pagenate'>page: ";
  $prev = $page - 1;
  $next = $page + 1;

  if($page > 1) {
    echo "<a href=\"$action?page=$prev\">previous</a>";
  }

  for($i = 1; $i <= $tpg; $i++) {
    if($i == $page)
      echo " <font style='color: red; background-color: white;'>$i</font>";
    else
      echo " <a href='$action?page=$i'>$i</a>";
  }

  if($page < $tpg) {
    echo " <a href='$action?page=$next'>next</a>";
  }
  echo "</div>\n";
}

function parse_sqldate($sqldate) {
   $dry = split("-", $sqldate);
   return mktime(0, 0, 0, $dry[1], $dry[2], $dry[0]);
}
?>