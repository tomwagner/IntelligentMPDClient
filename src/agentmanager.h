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
 * @file agentmanager.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Manager for agents. It runs one agent for one source.
 */

#ifndef AGENTMANAGER_H
#define	AGENTMANAGER_H
#include "agent.h"
#include "clientsettings.h"
#include "mpdclient.h"
#include "thread.h"
#include <list>

class AgentManager {
private:
  static AgentManager * instance;
public:

  static AgentManager *GetInstance() {
    if (instance == NULL) {
      instance = new AgentManager(false);
    }
    return instance;
  }

  void songChanged();
  void isSourcesChanged();
  bool checkIfAgentsEnabled();

  virtual ~AgentManager();

private:
  AgentManager(bool);
  unsigned m_sourcesCount;
  bool m_agentsEnabled;
  bool m_disabled;
  xmlMutexPtr mutex;

  void runAgents();
  void killAgents();

  std::list<source*> sourcesList;
  std::list<Agent *> agentList;
};

#endif	/* AGENTMANAGER_H */

