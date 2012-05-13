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
#include <cassert>

#define DEBUG 1
#define ENABLE_GUI 1

AgentManager * agentManager;


void updatePlayer(MPD::Client *, MPD::StatusChanges changed, void *) {

  if (changed.SongID) { //changed.PlayerState || 
    if (clientMPD.isPlaying()) {
#if DEBUG
      std::cout << "Song changed" << std::endl;
#endif

      MPD::Song song = clientMPD.GetCurrentSong();

      // set song parameters to storage
      storage.loadNewSong(song);


      agentManager->killAgents();
      agentManager->runAgents();

      // set text widgets
      gui->setSongLabel(song.GetArtist() + " - " + song.GetTitle());
      gui->setArtist(song.GetArtist());
      gui->setTitle(song.GetTitle());
      gui->setAlbum(song.GetAlbum());
      gui->setGenre(song.GetGenre());
      gui->setTimeScale(clientMPD.GetElapsedTime(), clientMPD.GetTotalTime());
      gui->setStatusBar(_("IMPC Playing: ") + song.GetFile());
    }
  } else if (changed.Volume) {
  }

}


IntelligentClient::IntelligentClient(int argc, char** argv) {


  // connect to MPD server
  clientMPD.SetHostname(clientSettings->getHost());
  clientMPD.SetPassword(clientSettings->getPassword());
  clientMPD.SetPort(clientSettings->getPort());
  clientMPD.Connect();

  clientMPD.SetStatusUpdater(updatePlayer, 0);

  //  Glib::signal_timeout().connect(
  //          sigc::mem_fun(*this, &IntelligentClient::updateStatus), 1000);


  // init agent manager
  try {
    if (argc >= 2)
      agentManager = new AgentManager(true);
    else
      agentManager = new AgentManager(false);

  } catch (InputException &e) {
    std::cerr << "Input exception:" << e.what() << std::endl;
  }
  // nainicializujeme widgety GUI


  // check if we are connected
  if (clientMPD.Connected()) {
    gui->setStatusBar(_("MPD connected: ") + clientSettings->getHost() + ":" + utils::intToString(clientSettings->getPort()));
    //    mWindow->showInfoDialog(_("MPD Client Successfully Connected"));
    //    std::cout << "playlistLen" << clientMPD.GetPlaylistContent() << std::endl;

  } else {
    gui->setStatusBar(_("Error while connecting to MPD server"));
    gui->showErrorDialog(_("Error in connecting to MPD"));
  }

#if ENABLE_GUI
  // run main window
  kit->run(*gui->getWindow());
#endif
}


IntelligentClient::~IntelligentClient() {
#if ENABLE_GUI
  delete kit;
  delete gui;
#endif
  delete agentManager;
  delete clientSettings;
}


bool IntelligentClient::updateStatus() {
  clientMPD.UpdateStatus();
  return true;
}

