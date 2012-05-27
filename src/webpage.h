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

#include <list>
#include <map>
#include <utility>

#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/xpath.h>
#include <libxml/xmlstring.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libxml/tree.h>
#include <libxml/xmlversion.h>
#include "utils.h"
#include "ClientException.h"
#include "storage.h"

#include <iostream>

typedef struct {
  std::string method;
  std::string url;
  std::string params;
} searchPair;

typedef struct {
  std::string name;
  std::string url;
} linkPair;

typedef struct {
  std::string alt;
  std::string title;
  std::string context;
  std::string url;
} imgPair;

typedef struct {
  std::string title;
  std::string text;
} article;

class Webpage {
public:
  Webpage(std::string html) throw (InputException, ClientException);
  ~Webpage();
  void searchOnPage(const char * keyword) throw (ClientException);


  std::string getH1();
  std::string getTitle();
  std::string getFaviconUrl();
  std::list<imgPair>& getImgList();
  std::list<imgPair> getImgList(std::string contains);
  std::list<imgPair> getImgListExactly(std::string contains);
  std::list<imgPair> getImgListWithName(std::string contains);
  std::list<linkPair*>& getImgLinkList();
  std::list<linkPair*> getImgLinkList(std::string contains);
  std::list<linkPair*>& getLinkList();
  std::list<linkPair*> getLinkList(std::string contains);
  std::list<linkPair*> getLinkListExactly(std::string contains);

  inline int getNumberOfLinks() const {
    return linkList.size();
  }

  std::list<searchPair*>& detectSearchForms() throw (ClientException);
  std::list<article *>& getParagraphList();
  std::list<article *> getParagraphList(std::string contains);
  std::list<article *> getParagraphListByTitle(std::string contains);



private:
  std::string formUrl;
  std::string formParams;
  void parseImgList();
  int minImgContentLength;
  void parseImgLinkList();
  void parseLinkList();
  void parseParagraphList();
  void findInputHidden(htmlNodePtr formRoot);
  void findSelects(htmlNodePtr formRoot);

  void findDeepText(xmlNode * node, std::string& linkText);
  void findDeepTextWithHeaders(xmlNode * node, std::string& htmlText);
  void findDeepTextWithSeparators(xmlNode * node, std::string & linkText);
  void findLinkTextFromImgTitles(xmlNode * node, std::string& linkText);
  static bool ci_linkList(linkPair* first, linkPair* second);
  static bool comparelinkList(linkPair* first, linkPair* second);
  static bool compareURL(linkPair* first, linkPair* second);
  static bool ci_paragraph(linkPair* first, linkPair* second);
  static bool compareParagraphs(std::string first, std::string second);
  static bool ci_paragraphs(std::string first, std::string second);
  static bool ci_imgList(imgPair first, imgPair second);
  static bool compareimgList(imgPair first, imgPair second);


  std::string getExtension(std::string& url);
  void removeEndBackslash(std::string &url);
  void removeAnchorFromLink(std::string &url);

  void copyElement(xmlNode * node, std::string& htmlText);
  bool startsWithUpper(std::string& p);
  bool endsWithDot(std::string& p);
  bool detectIfIsSentence(std::string& s);
  bool detectIfIsHeader(const xmlChar * name);

  // html doc 
  htmlDocPtr doc;
  htmlDocPtr res;
  htmlDocPtr parRes;

  //kontext XPath
  xmlXPathContextPtr xpathCtx;
  xmlXPathContextPtr xpathParCtx;

  //seznam formulářů
  std::list<searchPair*> searchList;
  // list of links on images
  std::list<linkPair*> imgLinkList;
  //seznam odkazů
  std::list<linkPair*> linkList;
  //seznam s obrázků
  std::list<imgPair> imgList;
  //seznam nadpisů
  std::list<std::string*> headerList;
  // list of joinned paragraphs between headers
  std::list<article *> paragraphList;
  article * pomArticle;


};

#endif

