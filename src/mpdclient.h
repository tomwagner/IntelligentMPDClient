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
 * @file mpdclient.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Provides connection managment to MPD server.
 */


#ifndef MPDCLIENT_H
#define	MPDCLIENT_H
#include "global.h"
#include "mpdsong.h"
#include "mpdlistener.h"
#include <vector>
#include <mpd/connection.h>
#include <mpd/client.h>
#include <pthread.h>


namespace MPD {
  class Song;
  class Listener;

  class Client {
  public:
    Client();
    virtual ~Client();

    bool connect(const std::string host, unsigned int port = 6600, const std::string password = "");
    void disconnect();
    bool isConnected();

    mpd_state getClientState();
    std::string getErrorMessage();

    void play();
    void pause();
    void stop();
    int getVolume();

    void nextSong();
    void previousSong();


    void getCurrentSong(Song& song);
    void addListener(MPD::Listener& listener);

  private:
    pthread_t m_thread;
    bool m_listening;
    struct mpd_connection* m_conn;
    struct mpd_connection* m_IdleConn;
    struct mpd_status* m_status;

    // pole posluchačů událostí
    std::vector<Listener*> m_Listeners;

    void startListenIdleMessages();
    void stopListenIdleMessages();
    static void* runListeningIdles(void* data);
    void *runListeningIdles();
  };
}
#endif	/* MPDCLIENT_H */

