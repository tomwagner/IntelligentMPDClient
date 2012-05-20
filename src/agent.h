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
 * @file agent.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Web Agent object.
 */

#ifndef AGENT_H
#define	AGENT_H

#include "ClientException.h"
#include "clientsettings.h"
#include "webpage.h"
#include "utils.h"
#include "http.h"
#include "mpdclient.h"
#include "thread.h"
#include "guimainwindow.h"
#include "storage.h"

#include<ctime>
#include <list>
#include <map>

typedef struct {
  std::string protocol_, host_, domain_, dirpath_, path_, query_;
} parsedUrl;

class Agent {
public:
  Agent(std::string url, MPD::Song s);
  virtual ~Agent();

  void run();
  static void* run(void* pInstance);
  void stop();
  bool changeSearch();



private:
  //tests
  unsigned webpageCount;
  unsigned acceptedImageCount;
  unsigned imageCount;
  unsigned textCount;
  unsigned acceptedTextCount;
  unsigned acceptedCount;
  unsigned removedTextCount;
  unsigned removedImageCount;
  Thread * t;
  utils::HTTP * http;
  hashwrapper * hashWrapper;

  MPD::Song currentMPDSong;

  int minCharsInParagraph;
  int minImageSize;

  bool permission;

  std::string sourceIcon;

  std::string actualUrl;
  std::string baseUrl; // http://*.domain.tld
  std::string baseDomain; //  domain.tld


  std::string currentArtist;
  std::string currentSong;
  std::string currentAlbum;

  // agent history
  std::map<std::string, AgentUrl> history;

  std::list<searchPair *> searchForms;
  // map <hash, url>
  std::map<std::string, AgentUrl> webMap;
  std::map<std::string, AgentUrl> imageMap;
  std::map<std::string, AgentUrl> youtubeMap;

  void insertUrlIntoMap(std::map<std::string, AgentUrl>& map, std::string& url, bool visited);
  void browseImageMap();

  Webpage * getWebpage(std::string url)throw (ClientException);
  bool isVisited(std::string url);

  std::string getSourceIconURL(Webpage * w);


  void saveArticles(std::list<article *> parList);
  void saveAlbumList(std::list<imgPair> imgList);
  void saveImageList(std::list<imgPair> imgList);

  Webpage * searchInForms(std::list<searchPair*> searchForms, std::string name);

  void recursiveSearch();

  bool detectIfIsValidPage();
  std::string standardizeUrl(std::string baseUrl);
  void splitKeyWords(const std::string &s, char delim, std::vector<std::string> &elems);


  // utils
  std::string absoluteUrl(std::string url);
  parsedUrl parseUrl(const std::string& url_s);
  std::string getDomain(const std::string& url);

  bool checkAcceptedDomain(std::string url);
};

#endif	/* AGENT_H */

