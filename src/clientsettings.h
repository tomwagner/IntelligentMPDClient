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
 * @file clientsetting.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Object contains the base setting of intelligent client. It provides
 * loading and saving from JSON configuration files.
 */

#ifndef CLIENTSETTINGS_H
#define	CLIENTSETTINGS_H

#include "global.h"
#include <fstream>
#include <jsoncpp/json/json.h>
#include <list>

typedef struct {
  bool active;
  std::string url;
} source;

class ClientSettings {
public:
  ClientSettings();
  virtual ~ClientSettings();

  // get methods
  std::string getHost();
  int getPort();
  std::string getPassword();

  std::string getConfPath();
  std::string getTempPath();


  // set methods
  void setHost(std::string host);
  void setPort(int port);
  void setPassword(std::string password);

  void setConfPath(std::string path);
  void setTempPath(std::string path);

  
  std::list<source*>& getSourcesList();
  void initSourcesList();
  void insertSource(bool active, std::string url);
  
private:

  void saveSources();
  void loadSources();

  void saveSettings();
  void loadSettings();
  
  void save();
  void load();
  


  // client settings variables
  std::string m_host;
  int m_port;
  std::string m_password;

  std::string m_tempPath;
  std::string m_confPath;

  std::list<source*> sourcesList;
};

#endif	/* CLIENTSETTINGS_H */

