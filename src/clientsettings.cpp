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

Config* Config::instance = NULL;

const std::string DEFAULTHOME = "/.impc/";


Config::Config() :
m_host("localhost"),
m_port(6600),
m_password(""),
m_confPath(""),
m_tempPath(""),
m_home(""),
m_remoteStorage(false),
m_enabledAgents(false) {
#if DEBUG
  std::cout << "clientSettings created" << std::endl;
#endif

  settingsFilename = "settings.json";
  sourceFilename = "sources.json";

  // set config and temp path
  m_confPath = m_home + "config/";
  m_tempPath = m_home + "tmp/";

  // load client settings
  loadConfig();

}


Config::~Config() {
#if DEBUG
  std::cout << "clientSettings destucted" << std::endl;
#endif
  // save before exit
  save();

  // clean
  initSourcesList();
}


void Config::setHost(std::string host) {
  m_host = host;
}


void Config::setPort(int port) {
  m_port = port;
}


void Config::setPassword(std::string password) {
  m_password = password;
}


void Config::setTempPath(std::string path) {
  m_tempPath = path;
}


void Config::setRemoteStorageEnabled(bool b) {
  m_remoteStorage = b;
}


bool Config::isRemoteStorageEnabled() {
  return m_remoteStorage;
}


void Config::setAgentsEnabled(bool b) {
  m_enabledAgents = b;
}


bool Config::isAgentsEnabled() {
  return m_enabledAgents;
}


std::string Config::getHost() const {
  return m_host;
}


int Config::getPort() const {
  return m_port;
}


std::string Config::getPassword() const {
  return m_password;
}


std::string Config::getTempPath() const {
  return m_tempPath;
}


void Config::loadSettings() {
  Json::Value root;
  Json::Reader reader;

  std::string content;
  std::string line;

  std::ifstream in((m_confPath + settingsFilename).c_str());
  while (std::getline(in, line))
    content += line;
  in.close();

  bool parsingSuccessful = reader.parse(content, root);

  if (content.empty()) return;
  if (!parsingSuccessful) {
    std::cout << "Error in parsing settings.json file" << reader.getFormatedErrorMessages();
    return;
  }

  // projdeme všechny položky a uložíme nastavení do objektu
  if (root.isMember("settings")) {
    const Json::Value settings = root["settings"];

    m_host = settings["host"].asString();
    m_port = settings["port"].asInt();
    m_password = settings["password"].asString();

    m_tempPath = settings["tempPath"].asString();

    m_remoteStorage = settings["remote"].asBool();
    m_enabledAgents = settings["enabledAgents"].asBool();
  }
}


void Config::saveSettings() {
  Json::Value root;
  Json::Value row;
  Json::StyledWriter writer;
  std::ofstream of((m_confPath + settingsFilename).c_str());

  if (!of.is_open()) {
    std::cerr << "Cannot open " + settingsFilename + " for saving." << std::endl;
  }


  // projdeme všechny položky objektu a uložíme je
  row["host"] = m_host;
  row["port"] = m_port;
  row["password"] = m_password;

  row["tempPath"] = m_tempPath;
  row["remote"] = m_remoteStorage;
  row["enabledAgents"] = m_enabledAgents;

  root["settings"] = row;

#if DEBUG
  std::cout << writer.write(root);
#endif
  // zapíšeme
  of << root.toStyledString();

  // uzavřeme soubor
  of.close();

}


void Config::loadSources() {
  Json::Value root;
  Json::Reader reader;

  std::string content;
  std::string line;
  std::ifstream in((m_confPath + sourceFilename).c_str());
  while (std::getline(in, line))
    content += line;
  in.close();


  bool parsingSuccessful = reader.parse(content, root);

  if (content.empty()) return;
  if (!parsingSuccessful) {
    std::cout << "Error in parsing " + sourceFilename + " file" << reader.getFormatedErrorMessages();
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


void Config::saveSources() {

  Json::Value root;
  Json::Value row;
  Json::StyledWriter writer;
  std::ofstream of((m_confPath + sourceFilename).c_str());

  if (!of.is_open()) {
    std::cerr << "Cannot open " + sourceFilename + " file for saving." << std::endl;
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


void Config::loadConfig() {
  // načtu nastavení programu        
  loadSettings();
  // načtu zdroje
  loadSources();

}


void Config::save() {
  //uložím nastavení
  saveSettings();
  // uložím zdroje
  saveSources();

}


void Config::initSourcesList() {
  std::list<source*>::iterator it;
  for (it = sourcesList.begin(); it != sourcesList.end(); it++) {
    delete *it;
  }
  sourcesList.clear();
}


std::list<source*>& Config::getSourcesList() {
  return sourcesList;
}


void Config::insertSource(bool active, std::string url) {
  source * row = new source;
  row->active = active;
  row->url = url;
  sourcesList.push_back(row);
}