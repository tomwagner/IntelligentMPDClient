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
 * @file SArticles.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */

#ifndef SARTICLE_H
#define	SARTICLE_H
#include <cstring>
#include "stext.h"
#include "sfeedback.h"
#include "ssource.h"
#include <jsoncpp/json/writer.h>
#include "bayesclassificator.h"

using namespace NaiveBayes;

namespace DataStorage {

  class SArticle : public SFeedback, public SSource {
  public:

    SArticle();
    virtual ~SArticle();
    
    SText * title;
    SText * text;
    ClassificatorResult objectclass;
    float relevance;
    
    Json::Value getArticle();
    void loadArticle(Json::Value article);
  private:


  };
}
#endif

