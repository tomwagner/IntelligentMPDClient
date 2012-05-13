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
 * @file salbum.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */

#include "salbum.h"
using namespace NaiveBayes;

namespace DataStorage {


  SAlbum::SAlbum() : name(new SText), img(new SImage), objectclass(NaiveBayes::unknownClass) {
  }


  SAlbum::~SAlbum() {
    
    delete img;
    delete name;
  }


  Json::Value SAlbum::getAlbum() {
    Json::Value album;
    album["name"] = name->getText();
    album["url"] = img->getUrl()->getText();
    album["title"] = img->title;
    album["alt"] = img->alt;
    album["context"] = img->context;
    album["hash"] = img->getHash();

    album["right"] = getNumOfRight();
    album["wrong"] = getNumOfWrong();
    album["voted"] = (!canIVote());
    album["sync"] = (isSynced());

    album["sourceName"] = getName();
    album["sourceUrl"] = getSourceName();
    album["sourceIcon"] = getUrl()->getText();
    return album;
  }


  void SAlbum::loadAlbum(Json::Value album) {
    name->set(album["name"].asString());
    img->setUrl(album["url"].asString());
    img->title = album["title"].asString();
    img->alt = album["alt"].asString();
    img->context = album["context"].asString();

    setNumOfRight(album["right"].asUInt());
    setNumOfWrong(album["wrong"].asUInt());
    setVoted(album["voted"].asBool());
    setSynced(album["sync"].asBool());

    setName(album["sourceName"].asString());
    setSourceName(album["sourceUrl"].asString());
    setUrl(album["sourceIcon"].asString());
  }
}