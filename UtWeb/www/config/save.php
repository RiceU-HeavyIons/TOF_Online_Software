<?php
  $lib = "../../lib/";
  include($lib . "connect.php");
  include($lib . "session.php");
  include($lib . "orm.php");

  $session = new Session("../..");
  $session->check_login('../config/new.php');

  $id = $_POST['id'];
  $type = $_POST['type'];

  if ($type == 'create') {
    $rc = new RunConfig();
    $rc->parse($_POST);
    $id = $rc->save();
    header("Location: show.php?id=$id");
  }
  
  if ($type == 'update') {
    $rc = new RunConfig($_POST['id']);
    $rc->parse($_POST);
    $id = $rc->save();
    header("Location: show.php?id=$id");
  }
?>