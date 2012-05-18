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


#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <list>
#include <stdlib.h>

typedef struct {
  bool active;
  std::string url;
} source;

class Config {
private:
  static Config * instance;

public:

  static Config *GetInstance() {
    if (instance == NULL) {
      instance = new Config();
    }
    return instance;
  }
  virtual ~Config();

  // get methods
  std::string getHost() const;
  int getPort() const;
  std::string getPassword() const;

  std::string getTempPath() const;


  // set methods
  void setHost(std::string host);
  void setPort(int port);
  void setPassword(std::string password);

  void setTempPath(std::string path);

  void setRemoteStorageEnabled(bool b);
  bool isRemoteStorageEnabled();
  
  void setAgentsEnabled(bool b);
  bool isAgentsEnabled();

  std::list<source*>& getSourcesList();
  void initSourcesList();
  void insertSource(bool active, std::string url);


  void saveSources();
  void saveSettings();
  
private:
  Config();
  std::string m_home;
  std::string settingsFilename;
  std::string sourceFilename;
  std::string DEFAULTSOURCESFILEPATH;

  void save();
  void loadSources();


  void loadSettings();


  void loadConfig();



  // client settings variables
  std::string m_host;
  int m_port;
  std::string m_password;

  std::string m_tempPath;
  std::string m_confPath;

  std::list<source*> sourcesList;
  bool m_remoteStorage;
  bool m_enabledAgents;
};


#endif	/* CLIENTSETTINGS_H */

