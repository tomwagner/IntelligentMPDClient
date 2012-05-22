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

AgentManager* AgentManager::instance = NULL;


AgentManager::AgentManager(bool status) : m_disabled(status),
m_sourcesCount(0) {
  // init parser
  xmlInitParser();

  m_agentsEnabled = Config::GetInstance()->isAgentsEnabled();
}


AgentManager::~AgentManager() {
  // kill agents
  killAgents();

  xmlCleanupParser();
}

/**
 * Method checks if agents enabled
 * @return 
 */
bool AgentManager::checkIfAgentsEnabled() {
  if (Config::GetInstance()->isAgentsEnabled()) {
    if (m_agentsEnabled == false) {
      m_agentsEnabled = true;
      if (!agentList.empty()) killAgents();
      runAgents();
    }

    return true;
  } else {
    if (m_agentsEnabled == true) {
      m_agentsEnabled = false;
      killAgents();
    }
    return false;
  }
}


void AgentManager::songChanged() {
  // if there is running agents, we kill them
  if (!agentList.empty()) killAgents();

  runAgents();
}


void AgentManager::isSourcesChanged() {
  if (!m_agentsEnabled) return;

  unsigned sourcesCount = sourcesList.size();

  // if number of sources no changed return
  if (m_sourcesCount != sourcesCount) {
    killAgents();
    runAgents();
    m_sourcesCount = sourcesCount;
  }
}


void AgentManager::runAgents() {
  if (!m_agentsEnabled) return;

  // load sources list
  sourcesList = Config::GetInstance()->getSourcesList();
  std::list<source*>::iterator it;


  // client not playing, we cant search
  if (!MPD::Client::GetInstance()->Connected()) {
    std::cout << "We are not connected to MPD! AGENT EXIT" << std::endl;
    return;
  }
  MPD::Client::GetInstance()->UpdateStatus();
  if (!MPD::Client::GetInstance()->isPlaying()) {
    std::cout << "We are not playing MPD! AGENT EXIT" << std::endl;
    return;
  }

  // get now playing song
  MPD::Client::GetInstance()->UpdateStatus();
  MPD::Song s = MPD::Client::GetInstance()->GetCurrentSong();

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





