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

#define DEBUG 0
#define TESTING 0


Agent::Agent(std::string url, MPD::Song s) :
baseUrl(standardizeUrl(url)),
actualUrl(url),
minCharsInParagraph(100),
minImageSize(100),
currentMPDSong(s),
permission(true) {

#if DEBUG
  std::cout << "Spuštěn Agent: " << baseUrl << std::endl;
#endif

#if TESTING
  webpageCount = 0;
  imageCount = 0;
  textCount = 0;
  acceptedCount = 0;
  acceptedTextCount = 0;
  acceptedImageCount = 0;
  removedImageCount = 0;
  removedTextCount = 0;
#endif

  // create HTTP object
  http = new utils::HTTP;

  // set request timeout to one second
  http->setMaxRequestTimeout(1);

  // init hash wrapper
  hashWrapper = new md5wrapper();

  // run new thread
  t = new Thread(run, this);
  t->start();
}


Agent::~Agent() {
#if DEBUG
  std::cout << "agent dest." << std::endl;
#endif

#if TESTING
  std::cout << "webpageCount: " << webpageCount << std::endl;
  std::cout << "acceptedImageCount: " << acceptedImageCount << std::endl;
  std::cout << "imageCount: " << imageCount << std::endl;
  std::cout << "textCount: " << textCount << std::endl;
  std::cout << "acceptedTextCount: " << acceptedTextCount << std::endl;
  std::cout << "acceptedCount: " << acceptedCount << std::endl;
  std::cout << "removedTextCount: " << removedTextCount << std::endl;
  std::cout << "removedImageCount: " << removedImageCount << std::endl;
#endif
  // stop agent
  stop();

  // cancel thread
  t->cancel();

  delete http;

  // delete hash wrapper
  delete hashWrapper;
}


void* Agent::run(void* pInstance) {
  Agent* pAgent = reinterpret_cast<Agent*> (pInstance);
  pAgent->run();
  return NULL;
}


void Agent::stop() {
  permission = false;
}


/**
 * Start running agent on base URL
 */
void Agent::run() {

  // set base url and domain
  parsedUrl u = parseUrl(baseUrl);

  baseUrl = u.protocol_ + "://" + u.host_;
  baseDomain = u.domain_;


  // non escaped
  currentArtist = currentMPDSong.GetArtist();
  currentSong = currentMPDSong.GetTitle();
  currentAlbum = currentMPDSong.GetAlbum();

#if DEBUG
  std::cout << "Artist:" << currentArtist << "Title:" << currentSong << "Album:" << currentAlbum << std::endl;
#endif




#if DEBUG
  std::cout << "START:" << baseUrl << std::endl;
#endif

  /*MAIN PAGE START*/
  Webpage * main;
  try {
    actualUrl = baseUrl;

    // get main page
    main = getWebpage(baseUrl);
    if (main == NULL) return;

    sourceIcon = getSourceIconURL(main);

    // get paragraphs about Artist
    std::list<article*> artList = main->getParagraphList(currentArtist);

    // save
    saveArticles(artList);

    // get paragraphs about Album
    std::list<article*> albList = main->getParagraphList(currentAlbum);

    // save
    saveArticles(albList);

    // get paragraphs about Track
    std::list<article*> trackList = main->getParagraphList(currentSong);

    // save
    saveArticles(trackList);

    // we find search forms
    searchForms = main->detectSearchForms();

  } catch (InputException &e) {
    std::cerr << "Agent Exception" << e.what() << std::endl;
    //    GUI::MainWindow::GetInstance()->showErrorDialog(e.what());
    return;
  } catch (ClientException &e) {
    std::cerr << e.what() << std::endl;
    //    GUI::MainWindow::GetInstance()->showErrorDialog(e.what());
    return;
  }

  /**MAIN PAGE END*/

  /**TRY SEARCH FORMS START*/
  try {
#if DEBUG
    std::cout << "Detected search forms:" << searchForms.size() << std::endl << std::endl;
#endif

    /** if we not found search form */
    if (searchForms.size() == 0) {
      std::list<linkPair *> linkList = main->getLinkList(currentArtist);
      std::list<linkPair *>::iterator it;

      for (it = linkList.begin(); it != linkList.end(); it++) {


        // if url not contains base domain, continue
        if (!checkAcceptedDomain(absoluteUrl((*it)->url))) continue;

#if DEBUG 
        std::cout << "Link.: '" << (*it)->name << "' url: '" << absoluteUrl((*it)->url) << "'" << std::endl;
#endif

        // go to found link
        if (isVisited(absoluteUrl((*it)->url))) continue;
        Webpage * w = getWebpage(absoluteUrl((*it)->url));
        if (w == NULL) return;

        // get paragraphs
        std::list<article *> parList = w->getParagraphList(currentArtist);

        //save paragraphs
        saveArticles(parList);

        // get albums
        std::list<imgPair> albList = w->getImgList(currentAlbum);

        // save albums
        saveAlbumList(albList);

        // get images
        std::list<imgPair> imgList = w->getImgList();

        // save images
        saveImageList(imgList);

        delete w;
      }
      //      return;
    }/* if we not found search form - end */

    /******************************
     *  ARTIST SEARCH START
     ******************************/

    Webpage * search = searchInForms(searchForms, currentArtist);
    if (search == NULL) return;
#if DEBUG
    std::cout << "H1:" << search->getH1() << std::endl;
#endif
    // get paragraphs
    std::list<article *> parList = search ->getParagraphList(currentArtist);

    // save
    saveArticles(parList);

    // get albums
    std::list<imgPair> albList = search->getImgList(currentAlbum);

    // save albums
    saveAlbumList(albList);

    // get images
    std::list<imgPair> imgList = search->getImgList();
#if DEBUG
    std::cout << "imgs " << imgList.size() << std::endl;
#endif
    // save images
    saveImageList(imgList);


    // get image link list (imgs with link)
    std::list<linkPair*> imgLinkList = search->getImgLinkList();
#if DEBUG
    std::cout << "img link list: " << imgLinkList.size() << std::endl;
#endif
    std::list<linkPair*>::iterator it;

    for (it = imgLinkList.begin(); it != imgLinkList.end(); it++) {
#if DEBUG
      std::cout << "img map insert: " << (*it)->url << std::endl;
#endif
      insertUrlIntoMap(imageMap, (*it)->url, false);
    }

    // browse imageMap
    browseImageMap();
    /******************************
     *  ARTIST SEARCH END
     ******************************/


    /******************************
     *  ARTIST SEARCH START - FOLLOW LINKS
     ******************************/
    // get link list with artist title
    std::list<linkPair *> linkList = search->getLinkList(currentArtist);
#if DEBUG
    std::cout << "arist found in search: " << linkList.size() << std::endl;
#endif
    for (it = linkList.begin(); it != linkList.end(); it++) {

      // if url not contains base domain, continue
      if (!checkAcceptedDomain(absoluteUrl((*it)->url))) continue;


#if DEBUG 
      std::cout << "Link.: '" << (*it)->name << "' url: '" << absoluteUrl((*it)->url) << "'" << std::endl;
#endif
      // go to found link
      if (isVisited(absoluteUrl((*it)->url))) continue;
      Webpage * w = getWebpage(absoluteUrl((*it)->url));
      if (w == NULL) continue;

      // get paragraphs
      std::list<article *> parList = w->getParagraphList();

      //save paragraphs
      saveArticles(parList);

      // get albums
      std::list<imgPair> albList = w->getImgList();

      // save albums
      saveAlbumList(albList);


      // get images
      std::list<imgPair> imgList = w->getImgList();

      std::cout << "imgs" << imgList.size() << std::endl;

      // save images
      saveImageList(imgList);


      // get image link list (imgs with link)
      std::list<linkPair*> imgLinkList = w->getImgLinkList();

      std::cout << "img link list: " << imgLinkList.size() << std::endl;

      std::list<linkPair*>::iterator it;

      for (it = imgLinkList.begin(); it != imgLinkList.end(); it++) {
        std::cout << "img map insert: " << (*it)->url << std::endl;
        insertUrlIntoMap(imageMap, (*it)->url, false);
      }

      // browse imageMap
      browseImageMap();

      delete w;
    }

    delete search;

  } catch (InputException &e) {
    std::cerr << "Chyba vstupu:" << e.what() << std::endl;
  } catch (ClientException &e) {


    std::cerr << e.what() << std::endl;

    //    return;
  }

  //  Storage::GetInstance()->currentArtist->saveMap("history", &history);
  //
  //  // print all visited pages
  //  std::map<std::string, AgentUrl>::iterator it;
  //  for (it = history.begin(); it != history.end(); it++) {
  //    std::cout << "history:" << (*it).second.url << "time: " << (*it).second.time << "visited: " << (*it).second.visited << std::endl;
  //  }

}


std::string Agent::getSourceIconURL(Webpage * w) {
  std::string sourceIcon;
  sourceIcon = absoluteUrl(w->getFaviconUrl());
  if (w->getFaviconUrl().empty())
    sourceIcon = absoluteUrl("/favicon.ico");


  return sourceIcon;
}


void Agent::saveArticles(std::list<article *> articleList) {

  std::list<article *>::iterator p;
  for (p = articleList.begin(); p != articleList.end(); p++) {
#if TESTING
    textCount++;
#endif
    // min chars in paragraph condition
    if ((*p)->text.length() < minCharsInParagraph) continue;

#if DEBUG
    //    std::cout << "S Article:" << "title:" << (*p)->title << "text: " << (*p)->text << std::endl;
#endif
    SArticle * a = new SArticle;
    a->title->set((*p)->title);
    a->text->set((*p)->text);

    a->setUrl(sourceIcon);
    a->setName(""); // TODO webpage name
    a->setSourceName(actualUrl);

    // classificate
    a->objectclass = Storage::GetInstance()->currentArtist->classificate(a->text->getText() + " " + a->title->getText());

    switch (a->objectclass) {
      case NaiveBayes::firstClass:
      case NaiveBayes::unknownClass:
        Storage::GetInstance()->currentArtist->saveArticle(a);
#if TESTING
        acceptedTextCount++;
        std::cout << "accepted text" << a->text->getText() << std::endl;
#endif
        break;
      case NaiveBayes::secondClass:
        // article is not related
#if TESTING
        removedTextCount++;
        std::cout << "removed text" << a->text->getText() << std::endl;
#endif
        delete a;
        break;
      default:
        std::cout << "ERR Classification" << std::endl;


        break;
    }
  }
}


void Agent::saveAlbumList(std::list<imgPair> imgList) {

  std::list<imgPair>::iterator i;
  for (i = imgList.begin(); i != imgList.end(); i++) {

    SAlbum * a = new SAlbum;
    a->name->set(currentAlbum);

    a->img->setUrl(absoluteUrl((*i).url));
    a->img->title = (*i).title;
    a->img->alt = (*i).alt;
    a->img->context = (*i).context;


    a->setUrl(sourceIcon);
    a->setName(""); // TODO webpage namem,
    a->setSourceName(actualUrl);


    // img is too small, dont save it
    if ((a->img->getHeight() < minImageSize) || (a->img->getWidth() < minImageSize)) {

      //      std::cout << "Image is too small :(";
      delete a;
      continue;
    }

    // if url not contains img (local path is null)
    if (!a->isImage()) {
      // save url to imageMap for browse (its probably webpage with bigger img)
      insertUrlIntoMap(imageMap, (*i).url, false);
      delete a;
      continue;
    }

    // classification
    a->objectclass = Storage::GetInstance()->currentArtist->classificate(a->img->alt + " " + a->img->context + " " + a->img->title);

    switch (a->objectclass) {
      case NaiveBayes::firstClass:
      case NaiveBayes::unknownClass:
        Storage::GetInstance()->currentArtist->saveAlbum(a);
        break;
      case NaiveBayes::secondClass:
        // image is not related
        delete a;
        break;
      default:
        std::cout << "ERR Classification" << std::endl;


        break;
    }
  }
}


void Agent::saveImageList(std::list<imgPair> imgList) {

  std::list<imgPair>::iterator i;
  for (i = imgList.begin(); i != imgList.end(); i++) {

#if TESTING
    imageCount++;
#endif

    //#if DEBUG
    //    std::cout << "saveImageList Alt:" << (*i).alt << "Img:" << absoluteUrl((*i).url) << std::endl;
    //    std::cout << "saveImageList Content:" << (*i).context << "Title:" << absoluteUrl((*i).title) << std::endl;
    //#endif

    SSlide * a = new SSlide;
    a->img->setUrl(absoluteUrl((*i).url));
    a->img->title = (*i).title;
    a->img->alt = (*i).alt;
    a->img->context = (*i).context;

    a->setUrl(sourceIcon);
    a->setName(""); // TODO webpage namem,
    a->setSourceName(actualUrl);

    //    std::cout << "width" << a->img->getWidth() << "height" << a->img->getHeight() << std::endl;

    // img is too small, dont save it
    if ((a->img->getHeight() < minImageSize) || (a->img->getWidth() < minImageSize)) {

      //      std::cout << "Image is too small :(";
      delete a;
      continue;
    }

    // if url not contains img (local path is null)
    if (!a->isImage()) {
      //
      //      std::cout << "saveImageList isImage: false";
      // save url to imageMap for browse (its probably webpage with bigger img)
      insertUrlIntoMap(imageMap, (*i).url, false);
      delete a;
      continue;
    }

    // classification
    a->objectclass = Storage::GetInstance()->currentArtist->classificate(a->img->alt + " " + a->img->context + " " + a->img->title);

    switch (a->objectclass) {
      case NaiveBayes::firstClass:
      case NaiveBayes::unknownClass:
        Storage::GetInstance()->currentArtist->saveImage(a);
#if TESTING
        acceptedImageCount++;
        std::cout << "accepted image" << a->img->getUrl()->getText() << std::endl;
#endif
        break;
      case NaiveBayes::secondClass:
#if TESTING
        removedImageCount++;
        std::cout << "removed image" << a->img->getUrl()->getText() << std::endl;
#endif
        // image is not related
        delete a;
        break;
      default:
        std::cout << "ERR Classification" << std::endl;


        break;
    }
  }
}


void Agent::browseImageMap() {
  // if agent ends, return
  if (!permission) return;

  std::map<std::string, AgentUrl>::iterator i;

  try {
    for (i = imageMap.begin(); i != imageMap.end(); i++) {

      // if was url visited, continue next
      if ((*i).second.visited) continue;

      if (isVisited(absoluteUrl((*i).second.url))) continue;
      Webpage * w = getWebpage(absoluteUrl((*i).second.url));
      if (w == NULL) continue;

      // get images contains artist
      std::list<imgPair> imgList = w->getImgList(currentArtist);
#if DEBUG
      std::cout << "browseImageMap: " << absoluteUrl((*i).second.url) << "imgs:" << imgList.size() << std::endl;
#endif
      // get albums
      std::list<imgPair> albList = w->getImgList(currentAlbum);

      // save albums
      saveAlbumList(albList);

      // save images
      saveImageList(imgList);

      // get images contains albums
      imgList = w->getImgList(currentAlbum);
#if DEBUG
      std::cout << "browseImageMap: " << absoluteUrl((*i).second.url) << "imgs:" << imgList.size() << std::endl;
#endif
      // save images
      saveImageList(imgList);

      // set url as visited
      (*i).second.visited = true;

      delete w;
    }

  } catch (InputException &e) {
    std::cerr << "Chyba vstupu:" << e.what() << std::endl;
  } catch (ClientException &e) {


    std::cerr << e.what() << std::endl;
    //    return;
  }
}


void Agent::insertUrlIntoMap(std::map<std::string, AgentUrl>& map, std::string& url, bool visited) {


  AgentUrl u;
  u.url = url;
  u.visited = visited;
  u.relevance = 0;
  u.time = std::time(0);

  // calculate hash of url
  std::string urlHash = hashWrapper->getHashFromString(url);

  map.insert(std::pair<std::string, AgentUrl > (urlHash, u));
}


bool Agent::isVisited(std::string url) {

  // check if webpage wasnt downloaded
  std::map<std::string, AgentUrl>::iterator found;
  if ((found = history.find(hashWrapper->getHashFromString(url))) != history.end()) {

    // check the date when we last visit
    if ((std::time(0)-(*found).second.time < 86400) && ((*found).second.visited == true)) {
      std::cout << "we visited page yesterday, skip" << std::endl;
      return true;
    }
  }

  //log url as visited
  insertUrlIntoMap(history, url, true);


  return false;
}


Webpage * Agent::getWebpage(std::string url) throw (ClientException) {
  std::string sourceHTML;

#if TESTING
  webpageCount++;
#endif

  if (!permission) {
    ClientException e("Agent: Running interrupted.");
    throw e;
  }

  // set actual url
  actualUrl = url;

#if DEBUG
  std::cout << ">>GO:" << actualUrl << std::endl;
  std::cout << ">>----------------------------------------" << std::endl;
#endif

  sourceHTML.clear();

  // download source
  try {
    actualUrl = http->GET(url, sourceHTML);
  } catch (InputException &e) {
    std::cerr << "Agent - InputException:" << e.what() << std::endl;
  } catch (ClientException &e) {
    std::cerr << e.what() << std::endl;
  }
#if DEBUG
  std::cout << ">>download end:" << std::endl;
#endif

  // set new base url
  parsedUrl u = parseUrl(actualUrl);
  baseUrl = u.protocol_ + "://" + u.host_ + u.dirpath_;

  // set actual webpage an parse source
  Webpage * actualWebpage = NULL;
  try {
    actualWebpage = new Webpage(sourceHTML);
  } catch (InputException &e) {
    std::cerr << "Agent - InputException:" << e.what() << std::endl;
  } catch (ClientException &e) {
    std::cerr << e.what() << std::endl;
  }

#if DEBUG
  std::cout << ">>getWebpage end:" << std::endl;
#endif


  return actualWebpage;
}


/**
 * Method returns Webpage after search
 * @param searchForms
 * @return 
 */
Webpage * Agent::searchInForms(std::list<searchPair*> searchForms, std::string name) {

  // escape song metadata
  std::string e_name = utils::HTTP::escape(name);
  std::string htmlSrc;

  // try search forms
  std::list<searchPair*>::iterator it;
  for (it = searchForms.begin(); it != searchForms.end(); it++) {


    // create question based on HTTP method
    if ((*it)->method == "GET") { // GET METHOD
#if DEBUG
      std::cout << (">>GET:" + absoluteUrl((*it)->url) + "?" + (*it)->params + e_name) << std::endl;
#endif
      // perform question
      actualUrl = http->GET(absoluteUrl((*it)->url) + "?" + (*it)->params + e_name, htmlSrc);


    } else if ((*it)->method == "POST") { // POST METHOD

#if DEBUG
      std::cout << (">>POST:" + absoluteUrl((*it)->url) + "?" + (*it)->params + e_name) << std::endl;
#endif
      // perform question
      actualUrl = http->POST(absoluteUrl((*it)->url), (*it)->params + e_name, htmlSrc);

    }
    break;
  }

  // create Webpage
  Webpage * newWebpage = new Webpage(htmlSrc);


  return newWebpage;
}


/**
 * Creates absolute url with prefix based on url address of source
 * @param url
 * @return 
 */
std::string Agent::absoluteUrl(std::string query) {

  // if url starts with http, return absolute url
  if ((query.find("http://") == 0) || (query.find("https://") == 0)) {
    return query;

    // strange url (from Wikipedia)
  } else if (query.find("//") == 0) {
    return ("http:" + query);

    // url is relative, concat with base
  } else if (query.find("/") == 0) {
    return ("http://" + baseDomain + query);

    // url is relative, concat with base
  } else if (query.find("./") == 0) {
    return ("http://" + baseDomain + query.substr(1));
    // else we try concatenate
  } else {


    return (baseUrl + "/" + query);
  }
}


/**
 * Method returns base url with no "/" at the end.
 * @param baseUrl
 * @return 
 */
std::string Agent::standardizeUrl(std::string baseUrl) {

  if (baseUrl.compare(baseUrl.length() - 1, 1, "/") == 0) {


    return baseUrl.substr(0, -1);
  }
  return baseUrl;
}


parsedUrl Agent::parseUrl(const std::string & url_s) {
  const std::string prot_end("://");
  parsedUrl u;
  std::string protocol_, host_, domain_, dirpath_, path_, query_;

  std::string::const_iterator prot_i = std::search(url_s.begin(), url_s.end(),
          prot_end.begin(), prot_end.end());
  protocol_.reserve(distance(url_s.begin(), prot_i));
  std::transform(url_s.begin(), prot_i,
          std::back_inserter(protocol_),
          std::ptr_fun<int, int>(tolower)); // protocol is icase
  if (prot_i == url_s.end())
    return u;
  std::advance(prot_i, prot_end.length());
  std::string::const_iterator path_i = std::find(prot_i, url_s.end(), '/');
  host_.reserve(distance(prot_i, path_i));
  std::transform(prot_i, path_i,
          std::back_inserter(host_),
          std::ptr_fun<int, int>(tolower)); // host is icase
  std::string::const_iterator query_i = std::find(path_i, url_s.end(), '?');
  path_.assign(path_i, query_i);
  if (query_i != url_s.end())
    ++query_i;
  query_.assign(query_i, url_s.end());

  std::string firstDomainHost;
  size_t found = host_.find_last_of(".");
  found = host_.substr(0, found).find_last_of(".");
  domain_ = host_.substr(found + 1, host_.length());


  // path only with dirs
  dirpath_ = path_.substr(0, path_.find_last_of("/"));

  u.protocol_ = protocol_;
  u.host_ = host_;
  u.domain_ = domain_;
  u.query_ = query_;
  u.path_ = path_;
  u.dirpath_ = dirpath_;


  return u;
}


std::string Agent::getDomain(const std::string & url) {
  parsedUrl u = parseUrl(url);

  return u.domain_;
}


void Agent::splitKeyWords(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {


    elems.push_back(item);
  }
}


bool Agent::checkAcceptedDomain(std::string url) {
  // if agent domain is search engine, we search in all webpages
  if (utils::ci_stringContains(baseDomain, "google")) return true;
  if (utils::ci_stringContains(baseDomain, "seznam")) return true;
  if (utils::ci_stringContains(baseDomain, "bing")) return true;
  if (utils::ci_stringContains(baseDomain, "yahoo")) return true;
  // else we search only in baseDomain
  if (utils::ci_stringContains(absoluteUrl(url), baseDomain)) return true;
#if DEBUG
  std::cout << "accepted domain:" << baseDomain << " " << absoluteUrl(url) << "false" << std::endl;
#endif  
  return false;
}