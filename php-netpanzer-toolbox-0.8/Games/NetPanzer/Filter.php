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

require_once ("Map.php");

class Games_NetPanzer_Filter {
	var $gameservers = array ();
	var $conf = array ();
	var $maps = array ();
	
	function Games_NetPanzer_Filter ($gameservers, $maps, $conf) {
		$this->setMaps ($maps);
		$this->setConf ($conf);
		$this->filter ($gameservers, $conf["filters"]);
	}
	
	function setGameservers ($gameservers) {
		$this->gameservers = array ();
		foreach ($gameservers as $gameserver) {
			$this->gameservers[] = $gameserver;
		}
	}
	
	function filter ($gameservers, $bys = array ("players")) {
		$this->setGameservers ($gameservers);
		
		foreach ($bys as $by) {
			if ($by == "players" or $by == "sortbyplayers") {
				$this->sortByPlayers ();
			} elseif ($by == "notempty") {
				$this->killEmptyServers ();
			} elseif ($by == "notfull") {
				$this->killFullServers ();
			} elseif ($by == "notdead") {
				$this->killDeadServers ();
			} elseif ($by == "sortplayersbykills") {
				$this->sortServersPlayersByKills ();
			} elseif ($by == "sortplayersbyscore") {
				$this->sortServersPlayersByScore ();
			} elseif ($by == "resolvemapinfo") {
				$this->resolveMapInfo ();
			}
		}
	}
	
	function getGameServers () {
		$return = array ();
		foreach ($this->gameservers as $i => $unused) {
			$return[$this->gameservers[$i]["host"].":".$this->gameservers[$i]["port"]] = $this->gameservers[$i];
		}
		return $return;
	}
	
	function countGameServers ($onlyavialables = 1) {
		if ($onlyavialables == 1) {
			$count = 0;
			foreach ($this->gameservers as $gs) {
				if (isset ($gs["status"]["error"]) and $gs["status"]["error"] == "timeout") {
				} else {
					$count++;
				}
			}
			return $count;
		} else {
			return count ($this->gameservers);
		}
	}
	
	function killEmptyServers () {
		foreach ($this->gameservers as $i => $server) {
			if (isset ($server["status"]["numplayers"]) and $server["status"]["numplayers"] == 0) {
				unset ($this->gameservers[$i]);
			}
		}
		$this->remerge ();
	}

	function killFullServers () {
		foreach ($this->gameservers as $i => $server) {
			if (isset ($server["status"]["numplayers"]) and isset ($server["status"]["maxplayers"]) and $server["status"]["numplayers"] == $server["status"]["maxplayers"]) {
				unset ($this->gameservers[$i]);
			}
		}
		$this->remerge ();
	}
	
	function killDeadServers () {
		foreach ($this->gameservers as $i => $server) {
			if (isset ($server["status"]["error"]) and $server["status"]["error"] == "timeout") {
				unset ($this->gameservers[$i]);
			}
		}
		$this->remerge ();
	}
	
	function resolveMapInfo () {
		if ($this->conf == array ()) {
			return "no conf given";
		}
		
		$map = new Games_NetPanzer_Map ($this->conf);
		
		if ($this->maps != array ()) {
			$map->setMaps ($this->maps);
		}
		
		foreach ($this->gameservers as $key => $gs) {
			if (isset ($gs["status"]["error"]) and $gs["status"]["error"] == "timeout") {
			} else {
				if ($map->isMap ($gs["status"]["mapname"])) {
					$this->gameservers[$key]["status"]["mapinfo"]["outposts"] = $map->getOutposts ($gs["status"]["mapname"]);
					$this->gameservers[$key]["status"]["mapinfo"]["spawns"] = $map->getSpawns ($gs["status"]["mapname"]);
				}
			}
		}
	}
	
	function setConf ($conf) {
		$this->conf = $conf;
	}
	
	function setMaps ($maps) {
		$this->maps = $maps;
	}
	
	function remerge () {
		$tmp = $this->gameservers;
		$this->setGameservers ($tmp);
	}
	
	function sortByPlayers () {
		$found = 1;
		while ($found != 0) {
			$found = 0;
			$i = 0;
			while (isset ($this->gameservers[$i]["status"]) and isset ($this->gameservers[$i+1]["status"])) {
				if (isset ($this->gameservers[$i]["status"]["numplayers"]) and isset ($this->gameservers[$i+1]["status"]["numplayers"])) {
					if ($this->gameservers[$i]["status"]["numplayers"] < $this->gameservers[$i+1]["status"]["numplayers"]) {
						// SWAP
						$tmp = $this->gameservers[$i];
						$this->gameservers[$i] = $this->gameservers[$i+1];
						$this->gameservers[$i+1] = $tmp;
												
						$found++;
					}
				} elseif (!isset ($this->gameservers[$i]["status"]["numplayers"]) and isset ($this->gameservers[$i+1]["status"]["numplayers"])) {
						// SWAP dead down
						$tmp = $this->gameservers[$i];
						$this->gameservers[$i] = $this->gameservers[$i+1];
						$this->gameservers[$i+1] = $tmp;
												
						$found++;
				}
				$i++;
			}
		}
	}
	
	function sortServersPlayersByScore () {
		foreach ($this->gameservers as $key => $server) {
			if (!isset ($server["status"]["error"]) and isset ($server["status"]["players"])) {
				$players = $server["status"]["players"];
				
				$found = 1;
				while ($found != 0) {
					$found = 0;
					$i = 0;
					while (isset ($players[$i]) and isset ($players[$i+1])) {
						if ($players[$i]["score"] < $players[$i+1]["score"]) {
							// SWAP
							$tmp = $players[$i];
							$players[$i] = $players[$i+1];
							$players[$i+1] = $tmp;
							
							$found++;
						}
						$i++;
					}
				}
				
				$this->gameservers[$key]["status"]["players"] = $players;
			}
		}
	}	
	
	function sortServersPlayersByKills () {
		foreach ($this->gameservers as $key => $server) {
			if (!isset ($server["status"]["error"]) and isset ($server["status"]["players"])) {
				$players = $server["status"]["players"];
				
				$found = 1;
				while ($found != 0) {
					$found = 0;
					$i = 0;
					while (isset ($players[$i]) and isset ($players[$i+1])) {
						if ($players[$i]["kills"] < $players[$i+1]["kills"]) {
							// SWAP
							$tmp = $players[$i];
							$players[$i] = $players[$i+1];
							$players[$i+1] = $tmp;
							
							$found++;
						}
						$i++;
					}
				}
				
				$this->gameservers[$key]["status"]["players"] = $players;
			}
		}
	}
}
?>