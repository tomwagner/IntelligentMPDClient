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
 * @file storage.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Ccreating temporary data storage for saving downloaded files.
 */

#ifndef STORAGE_H
#define	STORAGE_H

#include "clientsettings.h"
#include "mpdclient.h"
#include "utils.h"
#include "http.h"
#include "sartist.h"
#include "bayesclassificator.h"


using namespace NaiveBayes;

namespace DataStorage {

  typedef struct {
    SText name;
    //    std::list<std::string> articles;
    //    std::list<std::string> relatedVideos;
    //    std::list<std::string> about;
  } STrack;

  class Storage {
  public:
    Storage();
    virtual ~Storage();

    void loadNewSong(MPD::Song s);

    SArtist * currentArtist;
    STrack * currentTrack;

  private:
    HTTP * http;
    Json::Value record;

    // private structures
    std::string filePath;


    std::string localStorageDir;
    std::string remoteStorageURL;

    void clearStorage();


    bool fileExists(const char *filename);
    void loadArtist(const std::string& filename);

    std::string remoteLoadArtist(std::string artist);
    bool remoteSaveArtist(std::string filename);

    std::string createArtistFilePath(std::string artist);
    std::string createFilePath(std::string artist, std::string title);

  };

  extern Storage storage;
}
#endif	/* STORAGE_H */

