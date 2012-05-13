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
 * @file storage.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Ccreating temporary data storage for saving downloaded files.
 */
#include "storage.h"
#include <ostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include "guimainwindow.h"



namespace DataStorage {

  Storage storage = Storage();


  Storage::Storage() : filePath(""), http(new HTTP) {

    std::cout << "storage vytvořen" << std::endl;

    localStorageDir = "tmp/"; //clientSettings->getTempPath();
    remoteStorageURL = "http://www.impc.cz/api/";

    currentArtist = new SArtist;
    currentTrack = new STrack;
  }


  Storage::~Storage() {
    std::cout << "storage destruktor" << std::endl;

    // save current file
    currentArtist->saveArtist(filePath);

    delete currentTrack;
    delete currentArtist;
    delete http;
  }


  void Storage::loadNewSong(MPD::Song s) {
    // save current file
    currentArtist->saveArtist(filePath);

    // clear current Artist
    currentArtist->clear();

    // load new data to storage
    filePath = createArtistFilePath(s.GetArtist());


    currentArtist->name.set(s.GetArtist());
    currentArtist->album.set(s.GetAlbum());

    // remote load artist
    std::string remoteArtistContent = remoteLoadArtist(s.GetArtist());
    currentArtist->loadArtistFromContent(remoteArtistContent);

    // check if file exists
    // search in local temp
    currentArtist->loadArtistFromFile(filePath);


    // set pointers to GUI
    gui->articlesWidget->setArticlesWidget(&currentArtist->articles);
    gui->slideshowWidget->setSlideshowWidget(&currentArtist->images);

    std::map<std::string, std::map<std::string, SAlbum*>* >::iterator it;
    std::cout << "album: " << currentArtist->album.getText() << std::endl;
    std::cout << "hash: " << currentArtist->album.getHash() << std::endl;

//    if (!currentArtist->album.getText().empty()) {
      it = currentArtist->albums.find(currentArtist->album.getHash());
      if (it != currentArtist->albums.end()) {
        std::cout << "storage currentAlbum" << (*it).second->size() << std::endl;
        gui->coverWidget->setCoverWidget((*it).second);
      } else {
        std::cout << "storage currentAlbum not found" << std::endl;
        // current album not found, create new map for album images
        std::map<std::string, SAlbum *>* m = new std::map<std::string, SAlbum *>;
        currentArtist->albums.insert(std::pair<std::string, std::map<std::string, SAlbum*>* >(currentArtist->album.getHash(), m));
        gui->coverWidget->setCoverWidget(&*m);
      }
    }
//  }


  /**
   * Method creates filepath to current playing storage file about artist and
   * current playing song.
   * @param artist
   * @param title
   * @return filepath
   */
  std::string Storage::createFilePath(std::string artist, std::string title) {
    std::string filename;
    artist = utils::toLower(utils::trim(artist));
    title = utils::toLower(utils::trim(title));

    std::replace(artist.begin(), artist.end(), ' ', '-');
    std::replace(title.begin(), title.end(), ' ', '-');

    filename = artist + "-" + title + ".json";
    std::cout << filename << std::endl;


    return (localStorageDir + filename);
  }


  std::string Storage::createArtistFilePath(std::string artist) {
    std::string filename;
    artist = utils::toLower(utils::trim(artist));

    std::replace(artist.begin(), artist.end(), ' ', '-');
    std::replace(artist.begin(), artist.end(), '/', '-');

    filename = artist + ".json";
    std::cout << filename << std::endl;


    return (localStorageDir + filename);
  }


  bool Storage::fileExists(const char *filename) {
    std::ifstream ifile(filename);
    return ifile.is_open();
  }


  void Storage::remoteSaveArtist(std::string filename) {
    std::string output;

    http->POSTFile(remoteStorageURL + "?action=upload", filename, output);
    std::cout << "saved result:" << output << std::endl;
  }


  std::string Storage::remoteLoadArtist(std::string artist) {
    std::string output;

    http->GET(remoteStorageURL + "?action=search&artist=" + utils::toLower(artist), output);

    std::cout << "loaded result:" << output << std::endl;

    return output;
  }
}