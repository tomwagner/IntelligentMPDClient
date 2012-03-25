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
 * @file webagent.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */


#ifndef WEBAGENT_H
#define	WEBAGENT_H
#include "global.h"
#include <vector>
#include <sstream>
#include <fstream>
#include "webpage.h"

class WebAgent {
public:
  WebAgent(std::string url);
  WebAgent(const WebAgent& orig);
  virtual ~WebAgent();
protected:
    std::string domain;
private:
  bool isRelativeURL(std::string url);
  bool isAbsoluteURL(std::string url);
  bool isImageURL(std::string url);
  void splitKeyWords(const std::string &s, char delim, std::vector<std::string> &elems);
  
};

#endif
