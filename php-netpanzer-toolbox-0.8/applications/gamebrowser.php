<?php
/*
Copyright (C) 2004 Tobias Blersch <npb@schrelb.de>
                                                                                
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
                                                                                
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
                                                                                
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* * php - netPanzer - Browser
*	
* Displays all running netPanzer games.
* For  more  info  on  netPanzer  visit
* *  http://netpanzer.berlios.de
*
* * Configuration:
*
* Please note:	  All times are seconds.
*/

/** Include base configuration file: **/
require_once ("base.conf.php");


/** Specific configuration data: **/
$conf = array_merge ($conf, array (
	"cachelifetime" => 	60,
	"initialmasters" => array (
							"81.169.185.36",
							"81.173.119.122"
						),
	"reloadtime" => 	20,
	"timeout" => 		1,
	"cacheid" => 		"php-netPanzer-Browser",
	"template" => 		"display.smarty",
	"displayconf" => 	1,
	"filters" =>		array (
							"notempty",
							"sortbyplayers",
							"sortplayersbykills",
							"sortplayersbyscore",
							"resolvemapinfo",
						) // also possible: "notempty", "notfull", "notdead"
));

/** Include list with avialable maps **/
require_once ($conf["mapdatadir"] . "maplist.conf.php");

/**
* * Code:
**/

// Include the cache package
require_once('Cache/Lite.php');

// Create a Cache_Lite object
$Cache_Lite = new Cache_Lite(array('cacheDir' => $conf["cachedir"],'lifeTime' => $conf["cachelifetime"]));

if (isset ($_GET["refresh"]) and $_GET["refresh"] == "1") {
	$Cache_Lite->remove ($conf["cacheid"]);
}

// Test if thereis a valide cache for this id
if ($data = $Cache_Lite->get($conf["cacheid"])) {
	$data = unserialize ($data);
	$cachehit = 1;
	// echo "Cache Hit!\n";
} else { // No valid cache found (you have to make the page)
	$cachehit = 0;
	require_once ($conf["includepath"] . "Games/NetPanzer/Browser.php");
	$browser = new Games_NetPanzer_Browser ();
	
	foreach ($conf["initialmasters"] as $initmaster) {
		$browser->addMaster ($initmaster);
	}
	
	$browser->setTimeout ($conf["timeout"]);
	// Resolve game server list.
	$browser->Browse ();
	// Query each server for status info.
	$browser->getGameserversStatus ();
	
	// Apply selected filters.
	require_once ($conf["includepath"] . "Games/NetPanzer/Filter.php");
	$filter = new Games_NetPanzer_Filter ($browser->getGameServers (), $avialablemaps, $conf);
	
	$data["masterservers"] = $browser->getMasters();
	$data["gameservers"] = $filter->getGameServers ();
	$data["numgameservers"] = array (
		"all" => $filter->countGameServers (0),
		"active" => $filter->countGameServers ()
	);
    $Cache_Lite->save(serialize ($data));
}

if (!isset ($_GET["raw"]) or $_GET["raw"] != "1") {
	// Smarty output stuff:
	require_once ($conf["smartyclass"]);
	
	$smarty = new Smarty;
	
	$smarty->template_dir = $conf["templatedir"];
	$smarty->compile_dir = $conf["templatecachedir"];
	$smarty->config_dir = './configs/';
	$smarty->cache_dir = './cache/';
	
	$smarty->compile_check = true;
	
	$smarty->assign ("php_self", $_SERVER["PHP_SELF"]);
	$smarty->assign ("masterservers", $data["masterservers"]);
	$smarty->assign ("gameservers", $data["gameservers"]);
	if ($conf["displayconf"] == 1) {
		$smarty->assign ("conf", $conf);
	}
	$smarty->assign ("cachehit", $cachehit);
	$smarty->assign ("numgameservers", $data["numgameservers"]);
	$smarty->display ($conf["template"]);
} else {
	echo "<html><head></head><body><pre>";
	echo "Cache:\n   ";
	if ($cachehit == 1) {
		echo "Hit cache!\n";
	} else {
		echo "I had to build data :-( .\n";
	}
	if ($conf["displayconf"] == 1) {
		echo "\nConfiguration:\n";
		print_r ($conf);
	}
	echo "\nMasterservers:\n";
	print_r ($data["masterservers"]);
	echo "\nGameservers (". $data["numgameservers"]["active"] ."/". $data["numgameservers"]["all"] ." responded):\n";
	print_r ($data["gameservers"]);
	echo "</pre></body></html>";
}
?>