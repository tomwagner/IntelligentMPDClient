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
 * @file clientsetting.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Object contains the base setting of intelligent client. It provides
 * loading and saving from JSON configuration files.
 */

#include "clientsettings.h"
#include <cassert>
#define DEBUG 0

ClientSettings * clientSettings = new ClientSettings;



const std::string DEFAULTHOME = "/.impc/";


ClientSettings::ClientSettings() :
m_host("localhost"),
m_port(6600),
m_password(""),
m_confPath(""),
m_tempPath(""),
m_home(""),
DEFAULTCONFIGFILEPATH(""),
DEFAULTSOURCESFILEPATH("") {

  // set home folder (ends with /)
//  m_home += getenv("HOME");
//  m_home += "/.impc/";

  // set config and temp path
  m_confPath = m_home + "config/"; 
  m_tempPath = m_home + "tmp/";


  // set config files
  DEFAULTCONFIGFILEPATH = m_home + "config/settings.json";
  DEFAULTSOURCESFILEPATH = m_home + "config/sources.json";
  
  // load client settings
  loadClientSettings();
}


ClientSettings::~ClientSettings() {
  // save before exit
  save();

  // clean
  initSourcesList();
}


void ClientSettings::setHost(std::string host) {
  m_host = host;
}


void ClientSettings::setPort(int port) {
  m_port = port;
}


void ClientSettings::setPassword(std::string password) {
  m_password = password;
}


void ClientSettings::setConfPath(std::string path) {
  m_confPath = path;
}


void ClientSettings::setTempPath(std::string path) {
  m_tempPath = path;
}


std::string ClientSettings::getHost() const {
  return m_host;
}


int ClientSettings::getPort() const {
  return m_port;
}


std::string ClientSettings::getPassword() const {
  return m_password;
}


std::string ClientSettings::getConfPath() const {
  return m_confPath;
}


std::string ClientSettings::getTempPath() const {
  return m_tempPath;
}


void ClientSettings::loadSettings() {
  Json::Value root;
  Json::Reader reader;

  std::string content;
  std::string line;
  std::ifstream in(DEFAULTCONFIGFILEPATH.c_str());
  while (std::getline(in, line))
    content += line;
  in.close();

  bool parsingSuccessful = reader.parse(content, root);

  if (!parsingSuccessful) {
    std::cout << "Error in parsing settings.json file" << reader.getFormatedErrorMessages();
  }

  // projdeme všechny položky a uložíme nastavení do objektu
  if (root.isMember("settings")) {
    const Json::Value settings = root["settings"];

    m_host = settings["host"].asString();
    m_port = settings["port"].asInt();
    m_password = settings["password"].asString();
//    m_confPath = settings["confPath"].asString();
//    m_tempPath = settings["tempPath"].asString();
  }

}


void ClientSettings::saveSettings() {
  Json::Value root;
  Json::Value row;
  Json::StyledWriter writer;
  std::ofstream of(DEFAULTCONFIGFILEPATH.c_str());

  if (!of.is_open()) {
    std::cerr << "Cannot open settings file for saving." << std::endl;
  }


  // projdeme všechny položky objektu a uložíme je
  row["host"] = m_host;
  row["port"] = m_port;
  row["password"] = m_password;

  row["tempPath"] = m_tempPath;
  row["confPath"] = m_confPath;


  root["settings"] = row;

#if DEBUG
  std::cout << writer.write(root);
#endif
  // zapíšeme
  of << root.toStyledString();

  // uzavřeme soubor
  of.close();

}


void ClientSettings::loadSources() {
  Json::Value root;
  Json::Reader reader;

  std::string content;
  std::string line;
  std::ifstream in(DEFAULTSOURCESFILEPATH.c_str());
  while (std::getline(in, line))
    content += line;
  in.close();


  bool parsingSuccessful = reader.parse(content, root);

  if (!parsingSuccessful) {
    std::cout << "Error in parsing sources.json file" << reader.getFormatedErrorMessages();
  }

  // projdeme všechny zdroje a uložíme nastavení do widgetu
  if (root.isMember("sources")) {
    const Json::Value plugins = root["sources"];
    for (unsigned int index = 0; index < plugins.size(); ++index) {

      // pokud máme správné hodnoty, parsujeme dál
      if ((plugins[index].isMember("active")) && (plugins[index].isMember("url"))) {
        // uložíme zdroje do widgetu
        source * row = new source;
        row->active = plugins[index]["active"].asBool();
        row->url = plugins[index]["url"].asString();
        sourcesList.push_back(row);
      }
    }
  }
}


void ClientSettings::saveSources() {

  Json::Value root;
  Json::Value row;
  Json::StyledWriter writer;
  std::ofstream of(DEFAULTSOURCESFILEPATH.c_str());

  if (!of.is_open()) {
    std::cerr << "Cannot open source file for saving." << std::endl;
    return;
  }

  // projdeme všechny záznamy a uložíme je
  std::list<source*>::iterator it;
  for (it = sourcesList.begin(); it != sourcesList.end(); it++) {

    row["active"] = (*it)->active;
    row["url"] = (*it)->url;
    root["sources"].append(row);
  }
#if DEBUG
  std::cout << writer.write(root);
#endif
  // zapíšeme
  of << root.toStyledString();

  // uzavřeme soubor
  of.close();
}


void ClientSettings::loadClientSettings() {
  // načtu nastavení programu        
  loadSettings();
  // načtu zdroje
  loadSources();

}


void ClientSettings::save() {
  //uložím nastavení
  saveSettings();
  // uložím zdroje
  saveSources();

}


void ClientSettings::initSourcesList() {
  std::list<source*>::iterator it;
  for (it = sourcesList.begin(); it != sourcesList.end(); it++) {
    delete *it;
  }
  sourcesList.clear();
}


std::list<source*>& ClientSettings::getSourcesList() {
  return sourcesList;
}


void ClientSettings::insertSource(bool active, std::string url) {
  source * row = new source;
  row->active = active;
  row->url = url;
  sourcesList.push_back(row);
}