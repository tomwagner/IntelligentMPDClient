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
 * @file agent.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Web Agent object.
 */

#ifndef AGENT_H
#define	AGENT_H

#include "global.h"
#include "ClientException.h"
#include "clientsettings.h"
#include "webpage.h"
#include "utils.h"
#include "http.h"
#include "mpdclient.h"
#include <list>

class Agent {
public:
  Agent(MPD::Client * client, ClientSettings * settings, std::string url);
  virtual ~Agent();

  void run();
  bool changeSearch();

private:
  MPD::Client * clientMPD;
  ClientSettings * clientSettings;
  utils::HTTP * http;
  Webpage * webpage;

  // base
  std::string baseUrl;
  std::string actualUrl;
  std::string sourceHTML;

  std::list<searchPair *> searchForms;


  bool detectIfIsValidPage();


};

#endif	/* AGENT_H */

