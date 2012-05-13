/**
 * Copyright (C) 2012 Tomas Wagner <xwagne01@stud.fit.vutbr.cz>
 *
 * This file is part of impc (Intelligent Music Player Client).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rrv.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file agentmanager.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Manager for agents. It runs one agent for one source.
 */

#include "agentmanager.h"

#include <iostream>
//#define DISABLE_AGENTS 0


AgentManager::AgentManager(bool status) : DISABLE_AGENTS(status) {
  // init parser
  xmlInitParser();
}


AgentManager::~AgentManager() {
  // kill agents
  killAgents();

  xmlCleanupParser();
}


void AgentManager::runAgents() {

  if (DISABLE_AGENTS) return;

  // if there is running agents, we kill them
  if (!agentList.empty()) killAgents();

  // load sources list
  sourcesList = clientSettings->getSourcesList();
  std::list<source*>::iterator it;


  // client not playing, we cant search
  if (!clientMPD.Connected()) {
    std::cout << "We are not connected to MPD! AGENT EXIT" << std::endl;
    return;
  }
  clientMPD.UpdateStatus();
  if (!clientMPD.isPlaying()) {
    std::cout << "We are not playing MPD! AGENT EXIT" << std::endl;
    return;
  }
  
  // get now playing song
  clientMPD.UpdateStatus();
  MPD::Song s = clientMPD.GetCurrentSong();

  // create Agents for searching content
  for (it = sourcesList.begin(); it != sourcesList.end(); it++) {
    // if source is active
    if ((*it)->active) {
      Agent * jb007 = new Agent((*it)->url, s);
      agentList.push_back(jb007);
    }
  }
}


/**
 * Method kill all managed agents.
 */
void AgentManager::killAgents() {
  // we kill all created agents
  std::list<Agent *>::iterator it;
  for (it = agentList.begin(); it != agentList.end(); it++) {
    delete (*it);
  }


  // clear list
  agentList.clear();
}





