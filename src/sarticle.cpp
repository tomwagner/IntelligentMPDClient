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
 * @file sarticles.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */
#include "sarticle.h"


using namespace NaiveBayes;
namespace DataStorage {


  SArticle::SArticle() : title(new SText),
  text(new SText),
  objectclass(NaiveBayes::unknownClass) {
  }


  SArticle::~SArticle() {
    delete title;
    delete text;
  }


  Json::Value SArticle::getArticle() {
    Json::Value article;
    article["name"] = title->getText();
    article["hash"] = text->getHash();
    article["text"] = text->getText();

    article["right"] = getNumOfRight();
    article["wrong"] = getNumOfWrong();
    //    article["voted"] = (!canIVote());
    article["votedRight"] = isVotedRight();
    article["votedWrong"] = isVotedWrong();
    article["sync"] = isSynced();

    article["sourceName"] = getName();
    article["sourceUrl"] = getSourceName();
    article["sourceIcon"] = getUrl()->getText();
    return article;
  }


  void SArticle::loadArticle(Json::Value article) {
    title->set(article["name"].asString());
    text->set(article["text"].asString());

    setNumOfRight(article["right"].asUInt());
    setNumOfWrong(article["wrong"].asUInt());
    setVotedRight(article["votedRight"].asBool());
    setVotedWrong(article["votedWrong"].asBool());
    setSynced(article["sync"].asBool());

    setName(article["sourceName"].asString());
    setSourceName(article["sourceUrl"].asString());
    setUrl(article["sourceIcon"].asString());
  }



}
