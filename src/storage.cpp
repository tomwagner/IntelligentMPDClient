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

#define DEBUG 0



namespace DataStorage {
  Storage* Storage::instance = NULL;


  Storage::Storage() : filePath(""), http(new HTTP) {
    sleep(1);
#if DEBUG
    std::cout << "storage vytvořen" << std::endl;
#endif

    localStorageDir = Config::GetInstance()->getTempPath();
    remoteStorageURL = "http://www.impc.cz/api/";

    currentArtist = new SArtist;

    //    loadWidgets();
  }


  Storage::~Storage() {
#if DEBUG
    std::cout << "storage destruktor" << std::endl;
#endif
    // save current file
    currentArtist->saveArtist(filePath);

    if (Config::GetInstance()->isRemoteStorageEnabled()) {
      // save to shared storage
      if (remoteSaveArtist(filePath)) {
        currentArtist->setSynced(true);
      } else {
        currentArtist->setSynced(false);
      }


      // save with sync flags
      currentArtist->saveArtist(filePath);
    }
    delete currentArtist;
    delete http;
  }


  void Storage::loadWidgets() {

    //    GUI::MainWindow::GetInstance()->articlesWidget->clearArticlesWidget();

    // set pointers to GUI
    GUI::MainWindow::GetInstance()->articlesWidget->setArticlesWidget(currentArtist);
    GUI::MainWindow::GetInstance()->slideshowWidget->setSlideshowWidget(currentArtist);

    std::map<std::string, std::map<std::string, SAlbum*>* >::iterator it;

    it = currentArtist->albums.find(currentArtist->album.getHash());
    if (it != currentArtist->albums.end()) {
      GUI::MainWindow::GetInstance()->coverWidget->setCoverWidget((*it).second);
    } else {
      // current album not found, create new map for album images
      std::map<std::string, SAlbum *>* m = new std::map<std::string, SAlbum *>;
      currentArtist->albums.insert(std::pair<std::string, std::map<std::string, SAlbum*>* >(currentArtist->album.getHash(), m));
      GUI::MainWindow::GetInstance()->coverWidget->setCoverWidget(&*m);
    }
  }


  void Storage::loadNewSong(MPD::Song s) {
    // save current file
    currentArtist->saveArtist(filePath);

    if (Config::GetInstance()->isRemoteStorageEnabled()) {
      // save to shared storage
      if (remoteSaveArtist(filePath)) {
        currentArtist->setSynced(true);
      } else {
        currentArtist->setSynced(false);
      }

      // save with sync flags
      currentArtist->saveArtist(filePath);
    }

    // clear current Artist
    currentArtist->clear();

    // load new data to storage
    filePath = createArtistFilePath(s.GetArtist());


    currentArtist->name.set(s.GetArtist());
    currentArtist->album.set(s.GetAlbum());

    // if is enabled remote sharing, share
    if (Config::GetInstance()->isRemoteStorageEnabled()) {
      // remote load artist
      std::string remoteArtistContent = remoteLoadArtist(s.GetArtist());
      currentArtist->loadArtistFromContent(remoteArtistContent);
    }

    // check if file exists
    // search in local temp
    currentArtist->loadArtistFromFile(filePath);


    loadWidgets();
  }


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

    return (localStorageDir + filename);
  }


  std::string Storage::createArtistFilePath(std::string artist) {
    std::string filename;
    artist = utils::toLower(utils::trim(artist));

    std::replace(artist.begin(), artist.end(), ' ', '-');
    std::replace(artist.begin(), artist.end(), '/', '-');

    filename = artist + ".json";

    return (localStorageDir + filename);
  }


  bool Storage::fileExists(const char *filename) {
    std::ifstream ifile(filename);
    return ifile.is_open();
  }


  bool Storage::remoteSaveArtist(std::string filepath) {
    std::string output;
    if (filepath.empty()) return false;

    try {
      http->POSTFile(remoteStorageURL + "?action=upload", filepath, output);
    } catch (InputException &e) {
      std::cerr << "Input exception:" << e.what() << std::endl;
      return false;
    }
#if DEBUG
    std::cout << "saved result:" << output << std::endl;
#endif
    return true;
  }


  std::string Storage::remoteLoadArtist(std::string artist) {
    std::string output;
    try {
      http->GET((remoteStorageURL + "?action=search&artist=" + HTTP::escape(utils::toLower(artist))), output);

    } catch (InputException &e) {
      std::cerr << "Input exception:" << e.what() << std::endl;
    }
#if DEBUG
    std::cout << "loaded result:" << output << std::endl;
#endif
    return output;
  }
}