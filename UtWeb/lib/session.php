<?php
# $Id: session.php,v 1.10 2012/05/23 20:11:39 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto, Andrew Oldag. All rights reserved.

#session_save_path("/home/tofp/sessions");
session_save_path("/var/www/tof/sessions");
session_start();

class Session {
	var $rel;

	function Session($rel) {
	  $this->rel = $rel;
	}
	
	function login_button() {
		$rel = $this->rel ? $this->rel : "..";
		echo "<form action='{$rel}/account/login.php' method='post'>";
		echo "<input name='login' type='submit' value='login' />";
		echo "</form>";
	}

	function logout_button() {
		$rel = $this->rel ? $this->rel : "..";
		echo "<form action='{$rel}/account/logout.php' method='post'>";
		echo "<input name='logout' type='submit' value='logout' />";
		echo "<input name='request_type' type='hidden' value='logout'/>";
		echo "</form>";
	}
	
	function check_login($ref) {
		Session::update_ref($ref);
		if($_SESSION['known_user'] == 1) {
			return true;
		} else {
			$rel = $this->rel ? $this->rel : "..";
			header("Location: {$rel}/account/login.php");
			exit;
		}
	}
	
	function set_error($msg) {
		$_SESSION['error'] = $msg;
	}
	
	function error($arg) {
	  if (isset( $_SESSION['error']) )
	    $ret = $_SESSION['error'];
	  else
	    $ret = '';
	  if($arg['clear']) { $_SESSION['error'] = ''; }
	  return ($ret);
	}
		
	function authorize($arg) {
	  print "JS: this is authorize";
		$usr = $_SESSION['username'] = $arg['username'];
		$pwd = $_SESSION['password'] = $arg['password'];
		if($usr =='tof' && md5($pwd) == '75b2ade1102b7e4d92d03408ec074da5') {
			$_SESSION['known_user'] = 1;
			return(true);
		}
		elseif($usr =='tofmaster' && md5($pwd) == '9af930712fa5d4288d6108e079dee227') {
			$_SESSION['known_user'] = 1;
			return(true);
		}
		else {
			$_SESSION['known_user'] = 0;
			return(false);
		}
	}
	function login_again($msg) {
		$_SESSION['error'] = $msg;
		$rel = $this->rel ? $this->rel : "..";
		header("location: $rel/account/login.php");
	}
	function logout() {
		$_SESSION['known_user'] = 0;
		Session::set_error("logged out");
		header("location: ./login.php");
	}
	function loggedin() {
	  if (!isset( $_SESSION['known_user']) )
	    $_SESSION['known_user'] = 0;
	  return ($_SESSION['known_user'] == 1);
	}

	function username() {
	  if (isset( $_SESSION['username']) )
	    return $_SESSION['username'];
	  else
	    return false;
	}

# location stuff
	
	function goback($alt) {
		$loc = $_SESSION['ref'];
		if(!$loc) { $loc = $alt; }
		if(!$loc) { $loc = '/'; }
		header("Location: " . $loc);		
	}
	
	function update_ref($loc) {
		$_SESSION['ref'] = $loc;
	}
}
?>
