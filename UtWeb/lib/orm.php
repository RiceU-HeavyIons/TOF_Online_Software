<?php
# $Id: orm.php,v 1.66 2009/07/20 22:22:58 tofp Exp $
# Copyright (C) 2007, 2008, Kohei Kajimoto. All rights reserved.

class Component {
	var $id;
	var $serial;
	var $sn;
#	var $tray;
	var $tray_id;
	var $slot;
	var $component_type_id;
	var $type;
	var $date;
	var $chip_sn;
	var $note;

	function Component($id) {
		$q = mysql_query("SELECT * from components where id=$id");
		if($r = mysql_fetch_object($q)){
			$this->id = $id;
			$this->serial = $r->serial;
			$this->sn = $r->serial;
#			$this->tray = new Tray($r->tray_id);
			$this->tray_id = $r->tray_id;
			$this->slot = $r->slot;
			$this->component_type_id = $r->component_type_id;
			$this->type = $r->component_type_id;
			$this->date = $r->received_on;
			$this->chip_sn = $r->chip_sn;
			$this->note=$r->notes;
		} else {
			$this->id = 0;
		}
	}
	
	function count($cnd) {
		$stm = "SELECT COUNT(*) as count from components";
		if($cnd) { $stm .= " WHERE $cnd"; }
		$q = mysql_query($stm);
		$r = mysql_fetch_object($q);
		return $r->count;
	}

	function find_all($cnd, $ord, $lim) {
		$stm = "SELECT id FROM components";
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; }

		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new Component($r->id));
		}
		return($ar);
	}

	function find_one($cond, $ord) {
		$ar = Component::find_all($cond, $ord, 1);
		if (sizeof($ar)) {
		   return $ar[0];
		} else {
		   return null;
		}
	}
	
	function str_type() {
		switch($this->type) {
			case 1: return "TCPU";
			case 2: return "TDIG";
			case 3: return "TINO";
			case 4: return "MODULE";
			case 5: return "TTRIG";
		        case 6: return "SDetTDIG";
		        case 7: return "MINO";
			default: return "";
		}
	}
	function p_link() {
		if($this->id) {
		  $ret = "<a href='../component/show.php?id=" . $this->id . "'>";
		  $ret .= $this->sn . "</a>";
		} else
		  $ret = "";
		return $ret;
	}
}

class Module {
	var $id;
	var $serial;
	var $received_on;
	var $chinese_id;
	var $chinese_link;
	var $vendor_id;
	var $user;
	var $note;
	var $tray;
	var $slot;
    var $hvtest;
	
	function Module($id) {
		$this->id = null;
		if($id) {
			$q = mysql_query("SELECT * from modules where id=$id");
			if($r = mysql_fetch_object($q)){
				$this->id = $id;
				$this->serial = $r->serial;
				$tm = split(" ", $r->received_on);
				$this->received_on = $tm[0];
				$this->chinese_id = $r->chinese_id;
				$this->chinese_link = sprintf($r->chinese_link, $r->chinese_id);
				$this->vendor_id = $r->vendor_id;
				$this->note = $r->note;
				if($r->tray_id)
					$this->tray = new Tray($r->tray_id);
				else
					$this->tray = new Tray(0);
				$this->slot = $r->slot;
				$this->user = new User($r->user_id);
				$this->hvtest = new HvTestResult($id);
			}
		}
	}
		
	function find_all($cnd, $ord, $lim) {
		$stm = "SELECT * FROM modules";
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; } 
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new Module($r->id));
		}
		return($ar);
	}
	
	function find_all_light($cnd, $ord, $lim) {
		$stm = "SELECT id, serial FROM modules";
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; } 
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, array( "id" => $r->id, "serial" => $r->serial, "sn" => $r->serial));
		}
		return($ar);
	}
	
	function find_one($cond, $ord) {
		$ar = Module::find_all($cond, $ord, 1);
		return $ar[0];
	}

	function find_by_sn($sn) {
		$q = mysql_query("SELECT * from modules where serial='$sn'");
		if($r = mysql_fetch_object($q)) {
			return new Module($r->id);
		} else {
			return null;
		}
	}
	
	function find_by_threshold($tr, $nr) {
		$stm = "SELECT id FROM (" .
				"SELECT m.module_id id, s.serial sn, r.run_type_id type, min(m.max) mx " .
				" FROM module_results m " .
				" INNER JOIN runs r ON m.run_id = r.id " . 
				" INNER JOIN modules s on m.module_id = s.id " .
				" GROUP BY m.module_id, r.run_type_id) drv " .
				"WHERE (drv.type = 2 and drv.mx > {$tr}) or (drv.type = 3 and drv.mx > ${nr}) " .
				"GROUP BY id " .
				"ORDER BY sn";
		$q = mysql_query($stm);
		$ar = array();
		while($r = mysql_fetch_object($q))
			$ar[] = new Module($r->id);
		return $ar;
	}

	function find_fully_tested($tr, $nr) {
		$stm = "SELECT md.id id, md.serial sn FROM modules md " .
				" INNER JOIN " .
				" (SELECT m.module_id id, min(m.max) mx " .
				"   FROM module_results m " . 
				"     INNER JOIN runs r ON m.run_id = r.id " .
				"   WHERE r.run_type_id = 2 " .
				"   GROUP BY m.module_id " .
				"   HAVING min(m.max) < {$tr} ) tr" .
				" ON md.id = tr.id " .
				" INNER JOIN " .
				" (SELECT m.module_id id, min(m.max) mx " .
				"   FROM module_results m " . 
				"     INNER JOIN runs r ON m.run_id = r.id " .
				"   WHERE r.run_type_id = 3 " .
				"   GROUP BY m.module_id " .
				"   HAVING min(m.max) < {$nr} ) nr" .
				" ON md.id = nr.id " .
				" WHERE md.tray_id IS NULL" .
				" ORDER BY md.serial";
		$q = mysql_query($stm);
		$ar = array();
		while($r = mysql_fetch_object($q))
			$ar[] = new Module($r->id);
		return $ar;
	}
	
	function find_minimally_tested($tr, $nr, $vnd, $lim) {
		$v = ($vnd > 0)? "AND md.vendor_id = {$vnd}" : '';
		$stm = "SELECT md.id id, md.serial sn FROM modules md " .
				" INNER JOIN (SELECT m.module_id id, min(m.max) mx " .
				"   FROM module_results m " . 
				"     INNER JOIN runs r ON m.run_id = r.id " .
				"   WHERE r.run_type_id = 2 " .
				"   GROUP BY m.module_id " .
				"   HAVING min(m.max) < {$tr} ) tr ON md.id = tr.id " .
				" INNER JOIN (SELECT m.module_id id, min(m.max) mx " .
				"   FROM module_results m " . 
				"     INNER JOIN runs r ON m.run_id = r.id " .
				"   WHERE r.run_type_id = 3 " .
				"   GROUP BY m.module_id " .
				"   HAVING min(m.max) < {$nr} ) nr ON md.id = nr.id " .
				" WHERE md.tray_id IS NULL {$v}" .
				" UNION " .
				" SELECT md.id id, md.serial sn FROM modules md ".
				"  INNER JOIN hvtest hv ON md.id = hv.id " .
				"  WHERE md.tray_id IS NULL AND hv.pass=1 {$v} " .
				"ORDER BY sn";
		if($lim > 0)
			$stm .= " LIMIT ${lim}";

		$q = mysql_query($stm);
		$ar = array();
		while($r = mysql_fetch_object($q))
			$ar[] = new Module($r->id);
		return $ar;
	}
	
	function find_not_tested($cnd, $ord, $lim) {
		$stm = "SELECT id FROM modules where tray_id is null";
		if($cnd) { $stm .= " AND ($cnd)"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; } 
		$q = mysql_query( $stm );
		$al = array();
		while($r = mysql_fetch_array($q)) { array_push($al, $r['id']); }

		$hv = array();
		$q = mysql_query("SELECT id from hvtest");
		while($r = mysql_fetch_array($q)) { array_push($hv, $r['id']); }

		$ts = array();
		$q = mysql_query("SELECT module_id id from module_results group by module_id");
		while($r = mysql_fetch_array($q)) { array_push($ts, $r['id']); }

		$ar = array();
		foreach(array_diff($al, $ts, $hv) as $id) { array_push($ar, new Module($id)); }
		return $ar;
	}

	function first() {
		return Module::find_one("", "id asc");
	}
	
	function last() {
		return Module::find_one("", "id desc");
	}
	
	function count($cnd) {
		$stm = "SELECT COUNT(*) as count from modules";
		if($cnd) { $stm .= " WHERE $cnd"; }
		$q = mysql_query($stm);
		$r = mysql_fetch_object($q);
		return($r->count);
	}
	
	function TimingResolution() {
		$q = mysql_query("SELECT * from module_results where module_id=1");
		$a = array();
		while($r = mysql_fetch_object($q)) {
			$a[] = $r;
		}
		return($a);
	}
	
	function max_tr() {
		$max = 0;
		$q = mysql_query(
			"SELECT min(m.max) as mx FROM module_results m " . 
			" INNER JOIN runs r on m.run_id = r.id " .
			" where m.module_id={$this->id} and r.run_type_id = 2");
		if($r = mysql_fetch_object($q)) $max = $r->mx;
		return($max);
	}
	
	function max_nr() {
		$max = 0;
		$q = mysql_query(
			"SELECT min(m.max) as mx FROM module_results m " . 
			" INNER JOIN runs r on m.run_id = r.id " .
			" where m.module_id={$this->id} and r.run_type_id = 3");
		if($r = mysql_fetch_object($q)) $max = $r->mx;
		return($max);
	}
	
	function tested() {
		return (($this->max_nr() > 0) and ($this->max_tr() > 0.0));
	}
	function passed($tr, $nr) {
		return ( $this->tested() and 
			($this->max_nr() < $nr) and ($this->max_tr() < $tr));
	}
		
	function max_chinese_tr() {
		$max = 0;
		$q = mysql_query(
			"SELECT min(max) as mx FROM chinese_results " . 
			" where module_id={$this->id} and result_type = 1");
		if($r = mysql_fetch_object($q)) $max = $r->mx;
		return($max);
	}

	function max_chinese_nr() {
		$max = 0;
		$q = mysql_query(
			"SELECT min(max) as mx FROM chinese_results " . 
			" where module_id={$this->id} and result_type = 2");
		if($r = mysql_fetch_object($q)) $max = $r->mx;
		return($max);
	}


	function stats($tr, $nr) {
		$stats = array("total" => 0, "tested" => 0, "passed" => 0, "failed" => 0);

		$stm = "SELECT COUNT(*) count FROM modules";
		$q = mysql_query($stm);
		if($r = mysql_fetch_object($q)) $stats["total"] = $r->count;
		
		$stm = "SELECT COUNT(*) count FROM" .
				" (SELECT m.module_id id FROM module_results m " .
				"  INNER JOIN runs r ON m.run_id = r.id WHERE r.run_type_id = 2 " .
				"  GROUP BY m.module_id HAVING min(m.max) > 0) tr, " .
				" (SELECT m.module_id id FROM module_results m " .
				"  INNER JOIN runs r ON m.run_id = r.id WHERE r.run_type_id = 3 " .
				"  GROUP BY m.module_id HAVING min(m.max) > 0) nr " .
				" WHERE tr.id = nr.id";
		$q = mysql_query($stm);
		if($r = mysql_fetch_object($q)) $stats["tested"] = $r->count;
		
		$stm = "SELECT COUNT(*) count FROM" .
				" (SELECT m.module_id id FROM module_results m " .
				"  INNER JOIN runs r ON m.run_id = r.id WHERE r.run_type_id = 2 " .
				"  GROUP BY m.module_id HAVING min(m.max) < $tr) tr, " .
				" (SELECT m.module_id id FROM module_results m " .
				"  INNER JOIN runs r ON m.run_id = r.id WHERE r.run_type_id = 3 " .
				"  GROUP BY m.module_id HAVING min(m.max) < {$nr}) nr " .
				" WHERE tr.id = nr.id";
		$q = mysql_query($stm);
		if($r = mysql_fetch_object($q)) $stats["passed"] = $r->count;
		
		$stats["failed"] = $stats["tested"] - $stats["passed"];

		return $stats;
	}
	
	function p_size_tested() {
		return (ModuleSize::count("module_id={$this->id}") > 0)? "yes" : "no";
	}
	
	function p_note() {
		$note = preg_replace('/<a[^>]*>([^<>]*)<\/a>/', '$1', $this->note);
		if(is_null($note)) $note = "";
		if(strlen($note) > 32)
			$note = substr($note, 0, 32) . "...";
	return strtolower($note);
	}
   
	function p_hvtest() {
		return strtolower($this->hvtest->result);
	}
	
	function p_link() {
		return "<a href='../module/show.php?id=" . $this->id . "'>" . $this->serial . "</a>";
	}
}

class Run {
	var $id;
	var $config;
	var $status;
	var $type;
	var $user;
	var $start;
	var $stop;
	var $run_time;
	var $events;
	var $data_uri;
	var $note;
	
	var $mr_count;

	function Run($id) {
		$q = mysql_query("SELECT * from runs where id=$id");
		if($r = mysql_fetch_object($q)) {
			$this->id = $r->id;
			$this->config = new RunConfig($r->run_config_id);
			$this->type = new RunType($r->run_type_id);
			$this->status = new RunStatus($r->run_status_id);
			$this->user = new User($r->user_id);

			$this->start    = $r->start;
			$this->stop     = $r->stop;
			$this->run_time = strtotime($r->stop) - strtotime($r->start);
			$this->events   = $r->events;
			$this->data_uri = $r->data_uri;
			$this->note     = $r->note;

			$qq = mysql_query("SELECT COUNT(*) as cnt from module_results where run_id=$id");
			if($rr = mysql_fetch_object($qq)) {
			  $this->mr_count = $rr->cnt;
			}
		} else {
			$this->id;
		}
	}
	function find_all($cnd, $ord, $lim) {
		$stm = "SELECT id FROM runs";
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; } 
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new Run($r->id));
		}
		return($ar);
	}

	function find_with_config($cnd, $ord, $lim) {
		$stm = "SELECT r.id as id FROM runs r";
        $stm .= " inner join run_configs rc on r.run_config_id = rc.id";
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; } 
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new Run($r->id));
		}
		return($ar);
	}

	function find_tray_runs($cnd, $ord, $lim) {
		$stm = "SELECT * FROM runs";
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; }
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new RUN($r->id));
		}
		return ($ar);
	}

	function last($cnd) {
		$stm = "SELECT id from runs";
		if ($cnd) { $stm .= " WHERE $cnd"; }
		$stm .= " ORDER BY id desc LIMIT 1";
		$q = mysql_query($stm);
		$r = mysql_fetch_object($q);
		return new Run($r->id);
	}
	
	function count($cnd) {
		$stm = "SELECT COUNT(*) as count from runs";
		if($cnd) { $stm .= " WHERE $cnd"; }
		$q = mysql_query($stm);
		$r = mysql_fetch_object($q);
		return ($r->count);	
	}

	function count_with_config($cnd) {
		$stm = "SELECT COUNT(*) as count FROM runs r";
        $stm .= " inner join run_configs rc on r.run_config_id = rc.id";
		if($cnd) { $stm .= " WHERE $cnd"; }
		$q = mysql_query( $stm );
		$r = mysql_fetch_object($q);
        return($r->count);
	}

	function parse($ary) {
		$this->id         = $ary['id'];
		$this->status     = new RunStatus($ary['run_status_id']);
		$this->type       = new RunType($ary['run_type_id']);
		$this->config     = new RunCOnfig($ary['run_config_id']);
		$this->user       = new User($ary['user_id']);
		$this->start      = $ary['start'];
		$this->stop       = $ary['stop'];
		$this->run_time   = $ary['run_time'];
		$this->events     = $ary['events'];
		$this->data_uri   = $ary['data_uri'];
		$this->note       = $ary['note'];
	}
	
	function save() {
		if($this->id) {
			$stm = "UPDATE runs SET ";
			$stm .= join(",",
			         array(
			           "run_status_id={$this->status->id}",
			           "run_type_id={$this->type->id}",
			           "run_config_id={$this->config->id}",
			           "user_id={$this->user->id}",
			           "start='{$this->start}'",
			           "stop='{$this->stop}'",
			           "run_time={$this->run_time}",
			           "events={$this->events}",
			           "data_uri='{$this->data_uri}'",
			           "note='{$this->note}'"));
			$stm .= " WHERE id={$this->id}";
			$q = mysql_query($stm);
		} else {
			$stm = "INSERT INTO runs";
			$stm .=  " (";
			$stm .= join(",",
			           array('run_status_id', 'run_type_id', 'run_config_id',
			                 'user_id', 'start', 'stop',
			                 'run_time', 'events', 'data_uri', 'note') );
			$stm .= ") VALUES (";
			$stm .= join(', ',
			           array($this->status->id, $this->type->id, $this->config->id,
			                 $this->user->id,
			                 "'{$this->start}'", "'{$this->stop}'",
			                 $this->run_time, $this->events,
			                 "'{$this->data_uri}'", "'{$this->note}'" ));
			$stm .= ")";
			$q = mysql_query($stm);
		}
	}

	function str_run_time() {
		$dt = $this->run_time;
		$ret = '';
		if($d = intval($dt / 24 / 3600)) {
			if($d < 2) { $ret .= "$d day "; }
			else { $ret .= "$d days "; }
		}
		$dt %= (24*3600);
		if($ret || $h = intval($dt / 3600)) {
			if(!isset($h)) { $h = intval($dt / 3600); }
			if($h < 2) { $ret .= "$h hour "; }
			else { $ret .= "$h hours "; }
		}
		$dt %= 3600;
		if($ret || $m = intval($dt / 60)) {
			if(!isset($m)) { $m = intval($dt / 60); }
			if($m < 2) { $ret .= "${m} minute "; }
			else { $ret .= "${m} minutes "; }
		}
		$s = $dt % 60;
		if($s < 2) { $ret .= "$s second"; }
		else { $ret .= "$s seconds"; }
		
		return $ret;
	}
	
	function month_summary() {
		$stm = "SELECT substr(start, 1, 7) month, sum(1) count, ";
		$stm .= " sum(run_time) time FROM runs";
		$stm .= " WHERE start >= '2006-08-01 00:00:00'";
		$stm .= " GROUP BY month";
		
		$q = mysql_query($stm);
		$ary = array();
		while($r = mysql_fetch_array($q)) {
			array_push($ary,
				array(
					"month" => $r["month"],
					"count" => $r["count"],
					"time"  => $r["time"]
				)
			);
		}
		return $ary;
	}

	function user_summary() {
		$stm = "SELECT user_id, sum(1) count, sum(run_time) time, max(stop) last from runs";
		$stm .= " WHERE start >= '2006-08-01 00:00:00'";
		$stm .= " GROUP BY user_id";
		$stm .= " ORDER BY user_id asc";
		
		$q = mysql_query($stm);
		$ary = array();
		while($r = mysql_fetch_array($q)) {
			array_push($ary,
				array(
					"user" => new User($r["user_id"]),
					"count" => $r["count"],
					"time"  => $r["time"],
					"last"  => preg_replace("/ \d+:\d+:\d+/", '', $r["last"])
				)
			);
		}
		return $ary;
	}
	
	function stats_for_tray($tray_id) {
		$stm = "SELECT avg(mr.avg) as av, min(mr.min) as mi, max(mr.max) as mx FROM module_results mr";
		$stm .= " INNER JOIN modules m on mr.module_id = m.id";
		$stm .= " WHERE mr.run_id=" . $this->id;
		$stm .= " AND m.tray_id=" . $tray_id;
		$q = mysql_query($stm);
		$ret = "";
		if ($r = mysql_fetch_array($q)) {
			$ret = array(
					"avg" => $r["av"],
					"min" => $r["mi"],
					"max" => $r["mx"]);
		}
		return $ret;
	}
}
class RunConfigType {
	var $id;
	var $name;

	function RunConfigType($id) {
		$q = mysql_query("SELECT * from run_config_types where id=$id");
		if($r = mysql_fetch_object($q)) {
			$this->id = $id;
			$this->name = $r->name;
		} else {
			$this->id = null;
		}
	}
	function find_all($cnd, $ord, $lim) {
		$stm = "SELECT id FROM run_config_types";
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; } 
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new RunConfigType($r->id));
		}
		return($ar);
	}

	function options($id) {
		foreach(RunConfigType::find_all('', 'id', '') as $r) {
			if ($id == $r->id)
				echo "<option value='{$r->id}' selected>{$r->name}</option>\n";
			else
				echo "<option value='{$r->id}'>{$r->name}</option>\n";
		}
	}
}

class Location {
	var $id;
	var $name;
	
	function Location($id) {
		$q = mysql_query("SELECT id,name from locations where id=$id");
		if($r = mysql_fetch_object($q)) {
			$this->id = $id;
			$this->name = $r->name;
		} else {
			$this->id = null;
		}
	}
	
	function find_all($cnd, $ord, $lim) {
		$stm = "SELECT id FROM locations";
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; } 
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new Location($r->id));
		}
		return($ar);
	}

	function options($id) {
		foreach(Location::find_all("", "id", "") as $l) {
			if ($id == $l->id)
				echo "<option value='{$l->id}' selected>{$l->name}</option>\n";
			else
				echo "<option value='{$l->id}'>{$l->name}</option>\n";
		}
	}
}


class RunConfig {
	var $id;
	var $user;
	var $created_on;
	var $type;
	var $run_config_type;
	var $n_modules;
	var $n_pads;
	var $modules;
	var $hv_channel_plus;
	var $hv_channel_minu;
	var $hv_plus;
	var $hv_minu;
	var $threshold;
	var $impedance;
	var $gas_channe_1;
	var $gas_channe_2;
	var $gas_channe_3;
	var $flow_rate1;
	var $flow_rate2;
	var $flow_rate3;
	var $note;
	var $module_ids;
	var $start;
	var $stop;
	var $location;

	function RunConfig($id) {
		$q = mysql_query("SELECT * FROM run_configs WHERE id=$id");
		if($r = mysql_fetch_array($q)) {
			$this->id = $id;
			$this->user = new User($r["user_id"]);
			$this->created_on = $r["created_on"];
			$this->n_modules = $r["n_modules"];
			$this->n_pads = $r["n_pads"];
			for($i = 1; $i <= 4; $i++) {
				$this->modules[$i] = new Module($r["module${i}_id"]);
			}
			$this->hv_plus          = $r["hv_plus"];
			$this->hv_minus         = $r["hv_minus"];
			$this->hv_channel_plus  = $r["hv_channel_plus"];
			$this->hv_channel_minus = $r["hv_channel_minus"];

			$this->threshold  = $r['threshold'];
			$this->impedance  = $r['impedance'];

			$this->gas_channel_1 = $r['gas_channel_1'];
			$this->gas_channel_2 = $r['gas_channel_2'];
			$this->gas_channel_3 = $r['gas_channel_3'];
			$this->flow_rate1 = $r['flow_rate1'];
			$this->flow_rate2 = $r['flow_rate2'];
			$this->flow_rate3 = $r['flow_rate3'];

			$this->note       = $r['note'];
			$this->module_ids = $r['module_ids'];
			$this->run_config_type = new RunConfigType($r['run_config_type_id']);
			$this->type       = $this->run_config_type;
			$this->start      = $r['start'];
			$this->stop       = $r['stop'];
			$this->location   = new Location($r['location_id']);
		}
	}

	function find_all($cnd, $ord, $lim) {
		$stm = "SELECT id FROM run_configs";
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; } 
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new RunConfig($r->id));
		}
		return($ar);
	}

	function count($cnd) {
		$stm = "SELECT COUNT(*) as count from run_configs";
		if($cnd) { $stm .= " WHERE $cnd"; }
		$q = mysql_query($stm);
		$r = mysql_fetch_object($q);
		return $r->count;
	}

	function parse($ary) {
		$this->id                  = $ary['id'];
		$this->user                = new User($ary['user_id']);
		$this->created_on          = $ary['created_on'];
		$this->n_modules           = $ary['n_modules'];
		$this->n_pads              = $ary['n_pads'];
		$this->threshold           = $ary['threshold'];
		$this->impedance           = $ary['impedance'];
		$this->hv_plus             = $ary['hv_plus'];
		$this->hv_minus            = $ary['hv_minus'];
		$this->hv_channel_plus     = $ary['hv_channel_plus'];
		$this->hv_channel_minus    = $ary['hv_channel_minus'];
		$this->gas_channel_1       = $ary['gas_channel_1'];
		$this->gas_channel_2       = $ary['gas_channel_2'];
		$this->gas_channel_3       = $ary['gas_channel_3'];
		$this->flow_rate1          = $ary['flow_rate1'];
		$this->flow_rate2          = $ary['flow_rate2'];
		$this->flow_rate3          = $ary['flow_rate3'];
		$this->note                = $ary['note'];
		$this->module_ids          = str_replace("\r", '', $ary['module_ids']);
		$this->run_config_type     = new RunConfigType($ary['run_config_type_id']);
		$this->start               = $ary['start'];
		$this->stop                = $ary['stop'];
		$this->location            = new Location($ary['location_id']);
	}
	
	function save() {
		if($this->id) {
			$stm = "UPDATE run_configs SET ";
			$stm .= join(",",
			         array(
			           "user_id={$this->user->id}",
			           "created_on='{$this->created_on}'",
			           "n_modules={$this->n_modules}",
			           "n_pads={$this->n_pads}",
			           "threshold={$this->threshold}",
			           "impedance={$this->impedance}",
			           "hv_plus={$this->hv_plus}",
			           "hv_minus={$this->hv_minus}",
			           "flow_rate1={$this->flow_rate1}",
			           "flow_rate2={$this->flow_rate2}",
			           "flow_rate3={$this->flow_rate3}",
			           "note='{$this->note}'",
			           "module_ids='{$this->module_ids}'",
			           "run_config_type_id={$this->run_config_type->id}",
			           "start='{$this->start}'",
			           "stop='{$this->stop}'",
			           "hv_channel_plus={$this->hv_channel_plus}",
			           "hv_channel_minus={$this->hv_channel_minus}",
			           "gas_channel_1={$this->gas_channel_1}",
			           "gas_channel_2={$this->gas_channel_2}",
			           "gas_channel_3={$this->gas_channel_3}",
			           "location_id={$this->location->id}"));
			$stm .= " WHERE id={$this->id}";
			$q = mysql_query($stm);
		} else {
			$stm = "INSERT INTO run_configs";
			$stm .=  " (";
			$stm .= join(",",
			           array('user_id', 'created_on', 'n_modules', 'n_pads',
			                 'threshold', 'impedance',
			                 'hv_plus', 'hv_minus',
			                 'hv_channel_plus', 'hv_channel_minus',
			                 'flow_rate1', 'flow_rate2', 'flow_rate3',
			                 'note',
			                 'module_ids',
			                 'run_config_type_id',
			                 'start', 'stop',
			                 'gas_channel_1', 'gas_channel_2', 'gas_channel_3',
			                 'location_id') );
			$stm .= ") VALUES (";
			$stm .= join(', ',
			           array($this->user->id, "'{$this->created_on}'", $this->n_modules, $this->n_pads,
			                 $this->threshold, $this->impedance,
			                 $this->hv_plus, $this->hv_minus,
			                 $this->hv_channel_plus, $this->hv_channel_minus,
			                 $this->flow_rate1, $this->flow_rate2, $this->flow_rate3,
			                 "'{$this->note}'",
			                 "'{$this->module_ids}'",
			                 $this->run_config_type->id,
			                 "'{$this->start}'", "'{$this->stop}'",
			                 $this->gas_channel_1, $this->gas_channel_2, $this->gas_channel_3,
			                 $this->location->id));
			$stm .= ")";
			$q = mysql_query($stm);
			$this->id = mysql_insert_id();
		}
		return ($this->id);
	}
	
	function ids_array() {
		$ar = array();
		foreach(split("\n", $this->module_ids) as $ln) {
			$a = array();
			if(ereg("- ([0-9]+)$", $ln, $a)) {
				array_push($ar, $a[1]);
			}
		}
		return $ar;
	}
	
	function gas_stats($channel) {
		$stm = "SELECT sum(dt*value)/60.0/1000.0 as sm, avg(value) as av, min(value) as mi, max(value) as mx FROM gas_readings";
		$stm .= " WHERE gas_type_id={$channel}";
		$stm .= " AND datetime>='{$this->start}'";
		$stm .= " AND datetime<='{$this->stop}'";
		$q = mysql_query($stm);
		$ret = "";
		if ($r = mysql_fetch_array($q)) {
			$ret = array(
			        "sum" => $r["sm"],
					"avg" => $r["av"],
					"min" => $r["mi"],
					"max" => $r["mx"]);
		}
		return $ret;
	}
}

class RunStatus {
	var $id;
	var $name;
	
	function RunStatus($id) {
		$this->id = $id;
		$q = mysql_query("SELECT * from run_status where id=$id");
		if($r = mysql_fetch_object($q)) {
			$this->name = $r->name;
		}
	}
}

class RunType {
	var $id;
	var $name;
	
	function RunType($id) {
		$this->id = $id;
		$q = mysql_query("SELECT * from run_types where id=$id");
		if($r = mysql_fetch_object($q)) {
			$this->name = $r->name;
		}
	}
}

function Module_find($off, $lim){
	$ary = array();
	
	$q = mysql_query("SELECT * from modules limit $off, $lim");
	
}

function ModuleStats($tr, $nr) {
	$stats = array("total" => 0, "tested" => 0, "passed" => 0, "failed" => 0);
	$q = mysql_query("SELECT * from modules");
	while($r = mysql_fetch_object($q)){
		$stats["total"]++;
		$m = new Module($r->id);
		
		if($m->tested()) {
			$stats["tested"]++;
		
			if($m->passed($tr, $nr))
				$stats["passed"]++;
			else
				$stats["failed"]++;
		}
	}
	return $stats;
}

class ModuleResult {
	var $id;
	var $run;
	var $module;
	var $pad;
	var $avg;
	var $min;
	var $max;
	var $slot;
	var $note;
	var $npad = 6;
	var $data_uri;


	function ModuleResult($id) {
		$this->id = $id;
		$q = mysql_query("SELECT * from module_results where id=$id");
		if($r = mysql_fetch_array($q)) {
			$this->run = new Run($r["run_id"]);
			$this->module = new Module($r["module_id"]);
			$pad = array();
			for($i=1; $i <= $this->npad; $i++) {
				$this->pad[$i] = $r["pad$i"];
			}
			$this->avg = $r["avg"];
			$this->min = $r["min"];
			$this->max = $r["max"];
			$this->slot = $r["slot"];
			$this->note = $r["note"];
			$this->data_uri = $r["data_uri"];
		}
	}
	
	function find_all($cnd, $ord, $lim) {
		$table_name = "module_results";
		$stm = "SELECT * FROM " . $table_name;
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; } 
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new ModuleResult($r->id));
		}
		return($ar);
	}
	function p_note() {
		$note = preg_replace('/<a[^>]*>([^<>]*)<\/a>/', '$1', $this->note);
		if(is_null($note)) $note = "";
		if(strlen($note) > 16)
			$note = substr($note, 0, 16) . "...";
		return strtolower($note);
	}
}

class ChineseResult {
	var $id;
	var $pad;
	var $type;
	var $link;

	function ChineseResult($id) {
		$q = mysql_query("SELECT * from chinese_results where id=$id");
		if($r = mysql_fetch_array($q)) {
			$this->id = $id;
			$pad = array();
			for($i=1; $i <= 6; $i++) { $this->pad[$i] = $r["pad$i"]; }
			$this->type = $r["result_type"];
			$this->link = $r["data_uri"];
		} else {
			$this->id = 0;
		}
	}
	
	function find_all($cnd, $ord, $lim) {
		$table_name = "chinese_results";
		$stm = "SELECT * FROM " . $table_name;
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; } 
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new ChineseResult($r->id));
		}
		return($ar);
	}
	
	function count($cnd) {
		$stm = "SELECT COUNT(*) as count from chinese_results";
		if($cnd) { $stm .= " WHERE $cnd"; }
		$q = mysql_query($stm);
		$r = mysql_fetch_object($q);
		return($r->count);
	}
	
	function p_duplication() {
		$stm =	"SELECT c1.serial, c1.chinese_id, c1.data_uri " .
				"FROM chinese_results c1 INNER JOIN (" .
				" SELECT serial FROM chinese_results " .
				"  GROUP BY serial, result_type HAVING COUNT(*) > 1" .
				") c2 on c1.serial = c2.serial GROUP BY c1.serial, c1.chinese_id";
		$q = mysql_query($stm);
		$ar1 = array();
		while($r = mysql_fetch_object($q)) {
			$sn = $r->serial;
			if( ! array_key_exists($sn, $ar1)) $ar1[$sn] = array();
			array_push($ar1[$sn], "<a href='{$r->data_uri}'>{$r->chinese_id}</a>");
		}
		$ar2 = array();
		foreach($ar1 as $sn => $links) {
			array_push($ar2, $sn . " (" . join(", ", $links) . ")" );
		}
		return count($ar2) . " modules: " . join(", ", $ar2);
	}
	function count_unique_serial() {
		$q = mysql_query(
			"SELECT COUNT(*) count FROM " .
			"(SELECT 1 FROM chinese_results GROUP by serial) sns"
		);
		$r = mysql_fetch_object($q);
		return $r->count;
	}
}

class HvTestResult {
    var $id;
    var $pass;
    var $result;
    function HvTestResult($id) {
        $this->id = $id;
        $q = mysql_query("select * from hvtest where id=$id");
        $r=mysql_fetch_object($q);
        $this->pass = $r->pass;
        if(is_null($r->pass)) {
            $this->result = "Not Tested";
        } elseif($r->pass == 0) {
            $this->result = "Failed";
         } elseif($r->pass == 1) {
            $this->result = "Passed";
         } else {
            $this->result = "Fuck, I don't know the type!";
         }
    }
}

class ModuleSize {
	var $id;
	var $module;

	function ModuleSize($id) {
		$this->id = $id;
		$q = mysql_query("SELECT * from module_size where id=$id");
		if($r = mysql_fetch_array($q)) {
			$this->module = new Module($r["module_id"]);
		}
	}
	
	function find_all($cnd, $ord, $lim) {
		$table_name = "module_sizes";
		$stm = "SELECT * FROM " . $table_name;
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; } 
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new ModuleSize($r->id));
		}
		return($ar);
	}
	
	function count($cnd) {
		$table_name = "module_sizes";
		$stm = "SELECT count(*) as count FROM " . $table_name;
		if($cnd) { $stm .= " WHERE $cnd"; }
		$q = mysql_query($stm);
		$r = mysql_fetch_object($q);
		return($r->count);
	}
}

class TrayStatus {
	var $id;
	var $name;
	
	function TrayStatus($id) {
		$q = mysql_query("SELECT id,name from tray_status where id=$id");
		if($r = mysql_fetch_object($q)) {
			$this->id = $id;
			$this->name = $r->name;
		} else {
			$this->id = null;
		}
	}
	
	function find_all($cnd, $ord, $lim) {
		$stm = "SELECT id FROM tray_status";
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; } 
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new TrayStatus($r->id));
		}
		return($ar);
	}

	function options($id) {
		foreach(TrayStatus::find_all("", "id", "") as $l) {
			if ($id == $l->id)
				echo "<option value='{$l->id}' selected>{$l->name}</option>\n";
			else
				echo "<option value='{$l->id}'>{$l->name}</option>\n";
		}
	}
}
class Tray {
	var $id;
	var $user;
	var $start;
	var $ship;
	var $sn;
	var $serial;
	var $notes;
	var $status;
	var $tray_status;

	var $tcpu;
	var $tdig;
	var $tino;
	var $mino;
	var $module;
	
	var $info;
	var $doc;

	function Tray($id) {
		$q = mysql_query("SELECT * from trays where id=$id");
		if($r = mysql_fetch_object($q)){
			$this->id = $id;
			$this->user = new User($r->user_id);
			$this->start = $r->start;
			$this->ship = $r->ship;
			$this->sn = $r->sn;
			$this->serial = $r->sn;
			$this->status = $r->status;
			$this->notes = $r->notes;
			$this->tray_status = new TrayStatus($r->tray_status_id);

			$this->tcpu  = Component::find_one("tray_id={$id} AND component_type_id=1", "");
			$this->ttrig = Component::find_one("tray_id={$id} AND component_type_id=5", "");
			$this->tdig  = Component::find_all("tray_id={$id} AND component_type_id=2", "slot asc", "");
			$this->tino  = Component::find_all("tray_id={$id} AND component_type_id=3", "slot asc", "");
			$this->mino  = Component::find_all("tray_id={$id} AND component_type_id=7", "slot asc", "");
			$this->module = Module::find_all_light("tray_id={$id}", "slot asc", "");
			
			$this->info = TrayInfo::find_all("tray_id={$id}", "id asc", "");
			$this->doc = TrayDoc::find_by_sn($this->sn);

		} else {
			$this->id = 0;
		}
	}

	function find_all($cnd, $ord, $lim) {
		$table_name = "trays";
		$stm = "SELECT * FROM " . $table_name;
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; } 
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new Tray($r->id));
		}
		return($ar);
	}
	
	function find_by_sn($sn) {
		$q = mysql_query("SELECT id from trays where sn='$sn'");
		if($r = mysql_fetch_object($q)) {
			return new Tray($r->id);
		}
	}
	
	function find_runs($cnd, $ord, $lim) {
		$stm = "SELECT r.id FROM runs r";
		$stm .= " INNER JOIN run_configs rc on r.run_config_id = rc.id";
		$stm .= " WHERE (r.run_type_id = 4 OR r.run_type_id = 7)";
		$stm .= " AND (rc.run_config_type_id = 3 OR rc.run_config_type_id = 4) ";
		$stm .= " AND rc.module_ids RLIKE '\n- {$this->id}\n'";
		if($cnd) { $stm .= " AND ($cnd)"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; }
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new RUN($r->id));
		}
		return ($ar);
	}

	function cosmic_tested() {
		$stm = "SELECT count(r.id) as count FROM runs r";
		$stm .= " INNER JOIN run_configs rc on r.run_config_id = rc.id";
		$stm .= " WHERE (r.run_type_id = 7 && r.run_status_id = 3)";
		$stm .= " AND (rc.run_config_type_id = 3) ";
		$stm .= " AND rc.module_ids RLIKE '\n- {$this->id}\n'";
		$q = mysql_query( $stm );
		$r = mysql_fetch_object($q);
		return ($r->count > 0);
	}

	function count($cnd) {
		$stm = "SELECT COUNT(*) as count from trays";
		if($cnd) { $stm .= " WHERE $cnd"; }
		$q = mysql_query($stm);
		$r = mysql_fetch_object($q);
		return $r->count;
	}

	function p_link() {
		if($this->id) {
		  if ($this->sn < 200)
		    $ret = "<a href='../tray/view.php?id=" . $this->id . "'>";
		  else
		    $ret = "<a href='../mtd_tray/view.php?id=" . $this->id . "'>";
		  
		  $ret .= $this->sn . "</a>";
		} else
		  $ret = "";
		return $ret;
	}
	
   function ship_dates() {
      $stm = "SELECT ship from trays where ship is not null group by ship";
      $q = mysql_query($stm);
      $ar = array();
      while($r = mysql_fetch_object($q)) {
         $ar[$r->ship] = 0;
      }
      ksort($ar);
      return $ar;
   }

   function sealed_date() {
      $stm = "SELECT created_on from tray_notes where tray_id={$this->id} and note like '%ealed today%' order by created_on desc limit 1";
      $q = mysql_query($stm);
      $r = mysql_fetch_object($q);
      return ($r->created_on);
   }
}


class TrayInfo {
	var $id;
	var $tray_id;
	var $data_name;
	var $data;
	var $date;

	function TrayInfo($id) {
		$q = mysql_query("SELECT * from tray_info where id=$id");
		if($r = mysql_fetch_object($q)){
			$this->id = $id;
			$this->tray_id = $r->tray_id;
			$this->data_name = $r->data_name;
			$this->data = $r->data;
			$this->date = $r->date;
		} else {
			$this->id = 0;
		}
	}
	
	function find_all($cnd, $ord, $lim) {
		$stm = "SELECT * FROM tray_info";
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; }
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new TrayInfo($r->id));
		}
		return($ar);
	}

}


class TrayDoc {
  var $id;
  var $name;
  var $fname;
  var $date;
  
  function TrayDoc($id) {
#    $c = mysql_pconnect("localhost:3306", "tofp", "t4flight");
#    mysql_select_db("toftstand", $c);
    $q = mysql_query("SELECT * from toftstand.xoops_dms_objects where obj_id={$id}");
    if($r = mysql_fetch_object($q)) {
      $this->id = $r->obj_id;
      $this->name = $r->obj_name;
      $subq = mysql_query(
         "SELECT * from toftstand.xoops_dms_object_versions where obj_id=$id " .
         " ORDER BY major_version desc, minor_version desc, sub_minor_version desc LIMIT 1");
      if($subr = mysql_fetch_object($subq)) {
        $this->fname = $subr->file_name;
        $this->date  = date("Y-m-d H:i:s T", $subr->time_stamp);
      }
    } else
      $this->id = 0;
  }
  
  function find_by_sn($sn) {
    $s = sprintf("'%%[tray%03s]%%'", $sn);
    $stm = "SELECT obj_id FROM toftstand.xoops_dms_objects where obj_name like $s";
    $stm .= " ORDER BY obj_id asc";
    $q = mysql_query($stm);
    $ar = array();
    while($r = mysql_fetch_object($q)) {
      array_push($ar, new TrayDoc($r->obj_id));
    }
    return($ar);
  }
}

class TrayNote {
	var $id;
	var $created_on;
	var $user_id;
	var $tray_id;
	var $note;
	var $tray_note_type_id;

	function TrayNote($id) {
		$q = mysql_query("SELECT * from tray_notes where id=$id");
		if($r = mysql_fetch_object($q)){
			$this->id = $id;
			$this->created_on = $r->created_on;
			$this->user_id = $r->user_id;
			$this->tray_id = $r->tray_id;
			$this->note = $r->note;
			$this->tray_note_type_id = $r->tray_note_type_id;
		} else {
			$this->id = 0;
		}
	}
	
	function find_all($cnd, $ord, $lim) {
		$stm = "SELECT * FROM tray_notes";
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; }
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new TrayNote($r->id));
		}
		return($ar);
	}
	function last($cnd) {
		$stm = "SELECT id from tray_notes";
		if ($cnd) { $stm .= " WHERE $cnd"; }
		$stm .= " ORDER BY id desc LIMIT 1";
		$q = mysql_query($stm);
		$r = mysql_fetch_object($q);
		return new TrayNote($r->id);
	}
}

class User {
	var $id;
	var $name;
	var $name_last;
	var $name_first;
	var $email;

	function User($id) {
		$this->id = $id;
		$q = mysql_query("SELECT * from users where id=$id");
		if($r = mysql_fetch_object($q)) {
			$this->name = $r->name;
			$this->name_last = $r->name_last;
			$this->name_first = $r->name_first;
			$this->email = $r->email;
		}
	}
	function find_all($cnd, $ord, $lim) {
		$stm = "SELECT id FROM users";
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; }
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new User($r->id));
		}
		return($ar);
	}

	function options($id) {
		foreach(User::find_all('', 'id', '') as $r) {
			if ($id == $r->id)
				echo "<option value='{$r->id}' selected>{$r->name}</option>\n";
			else
				echo "<option value='{$r->id}'>{$r->name}</option>\n";
		}
	}
}

class Map {
	var $id;
	var $created_at;
	var $user;
	var $name;
	var $note;

	function Map($id) {
		$this->id = $id;
		$q = mysql_query("SELECT * from maps where id=$id");
		if($r = mysql_fetch_object($q)) {
			$this->created_at = $r->created_at;
			$this->user = new User($r->user_id);
			$this->name = $r->name;
			$this->note = $r->note;
		}
	}
	function find_all($cnd, $ord, $lim) {
		$stm = "SELECT id FROM maps";
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; }
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new Map($r->id));
		}
		return($ar);
	}

	function options($id) {
		foreach(Map::find_all('', 'id', '') as $r) {
			if ($id == $r->id)
				echo "<option value='{$r->id}' selected>{$r->name}</option>\n";
			else
				echo "<option value='{$r->id}'>{$r->name}</option>\n";
		}
	}
}

class MapTray {
	var $id;
	var $map_id;
	var $posision;
	var $tray_id;
	var $tcpu_id;
	var $tdig_id;
	var $note;

	function MapTray($id) {
		$this->id = $id;
		$q = mysql_query("SELECT * from map_trays where id=$id");
		if($r = mysql_fetch_object($q)) {
			$this->map_id   = $r->map_id;
			$this->position = $r->position;
			$this->tray_id  = $r->tray_id;
			$this->tcpu_id  = $r->tcpu_id;
			$this->tdig_id  = Array();
			$this->tdig_id[0] = $r->tdig1_id;
			$this->tdig_id[1] = $r->tdig2_id;
			$this->tdig_id[2] = $r->tdig3_id;
			$this->tdig_id[3] = $r->tdig4_id;
			$this->tdig_id[4] = $r->tdig5_id;
			$this->tdig_id[5] = $r->tdig6_id;
			$this->tdig_id[6] = $r->tdig7_id;
			$this->tdig_id[7] = $r->tdig8_id;
			$this->note = $r->note;
		}
	}
	function find_all($cnd, $ord, $lim) {
		$stm = "SELECT id FROM map_trays";
		if($cnd) { $stm .= " WHERE $cnd"; }
		if($ord) { $stm .= " ORDER BY $ord"; }
		if($lim) { $stm .= " LIMIT $lim"; }
		$q = mysql_query( $stm );
		$ar = array();
		while($r = mysql_fetch_object($q)) {
			array_push($ar, new MapTray($r->id));
		}
		return($ar);
	}

	function options($id) {
		foreach(Map::find_all('', 'id', '') as $r) {
			if ($id == $r->id)
				echo "<option value='{$r->id}' selected>{$r->name}</option>\n";
			else
				echo "<option value='{$r->id}'>{$r->name}</option>\n";
		}
	}
}
?>
