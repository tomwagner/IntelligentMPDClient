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
 * @file webpage.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Class for parsing HTML webpage content.
 */

#ifndef HTMLPARSER_H
#define	HTMLPARSER_H

#include "global.h"
#include <list>
#include <map>
#include <utility>

#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/HTMLparser.h>
#include <libxml2/libxml/HTMLtree.h>
#include <libxml2/libxml/xpath.h>
#include <libxml2/libxml/xmlstring.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xmlversion.h>
#include "utils.h"
#include "ClientException.h"

#include <iostream>
//typedef std::pair<std::string, std::string> searchPair;

typedef struct {
  std::string method;
  std::string url;
} searchPair;

typedef struct {
  std::string name;
  std::string url;
} linkPair;

typedef struct {
  std::string alt;
  std::string url;
} imgPair;

class Webpage {
public:
  Webpage(std::string html) throw (InputException, ClientException);
  ~Webpage();
  void searchOnPage(const char * keyword) throw (ClientException);


  std::string H1();
  std::string title();
  std::list<imgPair>& getImgList();
  std::list<linkPair*>& getLinkList();
  std::list<linkPair*> getLinkList(std::string contains);
  std::list<searchPair*>& detectSearchForms() throw (ClientException);
  std::list<std::string>& getParagraphList();
  std::list<std::string> getParagraphList(std::string contains);



private:
  std::string searchUrl;
  void parseLinkList();
  void parseParagraphList();
  void findInputHidden(htmlNodePtr element);
  void findDeepText(xmlNode * node, std::string& linkText);
  void findLinkTextFromImgTitles(xmlNode * node, std::string& linkText);
  static bool compareURL(linkPair* first, linkPair* second);
  static bool ci_paragraph(linkPair* first, linkPair* second);
  static bool compareParagraphs(std::string first, std::string second);
  static bool ci_paragraphs(std::string first, std::string second);


  // html doc 
  htmlDocPtr doc;
  htmlDocPtr res;
  
  //kontext XPath
  xmlXPathContextPtr xpathCtx;

  //seznam formulářů
  std::list<searchPair*> searchList;
  //seznam odkazů
  std::list<linkPair*> linkList;
  //seznam s obrázků
  std::list<imgPair> imgList;
  //seznam nadpisů
  std::list<std::string*> headerList;
  //seznam odstavců
  std::list<std::string> paragraphList;



};

#endif

