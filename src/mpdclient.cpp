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
 * @file mpdclient.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Provides connection managment to MPD server.
 */


#include "mpdclient.h"

namespace MPD {


  Client::Client() : m_listening(false) {
  }


  Client::~Client() {
    // zastavíme sledování zpráv z MPD serveru
    stopListenIdleMessages();

    // vyčistíme status
    mpd_status_free(m_status);

    // odpojíme se
    disconnect();

    // sloučíme vlákno pro sledování Idle TODO
    //pthread_join(m_thread, NULL);
    pthread_cancel(m_thread);
  }


  bool Client::connect(const std::string host, unsigned int port, const std::string password) {

    // vytvořím spojení pro zasílání příkazů na server
    m_conn = mpd_connection_new(host.c_str(), port, 0);
    if (m_conn == NULL) {
      std::cerr << mpd_connection_get_error_message(m_conn) << std::endl;
      return false;
    }

    // vytvořím spojení pro příjem Idle příkazů ze serveru
    m_IdleConn = mpd_connection_new(host.c_str(), port, 0);
    if (m_IdleConn == NULL) {
      std::cerr << mpd_connection_get_error_message(m_IdleConn) << std::endl;
      return false;
    }


    // pokud bylo nastaveno heslo, zašleme jej na server
    if (!password.empty()) {
      if (!mpd_run_password(m_conn, password.c_str())) {
        return false;
      }
      // nastavíme heslo i pro vlákno, které naslouchá stavům mpd
      mpd_run_password(m_IdleConn, password.c_str());
    }

    // načteme status mpd serveru
    m_status = mpd_run_status(m_conn);

    // spustíme sledování Idle příkazů z MPD serveru
    startListenIdleMessages();

    return true;
  }


  void Client::disconnect() {
    // odpojíme spojení se serverem
    if (m_conn)
      mpd_connection_free(m_conn);
    if (m_IdleConn)
      mpd_connection_free(m_IdleConn);
  }


  bool Client::isConnected() {
    if (!m_conn)
      return true;
    return false;
  }


  mpd_state Client::getClientState() {
    if (!isConnected())
    return mpd_status_get_state(m_status);
  }


  void Client::play() {
    if (!m_conn) return;
    mpd_run_play(m_conn);
    std::cout << "play" << getErrorMessage() << std::endl;
    std::cout << "finish " << ((mpd_response_finish(m_conn)) ? "OK" : "ERROR") << std::endl;
  }


  void Client::pause() {
    if (!m_conn) return;
    mpd_send_toggle_pause(m_conn);
    std::cout << "pause" << getErrorMessage() << std::endl;
    std::cout << "finish " << ((mpd_response_finish(m_conn)) ? "OK" : "ERROR") << std::endl;
  }


  void Client::stop() {
    if (!m_conn) return;
    mpd_run_stop(m_conn);
    std::cout << "stop" << getErrorMessage() << std::endl;
    std::cout << "finish " << ((mpd_response_finish(m_conn)) ? "OK" : "ERROR") << std::endl;

  }


  void Client::getCurrentSong(Song & song) {
    if (!m_conn) return;
    song.set(mpd_run_current_song(m_conn));

  }


  std::string Client::getErrorMessage() {
    if (!m_conn) return "";
    return mpd_connection_get_error_message(m_conn);
  }


  /**
   * Metoda vytvoří vlákno pro sledování stavů MPD serveru
   */
  void Client::startListenIdleMessages() {
 
    m_listening = true;

    if (pthread_create(&m_thread, NULL, &runListeningIdles, this) != 0) {
      std::cerr << "Chyba při vytváření vlákna posluchače Idle zpráv" << std::endl;
    }
  }


  void * Client::runListeningIdles(void* instance) {
    Client* thread = (Client*) instance;
    return thread->runListeningIdles();
  }


  /**
   * Metoda spustí sledování stavů z MPD serveru
   */
  void * Client::runListeningIdles() {
    if (!m_IdleConn) return (0);

    std::cout << "Vlákno pro poslech spuštěno" << std::endl;

    while (m_listening) {
      // začneme poslouchat zprávy z MPD serveru
      mpd_idle idle = mpd_run_idle(m_IdleConn);

      std::cout << "Mpd event : " << mpd_idle_name(idle) << std::endl;

      switch ((int) idle) {
        case MPD_IDLE_PLAYER:
          // změnil se stav přehrávání (START, STOP, PAUSE)
          for (unsigned int i = 0; i < m_Listeners.size(); i++)
            m_Listeners[i]->playerEvent();
          break;
        case MPD_IDLE_MIXER:
          std::cout << getVolume() << std::endl;
          // změnila se hlasitost
          break;
        case MPD_IDLE_OPTIONS:
          // změnil se status - crossfade, random, repeat
          break;

      }
    }
    std::cout << "Vlákno pro poslech konec" << std::endl;
    return (0);
  }


  /**
   * Metoda zastaví sledování stavů z MPD serveru
   */
  void Client::stopListenIdleMessages() {
    m_listening = false;
  }


  void Client::addListener(MPD::Listener & listener) {
    m_Listeners.push_back(&listener);
  }


  int Client::getVolume() {
    if (!m_conn) return 0;
    return mpd_status_get_volume(m_status);
  }


  void Client::nextSong() {
    if (!m_conn) return;
    mpd_run_next(m_conn);
  }


  void Client::previousSong() {
    if (!m_conn) return;
    mpd_run_previous(m_conn);
  }

}

