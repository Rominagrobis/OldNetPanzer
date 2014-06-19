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

#include "Classes/Network/ClientConnectDaemon.hpp"

#include <stdio.h>

#include "Units/UnitInterface.hpp"
#include "NetworkClient.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/GameManager.hpp"
#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/TeamManager.hpp"
#include "Interfaces/StrManager.hpp"
#include "Units/UnitProfileInterface.hpp"

#include "Resources/ResourceManager.hpp"

#include "Util/Log.hpp"
#include "ConnectNetMessage.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "Views/Game/LoadingView.hpp"

#include "Core/NetworkGlobals.hpp"

#include "System/Sound.hpp"
#include "Actions/ActionManager.hpp"

enum { _connect_state_idle = 0,
       _connect_state_waiting_link,
       _connect_state_waiting_connect_start,
       _connect_state_waiting_connect_result,
       _connect_state_wait_for_server_game_setup,
       _connect_state_setup_client_game,
       _connect_state_sync_profiles,
       _connect_state_connect_failure
     };


Timer ClientConnectDaemon::failure_display_timer;
Timer ClientConnectDaemon::time_out_timer;
int   ClientConnectDaemon::time_out_counter = 0;
unsigned char ClientConnectDaemon::connection_state;

#define _CLIENT_CONNECT_TIME_OUT_TIME (6.0)
#define _CLIENT_CONNECT_RETRY_LIMIT   (5)


void ClientConnectDaemon::startConnectDaemon()
{
    connection_state = _connect_state_idle;
}

void ClientConnectDaemon::shutdownConnectDaemon()
{
    ConnectMesgNetPanzerClientDisconnect client_disconnect;

    client_disconnect.setPlayerID(
        PlayerInterface::getLocalPlayerIndex());

    CLIENT->sendMessage(&client_disconnect,
                        sizeof(ConnectMesgNetPanzerClientDisconnect));

    CLIENT->sendRemaining();
    
    CLIENT->partServer();
}

void ClientConnectDaemon::startConnectionProcess( )
{
    failure_display_timer.changePeriod( 10 );
    time_out_timer.changePeriod( _CLIENT_CONNECT_TIME_OUT_TIME );
    time_out_counter = 0;
    connection_state = _connect_state_waiting_link;
    LoadingView::append("");
    LoadingView::append(NPString(_("Connecting to server ")) + gameconfig->serverConnect.c_str());
}

unsigned char ClientConnectDaemon::netMessageLinkAck(const NetMessage* message)
{
    ClientConnectJoinRequestAck *join_request_ack_mesg;
    char buf[80];
    unsigned char rval;

    join_request_ack_mesg = (ClientConnectJoinRequestAck *) message;

    switch( join_request_ack_mesg->getResultCode() )
    {
    case _join_request_result_success :
        LoadingView::append( _("Link to Server Established") );
        sprintf( buf, _("Protocol Version: %u"),
                 join_request_ack_mesg->getServerProtocolVersion());
        LoadingView::append( buf );
        rval = _connect_state_waiting_connect_start;
        break;

    case _join_request_result_invalid_protocol :
        LoadingView::append( _("Link to Server FAILED!") );
        LoadingView::append( _("Incompatible game:") );
        LoadingView::append( getNetpanzerProtocolMessage(join_request_ack_mesg->getServerProtocolVersion()));
//        sprintf( buf, "Server Protocol Version: %u",
//                join_request_ack_mesg->getServerProtocolVersion());
//        LoadingView::append( buf );
        rval = _connect_state_connect_failure;
        failure_display_timer.reset();
        break;

    case _join_request_result_server_busy :
        LoadingView::append( _("Link to Server FAILED!") );
        LoadingView::append( _("Server is VERY busy") );
        LoadingView::append( _("Please try in a few minutes") );
        rval = _connect_state_connect_failure;
        failure_display_timer.reset();
        break;

    case _join_request_result_already_connected :
        LoadingView::append( _("Link to Server FAILED!") );
        LoadingView::append( _("Your IP is already connected to this server") );
        LoadingView::append( _("The server limits multiple connections from same IP") );
        rval = _connect_state_connect_failure;
        failure_display_timer.reset();
        break;

    case _join_request_result_wrong_password :
        LoadingView::append( _("Link to Server FAILED!") );
        LoadingView::append( _("The password for server was wrong.") );
        rval = _connect_state_connect_failure;
        failure_display_timer.reset();
        break;

    case _join_request_result_banned :
        LoadingView::append( _("Link to Server FAILED!") );
        LoadingView::append( _("You are banned in this server") );
        rval = _connect_state_connect_failure;
        failure_display_timer.reset();
        break;

    default:
        LoadingView::append( _("Link to Server FAILED!") );
        LoadingView::append( _("Unknown result sent from server") );
        rval = _connect_state_connect_failure;
        failure_display_timer.reset();
        break;
    }

    return rval;
}

void ClientConnectDaemon::netMessageConnectProcessUpdate(const NetMessage* message )
{
    ConnectProcessUpdate *process_update;

    process_update = (ConnectProcessUpdate *) message;

    char buf[80];
    snprintf(buf, sizeof(buf), _("Your Position In Queue is %d "),
             process_update->getQueuePosition());

    LoadingView::append( buf );
}

void ClientConnectDaemon::netMessageConnectProcessMessage(const NetMessage* message)
{
    ConnectProcessStateMessage *state_mesg;

    char str_buf[128];

    state_mesg = (ConnectProcessStateMessage *) message;

    switch ( state_mesg->getMessageEnum() )
    {
    case _connect_state_message_load_game_data :
    {
        LoadingView::append( _("Loading Game Data ..." ));
    }
    break;

    case  _connect_state_message_sync_player_info :
    {
        LoadingView::append( _("Synchronizing Player Info ..." ));
    }
    break;

    case _connect_state_message_sync_player_info_percent :
    {
        snprintf(str_buf, sizeof(str_buf), "%s (%d%%)",
                 _("Synchronizing Player Info ..."),
                 state_mesg->getPercentComplete());
        LoadingView::update( str_buf );
    }
    break;

    case _connect_state_message_sync_unit_profiles:
        LoadingView::append(_("Synchronizing unit profiles..."));
        UnitProfileInterface::clearProfiles();
        break;

    case  _connect_state_message_sync_units :
    {
        LoadingView::append( _("Synchronizing Game Elements ..." ));
    }
    break;

    case _connect_state_message_sync_units_percent :
    {
        snprintf(str_buf, sizeof(str_buf), "%s (%d%%)",
                 _("Synchronizing Game Elements ..."),
                 state_mesg->getPercentComplete());
        LoadingView::update( str_buf );
    }
    break;

    case  _connect_state_sync_complete :
    {
        LoadingView::append( _("Game Synchronized") );
        LoadingView::loadFinish();
        connection_state = _connect_state_idle;
    }
    break;
    }
}

void ClientConnectDaemon::netMessageConnectServerDisconnect(const NetMessage* )
{
    ConsoleInterface::postMessage(Color::unitAqua, false, 0, _("Server has terminated the game") );
}

void ClientConnectDaemon::processNetMessage(const NetMessage* message)
{
    switch (message->message_id)
    {

    case _net_message_id_client_connect_process_state_mesg :
    {
        netMessageConnectProcessMessage( message );
    }
    break;

    case _net_message_id_connect_netPanzer_server_disconnect :
    {
        netMessageConnectServerDisconnect( message );
    }
    break;

    default :
        connectFsm( message );
        break;
    } // ** switch

}

void ClientConnectDaemon::serverConnectionBroken()
{
    LoadingView::show();
    LoadingView::append(_("Error in server connection"));
    failure_display_timer.reset();
    connection_state = _connect_state_connect_failure;
}

void ClientConnectDaemon::connectFailureResult( unsigned char result_code )
{
    switch( result_code )
    {
    case _connect_result_server_busy :
    {
        LoadingView::append( _("Connect Failure: Server Too Busy") );
    }
    break;

    case _connect_result_server_full :
    {
        LoadingView::append( _("Connect Failure: Server Full") );
    }
    break;

    case _connect_result_server_already_connected :
    {
        LoadingView::append( _("Connect Failure: You are aready connected") );
    }
    break;
    } // ** switch

}

void ClientConnectDaemon::connectFsm(const NetMessage* message )
{
    unsigned char message_id = message ? message->message_id : 0xff;

    switch ( connection_state )
    {
    case _connect_state_idle :
        return;

    case _connect_state_waiting_link:
        if ( message_id == _net_message_id_connect_join_game_request_ack )
        {
            time_out_timer.changePeriod( _CLIENT_CONNECT_TIME_OUT_TIME );
            time_out_counter = 0;
            connection_state = netMessageLinkAck( message );
        }
        else
        {
            if ( time_out_timer.count() )
            {
                if ( time_out_counter < _CLIENT_CONNECT_RETRY_LIMIT )
                {
                    LoadingView::append( _("Server is not responding...") );
                    time_out_counter++;
                }
                else
                {
                    LoadingView::append( _("Connection to server failed..."));
                    LoadingView::append( _("it might be older netpanzer or server is down") );
                    connection_state = _connect_state_connect_failure;
                    failure_display_timer.reset();
                }
            }
        }
        break;

    case _connect_state_waiting_connect_start :
        if ( message_id == _net_message_id_client_connect_process_update )
        {
            netMessageConnectProcessUpdate( message );
            break;
        }
        else if ( message_id == _net_message_id_client_start_connect )
        {
            LoadingView::append( _("Connecting ...") );

            ClientConnectRequest connect_request;
            CLIENT->sendMessage(&connect_request, sizeof(ClientConnectRequest));
            sound->playTankIdle();
            connection_state = _connect_state_waiting_connect_result;
        }
        else if ( message_id == _net_message_id_connect_server_full )
        {
            LoadingView::append( _("Connect Failure: Server Full") );

            failure_display_timer.reset();
            connection_state = _connect_state_connect_failure;
            break;
        }
        else
        {
            break;
        }

    case _connect_state_waiting_connect_result :
        if (message_id == _net_message_id_client_connect_result )
        {
            ClientConnectResult *connect_result;

            connect_result = (ClientConnectResult *) message;

            if ( connect_result->result_code != _connect_result_success )
            {
                connectFailureResult( connect_result->result_code );

                connection_state = _connect_state_connect_failure;
                failure_display_timer.reset();
            }
            else
            {
                ConnectClientSettings client_setting;

                client_setting.set(GameConfig::player_name->c_str());
                memcpy(&client_setting.player_flag, GameConfig::player_flag_data, sizeof(client_setting.player_flag));

                CLIENT->sendMessage( &client_setting, sizeof(ConnectClientSettings));

                connection_state = _connect_state_wait_for_server_game_setup;
            }
        }
        else if ( time_out_timer.count() )
        {
            if ( time_out_counter < _CLIENT_CONNECT_RETRY_LIMIT )
            {
                ClientConnectRequest connect_request;
                CLIENT->sendMessage( &connect_request, sizeof(ClientConnectRequest));
                time_out_counter++;
            }
            else
            {
                LoadingView::append( _("Connection To Server Failed") );
                connection_state = _connect_state_connect_failure;
                failure_display_timer.reset();
            }
        }
        break;

    case _connect_state_wait_for_server_game_setup :
        if ( message && message->message_id == _net_message_id_connect_server_game_setup )
        {
            ConnectMesgClientGameSetupPing client_game_setup_ping;

            int result_code;
            char str_buf[128];
            ConnectMesgServerGameSettings *game_setup;
            game_setup = (ConnectMesgServerGameSettings *) message;

            GameManager::startClientGameSetup( message, &result_code );

            if( result_code == _mapload_result_no_map_file )
            {
                sprintf( str_buf, _("MAP %s NOT FOUND!"), game_setup->map_name );
                LoadingView::append( str_buf);
                connection_state = _connect_state_connect_failure;
                failure_display_timer.reset();
            }
            else if( result_code == _mapload_result_no_wad_file )
            {
                LoadingView::append( _("MAP TILE SET NOT FOUND!") );
                LoadingView::append( _("please download the appropriate tileset") );
                LoadingView::append( _("from www.pyrosoftgames.com") );
                connection_state = _connect_state_connect_failure;
                failure_display_timer.reset();
            }
            else
            {
                LoadingView::append( _("Loading Game Data ...") );

                CLIENT->sendMessage( &client_game_setup_ping, sizeof(ConnectMesgClientGameSetupPing));

                connection_state = _connect_state_setup_client_game;
            }
        }

        break;

    case _connect_state_setup_client_game :
    {
        char str_buf[128];
        int percent_complete;

        if ( GameManager::clientGameSetup( &percent_complete ) == false )
        {
            ConnectMesgClientGameSetupAck client_game_setup_ack;

            sprintf( str_buf, "%s (%d%%)", _("Loading Game Data ..."), percent_complete);
            LoadingView::update( str_buf );

            CLIENT->sendMessage( &client_game_setup_ack, sizeof(ConnectMesgClientGameSetupAck));
            connection_state = _connect_state_sync_profiles;
        }
        else
        {
            ConnectMesgClientGameSetupPing client_game_setup_ping;

            sprintf( str_buf, "%s (%d%%)",_("Loading Game Data ..."), percent_complete);
            LoadingView::update( str_buf );
            CLIENT->sendMessage( &client_game_setup_ping, sizeof(ConnectMesgClientGameSetupPing));
        }
    }
    break;


    case _connect_state_sync_profiles :
    {
        // nothing, will change when sync complete
    }
    break;
    case _connect_state_connect_failure :
    {
        if ( failure_display_timer.count() == true )
        {
            ActionManager::runAction("loaderror");
            connection_state = _connect_state_idle;
        }
    }
    break;

    } // ** switch

}

void ClientConnectDaemon::connectProcess(const NetMessage* message)
{
    connectFsm( message );
}

void ClientConnectDaemon::connectProcess()
{
    connectFsm( 0 );
}
