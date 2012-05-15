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


//#include "mpdclient.h"
//#include "guimainwindow.h"
//#include "webpage.h"
#include "http.h"
//#include "ClientException.h"
#include "intelligentclient.h"

#include "stext.h"
#include "simage.h"
#include "storage.h"

#include "bayesclassificator.h"
#include "bayesindex.h"


int main(int argc, char** argv) {
  // languages gettext
  bindtextdomain(GETTEXT_PACKAGE, IMPC_LOCALEDIR);
  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
  textdomain(GETTEXT_PACKAGE);


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

//  try {
//
//
//    //    utils::HTTP test;
//    //    
//    //    std::string webHTML;
//    //    test.GET("http://www.vymena-odkazu.cz/phpinfo.php", webHTML);
//    //    std::cout << test.GETImage("http://www.seo-wagner.cz/favicon.ico", "tmp/","xxx");
//    //    return 0;
//    //utils::saveStringToFile("xxx", webHTML);
//    //test.GET("http://cs.wikipedia.org/wiki/The_Beatles", webHTML);
//    //test.GET("http://www.last.fm", webHTML);
//    //test.GET("http://www.seo-wagner.cz", webHTML);
//    //std::cout << webHTML.data();
//    //    test.POST("http://www.allmusic.com/search", webHTML);
//    //    std::cout << webHTML.data();
//    //HTMLParser *page = new HTMLParser(webHTML.data());
//    //    utils::loadStringFromFile("test/imgs", webHTML);
//    //    Webpage page = Webpage(webHTML);
//    ////
//    ////        page.detectSearchForms();
//    //    //    page.getLinkList("ahoj");
//    //        std::list<imgPair> imgList = page.getImgList();
//    //        std::list<imgPair>::iterator iit;
//    //        for (iit = imgList.begin(); iit != imgList.end(); iit++) {
//    //          std::cout << "Img.:" << (*iit).alt.data() << " url: " << (*iit).url.data() << std::endl;
//    //        }
//
//    //std::list<searchPair*> searchList = page->detectSearchForms();
//
//    //    std::list<std::string> paragraphList = page.getParagraphList("rock");
//    //    std::cout << "Title: " << page.title() << ", H1: " << page.H1() << std::endl;
//    //
//    //    std::list<std::string>::iterator it;
//    //    for (it = paragraphList.begin(); it != paragraphList.end(); it++) {
//    //      std::cout << "----------------------------------------" << std::endl;
//    //      std::cout << "Odst.:" << (*it).data() << std::endl;
//    //    }
//
//    //std::list<linkPair*> linkList = page->getLinkList(std::string(argv[1]));
//
//
//    //    std::list<linkPair*>::iterator it;
//    //    for (it = linkList.begin(); it != linkList.end(); it++) {
//    //      std::cout << "Link.:" << (*it)->name.data() << " url: " << (*it)->url.data() << std::endl;
//    //    }
//    //    std::list<searchPair*>::iterator it;
//    //    for (it = searchList.begin(); it != searchList.end(); it++) {
//    //      std::cout << "Metoda:" << (*it)->method.data() << " Link:" << (*it)->url.data() << std::endl;
//    //      // požadavek na vyhledávání
//    ////      if ((*it)->method == "GET"){
//    ////        std::string webHTML2;
//    ////        test.GET("http://www.last.fm"+(*it)->url+"The Beatles", webHTML2);
//    ////        std::cout << webHTML2.data();
//    ////      }
//    //    }
//
//
//
//  } catch (InputException &e) {
//    std::cerr << "Chyba vstupu:" << e.what() << std::endl;
//  } catch (ClientException &e) {
//    std::cerr << e.what() << std::endl;
//  }
//
//  try {
//    utils::HTTP test1;
//    std::string webHTML1;
//    //        utils::loadStringFromFile("test/lastfm_lenon", webHTML1);
//    //        utils::loadStringFromFile("test/imgs", webHTML1);
//    utils::loadStringFromFile(argv[1], webHTML1);
//    //
//    Webpage *page = new Webpage(webHTML1);
//    std::list<linkPair*> linkList = page->getLinkList("Anastacia");
//    std::list<linkPair*>::iterator it;
//    for (it = linkList.begin(); it != linkList.end(); it++) {
//      std::cout << "Link.:" << (*it)->name.data() << " url: " << (*it)->url.data() << std::endl;
//    }
//    //    page1->detectSearchForms();
//    std::cout << "H1.:" << page->getH1() << std::endl;
//    std::cout << "Title.:" << page->getTitle() << std::endl;
//    std::cout << "Favicon.:" << page->getFaviconUrl() << std::endl;
//
//    //      std::list<linkPair*> imgLinkList = page1->getImgLinkList();
//    //      std::list<linkPair*>::iterator it;
//    //      for (it = imgLinkList.begin(); it != imgLinkList.end(); it++) {
//    //        std::cout << "Link.:" << (*it)->name.data() << " url: " << (*it)->url.data() << std::endl;
//    //      }
//
//
//    //    výpis a stažení obrázků
//    //    std::list<imgPair> imgList = page->getImgList();
//    //    std::list<imgPair>::iterator it;
//    //    for (it = imgList.begin(); it != imgList.end(); it++) {
//    //      std::cout << "Obr.:" << (*it).url << " title:'" << (*it).title << "'" << " alt:'" << (*it).alt << "'" << std::endl;
//    //      std::cout << "Obr.:" << (*it).url << " Context:'" << (*it).context << "'" << std::endl;
//    //    }
//
//    //    std::list<article *> paragraphList = page->getParagraphList();
//    //    //    storage.createFile("The Beatles", "Help!");
//    //
//    //    std::cout << paragraphList.size() << std::endl;
//    //    std::list<article *>::iterator it;
//    //
//    //    for (it = paragraphList.begin(); it != paragraphList.end(); it++) {
//    //      NaiveBayes::Index * i = new NaiveBayes::Index;
//    //      //          if ((*it).length() < 100 ) continue;
//    //      std::cout << "----------------------------------------" << std::endl;
//    //
//    //      std::cout << "Nadpis.:" << (*it)->title << std::endl;
//    //      std::cout << "Odst.:" << (*it)->text << std::endl;
//    //      i->add((*it)->text);
//    //
//    //      NaiveBayes::ClassificatorResult res = storage.classificate(i);
//    //
//    //      switch (res) {
//    //        case NaiveBayes::firstClass:
//    //          std::cout << "First Class" << std::endl;
//    //          break;
//    //        case NaiveBayes::secondClass:
//    //          std::cout << "Second Class" << std::endl;
//    //          break;
//    //        case NaiveBayes::unknownClass:
//    //          std::cout << "Unknown Class" << std::endl;
//    //          break;
//    //        default:
//    //          std::cout << "ERR Classification" << std::endl;
//    //          break;
//    //      }
//    //      delete i;
//    //    }
//
//
//  } catch (InputException &e) {
//    std::cerr << "Chyba vstupu:" << e.what() << std::endl;
//  } catch (ClientException &e) {
//    std::cerr << e.what() << std::endl;
//  }
//  return 0;

  //      std::string p = page->getJoinnedParagraphList();
  //      storage.createFile("The Beatles", "Help!");
  //  
  //      NaiveBayes::Index * i = new NaiveBayes::Index;
  //      //          if ((*it).length() < 100 ) continue;
  //      //      std::cout << "----------------------------------------" << std::endl;
  //      //
  //      //      std::cout << "Odst.:" << (*it).c_str() << std::endl;
  //      std::cout << "Joinned:" << p << std::endl;
  //      i->add(p);
  //  
  //      NaiveBayes::ClassificatorResult res = storage.classificate(i);
  //  
  //      switch (res) {
  //        case NaiveBayes::firstClass:
  //          std::cout << "First Class" << std::endl;
  //          break;
  //        case NaiveBayes::secondClass:
  //          std::cout << "Second Class" << std::endl;
  //          break;
  //        case NaiveBayes::unknownClass:
  //          std::cout << "Unknown Class" << std::endl;
  //          break;
  //        default:
  //          std::cout << "ERR Classification" << std::endl;
  //          break;
  //      }
  //  
  //  
  //  
  //    } catch (InputException &e) {
  //      std::cerr << "Chyba vstupu:" << e.what() << std::endl;
  //    } catch (ClientException &e) {
  //      std::cerr << e.what() << std::endl;
  //    }




  //std::cout << web << std::endl;
  //  return 0;


  //  s->getName().getHash();
  //  std::cout << s->getName() << std::endl;
  //  std::cout << s->getHash().length() << std::endl;
  //  std::cout << s->getLocalPath() << std::endl;
  //  delete s;



  //  SText * text = new SText;
  //  text->set("Beatles were iconic rock group Liverpool, England. They frequently cited most commercially successful critically acclaimed modern history, innovative, cultural impact that helped define 1960s enormous influence that still felt today. Currently, Beatles one two  acts sell more than 1 billion records, only Elvis Presley having able achieve same feat.");
  //  //  std::map<std::string, int> wordsCountMap = text->getWordsCountMap();
  //  //  std::map<std::string, int>::iterator it;
  //  //  for (it = wordsCountMap.begin(); it != wordsCountMap.end(); it++)
  //  //    std::cout << (*it).first << " => " << (*it).second << std::endl;
  //
  //  SText * text2 = new SText;
  //  text2->set("The Beatles were an English rock band formed in Liverpool in 1960, and one of the most commercially successful and critically acclaimed acts in the history of popular music.[1] The group's best-known lineup consisted of John Lennon (rhythm guitar, vocals), Paul McCartney (bass guitar, vocals), George Harrison (lead guitar, vocals) and Ringo Starr (drums, vocals). Rooted in skiffle and 1950s rock and roll, the group later worked in many genres ranging from pop ballads to psychedelic rock, often incorporating classical and other elements in innovative ways. Their enormous popularity first emerged as Beatlemania; as their songwriting grew in sophistication by the late 1960s, they came to be perceived by many fans and cultural observers as an embodiment of the ideals shared by the era's sociocultural revolutions.\
//Initially a five-piece lineup that included Stuart Sutcliffe (bass) and Pete Best (drums), the band built their reputation playing clubs in Liverpool and Hamburg over a three-year period from 1960. Sutcliffe left the group in 1961, and Best was replaced by Starr the following year. Moulded into a professional act by manager Brian Epstein, their musical potential was enhanced by the creativity of producer George Martin. They gained popularity in the United Kingdom after their first single, Love Me Do, became a modest hit in late 1962, and they acquired the nickname the Fab Four as Beatlemania grew in Britain over the following year. By early 1964 they had become international stars, leading the British Invasion of the United States pop market. The band toured extensively around the world until August 1966, when they performed their final commercial concert. From 1966 they produced what many critics consider to be some of their finest material, including the innovative and widely influential albums Revolver (1966), Sgt. Peppers Lonely Hearts Club Band (1967), The Beatles (1968) and Abbey Road (1969). After their break-up in 1970, the ex-Beatles each enjoyed individual musical careers. Lennon was murdered in 1980, and Harrison died of cancer in 2001. McCartney and Starr remain active.\
//The Beatles are the best-selling band in history, with estimated sales of over one billion units.[2] They had more number-one albums on the UK charts and held the top spot longer than any other musical act. According to the RIAA, they have sold more albums in the US than any other group, and in 2008 they topped Billboard magazinelist of the all-time most successful Hot 100 artists. As of 2012, they hold the record for most number one hits on the Hot 100 chart with 20. They have received 7 Grammy Awards from the American National Academy of Recording Arts and Sciences, an Academy Award for Best Original Song Score and 15 Ivor Novello Awards from the British Academy of Songwriters, Composers and Authors. They were collectively included in Time magazines compilation of the 20th centurys 100 most influential people.");
  //  SText * text3 = new SText;
  //  text3->set("Early , consisted George Harrison, John Lennon, Paul McCartney, Stuart Sutcliffe, Pete Best. got its first major break playing Hamburg, Germany, at some rather seedy nightclubs, beginning 1960. While Germany, they also met Klaus Voormann, who would later design cover group’s 1966 Revolver. Stuart remained Germany after others decided return England, became engaged fellow artist Astrid Kirchherr, only die few years later brain hemorrhage.");
  //
  //
  //  SText * text4 = new SText;
  //  text4->set("Bon Jovi hard rock Sayreville, New Jersey. Fronted by lead singer namesake Jon Bon Jovi(born John Francis Bongiovi, Jr.), group originally achieved large - scale success 1980s.");
  //  SText * text5 = new SText;
  //  text5->set("So much has been said and written about the Beatles -- and their story is so mythic in its sweep -- that it's difficult to summarize their career without restating clichés that have already been digested by tens of millions of rock fans. To start with the obvious, they were the greatest and most influential act of the rock era, and introduced more innovations into popular music than any other rock band of the 20th century. Moreover, they were among the few artists of any discipline that were simultaneously the best at what they did and the most popular at what they did. Relentlessly imaginative and experimental, the Beatles grabbed a hold of the international mass consciousness in 1964 and never let go for the next six years, always staying ahead of the pack in terms of creativity but never losing their ability to communicate their increasingly sophisticated ideas to a mass audience. Their supremacy as rock icons remains unchallenged to this day, decades after their breakup in 1970.");
  //  SText * text7 = new SText;
  //  text7->set("Legendární liverpoolská skupina, zřejmě nejvýznamnější v dějinách rockové hudby. Lennon (nar. 1940) a McCartney (nar. 1942) se potkávají v létě 1957, aby spolu hráli ve skifflové skupině The Quarrymen. O rok později se k nim přidává G. Harrison (nar. 1943). V listopadu 1959 se tato skupina rozpadá. Trojice se na jaře 1960 spojuje s bubeníkem Pete Bestem (nar. 1941) a Staurtem Sutcliffem (nar. 1940), zpočátku v úloze kytaristy a zakládají skupinu Long John & The Silver Beatles, jejíž název postupně zkracují až na The Beatles.");
  //  SText * text8 = new SText;
  //  text8->set("Počátkem šedesátých let ta čtyři jména v Anglii nikdo neznal. O několik let později ležel nejslavnější kapele své doby u nohou celý svět. Všechno začalo v roce 1959 v Liverpoolu, když se čtyři úplně obyčejní kluci rozhodli, že budou nejlepší. Jenomže muzika, kterou chtěli dělat, byla hodně jiná než ta, co se denně linula z rádia i televize... Britsko-americký snímek mapuje začátky jedné z nejslavnějších kapel pop-music od okamžiku, kdy se ti čtyři kluci - ještě ovšem ve složení Lennon, Harrison, McCartney a Sutcliffe - v r. 1959 rozhodli, že chtějí dělat muziku a ustálili se na názvu The Beatles. Vychází nejen z množství materiálů, které jsou o slavné čtyřce k dispozici, ale i ze vzpomínek Peta Besta, původního bubeníka skupiny, jehož posléze Beatles nahradili Ringo Starrem. Richard Marquand, původně režisér dokumentárních snímků, tímto filmem debutoval v hrané tvorbě. Skladby Beatles z tohoto období nahrála pro film skupina RAIN.");
  //  SText * text6 = new SText;
  //  int i = 1;
  //  std::string arg;
  //  while (argv[i] != NULL) {
  //    arg += argv[i];
  //    arg += " ";
  //    i++;
  //  }
  //
  //  text6->set(utils::trim(arg));
  //
  //
  //  NaiveBayes::Index * beatles = new NaiveBayes::Index;
  //  beatles->add(text->getWordsCountMap());
  //  beatles->add(text2->getWordsCountMap());
  //  beatles->add(text3->getWordsCountMap());
  //  beatles->add(text3->getWordsCountMap());
  //  NaiveBayes::Index * nobeatles = new NaiveBayes::Index;
  //  nobeatles->add(text4->getWordsCountMap());
  //  nobeatles->add(text5->getWordsCountMap());
  //
  //
  //  NaiveBayes::Index * detection = new NaiveBayes::Index;
  //  detection->add(text8->getWordsCountMap());
  //
  //  NaiveBayes::Classificator * c = new NaiveBayes::Classificator;
  //  NaiveBayes::ClassificatorResult res = c->classificate(beatles, nobeatles, detection);
  //
  //  switch (res) {
  //    case NaiveBayes::firstClass:
  //      std::cout << "First Class" << std::endl;
  //      break;
  //    case NaiveBayes::secondClass:
  //      std::cout << "Second Class" << std::endl;
  //      break;
  //    case NaiveBayes::unknownClass:
  //      std::cout << "Unknown Class" << std::endl;
  //      break;
  //    default:
  //      std::cout << "ERR Classification" << std::endl;
  //      break;
  //  }
  //}

  //  Magick::Image image;
  //  Magick::Image blend;
  //  try {
  //    // convert bg_test.jpg \( -size 1600x1257 xc:black \) +swap -compose blend -set option:compose:args 50 -composite tmp2.png
  //    // Read a file into image object 
  //    image.read("ui/bg_test.jpg");
  //
  //    // Crop the image to specified size (width, height, xOffset, yOffset)
  //    image.opacity(10);
  //
  //    Magick::ColorRGB c(1, 1, 1);
  //    c.alpha(0.5);
  //
  //
  //    blend.fillColor(c);
  //
  //
  //    // Write the image to a file 
  //    blend.write("tmp/out.jpg");
  //  } catch (std::exception &error_) {
  //    std::cout << "Caught exception: " << error_.what() << std::endl;
  //    //    return 1;
  //  }
  //
  //  SText * t = new SText;
  //
  //  t->set("the beatles were an group");
  //
  //  std::cout << "distance:" << t->distance("were beatles") << std::endl;
  //   std::cout << "distance:" << t->distance("sting") << std::endl;
  //  std::cout << "distance:" << t->distance("sting were") << std::endl;
  //

  //  try {
  //
  //    HTTP http;
  //    http.GETImage("http://en.wikipedia.org/wiki/File:The_Fabs.JPG", "tmp/", "xxx");
  //    http.GETImage("http://upload.wikimedia.org/wikipedia/commons/thumb/d/df/The_Fabs.JPG/600px-The_Fabs.JPG", "tmp/", "xxx2");
  //  } catch (InputException &e) {
  //    std::cerr << "Chyba vstupu:" << e.what() << std::endl;
  //  } catch (ClientException &e) {
  //    std::cerr << e.what() << std::endl;
  //  }
  //  
  //    return 0;

  // spustíme instanci inteligentního klienta
  IntelligentClient(argc, argv);


  //  Thread * t = new Thread();
  //  t->create(vlakno);
  //  t->join();
  //  delete t;
  //  sleep(11);
  return 0;
}

