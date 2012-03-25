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
 * @file main.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Main
 */


#include <locale>
#include <glibmm-2.4/glibmm/i18n.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>


#include "mpdclient.h"
#include "guimainwindow.h"
#include "webpage.h"
#include "http.h"
#include "ClientException.h"
#include "intelligentclient.h"

#include "thread.h"


void *vlakno(void *arg) {
  sleep(10);
  std::cout << "vlákno:" << std::endl;
  return (void *) 0;
}


int main(int argc, char** argv) {
  bindtextdomain(GETTEXT_PACKAGE, IMPC_LOCALEDIR);
  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
  textdomain(GETTEXT_PACKAGE);

  //google::InitGoogleLogging(argv[0]);
  //  MPD::Client* c = new MPD::Client;
  //  std::string host = "localhost";
  //  int port = 6600;
  //
  //  c->connect(host, port);

  //c.stop();
  //  MPD::Song song;
  //  c->getCurrentSong(song);
  //std::cout << "Status:" << c.getClientState() << std::endl;
  //std::cout << "Autor: " << song.artist() << "\nNázev: " << song.title() << std::endl;
  //LOG(INFO) << "Autor " << song.artist() <<  std::endl;


  //  std::cout << "Last FM" << std::endl;
  //  // Last FM
  //  utils::loadStringFromFile("test/lastfm", file);
  //  HTMLParser *page = new HTMLParser(file.data());
  //  //page->getLinkList();
  //  page->detectSearchForm();
  //  
  //  std::cout << "Seznam" << std::endl;
  //  utils::loadStringFromFile("test/seznam", file);
  //  page = new HTMLParser(file.data());
  //  //page->getLinkList();
  //  page->detectSearchForm();
  //
  //
  //  std::cout << "Google" << std::endl;
  //  utils::loadStringFromFile("test/google", file);
  //  page = new HTMLParser(file.data());
  //  //page->getLinkList();
  //  page->detectSearchForm();
  //  

  //  std::cout << "Last FM - Lenon" << std::endl;
  //  utils::loadStringFromFile("test/lastfm_lenon", file);
  //  HTMLParser *page = new HTMLParser(file.data());
  //  //page->getLinkList("xxx");
  //  searchPair x = page->detectSearchForm();
  //  std::cout << "Metoda:" << x.first << "Link:" << x.second << std::endl;

  try {
    utils::HTTP test;
    std::string webHTML;
    //test.GET("http://cs.wikipedia.org/wiki/The_Beatles", webHTML);
    //test.GET("http://www.last.fm", webHTML);
    //test.GET("http://www.seo-wagner.cz", webHTML);
    //std::cout << webHTML.data();
//    test.POST("http://www.allmusic.com/search", webHTML);
//    std::cout << webHTML.data();
    //HTMLParser *page = new HTMLParser(webHTML.data());
    utils::loadStringFromFile("test/lastfm_beatles_text", webHTML);
    Webpage page = Webpage(webHTML);

    //    page.detectSearchForms();
    //    page.getLinkList("ahoj");
    //    std::list<imgPair> imgList = page.getImgList();
    //    std::list<imgPair>::iterator iit;
    //    for (iit = imgList.begin(); iit != imgList.end(); iit++) {
    //      std::cout << "Img.:" << (*iit).alt.data() << " url: " << (*iit).url.data() << std::endl;
    //    }

    //std::list<searchPair*> searchList = page->detectSearchForms();

    std::list<std::string> paragraphList = page.getParagraphList("rock");
    std::cout << "Title: " << page.title() << ", H1: " << page.H1() << std::endl;

    std::list<std::string>::iterator it;
    for (it = paragraphList.begin(); it != paragraphList.end(); it++) {
      std::cout << "----------------------------------------" << std::endl;
      std::cout << "Odst.:" << (*it).data() << std::endl;
    }

    //std::list<linkPair*> linkList = page->getLinkList(std::string(argv[1]));

    //    if (utils::ci_stringContains("voale Ahoj vole", "ahoj")) {
    //      std::cout << "První obsahuje" << std::endl;
    //    }
    //
    //    if (utils::ci_stringContains("Ahoj vole", "x") == false) {
    //      std::cout << "Druhý neobsahuje" << std::endl;
    //    }


    //    std::list<linkPair*>::iterator it;
    //    for (it = linkList.begin(); it != linkList.end(); it++) {
    //      std::cout << "Link.:" << (*it)->name.data() << " url: " << (*it)->url.data() << std::endl;
    //    }
    //    std::list<searchPair*>::iterator it;
    //    for (it = searchList.begin(); it != searchList.end(); it++) {
    //      std::cout << "Metoda:" << (*it)->method.data() << " Link:" << (*it)->url.data() << std::endl;
    //      // požadavek na vyhledávání
    ////      if ((*it)->method == "GET"){
    ////        std::string webHTML2;
    ////        test.GET("http://www.last.fm"+(*it)->url+"The Beatles", webHTML2);
    ////        std::cout << webHTML2.data();
    ////      }
    //    }



  } catch (InputException &e) {
    std::cerr << "Chyba vstupu:" << e.what() << std::endl;
  } catch (ClientException &e) {
    std::cerr << e.what() << std::endl;
  }

  //  try {
  //    utils::HTTP test1;
  //    std::string webHTML1;
  //    test1.GET("http://cs.wikipedia.org/wiki/The_Beatles", webHTML1);
  //    //
  //    HTMLParser *page1 = new HTMLParser(webHTML1);
  //    std::list<linkPair*> linkList = page1->getLinkList();
  //    std::list<linkPair*>::iterator it;
  //    for (it = linkList.begin(); it != linkList.end(); it++) {
  //      std::cout << "Link.:" << (*it)->name.data() << " url: " << (*it)->url.data() << std::endl;
  //    }
  //    //    page1->detectSearchForms();
  //
  //    // výpis a stažení obrázků
  //    //      std::list<imgPair> imgList = page1->getImgList();
  //    //      std::list<imgPair>::iterator it;
  //    //      for (it = imgList.begin(); it != imgList.end(); it++) {
  //    //        std::cout << "Obr.:" << (*it).url.data() << std::endl;
  //    //        std::string tmpUrl;
  //    //        std::string path;
  //    //        path.assign("http://cs.wikipedia.org");
  //    //        path.append((*it).url);
  //    //        test1.GETImage(path, tmpUrl);
  //    //      }
  //  } catch (InputException &e) {
  //    std::cerr << "Chyba vstupu:" << e.what() << std::endl;
  //  } catch (ClientException &e) {
  //    std::cerr << e.what() << std::endl;
  //  }


  //std::cout << web << std::endl;

  // spustíme instanci inteligentního klienta
  IntelligentClient(argc, argv);


//  Thread * t = new Thread();
//  t->create(vlakno);
//  t->join();
//  delete t;
//  sleep(11);
  return 0;
}

