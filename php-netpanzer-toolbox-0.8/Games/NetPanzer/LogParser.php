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

class Games_NetPanzer_LogParser {
	var $logfile;
	var $conf;
	
	function Games_NetPanzer_LogParser ($logfile, $conf) {
		$this->setConf($conf);
		$this->setLogfile ($logfile);
	}
	
	function setConf ($conf) {
		$this->conf = $conf;
	}
	
	function setLogfile ($logfile) {
		//$this->cleanup ();
		$this->logfile = $logfile;
	}
	
	function parse () {
		$logfile = file ($this->conf["logfilesdir"].$this->logfile);
		$logfile = str_replace ("\n", "", $logfile);
		$logfile = str_replace ("\r", "", $logfile);
		
		$return = array ();
		$i = 0;
		
		// Parse gameinfo till "game started"
		while (isset ($logfile[$i]) and !substr_count ($logfile[$i], "game started")) {
			// Parse gameinfo.
			$exp = explode (">", $logfile[$i], 2);
			$time = str_replace ("<", "", $exp[0]);
			$setting = explode (": ", $exp[1]);
			if (isset ($setting[1]) and $setting[1] != "") {
				$return["serversettings"][$setting[0]] = $setting[1];
			} else {
				$return["serversettings"]["Launched"] = $time;
			}
			$i++;
		}
		
		$return["logdata"] = array ();
		while (isset ($logfile[$i]) and !substr_count ($logfile[$i], "loading map '")) {
			// Parse logdata
			list ($time, $message) = explode (">", $logfile[$i], 2);
			$time = str_replace ("<", "", $time);
			
			if (substr_count ($message, "' has been occupied by '")) {
				// Occupied outpost.
				$exp = explode ("' has been occupied by '", $message);
				list ($outpost, $player) = str_replace ("'", "", $exp);
								
				$return["logdata"]["$time"][] = array ("opcode" => "occupied", "outpost" => $outpost, "player" => $player);
			} elseif (substr_count ($message, "' killed a '") and substr_count ($message, "' with his '")) {
				// 'Robo' killed a 'Bobcat' from 'Robo' with his 'Titan'.
				
				list ($expa, $expb) = explode ("' from '", $message);
				list ($killer, $victimtank) = explode ("' killed a '", $expa);
				list ($victim, $killertank) = explode ("' with his '", $expb);
				
				$killer = str_replace ("'", "", $killer);
				$killertank= str_replace ("'.", "", $killertank);
								
				$return["logdata"]["$time"][] = array (
					"opcode" => "killed",
					"killer" => $killer,
					"victim" => $victim,
					"killertank" => $killertank,
					"victimtank" => $victimtank
				);
				
			} elseif (substr_count ($message, "' has joined the game.")) {
				$player = str_replace (array ("'", " has joined the game."), array ("",""), $message);
				$return["logdata"]["$time"][] = array ("opcode" => "joined", "player" => $player);
				
			} elseif (substr_count ($message, "' has left the game.")) {
				$player = str_replace (array ("'", " has left the game."), array ("",""), $message);
				$return["logdata"]["$time"][] = array ("opcode" => "left", "player" => $player);
				
			} elseif (substr_count ($message, "Server: Round is over")) {
				$return["logdata"]["$time"][] = array ("opcode" => "roundover");

			} elseif (substr_count ($message, "' has been unexpectedly broken.") and substr_count ($message, "Connection to '")) {
				$player = str_replace (array ("Connection to '", "' has been unexpectedly broken."), "", $message);
				$return["logdata"]["$time"][] = array ("opcode" => "lostconnection", "player" => $player);
								
			} elseif (substr_count ($message, ": ")) {
				list ($player, $message) = explode (": ", $message);
				if (strlen ($message) > 0) {
					$return["logdata"]["$time"][] = array ("opcode" => "said", "player" => $player, "message" => $message);
				}
			}
			
			$i++;
		}
		
		$return["nextmap"] = array ();
		if (isset ($logfile[$i]) and substr_count ($logfile[$i], "loading map '")) {
			list ($time, $message) = explode (">", $logfile[$i], 2);
			$time = str_replace ("<", "", $time);
			$map = str_replace (array ("loading map '", "'."), "", $message);
			
			$return["nextmap"] = array ("map" => $map, "time" => $time);
		}
		
		// MatzeB	Regex to query the map line .*Map:\s*(.*)\n
		// MatzeB	Regex for an occupied line: .*'(.*)' has been occupied by '(.*)'
		
		return $return;
	}
}
?>
