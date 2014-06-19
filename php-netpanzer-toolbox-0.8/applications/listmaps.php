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
* * This script displays a list of all locally avialable maps.
* 
* * Configuration:
*
* Please note:	  All times are seconds.
*/

/** Include base configuration file: **/
require_once ("base.conf.php");


/** Specific configuration data: **/
$conf = array_merge ($conf, array (
	"cachelifetime" =>	7200,	// Cache processed images for 2h = 7200s
	"cacheid" => 		"php-netPanzer-Browser-maplist",
	"template" => 		"maplist.smarty",
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

if ($data = $Cache_Lite->get($conf["cacheid"])) {
	$cachehit = 1;
	$data = unserialize ($data);
} else {
	$cachehit = 0;
	require_once ($conf["includepath"] . "Games/NetPanzer/Map.php");
	
	$maps = new Games_NetPanzer_Map ($conf);
	$maps->setMaps ($avialablemaps);
	
	$data = array ();
	$c = 0;	
	foreach ($avialablemaps as $map) {
		$data["$map"]["name"] = $map;
		$data["$map"]["outposts"] = $maps->getOutposts ($map);
		$data["$map"]["spawns"] = $maps->getSpawns ($map);
		$c++;
	}
	
	// Sort by number of outposts:
	
	// Transform string keys to numbers: 
	$tmpdata = array ();
	foreach ($data as $map) {
		$tmpdata[] = $map;
	}
	
	// Bubblesort
	$found = 1;
	while ($found != 0) {
		$prev = NULL;
		$found = 0;
		foreach ($tmpdata as $key => $map) {
			if ($prev !== NULL) {
				if ($tmpdata[$prev]["outposts"]["number"] < $map["outposts"]["number"]) {	
					// SWAP
					
					$tmp = $tmpdata[$prev];
					$tmpdata[$prev] = $map;
					$tmpdata[$key] = $tmp;
					
					$found++;
				} else {
					$prev = $key;
				}
			} else {
				$prev = $key;
			}
		}
	}
	// Transform nummeric keys to strings: 
	$data = array ();
	foreach ($tmpdata as $map) {
		$data[$map["name"]] = $map;
	}
	
    $Cache_Lite->save(serialize ($data));
}

// Smarty output stuff:
require_once ($conf["smartyclass"]);

$smarty = new Smarty;

$smarty->template_dir = $conf["templatedir"];
$smarty->compile_dir = $conf["templatecachedir"];
$smarty->config_dir = './configs/';
$smarty->cache_dir = './cache/';

$smarty->compile_check = true;

$smarty->assign ("maplist", $data);
$smarty->assign ("cachehit", $cachehit);

header ("Cache-Control: max-age=". $conf["cachelifetime"] );
$smarty->display ($conf["template"]);
?>