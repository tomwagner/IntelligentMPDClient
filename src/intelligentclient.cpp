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


void updatePlayer(MPD::Client *, MPD::StatusChanges changed, void *) {

  if (changed.SongID) {
    if (MPD::Client::GetInstance()->isPlaying()) {
#if DEBUG
      std::cout << "Song changed" << std::endl;
#endif

      MPD::Song song = MPD::Client::GetInstance()->GetCurrentSong();

      // set song parameters to storage
      Storage::GetInstance()->loadNewSong(song);

      AgentManager::GetInstance()->runAgents();

      // set text widgets
      GUI::MainWindow::GetInstance()->setSongLabel(song.GetArtist() + " - " + song.GetTitle());
      GUI::MainWindow::GetInstance()->setArtist(song.GetArtist());
      GUI::MainWindow::GetInstance()->setTitle(song.GetTitle());
      GUI::MainWindow::GetInstance()->setAlbum(song.GetAlbum());
      GUI::MainWindow::GetInstance()->setGenre(song.GetGenre());
      GUI::MainWindow::GetInstance()->setTimeScale(MPD::Client::GetInstance()->GetElapsedTime(), MPD::Client::GetInstance()->GetTotalTime());
      GUI::MainWindow::GetInstance()->setStatusBar(_("IMPC Playing: ") + song.GetFile());
    }
  } else if (changed.ElapsedTime) {
    if (!Config::GetInstance()->isAgentsEnabled()){
      AgentManager::GetInstance()->killAgents();
    }
  }



}


IntelligentClient::IntelligentClient(int argc, char** argv) {
  std::cout << "ic" << std::endl;

  // connect to MPD server
  MPD::Client::GetInstance()->SetHostname(Config::GetInstance()->getHost());
  MPD::Client::GetInstance()->SetPassword(Config::GetInstance()->getPassword());
  MPD::Client::GetInstance()->SetPort(Config::GetInstance()->getPort());
  MPD::Client::GetInstance()->Connect();

  MPD::Client::GetInstance()->SetStatusUpdater(updatePlayer, 0);

  // check if we are connected
  if (MPD::Client::GetInstance()->Connected()) {
    GUI::MainWindow::GetInstance()->setStatusBar(_("MPD connected: ") + Config::GetInstance()->getHost() + ":" + utils::intToString(Config::GetInstance()->getPort()));
  } else {
    GUI::MainWindow::GetInstance()->setStatusBar(_("Error while connecting to MPD server"));
    GUI::MainWindow::GetInstance()->showErrorDialog(_("Error in connecting to MPD"));
  }

#if ENABLE_GUI
  // run main window
  kit.run(*GUI::MainWindow::GetInstance()->getWindow());
#endif
}


IntelligentClient::~IntelligentClient() {

#if ENABLE_GUI
  delete GUI::MainWindow::GetInstance();
#endif
  delete AgentManager::GetInstance();
  delete Config::GetInstance();
  delete Storage::GetInstance();
  delete MPD::Client::GetInstance();
}


bool IntelligentClient::updateStatus() {
  MPD::Client::GetInstance()->UpdateStatus();
  return true;
}

