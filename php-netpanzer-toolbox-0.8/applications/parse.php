<html>

<head>
<meta http-equiv="Content-Type" content="text/html; charset=windows-1252">
<title></title>
</head>

<body><pre>
<?php
require_once ("../Games/NetPanzer/LogParser.php");

/** Include base configuration file: **/
require_once ("base.conf.php");

/** Specific configuration data: **/
$conf = array_merge ($conf, array (
	"cachelifetime" => 	600,
	"cacheid" =>		"parsedlog",
	"logfilesdir" => 	"../data/logs/ndm/"
	//"logfilesdir" => 	"C:\Dokumente und Einstellungen\Abitos Schrelb\.netpanzer\logs\\"
));

$conf["cacheid"] .= $conf["logfilesdir"];

// Include the cache package
require_once('Cache/Lite.php');

// Create a Cache_Lite object
$Cache_Lite = new Cache_Lite(array('cacheDir' => $conf["cachedir"],'lifeTime' => $conf["cachelifetime"]));

if (isset ($_GET["refresh"]) and $_GET["refresh"] == "1") {
	$Cache_Lite->remove ($conf["cacheid"]);
}

// Test if thereis a valide cache for this id
if ($data = $Cache_Lite->get($conf["cacheid"])) {
	$logs = unserialize ($data);
	unset ($data);
} else {
	$log = new Games_NetPanzer_LogParser ("sample.log", $conf);
	$dh = opendir($conf["logfilesdir"]);
	echo "<b>Logfiles:</b>\n";
	$logs = array ();
	$c = 0;
	while (($file = readdir($dh)) !== false) { 
		if (filetype($conf["logfilesdir"] . $file) == "file") {
			echo '<a href="'.$conf["logfilesdir"].$file.'">'."$file</a>\n";
			$log->setLogfile ($file);
			$logg = $log->parse ();
			
			$launchtime = strtotime ($logg["serversettings"]["Launched"]);
			if (isset ($logs["$launchtime"])) {
				$launchtime = $c;
				$c++;
			}
			$logs["$launchtime"] = $logg;
		}
	}
	closedir($dh);
	
	$found = "a";
	while ($found != 0) {
		$oldkey = "";
		foreach ($logs as $time => $data) {
			if ($oldkey != "") {
				if ($oldkey < $time) {
					//SWAP
					$tmp = $data;
					
					$tmp = $logs[$time];
					$logs[$time] = $logs[$oldkey];
					$logs[$oldkey] = $tmp;
					
					if ($found == "a") {
						$found = 1;
					} else {
						$found++;
					}
				} else {
					$oldkey = $time;
				}
			}
		}
	}
	
    $Cache_Lite->save(serialize ($logs));
}

#print_r ($logs);

foreach ($logs as $starttime => $data) {
	foreach ($data["logdata"] as $time => $opset) {
		foreach ($opset as $op) {
			if ($op["opcode"] == "said") {
				echo $time . "	" . $op["player"] . "	said	" . $op["message"] . "\n";
			}
		}
	}
}

?></pre>
</body>

</html>
