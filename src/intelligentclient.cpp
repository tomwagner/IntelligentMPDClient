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

#define DEBUG 0
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

      // run agents
      AgentManager::GetInstance()->songChanged();

      // set text widgets
      GUI::MainWindow::GetInstance()->setSongLabel(song.GetArtist() + " - " + song.GetTitle());
      GUI::MainWindow::GetInstance()->setArtist(song.GetArtist());
      GUI::MainWindow::GetInstance()->setTitle(song.GetTitle());
      GUI::MainWindow::GetInstance()->setAlbum(song.GetAlbum());
      GUI::MainWindow::GetInstance()->setGenre(song.GetGenre());
      GUI::MainWindow::GetInstance()->setTimeScale(MPD::Client::GetInstance()->GetElapsedTime(), MPD::Client::GetInstance()->GetTotalTime());
    }
  }
  if (changed.ElapsedTime) {

    //    if (!Config::GetInstance()->isAgentsEnabled()) {
    //      AgentManager::GetInstance()->killAgents();
    //    }
    //
    //    AgentManager::GetInstance()->isSourcesChanged();

    // load new info to widgets
    GUI::MainWindow::GetInstance()->articlesWidget->updateArticlesWidget();
    GUI::MainWindow::GetInstance()->slideshowWidget->updateSlideshowWidget();
    //    GUI::MainWindow::GetInstance()->coverWidget->updateCoverWidget();


    GUI::MainWindow::GetInstance()->setTimeScale(MPD::Client::GetInstance()->GetElapsedTime(), MPD::Client::GetInstance()->GetTotalTime());

    if (MPD::Client::GetInstance()->isPlaying()) {
      MPD::Song song = MPD::Client::GetInstance()->GetCurrentSong();

      GUI::MainWindow::GetInstance()->setSongLabel(song.GetArtist() + " - " + song.GetTitle());

      GUI::MainWindow::GetInstance()->setStatusBar(_("IMPC Playing: ") + song.GetFile());
    }
  }
  if (changed.PlayerState) {

    MPD::PlayerState s = MPD::Client::GetInstance()->GetState();

    if (s == MPD::psPlay) {
      GUI::MainWindow::GetInstance()->on_play();
      GUI::MainWindow::GetInstance()->setPlayButtonActive(true);

      GUI::MainWindow::GetInstance()->setBitrate(MPD::Client::GetInstance()->GetBitrate());

    } else if (s == MPD::psPause) {
      GUI::MainWindow::GetInstance()->on_pause();
      GUI::MainWindow::GetInstance()->setPlayButtonActive(false);
    } else if (s == MPD::psStop) {
      GUI::MainWindow::GetInstance()->on_stop();
      GUI::MainWindow::GetInstance()->articlesWidget->clearArticlesWidget();
      GUI::MainWindow::GetInstance()->slideshowWidget->clearSlide();
    }
  }

  if (changed.DBUpdating) {
    GUI::MainWindow::GetInstance()->artistsWidget->reload();
  }

  if (changed.Volume) {
    // we set default volume
    GUI::MainWindow::GetInstance()->setVolume((double) MPD::Client::GetInstance()->GetVolume());
  }

  AgentManager::GetInstance()->checkIfAgentsEnabled();
}


IntelligentClient::IntelligentClient(int argc, char** argv) : isTimeout(false) {
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

  runTimer();

#if ENABLE_GUI
  // run main window
  kit.run(*GUI::MainWindow::GetInstance()->getWindow());
#endif
}


IntelligentClient::~IntelligentClient() {
  stopTimer();

#if ENABLE_GUI
  delete GUI::MainWindow::GetInstance();
#endif
  delete AgentManager::GetInstance();
  delete Storage::GetInstance();
  delete MPD::Client::GetInstance();
  delete Config::GetInstance();
}


bool IntelligentClient::updateStatus() {
  if (MPD::Client::GetInstance()->Connected())
    MPD::Client::GetInstance()->UpdateStatus();

  return true;
}


void IntelligentClient::runTimer() {
  if (isTimeout == true) return;

  updateTimeout = Glib::signal_timeout().connect(
          sigc::mem_fun(*this, &IntelligentClient::updateStatus), 1000);

  isTimeout = true;
}


void IntelligentClient::stopTimer() {
  isTimeout = false;

}
