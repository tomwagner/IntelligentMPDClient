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
 * @file agent.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Web Agent object.
 */

#include "agent.h"


Agent::Agent(MPD::Client * client, ClientSettings * settings, std::string url) :
clientMPD(client),
clientSettings(settings),
baseUrl(url),
actualUrl(url) {

  std::cout << "Spuštěn Agent: " << baseUrl << std::endl;

  // create HTTP object
  http = new utils::HTTP;
  // download source
  http->GET(url, sourceHTML);
  // parse source
  try {
    webpage = new Webpage(sourceHTML);

    // we find search forms
    searchForms = webpage->detectSearchForms();

  } catch (InputException &e) {
    std::cerr << "Chyba vstupu:" << e.what() << std::endl;
  } catch (ClientException &e) {
    std::cerr << e.what() << std::endl;
  }
}


Agent::~Agent() {
  delete http;
  delete webpage;
}


/**
 * Start running agent on base URL
 */
void Agent::run() {

  // try search forms
  std::list<searchPair*>::iterator it;
  for (it = searchForms.begin(); it != searchForms.end(); it++) {
    std::string htmlSrc;


    if ((*it)->method == "GET") {
      http->GET(baseUrl + (*it)->url + "Beatles", htmlSrc);
      std::cout << (baseUrl + (*it)->url + "Beatles") << std::endl;

      try {
        Webpage w = Webpage(htmlSrc);
        std::cout << w.H1() << std::endl;
        //std::cout << searchSource.c_str() << std::endl;
        // get link list
        std::list <linkPair *> linkList = w.getLinkList("The Beatles");
        std::list<linkPair*>::iterator it;
        for (it = linkList.begin(); it != linkList.end(); it++) {
          std::cout << "Link.:" << (*it)->name.data() << " url: " << (*it)->url.data() << std::endl;
        }

      } catch (InputException &e) {
        std::cerr << "Chyba vstupu:" << e.what() << std::endl;
      } catch (ClientException &e) {
        std::cerr << e.what() << std::endl;
      }
    }
  }
}



