/*
Copyright (C) 2012 Netpanzer Team. (www.netpanzer.org), Laurent Jacques

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

#ifndef __VOTEMANAGER_HPP__
#define __VOTEMANAGER_HPP__

#include "Classes/Network/NetMessage.hpp"
#include "Util/Timer.hpp"

enum { surrender_vote,
       kick_player_vote,
       unassigned_vote
     };

enum { vote_yes,
       vote_no,
       vote_nothing
     };


class VoteManager
{
protected:
    static Uint8 type_vote;
    static bool vote_in_progress;
    static Uint8 *player_vote;
    static Uint8 vote_counter;
    static Uint8 players_in_vote;
    static Timer voteTimer;
    static Uint8 vote_team;
public:
    static bool checkVoteTimer();
    static void checkPlayersVote();
    static void executeVoteAction();
    static void resetVote();
    static void startVote(Uint8 type);
    static void playerVote(Uint8 responce);
    static void serverSendRequestVote();
    static void netMessageReceiveRequestVote(const NetMessage* message);
    static int getTimer();
    
    static void playerRequest_startSurrenderVote(const PlayerID player_id);
    static void playerRequest_voteSelected(const PlayerID player_id, Uint8 selected_vote);
};

#endif
