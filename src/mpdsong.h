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
 * @file mpdsong.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Data structure for song. Provides information about song.
 */
#ifndef MPDSONG_H
#define	MPDSONG_H
#include "global.h"
#include <mpd/client.h>
#include <sstream>
#include <cmath>
namespace MPD {

  class Song {
  public:
    Song();
    virtual ~Song();
    std::string getArtistAndSongName() const;
    std::string artist() const;
    std::string title() const;
    std::string album() const;
    std::string genre() const;
    std::string uri() const;
    unsigned getDuration() const;
    std::string getDurationInString() const;
    void set(struct mpd_song* MPDsong_o);
  private:
    struct mpd_song* MPDsong;
  };
}
#endif	/* MPDSONG_H */

