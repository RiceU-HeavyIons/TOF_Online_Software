<?php
  $lib = "../../lib/";
  include($lib . "connect.php");
  include($lib . "session.php");
  include($lib . "orm.php");

  $session = new Session("../..");
  $session->check_login('../run/list.php');

  if ($_POST['type'] == 'create') {
    $rc = new Run();
    $rc->parse($_POST);
    $rc->save();
    header("Location: list.php");
  }
  
  if ($_POST['type'] == 'update') {
    $rc = new Run($_POST['id']);
    $rc->parse($_POST);
    $rc->save();
    header("Location: show.php?id={$rc->id}");
  }
?>