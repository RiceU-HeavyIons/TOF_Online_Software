<?php
# $Id: session.php,v 1.9 2008/09/25 20:00:35 tofp Exp $
# Copyright (C) 2007, Kohei Kajimoto, Andrew Oldag. All rights reserved.

class Session {
	var $rel;

	function Session($rel) { $this->rel = $rel; }
	
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
		$ret = $_SESSION['error'];
		if($arg['clear']) { $_SESSION['error'] = ''; }
		return ($ret);
	}
		
	function authorize($arg) {
		$usr = $_SESSION['username'] = $arg['username'];
		$pwd = $_SESSION['password'] = $arg['password'];
		if($usr =='tof' && md5($pwd) == '75b2ade1102b7e4d92d03408ec074da5') {
			$_SESSION['known_user'] = 1;
			return(true);
		} elseif($usr =='tofmaster' && md5($pwd) == '9af930712fa5d4288d6108e079dee227') {
			$_SESSION['known_user'] = 1;
			return(true);
		} else {
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
		return ($_SESSION['known_user'] == 1);
	}

	function username() {
		return $_SESSION['username'];
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
session_save_path("/homes/tofp/sessions");
session_start();
?>
