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

require_once ("Masterserver.php");
require_once ("Gameserver.php");

class Games_NetPanzer_Browser {
	var $masterservers;
	var $visitedmasters;
	var $mastersstack;
	var $gameservers;
	var $timeout;
	var $maps;

	function Games_NetPanzer_Browser () {
		$this->visitedmasters = array ();
		$this->mastersstack = array ();
		$this->gameservers = array ();
		$this->timeout = 2;
		$this->maps = array ();
	}
	
	function addMaster ($host, $port = 28900) {
		array_push ($this->mastersstack, array ("host" => $host, "port"=>$port));
	}
	
	function Browse () {
		// Recursively browse all masterservers and ask them for games.
		while (count ($this->mastersstack) > 0) {
			$currentmaster = array_pop ($this->mastersstack);
			if (!isset ($this->visitedmasters[$currentmaster["host"].":".$currentmaster["port"]])) {
				$this->visitedmasters[$currentmaster["host"].":".$currentmaster["port"]] = $currentmaster;
				$browser = new Games_NetPanzer_Masterserver ($currentmaster["host"], $currentmaster["port"]);
				$browser->setTimeout ($this->timeout);
				if ($browser->isConnected ()) {
					$masterlist = $browser->getMasters ();
					foreach ($masterlist as $amaster) {
						if (!isset ($this->visitedmasters[$amaster["host"].":".$amaster["port"]])) {
							array_push ($this->mastersstack, $amaster);
						}
					}
					$games = $browser->getGames ();
					foreach ($games as $agame) {
						if (!isset ($this->gameservers[$agame["host"].":".$agame["port"]])) {
							$this->gameservers[$agame["host"].":".$agame["port"]] = $agame;
							$this->gameservers[$agame["host"].":".$agame["port"]]["masterserver"] = $currentmaster;
						}
					}
					$browser->disconnect ();
				} else {
					$this->visitedmasters[$currentmaster["host"].":".$currentmaster["port"]]["error"] = "timeout";
				}
				unset ($browser);
			}
		}
	}
	
	function getGameServers () {
		return $this->gameservers;
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
	
	function addServer ($host, $port = 3030, $master = NULL) {
		$this->gameservers[$host.":".$port] = array ("host" => $host, "port" => $port);
		if ($master != NULL) {
			$this->gameservers[$host.":".$port]["masterserver"] = $master;
		}
	}
	
	function getMasters () {
		return $this->visitedmasters;
	}
	
	function getGameserversStatus () {
		// Query each gameserver.
		$gq = new Games_NetPanzer_Gameserver ();
		$gq->setTimeout ($this->timeout);
		foreach ($this->gameservers as $gkey=>$game) {
			$gq->reset ($game);
			$this->gameservers[$gkey]["status"] = $gq->getStatus ();
		}
		$gq->disconnect ();
	}
	
	function setTimeout ($timeout) {
		$this->timeout = $timeout;
	}
}

?>