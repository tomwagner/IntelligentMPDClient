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
 * @file intelligentclient.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Main object which runs the Intelligent Client.
 */

#include "intelligentclient.h"


IntelligentClient::IntelligentClient(int argc, char** argv) {

  // nainicializujeme objekt nastavení
  clientSettings = new ClientSettings();

  // nainicializujeme objekt pro uchování dat
  clientStorage = new Storage(clientSettings);

  // připojíme se k MPD serveru
  clientMPD = new MPD::Client();
  bool clientConn = clientMPD->connect(clientSettings->getHost(), clientSettings->getPort(), clientSettings->getPassword());

  // nainicializujeme objekt manažera agentů
  clientManager = new AgentManager(clientMPD, clientSettings);

  // nainicializujeme widgety GUI


  // spustíme GUI
  kit = new Gtk::Main();

  GUI::MainWindow * mWindow = new GUI::MainWindow(clientMPD, clientSettings);



  // pokud se klientské spojení nezdařilo, zobrazíme do status baru
  if (clientConn) {
    mWindow->setStatusBarTitle(_("MPD connected: ") + clientSettings->getHost() + ":" + utils::intToString(clientSettings->getPort()));
    //    mWindow->showInfoDialog(_("MPD Client Successfully Connected"));
  } else {
    mWindow->setStatusBarTitle(_("Error while connecting to MPD server"));
    mWindow->showErrorDialog(_("Error in connecting to MPD"));
  }

  // spustíme hlavní okno
  kit->run(*mWindow->getWindow());
}


IntelligentClient::~IntelligentClient() {
  delete kit;
  delete clientMPD;
  delete clientManager;
  delete clientStorage;
  delete clientSettings;
}

