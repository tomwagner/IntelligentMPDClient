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
 * @file sslide.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */
#include "sslide.h"

using namespace NaiveBayes;

namespace DataStorage {


  SSlide::SSlide() : img(new SImage), objectclass(NaiveBayes::unknownClass) {

  }


  SSlide::~SSlide() {
    delete img;
  }


  Json::Value SSlide::getSlide() {
    Json::Value slide;

    slide["title"] = img->title;
    slide["alt"] = img->alt;
    slide["context"] = img->context;
    slide["url"] = img->getUrl()->getText();


    slide["right"] = getNumOfRight();
    slide["wrong"] = getNumOfWrong();
    slide["voted"] = (!canIVote());
    slide["sync"] = isSynced();

    slide["sourceName"] = getName();
    slide["sourceUrl"] = getSourceName();
    slide["sourceIcon"] = getUrl()->getText();
    return slide;
  }


  void SSlide::loadSlide(Json::Value slide) {

    img->title = slide["title"].asString();
    img->alt = slide["alt"].asString();
    img->context = slide["context"].asString();

    img->setUrl(slide["url"].asString());


    setNumOfRight(slide["right"].asUInt());
    setNumOfWrong(slide["wrong"].asUInt());
    setVoted(slide["voted"].asBool());
    setSynced(slide["sync"].asBool());

    setName(slide["sourceName"].asString());
    setSourceName(slide["sourceUrl"].asString());
    setUrl(slide["sourceIcon"].asString());
  }



}
