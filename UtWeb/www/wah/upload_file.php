<?php
if ($_FILES["file"]["error"] > 0 ) {
  echo $_FILES["file"]["error"];
} else {
  umask(0);
  system("./parse.rb {$_FILES['file']['name']} {$_FILES['file']['tmp_name']}");
}
?>
