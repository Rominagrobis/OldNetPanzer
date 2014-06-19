/*
Copyright (C) 1998 Pyrosoft Inc. (www.pyrosoftgames.com), Matthew Bogue

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
#ifndef _PLAYERNETMESSAGE_HPP
#define _PLAYERNETMESSAGE_HPP

#include "Core/CoreTypes.hpp"
#include "NetMessage.hpp"

#include "Network/PlayerRequests/ChangeFlagRequest.hpp"

enum { _net_message_id_player_connect_id,
       _net_message_id_player_sync_flag,
       _net_message_id_player_update_flag,
       _net_message_id_player_sync_state,
       _net_message_id_player_score_update,
       _net_message_id_player_flagtimer_update,
       _net_message_id_team_change_update,
       _net_message_id_player_ready_update,
       _net_message_id_player_team_score_sync,
       _net_message_id_player_vote_request,
       _net_message_id_player_vote,
       _net_message_id_alliance_created_update,
       _net_message_id_alliance_broken_update
     };

#ifdef MSVC
#pragma pack(1)
#endif

class PlayerConnectID : public NetMessage
{
public:
    PlayerID player_id;

    PlayerConnectID(PlayerID id)
        : player_id(id)
    {
        message_class = _net_message_class_player;
        message_id = _net_message_id_player_connect_id;
    }
}
__attribute__((packed));


class PlayerStateSync : public NetMessage
{
public:
    NetworkPlayerState player_state;

    PlayerStateSync()
    {
        message_class = _net_message_class_player;
        message_id = _net_message_id_player_sync_state;
    }

    PlayerStateSync(NetworkPlayerState state)
        : player_state(state)
    {
        message_class = _net_message_class_player;
        message_id = _net_message_id_player_sync_state;
    }
}
__attribute__((packed));

class PlayerScoreUpdate : public NetMessage
{
private:
    PlayerID kill_by_player_index;
    PlayerID kill_on_player_index;
public:
    Uint8  unit_type;

    PlayerScoreUpdate()
    {
        message_class = _net_message_class_player;
        message_id = _net_message_id_player_score_update;
    }

    void set(PlayerID kill_by_index, PlayerID kill_on_index, Uint8 unit_type)
    {
        kill_by_player_index = kill_by_index;
        kill_on_player_index = kill_on_index;
        this->unit_type = unit_type;
    }

    void setKillByPlayerIndex(PlayerID kill_by_index)
    {
        kill_by_player_index = kill_by_index;
    }
    PlayerID getKillByPlayerIndex() const
    {
        return kill_by_player_index;
    }

    void setKillOnPlayerIndex(PlayerID kill_on_index)
    {
        kill_on_player_index = kill_on_index;
    }
    PlayerID getKillOnPlayerIndex() const
    {
        return kill_on_player_index;
    }
}
__attribute__((packed));

class PlayerFlagSync : public ChangeFlagRequest
{
public:
    PlayerID player_id;
    PlayerFlagSync()
    {
        message_class = _net_message_class_player;
        message_id = _net_message_id_player_sync_flag;
    }
} __attribute__((packed));

class PlayerFlagTimerUpdate : public NetMessage
{
public:
    Uint16 flag_timer;

    PlayerFlagTimerUpdate(Uint16 flagtimer)
    {
        message_class = _net_message_class_player;
        message_id = _net_message_id_player_flagtimer_update;
        flag_timer = flagtimer;
    }

    Uint16 getflagtimer() const
    {
        return flag_timer;
    }
}
__attribute__((packed));

enum { change_team_request,
       change_team_Accepted
     };

class PlayerTeamUpdate : public NetMessage
{
public:
    PlayerID player_id;
    TeamID team_id;

    PlayerTeamUpdate()
    {
        message_class = _net_message_class_player;
        message_id = _net_message_id_team_change_update;
    }

}
__attribute__((packed));

enum { ready_request,
       ready_Accepted};

class PlayerReadyUpdate : public NetMessage
{
public:
    PlayerID player_id;

    PlayerReadyUpdate()
    {
        message_class = _net_message_class_player;
        message_id = _net_message_id_player_ready_update;
    }
} __attribute__((packed));

class TeamScoreSync : public NetMessage
{
private:
    short TeamKills;
    short TeamLosses;
public:
    Uint8 TeamID;

    TeamScoreSync()
    {
        message_class = _net_message_class_player;
        message_id = _net_message_id_player_team_score_sync;
    }

    void set(Uint8 Team_ID, short kills, short Losses)
    {
        TeamKills = kills;
        TeamLosses =  Losses;
        TeamID = Team_ID;
    }

    void setKills(short kills)
    {
        TeamKills = kills;
    }
    short getKills() const
    {
        return TeamKills;
    }

    void setLosses(short Losses)
    {
        TeamLosses = Losses;
    }
    short getLosses() const
    {
        return TeamLosses;
    }
}
__attribute__((packed));

class PlayerVoteRequested : public NetMessage
{
public:
    Uint8  vote_type;

    PlayerVoteRequested()
    {
        message_class = _net_message_class_player;
        message_id = _net_message_id_player_vote_request;
    }

    void set(Uint8 vote_type)
    {
        this->vote_type = vote_type;
    }
}
__attribute__((packed));

class PlayerSendVote : public NetMessage
{
private:
    PlayerID player_index;
public:
    Uint8 player_vote;

    PlayerSendVote()
    {
        message_class = _net_message_class_player;
        message_id = _net_message_id_player_vote;
    }

    void set(PlayerID player_idx, bool player_vote)
    {
        this->player_index = player_idx;
        this->player_vote = player_vote;
    }

    PlayerID getPlayerIndex() const
    {
        return player_index;
    }
}
__attribute__((packed));

#ifdef MSVC
#pragma pack()
#endif

#endif // ** _PLAYERNETMESSAGE_HPP
