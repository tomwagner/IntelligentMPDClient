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
 * @file sartist.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */

#ifndef SARTIST_H
#define	SARTIST_H
#include "stext.h"
#include "sarticle.h"
#include "sslide.h"
#include "simage.h"
#include "salbum.h"
#include "bayesclassificator.h"

namespace DataStorage {
  using namespace NaiveBayes;

  typedef struct {
    bool visited;
    std::string url;
    double relevance;
    std::time_t time;
  } AgentUrl;

  class SArtist {
  public:
    SArtist();
    virtual ~SArtist();

    void loadArtistFromContent(std::string& content);
    void loadArtistFromFile(const std::string& filePath);
    void saveArtist(std::string& filePath);
    void clear();

    void setArtist(std::string name);
    void setAlbum(std::string name){
      album.set(name);
    };
    void setTrack(std::string name);

    void saveImage(SSlide *);
    void saveArticle(SArticle *);
    void saveAlbum(SAlbum *);

    void loadMap(std::string mapName, std::map<std::string, AgentUrl>* map);
    void saveMap(std::string mapName, std::map<std::string, AgentUrl>* map);

    ClassificatorResult classificate(std::string s);
    ClassificatorResult classificate(Index * item);

    SText name;
    SText album;
    SText webpage;

    // pair <hash, article>
    std::map<std::string, SSlide*> images;
    std::map<std::string, SArticle *> articles;
//    std::map<std::string, SAlbum*> albums;
    std::map<std::string, std::map<std::string, SAlbum*>* > albums;
  private:
    NaiveBayes::Classificator * m_classificator;
    NaiveBayes::Index * m_right;
    NaiveBayes::Index * m_wrong;

    Json::Value maps;

    void classificateArtist();
  };
}
#endif	/* SARTIST_H */

