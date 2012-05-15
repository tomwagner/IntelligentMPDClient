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
 * @file sartist.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */

#include "sartist.h"

namespace DataStorage {


  SArtist::SArtist() : m_classificator(new NaiveBayes::Classificator),
  m_right(new NaiveBayes::Index),
  m_wrong(new NaiveBayes::Index),
  m_synced(false) {
  }


  SArtist::~SArtist() {
    clear();

    // delete classificator
    delete m_wrong;
    delete m_right;
    delete m_classificator;
  }


  void SArtist::clear() {
    // reset synced
    m_synced = false;

    // clear articles
    std::map<std::string, SArticle*>::iterator a;

    for (a = articles.begin(); a != articles.end(); a++) {
      delete (*a).second;
    }
    articles.clear();

    // clear images
    std::map<std::string, SSlide*>::iterator i;

    for (i = images.begin(); i != images.end(); i++) {
      delete (*i).second;
    }
    images.clear();

    // clear albums
    std::map<std::string, std::map<std::string, SAlbum*>* >::iterator al0;
    std::map<std::string, SAlbum*>::iterator al1;


    for (al0 = albums.begin(); al0 != albums.end(); al0++) {
      for (al1 = (*al0).second->begin(); al1 != (*al0).second->end(); al1++) {
        delete (*al1).second;
      }
      (*al0).second->clear();
      delete (*al0).second;
    }
    albums.clear();

    // clear classificator
    m_right->clear();
    m_wrong->clear();
  }


  ClassificatorResult SArtist::classificate(std::string s) {
    Index * unknown = new Index;
    unknown->add(s);

    ClassificatorResult result = classificate(unknown);
    delete unknown;

    return result;
  }


  ClassificatorResult SArtist::classificate(Index * item) {
    return m_classificator->classificate(m_right, m_wrong, item);
  }


  /**
   * Method loads storage file from filepath
   * @param filePath
   */
  void SArtist::loadArtistFromFile(const std::string& filePath) {
    std::string line;
    std::string content;
    std::ifstream in(filePath.c_str());

    // if file doesnt exist, we create new empty 
    if (!in.is_open()) {
      std::cout << "Storage: Local Artist File not found" << std::endl;
      return;
    }
    while (std::getline(in, line))
      content += line;

    in.close();

    loadArtistFromContent(content);
  }


  void SArtist::loadArtistFromContent(std::string& content) {
    Json::Value root;
    Json::Reader reader;

    // if file is empty, do nothing
    if (content.empty()) return;

    // parse JSON file
    bool parsingSuccessful = reader.parse(content, root);

    if (!parsingSuccessful) {
      std::cout << "Storage: Error in parsing input file" << reader.getFormatedErrorMessages();
    }

    Json::Value artist = root["artist"];

    // load settings to object variables

    // load articles
    if (artist.isMember("articles")) {

      const Json::Value articles = artist["articles"];

      for (int index = 0; index < articles.size(); ++index) {
        SArticle * a = new SArticle;
        a->loadArticle(articles[index]);

        // learn classificator 
        FeedbackResult r = a->isRight();

        switch (r) {
          case rightClass:
            m_right->add(a->text->getText());
            break;
          case wrongClass:
            m_wrong->add(a->text->getText());
            break;
          default:
            break;
        }

        saveArticle(a);
      }
    }

    // load albums
    if (artist.isMember("albums")) {
      const Json::Value albmus = artist["albums"];

      for (Json::Value::iterator it = artist["albums"].begin(); it != artist["albums"].end(); ++it) {
        std::cout << it.key().asString();
        for (int index = 0; index < albmus[it.key().asString()].size(); ++index) {
          SAlbum * al = new SAlbum;
          al->loadAlbum(albmus[it.key().asString()][index]);

          // learn classificator 
          FeedbackResult r = al->isRight();

          switch (r) {
            case rightClass:
              m_right->add(al->img->title);
              m_right->add(al->img->alt);
              m_right->add(al->img->context);
              break;
            case wrongClass:
              m_wrong->add(al->img->title);
              m_wrong->add(al->img->alt);
              m_wrong->add(al->img->context);
              break;
            default:
              break;
          }
          saveAlbum(al);
        }
      }
    }

    // load images
    if (artist.isMember("images")) {

      const Json::Value images = artist["images"];

      for (int index = 0; index < images.size(); ++index) {
        SSlide * s = new SSlide;
        s->loadSlide(images[index]);

        // if img doesnt exists in cache, we cant import it
        if (!s->img->isImage()) {
          delete s;
          continue;
        }

        FeedbackResult r = s->isRight();

        switch (r) {
          case rightClass:
            m_right->add(s->img->title);
            m_right->add(s->img->alt);
            m_right->add(s->img->context);
            break;
          case wrongClass:
            m_wrong->add(s->img->title);
            m_wrong->add(s->img->alt);
            m_wrong->add(s->img->context);
            break;
          default:
            break;
        }

        saveImage(s);
      }
    }
    if (artist.isMember("maps")) {
      maps = artist["maps"];
    }
    // after load, classificate other data
    classificateArtist();
  }


  void SArtist::addAsRight(std::string& s) {
    m_right->add(s);
  }


  void SArtist::addAsWrong(std::string& s) {
    m_wrong->add(s);
  }


  void SArtist::classificateArtist() {
    // classificate articles
    std::map<std::string, SArticle *>::iterator it;
    for (it = articles.begin(); it != articles.end(); it++) {
      (*it).second->objectclass = classificate((*it).second->title->getText() + " " + (*it).second->text->getText());
    }


    // classificate images
    std::map<std::string, SSlide *>::iterator itI;
    for (itI = images.begin(); itI != images.end(); itI++) {
      (*itI).second->objectclass = classificate((*itI).second->img->title + " " + (*itI).second->img->alt + " " + (*itI).second->img->context);
    }
  }


  /**
   * Method saves current opened storage file.
   */
  void SArtist::saveArtist(std::string & filePath) {
    Json::Value root;
    Json::Value artist;
    Json::Value track;
    Json::StyledWriter writer;


    if (filePath.empty()) return;

    // create output descriptor
    std::ofstream of(filePath.c_str(), std::ios::out | std::ios::trunc);

    if (!of.is_open()) {
      std::cout << "Storage: Error in opening output file" << std::endl;
    }

    // if output file doesn't loaded
    if (of == NULL) return;

    if (!of.is_open()) {
      std::cerr << "Cannot open storage file for saving." << std::endl;
      return;
    }


    // projdeme všechny položky objektu a uložíme je
    Json::Value a;
    a["name"] = name.getText();
    artist["artist"] = a;


    // save articles
    std::map<std::string, SArticle *>::iterator it;
    for (it = articles.begin(); it != articles.end(); it++) {
      // set sychnronized flag
      if (isSynced())
        (*it).second->setSynced(m_synced);
      artist["articles"].append((*it).second->getArticle());
    }

    // save albums
    std::map<std::string, std::map<std::string, SAlbum*>* >::iterator itA;
    std::map<std::string, SAlbum*>::iterator itB;
    for (itA = albums.begin(); itA != albums.end(); itA++) {
      for (itB = (*itA).second->begin(); itB != (*itA).second->end(); itB++) {
        // set sychnronized flag
        if (isSynced())
          (*itB).second->setSynced(m_synced);
        artist["albums"][(*itB).second->name->getHash()].append((*itB).second->getAlbum());
      }
    }

    // save images
    std::map<std::string, SSlide *>::iterator itI;
    for (itI = images.begin(); itI != images.end(); itI++) {
      if (isSynced())
          (*itI).second->setSynced(m_synced);
      artist["images"].append((*itI).second->getSlide());
    }

    //  artist["webpage"] = webpage.name;
    artist["tags"][""];
    artist["concerts"] = "";


    root["artist"] = artist;

    // insert maps, with history, ..
    root["maps"] = maps;


    // write
    of << root.toStyledString();
    of.close();
  }


  void SArtist::saveArticle(SArticle * article) {
    articles.insert(std::pair<std::string, SArticle *>(article->text->getHash(), article));
  }


  void SArtist::saveImage(SSlide * image) {
    images.insert(std::pair<std::string, SSlide *>(image->img->getHash(), image));
  }


  void SArtist::saveAlbum(SAlbum * album) {
    std::map<std::string, std::map<std::string, SAlbum*>* >::iterator it;
    it = albums.find(album->name->getHash());

    if (it != albums.end()) {
      (*it).second->insert(std::pair<std::string, SAlbum *>(album->img->getHash(), album));
    } else {
      std::map<std::string, SAlbum *>* m = new std::map<std::string, SAlbum *>;
      m->insert(std::pair<std::string, SAlbum *>(album->img->getHash(), album));
      albums.insert(std::pair<std::string, std::map<std::string, SAlbum*>* >(album->name->getHash(), m));

    }
  }


  void SArtist::saveMap(std::string mapName, std::map<std::string, AgentUrl>* map) {
    Json::Value newMap;

    std::map<std::string, AgentUrl>::iterator it;
    for (it = map->begin(); it != map->end(); it++) {
      Json::Value row;
      row["key"] = (*it).first;
      row["visited"] = (*it).second.visited;
      row["url"] = (*it).second.url;
      row["time"] = (int) (*it).second.time;
      row["relevance"] = (*it).second.relevance;
      newMap[mapName].append(row);
    }
    maps.append(newMap);
  }


  void SArtist::loadMap(std::string mapName, std::map<std::string, AgentUrl>* map) {
    if (maps.isMember(mapName)) {
      Json::Value loadMap = maps[mapName];
      for (int index = 0; index < loadMap.size(); ++index) {
        AgentUrl u;
        u.visited = loadMap["visited"].asBool();
        u.url = loadMap["url"].asString();
        u.time = (time_t) loadMap["time"].asInt();
        u.relevance = loadMap["relevance"].asDouble();
        map->insert(std::pair<std::string, AgentUrl > (loadMap["key"].asString(), u));
      }
    }
  }


  void SArtist::setSynced(bool sync) {
    m_synced = sync;
  }
}

