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


#ifndef SSLIDE_H
#define	SSLIDE_H
#include "stext.h"
#include "simage.h"
#include "ssource.h"
#include "sfeedback.h"
#include <jsoncpp/json/writer.h>
#include "bayesclassificator.h"

using namespace NaiveBayes;
namespace DataStorage {

  class SSlide : public SFeedback, public SSource {
  public:

    SSlide();
    virtual ~SSlide();

    SImage * img;
    std::string title;
    ClassificatorResult objectclass;
    float relevance;

    Json::Value getSlide();
    void loadSlide(Json::Value);

  };
}
#endif	/* SSLIDE_H */

