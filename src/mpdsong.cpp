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
 * @file mpdsong.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Data structure for song. Provides information about song.
 */

#include "mpdsong.h"

namespace MPD {


  Song::Song()
  : MPDsong(NULL) {
  }


  Song::~Song() {
    if (MPDsong) {
      mpd_song_free(MPDsong);
    }
  }


  /**
   * Metoda nastaví objekt 
   * @param MPDsong_o
   */
  void Song::set(struct mpd_song* MPDsong_o) {
    if (MPDsong) {
      mpd_song_free(MPDsong);
    }

    MPDsong = MPDsong_o;
  }


  std::string Song::getArtistAndSongName() const {
    std::string songFull(this->artist().c_str());
    songFull.append(" - ");
    songFull.append(this->title().c_str());
    return songFull;
  }


  std::string Song::artist() const {
    std::string artist;
    if (MPDsong != NULL) {
      const char * tag = mpd_song_get_tag(MPDsong, MPD_TAG_ARTIST, 0);
      if (tag != NULL) artist.assign(tag);
    }
    return artist;
  }


  std::string Song::title() const {
    std::string title;
    if (MPDsong != NULL) {
      const char * tag = mpd_song_get_tag(MPDsong, MPD_TAG_TITLE, 0);
      if (tag != NULL) title.assign(tag);
    }
    return title;
  }


  std::string Song::album() const {
    std::string album;
    if (MPDsong != NULL) {
      const char * tag = mpd_song_get_tag(MPDsong, MPD_TAG_ALBUM, 0);
      if (tag != NULL) album.assign(tag);
    }

    return album;
  }


  std::string Song::genre() const {
    std::string genre;
    if (MPDsong != NULL) {
      const char * tag = mpd_song_get_tag(MPDsong, MPD_TAG_GENRE, 0);
      if (tag != NULL) genre.assign(tag);
    }

    return genre;
  }


  std::string Song::uri() const {
    std::string uri;
    if (MPDsong != NULL) {
      const char * u = mpd_song_get_uri(MPDsong);
      if (u != NULL) uri.assign(u);
    }

    return uri;
  }


  unsigned Song::getDuration() const {
    unsigned duration = 0;
    if (MPDsong != NULL) {
      duration = mpd_song_get_duration(MPDsong);
    }
    return duration;
  }


  std::string Song::getDurationInString() const {
    std::ostringstream oss;
    std::string duration;
    unsigned d = getDuration();
    int hours = d / 3600;
    int minutes = d / 60;
    int seconds = d % 60;
    
    if (hours != 0) {
      oss << hours << ":";
      if (minutes <= 9) oss << "0";
      oss << minutes << ":";
      // úprava počáteční nuly
      if (seconds <= 9) oss << "0";
      oss << seconds;
    } else {
      oss << minutes << ":";
      // úprava počáteční nuly
      if (seconds <= 9) oss << "0";
      oss << seconds;
    }
    return duration = oss.str();
  }


}