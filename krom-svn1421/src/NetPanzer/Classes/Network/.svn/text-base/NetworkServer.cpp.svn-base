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

#include <algorithm>

#include "NetMessage.hpp"
#include "Core/CoreTypes.hpp"

#include "Util/Log.hpp"
#include "Classes/Network/NetworkServer.hpp"

#include "Classes/Network/ServerConnectDaemon.hpp"
#include "Classes/Network/NetworkState.hpp"

#include "Interfaces/PlayerInterface.hpp"
#include "Interfaces/ConsoleInterface.hpp"
#include "Interfaces/GameConfig.hpp"
#include "Interfaces/TeamManager.hpp"

#include "Objectives/ObjectiveInterface.hpp"
#include "SystemNetMessage.hpp"
#include "ConnectNetMessage.hpp"

#include "Network/TCPSocket.hpp"
#include "Core/NetworkGlobals.hpp"

NetworkServer* SERVER = 0;

NetworkServer::NetworkServer()
        : NetworkInterface(), socket(0)
{
    // nothing
}

NetworkServer::~NetworkServer()
{
    if ( socket )
        socket->destroy();
}

void NetworkServer::resetClientList()
{
    ClientList::iterator i = client_list.begin(),
                         e = client_list.end();
    
    while ( i != e )
    {
        delete *i;
        i++;
    }
    
    client_list.clear();
}

void NetworkServer::cleanUpClientList()
{
    ClientList::iterator i = client_list.begin(),
                         e = client_list.end();
    
    while ( i != e )
    {
        if( ! (*i)->isConnected() )
        {
            delete (*i);
            i = client_list.erase(i);
            continue;
        }
        i++;
    }
}

bool NetworkServer::addClientToSendList( ClientSocket * client )
{
    client_list.push_back(client);
    return true;
}

void
NetworkServer::openSession()
{
    // nothing
}

void
NetworkServer::hostSession()
{
    if ( socket )
    {
        socket->destroy();
        socket = 0;
    }

    resetClientList();
    
    try
    {
        Address addr = Address::resolve(*GameConfig::server_bindaddress,
                                        GameConfig::server_port,
                                        true, true ); // tcp for binding
        
        socket = new TCPListenSocket(addr, this);
        
    }
    catch(...)
    {
        if (socket)
            socket->destroy();
        socket = 0;
        throw;
    }
}

void
NetworkServer::closeSession()
{
    if ( socket )
        socket->destroy();
    
    socket = 0;
}

void
NetworkServer::broadcastMessage(const NetMessage *message, size_t size)
{
    ClientList::iterator i = client_list.begin(),
                         e = client_list.end();
    
    while ( i != e )
    {
        (*i)->sendMessage( message, size);
        
        i++;
    }
}

void
NetworkServer::sendMessage(const PlayerID player_index, NetMessage* message,
        size_t size)
{
    if( socket == 0 )
        return;
    
    ClientList::iterator i = client_list.begin(),
                         e = client_list.end();
    
    while ( i != e )
    {
        if ( (*i)->getPlayerIndex() == player_index )
        {
            (*i)->sendMessage( message, size);

            NetworkState::incPacketsSent(size);
            return; // premature exit
        }
        i++;
    }

    // if didn't found player we are here.
    LOGGER.warning("NetworkServer: sendMessage to unknown client: (index) %d", player_index);
}

bool
NetworkServer::getPacket(NetPacket* packet)
{
    if (receive_queue.isReady())
    {
        receive_queue.dequeue(packet);
        NetworkState::incPacketsReceived(packet->getSize());

#ifdef NETWORKDEBUG
        NetPacketDebugger::logPacket("R", packet);
#endif
        return true;
    }

    return false;
}

void
NetworkServer::niceDisconnect(ClientSocket * client)
{
    ClientList::iterator i = client_list.begin(),
                         e = client_list.end();
    
    while ( i != e && *i != client )
    {
        ++i;
    }

    if ( i != client_list.end() )
    {
        // XXX hack
        (*i)->hardClose();
        onClientDisconected(client, 0);
    }
}

void
NetworkServer::kickClient(const PlayerID index)
{
    ClientList::iterator i = client_list.begin(),
                         e = client_list.end();
    
    while ( i != e )
    {
        if ( (*i)->getPlayerIndex() == index )
        {
            // XXX hack
            PlayerState *p = PlayerInterface::getPlayer(index);
            if ( p )
            {
                p->setStateKicked();
            }

            (*i)->hardClose();
            onClientDisconected(*i, "kicked");
            return;
        }
        i++;
    }
}

void
NetworkServer::sendRemaining()
{
    ClientList::iterator i = client_list.begin(),
                         e = client_list.end();
    
    while ( i != e )
    {
        (*i)->sendRemaining();
        i++;
    }
}

bool
NetworkServer::isAlreadyConnected( ClientSocket * client )
{
    if ( GameConfig::game_allowmultiip )
    {
        return false;
    }

    ClientList::iterator i = client_list.begin(),
                         e = client_list.end();
    
    while ( i != e )
    {
        if ( (*i)->getIPAddress() == client->getIPAddress() )
        {
            return true;
        }
        i++;
    }

    return false;
}

std::string
NetworkServer::getIP(const PlayerID player_index)
{
    ClientList::iterator i = client_list.begin(),
                         e = client_list.end();
    
    while ( i != e )
    {
        if ( (*i)->getPlayerIndex() == player_index )
        {
            return (*i)->getFullIPAddress();
        }
        
        i++;
    }

    return "Not a client";
}

void
NetworkServer::onSocketError(TCPListenSocket *so)
{
    (void)so;
    LOGGER.warning("NetworkServer: Listen Socket error, something bad could happen from now");

}

TCPSocketObserver *
NetworkServer::onNewConnection(TCPListenSocket *so, const Address &fromaddr)
{
    (void)so;
    (void)fromaddr;
    return new ClientSocket(this);
}

void
NetworkServer::onClientConnected(ClientSocket *s)
{
    LOGGER.debug("NetworkServer: client connected [%d]", s->getId());
}

void
NetworkServer::onClientDisconected(ClientSocket *s, const char * msg)
{
    LOGGER.debug("NetworkServer::onClientDisconected( %d, '%s')", s->getId(), msg ? msg : "nice");
    
    bool cleandisconnect = false;
    bool sendalert = true;
    
    if ( ServerConnectDaemon::inConnectQueue(s) )
    {
        // player was connecting and dropped.
        ServerConnectDaemon::removeClientFromQueue(s);
        sendalert = false;
        LOGGER.debug("NetworkServer::onClientDisconected player was connecting");
    }
    
    if ( NetworkInterface::receive_queue.isReady() )
    {
        unsigned long frontsave = NetworkInterface::receive_queue.front;
        while ( NetworkInterface::receive_queue.isReady() )
        {
            LOGGER.debug("NetworkServer::onClientDisconected there was a packet");
            NetPacket packet;

            NetworkInterface::receive_queue.dequeue(&packet);

            if ( packet.fromClient == s )
            {
                LOGGER.debug("NetworkServer::onClientDisconected the packet was from our friend");
                const NetMessage * netmessage = packet.getNetMessage();
                if (   netmessage->message_class == _net_message_class_connect 
                    && netmessage->message_id == _net_message_id_connect_netPanzer_client_disconnect )
                {
                    LOGGER.debug("NetworkServer::onClientDisconected so is a clean disconnect");
                    cleandisconnect = true;
                }
            }
        }
        NetworkInterface::receive_queue.front = frontsave;
    }
    
    PlayerID player_index = s->getPlayerIndex();
    
    if ( player_index != INVALID_PLAYER_ID )
    {
        PlayerState * player = PlayerInterface::getPlayer(player_index);
        bool kicked = player ? player->isKicked() : false;

        if ( player && sendalert && ! kicked)
        {
            if (GameConfig::game_teammode == true)
            ConsoleInterface::postMessage(Color::cyan, true, player->getFlag(),
                                      "'%s' has left.",
                                      player->getName().c_str());
        }

        ObjectiveInterface::disownPlayerObjectives( player_index );
        UnitInterface::destroyPlayerUnits( player_index );
        PlayerInterface::disconnectPlayerCleanup( player_index );    

        if ( sendalert )
        {
            SystemConnectAlert scmsg;
            if ( cleandisconnect || ! msg )
            {
                scmsg.set( player_index, _connect_alert_mesg_disconnect);
            }
            else if ( kicked )
            {
                scmsg.set( player_index, _connect_alert_mesg_client_kicked);
            }
            else
            {
                scmsg.set( player_index, _connect_alert_mesg_client_drop );
            }
            SERVER->broadcastMessage(&scmsg, sizeof(scmsg));
        }
        if (GameConfig::game_teammode == true)
        {
            TeamManager::removePlayer(player->getID(), player->getTeamID());
            TeamManager::BalancedTeam();
        }
    }
}
