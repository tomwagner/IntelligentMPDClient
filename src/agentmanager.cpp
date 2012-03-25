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
#define DISABLE_AGENTS 1


AgentManager::AgentManager(MPD::Client * client, ClientSettings * settings) :
clientMPD(client),
clientSettings(settings) {



  if (DISABLE_AGENTS) return;
  //std::cout << "AgentManager vytvořen" << std::endl;

  // we create new agent 
  std::list<source*> sourcesList = clientSettings->getSourcesList();
  std::list<source*>::iterator it;
  for (it = sourcesList.begin(); it != sourcesList.end(); it++) {
    // if source is active
    if ((*it)->active) {
      Agent * jb007 = new Agent(clientMPD, clientSettings, (*it)->url);
      agentList.push_back(jb007);
      jb007->run();
    }
  }
}


AgentManager::~AgentManager() {

  // we kill all created agents
  std::list<Agent *>::iterator it;
  for (it = agentList.begin(); it != agentList.end(); it++) {
    delete (*it);
  }
}

//****************TODO*************
void AgentManager::playerEvent() {
//  // song changed, we change search keywords for Agents
//  std::list<Agent *>::iterator it;
//  for (it = agentList.begin(); it != agentList.end(); it++) {
//    (*it).changeSearch();
//  }
}

