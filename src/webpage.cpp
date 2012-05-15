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
 * @file webpage.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Class for parsing HTML webpage content.
 */

#define DEBUG 0
#define DEBUGHideParsing 1


#include "webpage.h"

using namespace DataStorage;


Webpage::Webpage(std::string html) throw (InputException, ClientException)
: formUrl(""), xpathCtx(NULL), minImgContentLength(300) {

  // zkontrolujeme vstup
  if (html.empty()) {
    InputException e("HTMLParser ERROR: No input found!");
    throw e;
  }

  // zakážeme výstup na std::cerr
  // z knihovny se vypisují chyby parsování, které nejsou důležité
#if DEBUGHideParsing
  utils::setupCERRedirection();
  utils::redirectStdErrToNull();
#endif

  // naparsujeme dokument
  doc = htmlParseDoc((xmlChar*) html.data(), NULL);

  // obnovíme std::cerr
#if DEBUGHideParsing
  utils::restoreStdErr();
#endif

  if (doc == NULL) {
    ClientException e("HTMLParser ERROR: No html found on input!");
    throw e;
    return;
  }

  // zjistím kódování
  std::string encoding;
  const char* e = (const char*) htmlGetMetaEncoding(doc);

  if (e != NULL)
    encoding.assign(e);
#if DEBUG
  std::cout << "Encoding:" << (const char*) encoding.c_str() << std::endl;
#endif

  // pokud nelze kódování detekovat, použijeme implicitně utf-8
  if (encoding.empty()) {
    encoding.assign("utf-8");

    // uvolním doc
    xmlFreeDoc(doc);

    // z knihovny se vypisují chyby parsování, které nejsou důležité
#if DEBUGHideParsing
    utils::setupCERRedirection();
    utils::redirectStdErrToNull();
#endif

    doc = htmlParseDoc((xmlChar*) html.data(), encoding.c_str());

    // obnovíme std::cerr
#if DEBUGHideParsing  
    utils::restoreStdErr();
#endif
#if DEBUG
    std::cout << "Vnucený Encoding:" << (const char*) encoding.c_str() << std::endl;
#endif
  } else {
    // nastavím kódování dokumentu
    htmlSetMetaEncoding(doc, BAD_CAST encoding.c_str());
  }

  // apply xslt tranformation to remove <script>
  xsltStylesheetPtr noscriptStyle = xsltParseStylesheetFile(BAD_CAST "xslt/noscripts_style");
  xsltStylesheetPtr paragraphStyle = xsltParseStylesheetFile(BAD_CAST "xslt/paragraph_style");
  const char * params[1];
  params[0] = NULL;

  res = xsltApplyStylesheet(noscriptStyle, doc, params);

  // apply stylesheet to easy paragraph getting
  parRes = xsltApplyStylesheet(paragraphStyle, doc, params);

  // free old doc
  xmlFreeDoc(doc);

  // create xpath contexts
  xpathCtx = xmlXPathNewContext(res);
  xpathParCtx = xmlXPathNewContext(parRes);

  // parse images
  parseImgList();

  // parse links
  parseLinkList();

  // parse img link list
  parseImgLinkList();

  // parse paragraphs
  parseParagraphList();

  // clean stylesheets
  xsltFreeStylesheet(noscriptStyle);
  xsltFreeStylesheet(paragraphStyle);

}


Webpage::~Webpage() {
#if DEBUG
  std::cout << "HTMLParser destruktor" << std::endl;
#endif
  // destrukce seznamu vyhledávacích formulářů
  std::list<searchPair*>::iterator sIt;
  for (sIt = searchList.begin(); sIt != searchList.end(); sIt++) {
    delete (*sIt);
  }
  // destrukce seznamu odkazů
  std::list<linkPair*>::iterator lIt;
  for (lIt = linkList.begin(); lIt != linkList.end(); lIt++) {
    delete (*lIt);
  }

  // delete paragraph list
  std::list<article*>::iterator aIt;
  for (aIt = paragraphList.begin(); aIt != paragraphList.end(); aIt++) {
    delete (*aIt);
  }


  // uvolníme document res vzniklý po transformaci XSLT 
  xmlFreeDoc(res);
  xmlFreeDoc(parRes);

  // uvolníme kontext XPath
  xmlXPathFreeContext(xpathCtx);
  //
  //  // vyčistíme xslt a xml parsery
  xsltCleanupGlobals();
#if DEBUG
  std::cout << "HTMLParser destructor end" << std::endl;
#endif
}


std::list<searchPair*>& Webpage::detectSearchForms() throw (ClientException) {
  std::string formMethod;
  std::string formAction;
  std::string inputName;

  //

  if (xpathCtx == NULL) {
    ClientException e("HTMLParser ERROR: No xmlXPathContextPtr context found!");
    throw e;
  }

  xmlXPathObjectPtr screenXPathObj = xmlXPathEvalExpression(BAD_CAST "//form//input[@type='text']", xpathCtx);

  xmlNodeSetPtr screenNodes = screenXPathObj->nodesetval;

  if (screenNodes == NULL) {
    //vyčistíme objekt XPath
    xmlXPathFreeObject(screenXPathObj);

    return searchList;
  }

  for (int j = 0; j < screenNodes->nodeNr; j++) {

    // nalezený input type="text"
    xmlNodePtr input = screenNodes->nodeTab[j];
#if DEBUG
    printf("Form xxx Name = %s\n", input->name);
#endif
    for (xmlAttrPtr attr = input->properties; attr != NULL; attr = attr->next) {
      if (xmlStrcasecmp(attr->name, BAD_CAST "name") == 0) {
        inputName.assign((const char*) attr->children->content);
      }
    }
#if DEBUG   
    printf("Form Name = %s\n", inputName.data());
#endif
    // hledáme form uzel
    xmlNodePtr formNode = input->parent;

    // pokud není přímý předchůdce form, tak ho najdeme
    while ((xmlStrcasecmp(formNode->name, BAD_CAST "form") != 0)) {
      formNode = formNode->parent;
      // pokud je již konec stromu, končíme
      if (formNode == NULL) break;
    }

    // zjistíme form atributy
    if (xmlStrcasecmp(formNode->name, BAD_CAST "form") == 0) {
      // zpracuji atributy formu
      for (xmlAttrPtr attr = formNode->properties; attr != NULL; attr = attr->next) {
        if (xmlStrcasecmp(attr->name, BAD_CAST "method") == 0) {
          formMethod.assign((const char*) attr->children->content);
        } else if (xmlStrcasecmp(attr->name, BAD_CAST "action") == 0) {
          formAction.assign((const char*) attr->children->content);
        }
      }
    }
#if DEBUG
    printf("Form Method = %s, Action = %s\n", formMethod.data(), formAction.data());
#endif
    // set search url
    formUrl.assign(formAction);

    // if action contains parameters
    // we move them from form url
    std::size_t found = -1;
    if ((found = formAction.find("?")) != -1) {
#if DEBUG
      std::cout << "Found parameter on position:" << (int) found << std::endl;
#endif
      // set Form url
      formUrl.assign(formAction.substr(0, found));
      // set search params
      formParams.assign(formAction.substr(found + 1)).append("&");
    }

    //zjištění skrytých atributů v formu, nutných k odeslání
    findInputHidden(formNode);

    // find select elements in form
    findSelects(formNode);

    //připojíme do adresy poslední část a to inputbox
    formParams.append(inputName).append("=");

    // pokud není uvedena metoda, implicitně vracíme GET
    if (formMethod.empty()) formMethod.assign("GET");

    // implicitně vracíme metodu velkými písmeny
    formMethod = utils::toUpper(formMethod);

    // vytvoříme záznam
    searchPair *p = new searchPair;
    p->method = formMethod;
    p->url = formUrl;
    p->params = formParams;

#if DEBUG
    std::cout << "\n\nFORM URL: " << p->url << "\nmethod: " << p->method << "\nparams: " << p->params << "\n\n" << std::endl;
#endif
    searchList.push_back(p);

    // IMPORTANT clear variables to next iteration
    formUrl.clear();
    formParams.clear();
  }

  //vyčistíme objekt XPath
  xmlXPathFreeObject(screenXPathObj);

  return searchList;
}


/**
 * Porovná shodnost url adresy a jména odkazu, vrací true, pokud jsou páry stejné
 * @param first
 * @param second
 * @return 
 */
bool Webpage::compareURL(linkPair* first, linkPair* second) {

  // porovnáváme shodnost url adresy a alespoň délku titulku TODO (možno zesložitit)
  if ((utils::ci_stringCompare(first->url, second->url)) &&
          (first->name.length() == second->name.length())) {

    // vymažeme stejný záznam z paměti
    delete second;
    // nalezena shoda
    return true;
  }
  return false;
}


/**
 * Privátní porovnávací metoda pro dvojici odkazu
 * @param first
 * @param second
 * @return 
 */
bool Webpage::ci_paragraph(linkPair* first, linkPair* second) {
  unsigned int i = 0;


  while ((i < first->url.length()) && (i < second->url.length())) {
    if (tolower(first->url[i]) < tolower(second->url[i])) return true;
    else if (tolower(first->url[i]) > tolower(second->url[i])) return false;
    ++i;
  }
  if (first->url.length() < second->url.length()) return true;
  return false;
}


/**
 * Metoda vrací seznam odkazů, obsahující zadaný řetězec
 * @param contains
 * @return 
 */
std::list<linkPair*> Webpage::getLinkList(std::string contains) {
  std::list<linkPair*> localLinkList;
  std::list<linkPair*>::iterator it;
  for (it = linkList.begin(); it != linkList.end(); it++) {
    if (utils::ci_stringContains((*it)->name, contains)) {
      localLinkList.push_back((*it));
      //std::cout << "Link.:" << (*it)->name.data() << " url: '" << (*it)->url.data() << "'" << std::endl;
    }
  }


  return localLinkList;
}


std::list<linkPair*> Webpage::getLinkListExactly(std::string contains) {
  std::list<linkPair*> localLinkList;
  std::list<linkPair*>::iterator it;
  for (it = linkList.begin(); it != linkList.end(); it++) {

    if ((*it)->name.compare(contains) == 0) {
      localLinkList.push_back((*it));
    }
  }

  // seřadíme a vyházíme duplicity
  localLinkList.sort(ci_paragraph);
  localLinkList.unique(compareURL);
  return localLinkList;
}


///**
// * Compare method for two links
// * @param first
// * @param second
// * @return 
// */
//bool Webpage::ci_linkList(linkPair * first, linkPair * second) {
//  unsigned int i = 0;
//
//
//  while ((i < first->url.length()) && (i < second->url.length())) {
//    if (tolower(first->url[i]) < tolower(second->url[i])) return true;
//    else if (tolower(first->url[i]) > tolower(second->url[i])) return false;
//    ++i;
//  }
//  if (first->url.length() < second->url.length()) return true;
//  return false;
//}
//
//
//bool Webpage::comparelinkList(linkPair * first, linkPair * second) {
//
//  // porovnáváme shodnost url adresy a alespoň délku titulku TODO (možno zesložitit)
//  if ((utils::ci_stringCompare(first->url, second->url))
//  && (utils::ci_stringCompare(first->name, second->name))) {
//
//    // delete same row
//    delete second;
//    // nalezena shoda
//    return true;
//  }
//  return false;
//}


/**
 * Metoda vrací titulek stránky
 * @return 
 */
std::string Webpage::getTitle() {
  std::string title;
  xmlXPathObjectPtr screenXPathObj = xmlXPathEvalExpression(BAD_CAST "//head//title", xpathCtx);
  xmlNodeSetPtr screenNodes = screenXPathObj->nodesetval;

  findDeepText(screenNodes->nodeTab[0]->children, title);

#if DEBUG
  std::cout << "Title.:" << title << std::endl;
#endif

  //vyčistíme objekt XPath
  xmlXPathFreeObject(screenXPathObj);

  return title;
}


/**
 * Function returns H1 header of webpage
 * @return 
 */
std::string Webpage::getH1() {
  std::string h1;
  xmlXPathObjectPtr screenXPathObj = xmlXPathEvalExpression(BAD_CAST "//h1", xpathCtx);
  if (screenXPathObj == NULL) return h1;
  xmlNodeSetPtr screenNodes = screenXPathObj->nodesetval;

  if (screenNodes->nodeTab != NULL) {
    // we are estimate more H1 headers (concatenate them)
    for (int j = 0; j < screenNodes->nodeNr; j++) {
      if (j != 0) h1 += " ";
      findDeepText(screenNodes->nodeTab[j]->children, h1);
    }
  }
#if DEBUG
  std::cout << "H1.:" << h1 << std::endl;
#endif

  // clean xpath
  xmlXPathFreeObject(screenXPathObj);

  return h1;
}


/**
 * Porovná shodnost url adresy a jména odkazu, vrací true, pokud jsou páry stejné
 * @param first
 * @param second
 * @return 
 */
bool Webpage::compareParagraphs(std::string first, std::string second) {

  // porovnáváme shodnost url adresy a alespoň délku titulku TODO (možno zesložitit)
  if (utils::ci_stringCompare(first, second)) {
    // nalezena shoda
    return true;
  }
  return false;
}


/**
 * Privátní porovnávací metoda pro dvojici odkazu
 * @param first
 * @param second
 * @return 
 */
bool Webpage::ci_paragraphs(std::string first, std::string second) {
  unsigned int i = 0;


  while ((i < first.length()) && (i < second.length())) {
    if (tolower(first[i]) < tolower(second[i])) return true;
    else if (tolower(first[i]) > tolower(second[i])) return false;
    ++i;
  }
  if (first.length() < second.length()) return true;
  return false;
}


/**
 * Metoda nalezne odstavce textu v daném dokumentu
 */
void Webpage::parseParagraphList() {

  xmlXPathObjectPtr screenXPathObj = xmlXPathEvalExpression(BAD_CAST "//body", xpathParCtx);
  xmlNodeSetPtr screenNodes = screenXPathObj->nodesetval;

  std::string s;

  //  if (screenXPathObj == NULL) return h1; 

  xmlNodePtr node = screenNodes->nodeTab[0]->children;

  pomArticle = new article;
  while (node != NULL) {
    //std::cout << "node:" << node->name << std::endl;
    findDeepTextWithHeaders(node, s);
    node = node->next;
  }
  paragraphList.push_back(pomArticle);
}


bool Webpage::startsWithUpper(std::string& p) {
  p = utils::removeMultipleWhiteSpaces(p);
  p = utils::trim(p);

  if (utils::toUpper(p.substr(0, 1)).compare(p.substr(0, 1)) == 0) {
    return true;
  }
  return false;
}


bool Webpage::endsWithDot(std::string& p) {
  p = utils::removeMultipleWhiteSpaces(p);
  p = utils::trim(p);

  // if sentence ends with dot, true
  if (p.substr(p.length() - 1, 1).compare(".") == 0) {
    if (p.substr(p.length() - 2, 1).compare(".") != 0)
      return true;
  }
  return false;
}


bool Webpage::detectIfIsSentence(std::string& p) {
  p = utils::removeMultipleWhiteSpaces(p);
  p = utils::trim(p);

  if (p.length() < 3) return false;

  //  std::cout << "---------------------" << std::endl;
  //  std::cout << p << std::endl;
  //  std::cout << p.substr(0, 1) << std::endl;
  //  std::cout << p.substr(p.length() - 1, 1) << std::endl;
  //  std::cout << "---------------------" << std::endl;

  if (utils::toUpper(p.substr(0, 1)).compare(p.substr(0, 1)) == 0) {
    // if sentence ends with dot, true
    //    if ((p.substr(p.length() - 1, 1).compare(".") == 0) || (p.substr(p.length() - 1, 1).compare("!") == 0)) {
    //      if (p.substr(p.length() - 2, 1).compare(".") != 0)
    //        return true;
    //    }
    if (p.find_last_of(".") != std::string::npos)
      return true;
  }
  return false;
}


std::string Webpage::getFaviconUrl() {
  std::string favicon;
  xmlXPathObjectPtr screenXPathObj = xmlXPathEvalExpression(BAD_CAST "//head//link[@rel=\"shortcut icon\"]", xpathCtx); //  //@href[contains(.,'byt')]
  xmlNodeSetPtr screenNodes = screenXPathObj->nodesetval;

  // search attributes
  if (screenNodes == NULL) return favicon;
  if (screenNodes->nodeTab == NULL) return favicon;
  if (screenNodes->nodeTab[0] == NULL) return favicon;
  for (xmlAttrPtr attr = screenNodes->nodeTab[0]->properties; attr != NULL; attr = attr->next) {
    if (xmlStrcasecmp(attr->name, BAD_CAST "href") == 0) {
      // set favicon url
      favicon.assign((const char*) attr->children->content);
    }
  }

#if DEBUG
  std::cout << "Favicon:" << favicon << std::endl;
#endif

  //vyčistíme objekt XPath
  xmlXPathFreeObject(screenXPathObj);

  return favicon;
}


void Webpage::copyElement(xmlNode * node, std::string& htmlText) {
  if (xmlStrcasecmp(node->name, BAD_CAST "text") != 0) {
    htmlText.append("<");
    htmlText.append((char*) node->name);

    if (node->properties != NULL) {
      for (xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {
        if (xmlStrcasecmp(attr->name, BAD_CAST "itemprop") == 0) continue;
        htmlText.append(" ");
        htmlText.append((char*) attr->name);
        htmlText.append("=\"");
        htmlText.append((char*) attr->children->content);
        htmlText.append("\"");
      }
    }

    htmlText.append(">");
  }

  if (node->children != NULL)
    copyElement(node->children, htmlText);
  if (node->content != NULL)
    htmlText.append((char*) node->content);

  if (xmlStrcasecmp(node->name, BAD_CAST "text") != 0) {
    htmlText.append("</");
    htmlText.append((char*) node->name);
    htmlText.append(">");
  }
}


/**
 * Recursive function to find all paragraph in webpage.
 * need to init pomArticle before run!
 * @param node
 * @param linkText
 * @param foundHeader
 */
void Webpage::findDeepTextWithHeaders(xmlNode * node, std::string& linkText) {
  if (node == NULL) return;
  //std::cout << node->name << std::endl;
  //if (pomArticle == NULL) pomArticle = new article;

  // append header
  if (detectIfIsHeader(node->name)) {
    if (node->children != NULL) {

      std::string s;
      if (node->children->content != NULL) {
        s = (const char*) node->children->content;
        s = utils::trim(s);
        // if (pomArticle->text.length() > 0) {
        paragraphList.push_back(pomArticle);
        pomArticle = new article;
#if DEBUG
        std::cout << "nadpis: " << s << std::endl << std::endl;
#endif
        pomArticle->title = s;

        //if (node->children->content != NULL) linkText.append("<big>").append(s).append("</big>\n");
      }
    }
  }
  //    if ((xmlStrcasecmp(node->name, BAD_CAST "br") == 0) ) {
  //      linkText.append("\n");
  //    }

  // append text
  if (xmlStrcasecmp(node->name, BAD_CAST "text") == 0) {
    if (node->content != NULL) {

      std::string s;
      s = (const char*) node->content;
      s = utils::trim(s);
      if (detectIfIsSentence(s)) {
#if DEBUG
        std::cout << "text: " << s << std::endl << std::endl;
#endif
        pomArticle->text += s;
      }
      //if (detectIfIsSentence(s)) linkText.append(s).append("\n");
    }
  }

  // no children, return
  if (node->children == NULL) return;

  // search in childern
  for (xmlNode* n = node->children; n != NULL; n = n->next) {
    findDeepTextWithHeaders(n, linkText);
  }

}


bool Webpage::detectIfIsHeader(const xmlChar * name) {
  std::string s;
  s = (char*) name;

  if ((s.length() != 2)) return false;
  if (s.substr(0, 1).compare("h") == 0) return true;

  return false;
}


/**
 * Metoda vrací seznam odstavců na dané webové stránce
 * @return 
 */
std::list<article *>& Webpage::getParagraphList() {


  return paragraphList;
}


/**
 * Metoda vrací seznam odstavců na dané webové stránce
 * @return 
 */
std::list<article *> Webpage::getParagraphList(std::string contains) {

  std::list<article *> localParagraphList;
  std::list<article *>::iterator it;
  for (it = paragraphList.begin(); it != paragraphList.end(); it++) {
    if (utils::ci_stringContains((*it)->text, contains)) {
      localParagraphList.push_back((*it));
    }
  }

  return localParagraphList;
}


std::list<article *> Webpage::getParagraphListByTitle(std::string contains) {
  std::list<article *> localParagraphList;
  std::list<article *>::iterator it;
  for (it = paragraphList.begin(); it != paragraphList.end(); it++) {
    if (utils::ci_stringContains((*it)->title, contains)) {
      localParagraphList.push_back((*it));
    }
  }

  return localParagraphList;
}


std::string Webpage::getExtension(std::string & url) {
  std::string s;
  int len = url.length();

  if (len < 4) return s;
  s = url.substr(url.length() - 4, 4);


  return s;
}


void Webpage::removeEndBackslash(std::string & url) {
  //  std::cout << url.substr(url.length() - 1, 1) << std::endl;
  if (url.length() == 0) return;

  if (url.substr(url.length() - 1, 1).compare("/") == 0) {
    url = url.substr(0, url.length() - 1);
  }
}


void Webpage::removeAnchorFromLink(std::string & url) {
  size_t found;
  found = url.find("#");

  if (found == std::string::npos) return;

  url = url.substr(0, found);
}


void Webpage::parseImgLinkList() {
  //  we add hypertext links to images
  xmlXPathObjectPtr screenXPathObj = xmlXPathEvalExpression(
          BAD_CAST "//body//a[@href[not(contains(.,'.jpg'))"
          "and not(contains(.,'.png')) "
          "and not(contains(.,'.gif'))"
          //          "and not(contains(.,'.#'))"
          "and not(contains(.,'http://'))"
          "]]/img", xpathCtx);
  xmlNodeSetPtr screenNodes = screenXPathObj->nodesetval;

  if (screenNodes == NULL) return;

#if DEBUG
  std::cout << "Celkem nalezeno:" << screenNodes->nodeNr << std::endl;
#endif
  for (int j = 0; j < screenNodes->nodeNr; j++) {
    //    printf("Obrázky %s\n", screenNodes->nodeTab[j]->name);
    linkPair * i = new linkPair;
    if (screenNodes->nodeTab[j]->parent == NULL) continue;
    for (xmlAttrPtr attr = screenNodes->nodeTab[j]->parent->properties; attr != NULL; attr = attr->next) {

      // hledám atributy
      if (xmlStrcasecmp(attr->name, BAD_CAST "href") == 0) {
        // url
        i->url.assign((const char*) attr->children->content);
      }
    }

    for (xmlAttrPtr attr = screenNodes->nodeTab[j]->properties; attr != NULL; attr = attr->next) {

      // hledám atributy
      if (xmlStrcasecmp(attr->name, BAD_CAST "alt") == 0) {
        // url
        i->name.append((const char*) attr->children->content);
        removeAnchorFromLink(i->url);
      }

      if (xmlStrcasecmp(attr->name, BAD_CAST "title") == 0) {
        // url
        i->name.append((const char*) attr->children->content);
        removeAnchorFromLink(i->url);
      }
    }


#if DEBUG
    std::cout << i->url << " name: " << i->name << std::endl;
#endif

    removeAnchorFromLink(i->url);
    removeEndBackslash(i->url);
    if ((i->url.empty())) continue;
    imgLinkList.push_back(i);
  }

  // clean XPath object
  xmlXPathFreeObject(screenXPathObj);

  // seřadíme a vyházíme duplicity
  imgLinkList.sort(ci_paragraph);
  imgLinkList.unique(compareURL);

#if DEBUG


  std::cout << "\nAfter sort and duplicit remove : " << imgLinkList.size() << std::endl;
  std::list<linkPair *>::iterator it;
  for (it = imgLinkList.begin(); it != imgLinkList.end(); it++) {
    //      std::cout << "Img.:" << (*it).url << std::endl;


    std::cout << (*it)->url << " len: " << (*it)->url.length() << " name: " << (*it)->name << std::endl;
  }
#endif

}


/**
 * Function returns links list to webpages which probably contains big images.
 * @return 
 */
std::list<linkPair*>& Webpage::getImgLinkList() {
  return imgLinkList;
}

//
//std::list<linkPair*> Webpage::getImgLinkList(std::string contains) {
//  std::list<linkPair*> localLinkList;
//  std::list<linkPair*>::iterator it;
//  for (it = linkList.begin(); it != linkList.end(); it++) {
//
//    if ((*it)->name.compare(contains) == 0) {
//      localLinkList.push_back((*it));
//    }
//  }
//
//  // seřadíme a vyházíme duplicity
//  localLinkList.sort(ci_paragraph);
//  localLinkList.unique(compareURL);
//  return localLinkList;
//}


void Webpage::parseImgList() {
  // projdeme všechny img prvky v html souboru
  xmlXPathObjectPtr screenXPathObj = xmlXPathEvalExpression(BAD_CAST "//body//img[@src[contains(.,'.jpg') or contains(.,'.JPG') or contains(.,'.jpeg') or contains(.,'.JEPG')]]", xpathCtx);
  xmlNodeSetPtr screenNodes = screenXPathObj->nodesetval;
  for (int j = 0; j < screenNodes->nodeNr; j++) {

    imgPair i;
    for (xmlAttrPtr attr = screenNodes->nodeTab[j]->properties; attr != NULL; attr = attr->next) {

      // hledám atributy
      if (xmlStrcasecmp(attr->name, BAD_CAST "src") == 0) {

        // url
        i.url.assign((const char*) attr->children->content);

#if DEBUG
        if (attr->children != NULL)
          printf("Image %s\n", attr->children->content);
#endif
      } else if (xmlStrcasecmp(attr->name, BAD_CAST "alt") == 0) {
#if DEBUG
        printf("Image alt %s\n", attr->children->content);
#endif       
        i.alt.assign((const char*) attr->children->content);
      } else if ((xmlStrcasecmp(attr->name, BAD_CAST "title") == 0)) {
#if DEBUG
        printf("Image title %s\n", attr->children->content);
#endif   
        i.title.assign((const char*) attr->children->content);
      }
    } // node attr end


    // if we have no text from attributes
    //    if ((i.alt.empty()) && (i.title.empty())) {

    xmlNode * contextNode = screenNodes->nodeTab[j]->parent;

    while ((i.context.length() < minImgContentLength) && (contextNode != NULL)) {
      findDeepTextWithSeparators(contextNode, i.context);
      contextNode = contextNode->parent;
    }
    // substring to min Img Content Length
    i.context = i.context.substr(0, minImgContentLength);

    i.context = utils::removeMultipleWhiteSpaces(i.context);
    utils::trim(i.context);
    //    }



    // filter empty and gif images
    if ((i.url.empty())) continue;
    //      if ((getExtension(i.url) != ".png") && (getExtension(i.url) != ".jpg")) continue;

#if DEBUG
    std::cout << i.url << " alt: " << i.alt << " title: " << i.title << std::endl;
#endif
    imgList.push_back(i);
  } //nodes end
  // clean XPath object
  xmlXPathFreeObject(screenXPathObj);




  //  we add hypertext links to images
  screenXPathObj = xmlXPathEvalExpression(BAD_CAST "//body//a[@href[contains(.,'.jpg') or contains(.,'.JPG') or contains(.,'.jpeg') or contains(.,'.JEPG')]]", xpathCtx);
  screenNodes = screenXPathObj->nodesetval;
  for (int j = 0; j < screenNodes->nodeNr; j++) {
    //    printf("Obrázky %s\n", screenNodes->nodeTab[j]->name);
    imgPair i;
    for (xmlAttrPtr attr = screenNodes->nodeTab[j]->properties; attr != NULL; attr = attr->next) {

      // hledám atributy
      if (xmlStrcasecmp(attr->name, BAD_CAST "href") == 0) {
        // url
        i.url.assign((const char*) attr->children->content);
#if DEBUG
        if (attr->children != NULL)
          printf("Image %s\n", attr->children->content);
#endif
      } else if (xmlStrcasecmp(attr->name, BAD_CAST "alt") == 0) {
#if DEBUG
        printf("Image alt %s\n", attr->children->content);
#endif       
        i.alt.assign((const char*) attr->children->content);
      } else if ((xmlStrcasecmp(attr->name, BAD_CAST "title") == 0)) {
#if DEBUG
        printf("Image title %s\n", attr->children->content);
#endif   
        i.title.assign((const char*) attr->children->content);
      }
    } // attr end

    // if we have no text from attributes
    //    if ((i.alt.empty()) && (i.title.empty())) {
    xmlNode * contextNode = screenNodes->nodeTab[j]->parent;

    while ((i.context.length() < minImgContentLength) && (contextNode != NULL)) {
      findDeepTextWithSeparators(contextNode, i.context);
      contextNode = contextNode->parent;
    }
    // substring to min Img Content Length
    i.context = i.context.substr(0, minImgContentLength);

    i.context = utils::removeMultipleWhiteSpaces(i.context);
    utils::removeTabulators(i.context);
    utils::trim(i.context);
    //    }

#if DEBUG
    std::cout << i.url << " alt: " << i.alt << " title: " << i.title << std::endl;
#endif
    if ((i.url.empty())) continue;
    imgList.push_back(i);
  }

  // clean XPath object
  xmlXPathFreeObject(screenXPathObj);

  // seřadíme a vyházíme duplicity
  imgList.sort(ci_imgList);
  imgList.unique(compareimgList);

#if DEBUG

  std::cout << "\nAfter sort and duplicit remove" << std::endl;
  std::list<imgPair>::iterator it;
  for (it = imgList.begin(); it != imgList.end(); it++) {
    //      std::cout << "Img.:" << (*it).url << std::endl;


    std::cout << (*it).url << " xalt: " << (*it).alt << " xtitle: " << (*it).title << std::endl;
  }
#endif
}


/**
 * Function returns sorted uniq image list of webpage
 * Removes images different from jpg or png
 * @return std::list<imgPair>&
 */
std::list<imgPair>& Webpage::getImgList() {
  return imgList;
}


std::list<imgPair> Webpage::getImgList(std::string contains) {
  std::list<imgPair> localImgList;
  std::list<imgPair>::iterator it;
  for (it = imgList.begin(); it != imgList.end(); it++) {
    if ((utils::ci_stringContains((*it).alt, contains)) ||
            (utils::ci_stringContains((*it).title, contains)) ||
            (utils::ci_stringContains((*it).context, contains))) {
      localImgList.push_back((*it));
    }
  }

  return localImgList;
}


std::list<imgPair> Webpage::getImgListWithName(std::string contains) {
  std::list<imgPair> localImgList;
  std::list<imgPair>::iterator it;
  for (it = imgList.begin(); it != imgList.end(); it++) {
    if (utils::ci_stringContains((*it).url, contains)) {
      localImgList.push_back((*it));
    }
  }

  return localImgList;
}


std::list<imgPair> Webpage::getImgListExactly(std::string contains) {
  std::list<imgPair> localImgList;
  std::list<imgPair>::iterator it;
  for (it = imgList.begin(); it != imgList.end(); it++) {

    if (((*it).alt.compare(contains) == 0) ||
            ((*it).title.compare(contains) == 0)) {
      localImgList.push_back((*it));
    }
  }


  return localImgList;
}


/**
 * Privátní porovnávací metoda pro dvojici odkazu
 * @param first
 * @param second
 * @return 
 */
bool Webpage::ci_imgList(imgPair first, imgPair second) {
  unsigned int i = 0;


  while ((i < first.url.length()) && (i < second.url.length())) {
    if (tolower(first.url[i]) < tolower(second.url[i])) return true;
    else if (tolower(first.url[i]) > tolower(second.url[i])) return false;
    ++i;
  }
  if (first.url.length() < second.url.length()) return true;


  return false;
}


bool Webpage::compareimgList(imgPair first, imgPair second) {

  // porovnáváme shodnost url adresy a alespoň délku titulku TODO (možno zesložitit)
  if ((utils::ci_stringCompare(first.url, second.url))
          ) {


    return true;
  }
  return false;
}


/**
 * Funkce nalezne text v potomcích uzlu node a vrací jej 
 * @param node
 * @param linkText
 */
void Webpage::findDeepText(xmlNode * node, std::string & linkText) {
  if (node == NULL) return;
  if (node->content != NULL) linkText.append((const char*) node->content);

  // no childrens, return
  if (node->children == NULL) return;

  // projdeme všechny děti uzlu a hledáme textové informace
  for (xmlNode* n = node->children; n != NULL; n = n->next) {
    findDeepText(n, linkText);
  }
}


void Webpage::findDeepTextWithSeparators(xmlNode * node, std::string & linkText) {
  if (node == NULL) return;
  if (node->content != NULL) linkText.append((const char*) node->content).append(" ");

  // no childrens, return
  if (node->children == NULL) return;

  // projdeme všechny děti uzlu a hledáme textové informace
  for (xmlNode* n = node->children; n != NULL; n = n->next) {
    findDeepTextWithSeparators(n, linkText);
  }
}


/**
 * Rekurzivní metoda hledá informace z titulků obrázků. Důležité např. při odkazu, který je
 * umístěn na obrázku a okolí neobsahuje textovou informaci.
 * Doporučuje: volání pokud se nenalezne v potomcích žádná textová informace.
 * @param node - uzel, ve kterém hledáme
 * @param linkText - vracený text
 */
void Webpage::findLinkTextFromImgTitles(xmlNode * node, std::string & imgTitle) {
  if (node->children == NULL) return;
  std::string str;

  // projdeme všechny potomky uzlu a hledáme textové informace z obrázků
  for (xmlNode* n = node->children; n != NULL; n = n->next) {
    findLinkTextFromImgTitles(n, imgTitle);
    // pokud hledáme pouze obrázky
    if (xmlStrcasecmp(n->name, BAD_CAST "img") == 0) {
      // projedeme atributy, hledáme title
      for (xmlAttrPtr attr = n->properties; attr != NULL; attr = attr->next) {
        if ((xmlStrcasecmp(attr->name, BAD_CAST "title") == 0) ||
                (xmlStrcasecmp(attr->name, BAD_CAST "alt") == 0)) {
          // našel jsem title
          if (attr->children->content != NULL) {


            str.assign((const char*) attr->children->content);
            imgTitle.append(str);
#if DEBUG
            //printf("<%s>\"%s\" \n", n->name, str.c_str());
#endif
          }
        }
      }
    }
  }
}


/**
 * Funkce na rozparsování seznamu odkazů z dané webové stránky
 */
void Webpage::parseLinkList() {
  xmlXPathObjectPtr screenXPathObj = xmlXPathEvalExpression(BAD_CAST "//@href[normalize-space(.) and not(contains(.,'.jpg'))]", xpathCtx); //not(contains(.,'.png'))
  xmlNodeSetPtr screenNodes = screenXPathObj->nodesetval;
  for (int j = 0; j < screenNodes->nodeNr; j++) {

    // chceme pouze prvky typu a - odkazy
    if (xmlStrcasecmp(screenNodes->nodeTab[j]->parent->name, BAD_CAST "a") == 0) {
      linkPair *l = new linkPair;
      //printf("Odkazy Text: %s, \n",screenNodes->nodeTab[j]->parent->children->content);
      if (screenNodes->nodeTab[j]->parent->content != NULL)
        l->name.assign((const char*) screenNodes->nodeTab[j]->parent->children->content);
      l->url.assign((const char*) screenNodes->nodeTab[j]->children->content);
      //printf("Odkazy Text: %s, \n", screenNodes->nodeTab[j]->parent->content);

      std::string linkText;

      // zabředneme do potomků a hledáme text ohraničený odkazem
      findDeepTextWithSeparators(screenNodes->nodeTab[j]->parent, linkText);

      // remove white spaces
      linkText = utils::removeMultipleWhiteSpaces(linkText);
      
      // trim
      linkText = utils::trim(linkText);

      // pokud nenalezneme text, užijeme text z titulků a altů obrázků
      if (linkText.empty()) {
        findLinkTextFromImgTitles(screenNodes->nodeTab[j]->parent, linkText);
      }

      // ořežeme bílé znaky ze začátku a konce
      linkText = utils::trim(linkText);


      // uložíme titulek
      l->name.assign(linkText);

#if DEBUG
      //printf("\"%s\" url:%s\n", l->name.c_str(), l->url.c_str());
#endif
      // uložíme do seznamu
      linkList.push_back(l);
    }
  }

  //vyčistíme objekt XPath
  xmlXPathFreeObject(screenXPathObj);

  // seřadíme a vyházíme duplicity
  linkList.sort(ci_paragraph);
  linkList.unique(compareURL);
}


/**
 * Funkce vrací referenci na seznam odkazů
 * @return 
 */
std::list<linkPair*>& Webpage::getLinkList() {
  return linkList;
}


/**
 * Recurisve method to find all input elements type="hidden" and type="submit"
 * in form.
 * @param element
 */
void Webpage::findInputHidden(htmlNodePtr formRoot) {

  for (htmlNodePtr node = formRoot; node != NULL; node = node->next) {
    if (node->type == XML_ELEMENT_NODE) {
      if (xmlStrcasecmp(node->name, BAD_CAST "input") == 0) {
        bool hiddenFound = false;
        std::string attrName;
        std::string attrValue;
        for (xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {

          if (xmlStrcasecmp(attr->name, BAD_CAST "type") == 0) {
            // našel jsem hidden input
            if ((xmlStrcasecmp(attr->children->content, BAD_CAST "hidden") == 0) ||
                    (xmlStrcasecmp(attr->children->content, BAD_CAST "submit") == 0)) {
              hiddenFound = true;
            }
          } else if (xmlStrcasecmp(attr->name, BAD_CAST "name") == 0) {
            attrName.assign((const char*) attr->children->content);
          } else if (xmlStrcasecmp(attr->name, BAD_CAST "value") == 0) {
            attrValue.assign((const char*) attr->children->content);
          }
        }
        if (hiddenFound) {
#if DEBUG
          printf("Hidden name: %s, value: %s\n", attrName.data(), attrValue.data());
#endif
          // concat params in search form
          formParams.append(attrName).append("=").append(attrValue).append("&");
          attrName.clear();
          attrValue.clear();
        }
      }
      if (node->children != NULL) {


        findInputHidden(node->children);
      }
    }
  }
}


/**
 * Recurisve method to find all input elements type="hidden" and type="submit"
 * in form.
 * @param element
 */
void Webpage::findSelects(htmlNodePtr formRoot) {

  for (htmlNodePtr node = formRoot; node != NULL; node = node->next) {

    if (node->type == XML_ELEMENT_NODE) {
      if (xmlStrcasecmp(node->name, BAD_CAST "select") == 0) {
        bool selectFound = true;
        std::string selectName;
        std::string selectValue;
        std::string optionName;
        std::string optionValue;

        for (xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {
          if (xmlStrcasecmp(attr->name, BAD_CAST "name") == 0) {
            selectName.assign((const char*) attr->children->content);
          }
        }

        // parse select options
        if (node->children != NULL) {
          for (htmlNodePtr nodeOpt = node->children; nodeOpt != NULL; nodeOpt = nodeOpt->next) {
            if (node->type == XML_ELEMENT_NODE) {
              if (xmlStrcasecmp(nodeOpt->name, BAD_CAST "option") == 0) {
                // option found


                if (node->children->content != NULL)
                  optionName = (const char*) node->children->content;

                for (xmlAttrPtr attr = nodeOpt->properties; attr != NULL; attr = attr->next) {
                  if (xmlStrcasecmp(attr->name, BAD_CAST "value") == 0) {
                    optionValue.assign((const char*) attr->children->content);
                  }
                }
              }
              break; // TODO - we select first option parameter
            }
          }
        }

        if (selectFound) {
          // !
          selectValue = optionValue;
#if DEBUG
          printf("Select name: %s, value: %s\n", selectName.data(), selectValue.data());
          printf("Option name: %s\n", optionName.data());
#endif
          // concat params in search form (creates attr=value&)
          formParams.append(selectName).append("=").append(selectValue).append("&");
          selectName.clear();
          selectValue.clear();
        }
      }
      if (node->children != NULL) {
        findSelects(node->children);
      }
    }
  }
}
