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

#include "Interfaces/VoteManager.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/ChatInterface.hpp"
#include "Interfaces/GameControlRulesDaemon.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/TeamManager.hpp"
#include "Interfaces/StrManager.hpp"
#include "Classes/Network/PlayerNetMessage.hpp"
#include "Classes/Network/NetworkServer.hpp"
#include "Classes/Network/NetworkClient.hpp"
#include "Classes/Network/NetworkState.hpp"
#include "Views/Components/Desktop.hpp"
#include "Views/Game/VoteBox.hpp"

bool   VoteManager::vote_in_progress = false;
Uint8  VoteManager::type_vote = unassigned_vote;
Uint8  *VoteManager::player_vote = 0;
Uint8  VoteManager::vote_counter = 0;
Uint8  VoteManager::players_in_vote = 0;
Timer  VoteManager::voteTimer;
Uint8  VoteManager::vote_team = 0xFF;

VoteBox *votebox;

const char *VoteStrings[1] = {"Surrendering vote, you choose surrendering?\0"};

void VoteManager::startVote(Uint8 type)
{
    vote_in_progress = true;
    type_vote = type;
    if (GameConfig::game_teammode)
        vote_counter = TeamManager::CountPlayerinTeam(vote_team);
    else
        vote_counter = PlayerInterface::getActivePlayerCount();
    players_in_vote = vote_counter;
}

void VoteManager::netMessageReceiveRequestVote(const NetMessage* message)
{
    const PlayerVoteRequested* vote_request
    = (const PlayerVoteRequested *) message;
    
    votebox = new VoteBox(VoteStrings[vote_request->vote_type]);
    
    Desktop::add(votebox);
    Desktop::setVisibility("votebox", true);
}

void VoteManager::resetVote()
{
    vote_in_progress = false;
    type_vote = unassigned_vote;
    delete[] player_vote;
    
    if (votebox)
    {
        Desktop::remove(votebox);
    }
    
    player_vote = new Uint8 [PlayerInterface::getMaxPlayers()];
    for ( int i = 0; i < PlayerInterface::getMaxPlayers(); i++ )
    {
        player_vote[ i ] = vote_nothing;
    }
    if ( NetworkState::status == _network_state_server )
        voteTimer.changePeriod(70);// more time for prevent some client retard (lag)
    else
        voteTimer.changePeriod(60);
    
    voteTimer.reset();
    players_in_vote = 0;
    vote_team = 0xFF;
}

void VoteManager::serverSendRequestVote()
{
    PlayerVoteRequested vote_request;
    vote_request.set(type_vote);
    if (GameConfig::game_teammode)
        TeamManager::sendMessageToTeam(vote_team, &vote_request, sizeof(PlayerVoteRequested));
    else
        SERVER->broadcastMessage(&vote_request, sizeof(PlayerVoteRequested));
}

void VoteManager::executeVoteAction()
{
    switch (type_vote)
    {
        case surrender_vote :
            GameControlRulesDaemon::forceEndRound();
            break;
    }
}

void VoteManager::checkPlayersVote()
{
    Uint8 yes_vote = 0, no_vote = 0;

    for ( int i = 0; i < PlayerInterface::getMaxPlayers(); i++ )
    {
        if (player_vote[ i ] == vote_yes)
            yes_vote++;
        else
        if (player_vote[ i ] == vote_no)
            no_vote++;
    }
    char buff[100];

    int no_percent = ((players_in_vote-yes_vote)*100)/players_in_vote;
    int yes_percent = ((players_in_vote-no_vote)*100)/players_in_vote;

    bool accepted_vote = ( yes_percent > 80 );

    sprintf(buff, "%d %% players has voted YES, %d %% has voted NO", yes_percent, no_percent);

    if ( GameConfig::game_teammode )
    {
        TeamManager::serversayToTeam(vote_team, buff);
        if ( accepted_vote )
        {
            sprintf(buff, "%s team has accepted surrendering. Round is over",
                    TeamManager::getTeamName(vote_team).c_str());
            ChatInterface::serversay(buff);
        }
    }
    else
    {
        ChatInterface::serversay(buff);
    }
    vote_in_progress = false;
    if ( accepted_vote ) executeVoteAction();
}

void VoteManager::playerVote(Uint8 responce)
{
    Desktop::setVisibility("votebox", false);
    Desktop::remove(votebox);

    PlayerSendVote vote_player;
    vote_player.set(PlayerInterface::getLocalPlayerIndex(), responce);
    CLIENT->sendMessage( &vote_player, sizeof(PlayerSendVote));
}

bool VoteManager::checkVoteTimer()
{
    //server need to check the timer for stop vote
    //if player crash or quit the game, vote don't ending
    if (vote_in_progress)
    {
        return voteTimer.count();
    }
    return false;
}

int VoteManager::getTimer()
{
    return (int) voteTimer.getTimeLeft();
}

void VoteManager::playerRequest_startSurrenderVote(const PlayerID player_id)
{
    if (vote_in_progress)
    {
        return;
    }
    
    resetVote();
    
    vote_team = PlayerInterface::getPlayer(player_id)->getTeamID();
    
    startVote(surrender_vote);
    
    serverSendRequestVote();
    
    char buff[100];
    sprintf(buff, "Player %s request vote",
            PlayerInterface::getPlayer(player_id)->getName().c_str());
    
    if (GameConfig::game_teammode)
        TeamManager::serversayToTeam(vote_team, buff);
    else
        ChatInterface::serversay(buff);
}

void VoteManager::playerRequest_voteSelected(const PlayerID player_id, Uint8 selected_vote)
{
    if (vote_in_progress)
    {
        vote_counter--;
        player_vote[ player_id ] = selected_vote;
        
        char buff[100];
        sprintf(buff, "player %s has voted, waiting %d votes",
                PlayerInterface::getPlayer(player_id)->getName().c_str(),
                vote_counter);
        
        if (GameConfig::game_teammode)
        {
            TeamManager::serversayToTeam(vote_team, buff);
        }
        else
        {
            ChatInterface::serversay(buff);
        }
        
        if (vote_counter < 1)
        {
            checkPlayersVote();
        }
    }
}
