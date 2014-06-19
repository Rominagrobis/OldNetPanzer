/*
Copyright (C) 2004 by Matthias Braun <matze@braunis.de>

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


#include <unistd.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "Classes/Network/NetPacket.hpp"
#include "Classes/Network/UnitNetMessage.hpp"
#include "Units/UnitOpcodes.hpp"

#include "NetPacketDebugger.hpp"
#include "Classes/OpcodeDebugger.hpp"
#include "ConnectNetMessage.hpp"

void NetPacketDebugger::logPacket(const char* domain, const NetPacket* packet)
{
    logMessage(domain, packet->getNetMessage());
}

void NetPacketDebugger::logMessage(const char* domain, const NetMessage* message)
{
    static std::ofstream* log = 0;
    if(log == 0) {
        std::stringstream filename;
        filename << "packets_" << getpid() << ".txt";
        log = new std::ofstream(filename.str().c_str());
    }

    *log << domain << ": (" << (int) message->message_class << "/"
                   << (int) message->message_id << ") ";
    switch(message->message_class) {
        case _net_message_class_multi:
        {
            logMultiMessage(*log, message);
            break;
        }
        case _net_message_class_system:
        {
            *log << "System";
            break;
        }
        case _net_message_class_connect:
        {
            logConnectMessage(*log, message);
            break;
        }
        case _net_message_class_player:
        {
            *log << "Player";
            break;
        }
        case _net_message_class_unit:
        {
            logUnitMessage(*log, message);
            break;
        }
        case _net_message_class_player_commands:
        {
            logTerminalMessage(*log, message);
            break;
        }
        case _net_message_class_objective:
        {
            *log << "Objective";
            break;
        }
        case _net_message_class_game_control:
        {
            *log << "Game Control";
            break;
        }
        case _net_message_class_powerup:
        {
            *log << "Powerup";
            break;
        }
        default:
        {
            *log << "?";
            break;
        }
    }
    *log << std::endl;

    log->flags(std::ios::hex);
    log->fill('0');
    log->flags(std::ios::dec);
    *log << std::endl;
}

void NetPacketDebugger::logMultiMessage(std::ostream& log,
        const NetMessage* message)
{
    log << "multimessage:\n";
}

void NetPacketDebugger::logConnectMessage(std::ostream& log,
        const NetMessage* message)
{
    log << "Connect ";
    switch(message->message_id) {
        case _net_message_id_connect_join_game_request:
            log << "(join game request)";
            break;
        case _net_message_id_connect_join_game_request_ack:
            log << "(join game request ack)";
            break;
        case _net_message_id_client_start_connect:
            log << "(client start connect)";
            break;
        case _net_message_id_client_connect_request:
            log << "(client connect request)";
            break;
        case _net_message_id_client_connect_result:
            log << "(client connect result)";
            break;
        case _net_message_id_connect_client_settings:
            log << "(client settings)";
            break;
        case _net_message_id_client_connect_process_update:
            log << "(client connect process update)";
            break;
        case _net_message_id_client_connect_process_state_mesg:
            log << "(client connect process state message)";
            break;
        case _net_message_id_connect_server_game_setup:
            log << "(server game setup)";
            break;
        case _net_message_id_connect_client_game_setup_ping:
            log << "(client game setup ping)";
            break;
        case _net_message_id_connect_client_game_setup_ack:
            log << "(client game setup ack)";
            break;
        case _net_message_id_connect_client_abort:
            log << "(client abort)";
            break;
        case _net_message_id_connect_netPanzer_client_disconnect:
            log << "(netpnatzer client disconnect)";
            break;
        case _net_message_id_connect_netPanzer_server_disconnect:
            log << "(netpanzer server disconnect)";
            break;
        default:
            log << "(invalid messageid)";
            break;
    }
}

void NetPacketDebugger::logTerminalMessage(std::ostream& log,
        const NetMessage* message)
{
    log << "Terminal/";
    switch(message->message_id) {
        default:
        {
            log << "?";
            break;
        }
    }
}

void NetPacketDebugger::logUnitMessage(std::ostream& log, const NetMessage* message)
{
    log << "Unit/";
    switch(message->message_id) {
        case _net_message_id_opcode_mesg:
        {
            log << "Opcode";
            logUnitOpcodeMessage(log, message);
            break;
        }
        case _net_message_id_ini_sync_mesg:
        {
            log << "IniSync";
            break;
        }
        case _net_message_id_destroy_unit:
        {
            log << "Destroy";
            break;
        }
        case _net_message_id_create_unit:
        {
            log << "Create";
            break;
        }
        case _net_message_id_unit_sync_integrity_check:
        {
            log << "Sync";
            break;
        }
        default:
        {
            log << "?";
            break;
        }
    }
}

void NetPacketDebugger::logUnitOpcodeMessage(std::ostream& log,
        const NetMessage* message)
{
//    UnitOpcodeMessage* opcodes = (UnitOpcodeMessage*) message;
//    Uint8* dataptr = opcodes->data;
//    Uint8* dataend
//        = dataptr + (message->getSize() - UnitOpcodeMessage::getHeaderSize());
//    while(dataptr < dataend) {
//        log << "\n  ";
//        OpcodeDebugger::logOpcode(log, (UnitOpcode*) dataptr);
//        dataptr += sizeof(UnitOpcodeStruct);
//    }
}

