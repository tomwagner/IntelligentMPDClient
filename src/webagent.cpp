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
 * @file webagent.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */

#include "webagent.h"


WebAgent::WebAgent(std::string url) {
  
  //TODO pokusíme se načíst informace z databáze znalostí
  
  
  Webpage page = Webpage(url);
  std::list<searchPair*> forms = page.detectSearchForms();
  
  
}


WebAgent::WebAgent(const WebAgent& orig) {
}


WebAgent::~WebAgent() {
}


bool WebAgent::isRelativeURL(std::string url) {
  std::string f;
  f = url[0];
  std::string s;
  s= url[1];
  if ((f == "/") || ((f == ".") && (s == "/"))) {
    return true;
  }
  return false;
}

bool WebAgent::isAbsoluteURL(std::string url) {
  return !isRelativeURL(url);
}


void WebAgent::splitKeyWords(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
}
