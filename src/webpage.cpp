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

#define DEBUG 1
#define DEBUGHideParsing 1


#include "webpage.h"


Webpage::Webpage(std::string html) throw (InputException, ClientException)
: searchUrl(""), xpathCtx(NULL) {

  // zkontrolujeme vstup
  if (html.data() == NULL) {
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

  // aplikujeme XSLT transformaci, kdy odstraníme všechny tagy <script> jazyka JavaScript
  xsltStylesheetPtr cur = xsltParseStylesheetFile(BAD_CAST "xslt/noscripts_style");
  const char * params[1];
  params[0] = NULL;

  res = xsltApplyStylesheet(cur, doc, params);

  // uvolním doc
  xmlFreeDoc(doc);

  // vytvořím kontext pro XPath
  xpathCtx = xmlXPathNewContext(res);

  // vyparsuje odkazy
  parseLinkList();

  // vyparsuje odstavce
  parseParagraphList();

  // vyčistíme stylesheet
  xsltFreeStylesheet(cur);
}


Webpage::~Webpage() {
  std::cout << "HTMLParser destruktor" << std::endl;

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


  // uvolníme document res vzniklý po transformaci XSLT 
  xmlFreeDoc(res);

  // uvolníme kontext XPath
  xmlXPathFreeContext(xpathCtx);

  // vyčistíme xslt a xml parsery
  xsltCleanupGlobals();
  xmlCleanupParser();

  std::cout << "HTMLParser konec" << std::endl;
}


std::list<searchPair*>& Webpage::detectSearchForms() throw (ClientException) {
  std::string formMethod;
  std::string formAction;
  std::string inputName;
  std::string url;
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
    //vytvoříme url adresu pro vyhledávání
    searchUrl.append(formAction).append("?");

    //zjištění skrytých atributů v formu, nutných k odeslání
    findInputHidden(formNode);

    //připojíme do adresy poslední část a to inputbox
    searchUrl.append(inputName).append("=");
    url = searchUrl;
#if DEBUG
    std::cout << "SEARCH URL: " << searchUrl.data() << std::endl;
#endif

    // pokud není uvedena metoda, implicitně vracíme GET
    if (formMethod.empty()) formMethod.assign("GET");

    // implicitně vracíme metodu velkými písmeny
    formMethod = utils::toUpper(formMethod);

    // vytvoříme záznam
    searchPair *p = new searchPair;
    p->method = formMethod;
    p->url = url;

    searchList.push_back(p);

    searchUrl.clear();
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


/**
 * Metoda vrací titulek stránky
 * @return 
 */
std::string Webpage::title() {
  std::string title;
  xmlXPathObjectPtr screenXPathObj = xmlXPathEvalExpression(BAD_CAST "//head//title", xpathCtx); //  //@href[contains(.,'byt')]
  xmlNodeSetPtr screenNodes = screenXPathObj->nodesetval;

  findDeepText(screenNodes->nodeTab[0]->children, title);

#if DEBUG
  printf("Title %s\n", title.c_str());
#endif

  //vyčistíme objekt XPath
  xmlXPathFreeObject(screenXPathObj);

  return title;
}


/**
 * Funkce vrací nadpis H1 dané webové stránky
 * @return 
 */
std::string Webpage::H1() {
  std::string h1;
  xmlXPathObjectPtr screenXPathObj = xmlXPathEvalExpression(BAD_CAST "//h1", xpathCtx); //  //@href[contains(.,'byt')]
  xmlNodeSetPtr screenNodes = screenXPathObj->nodesetval;

  // pokud prvek existuje, přiřadíme jej do stringu
  //if (screenNodes->nodeTab[0]->children->content == NULL) return h1;

  //h1.assign((const char*) screenNodes->nodeTab[0]->children->content);

  findDeepText(screenNodes->nodeTab[0]->children, h1);

#if DEBUG
  printf("H1 %s\n", h1.c_str());
#endif

  //vyčistíme objekt XPath
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

  // TODO určit velikost odstavce, detekce věty apod.
  xmlXPathObjectPtr screenXPathObj1st = xmlXPathEvalExpression(BAD_CAST "//body//text()", xpathCtx);
  xmlNodeSetPtr screenNodes1st = screenXPathObj1st->nodesetval;
  for (int j = 0; j < screenNodes1st->nodeNr; j++) {
    std::string p;

    // vložím do stringu
    p.assign((const char*) screenNodes1st->nodeTab[j]->content);

    // ořežu bílé znaky
    p = utils::trim(p);

    // odstraníme vícenásobné mezery
    p = utils::removeMultipleWhiteSpaces(p);
    
    // vložím do seznamu odstavců
    if (!p.empty())
      paragraphList.push_back(p);
  }
  //vyčistíme objekt XPath
  xmlXPathFreeObject(screenXPathObj1st);

  xmlXPathObjectPtr screenXPathObj2nd = xmlXPathEvalExpression(BAD_CAST "//body//p", xpathCtx);
  xmlNodeSetPtr screenNodes2nd = screenXPathObj2nd->nodesetval;

  for (int j = 0; j < screenNodes2nd->nodeNr; j++) {
    //if (utils::ci_stringContains(std::string((const char*) screenNodes->nodeTab[j]->content), std::string("beatles"))) {
    //std::cout << "--------------------------------------------------------------" << std::endl;
    //printf("'%s'\n", screenNodes->nodeTab[j]->content);
    std::string p;

    if (screenNodes2nd->nodeTab[j]->children != NULL)
      findDeepText(screenNodes2nd->nodeTab[j], p);

    // ořežu bílé znaky
    p = utils::trim(p);

    // odstraníme vícenásobné mezery
    p = utils::removeMultipleWhiteSpaces(p);


    // vložím do seznamu odstavců
    if (!p.empty())
      paragraphList.push_back(p);
    //}
  }
  //vyčistíme objekt XPath
  xmlXPathFreeObject(screenXPathObj2nd);

  // seřadíme a vyházíme duplicity
  paragraphList.sort(ci_paragraphs);
  paragraphList.unique(compareParagraphs);
}


/**
 * Metoda vrací seznam odstavců na dané webové stránce
 * @return 
 */
std::list<std::string>& Webpage::getParagraphList() {
  return paragraphList;
}


/**
 * Metoda vrací seznam odstavců na dané webové stránce
 * @return 
 */
std::list<std::string> Webpage::getParagraphList(std::string contains) {

  std::list<std::string> localParagraphList;
  std::list<std::string>::iterator it;
  for (it = paragraphList.begin(); it != paragraphList.end(); it++) {
    if (utils::ci_stringContains((*it), contains)) {
      localParagraphList.push_back((*it));
    }
  }

  return localParagraphList;
}


/**
 * Funkce vrací seznam obrázků dané webové stránky
 * @return 
 */
std::list<imgPair>& Webpage::getImgList() {

  // projdeme všechny img prvky v html souboru
  xmlXPathObjectPtr screenXPathObj = xmlXPathEvalExpression(BAD_CAST "//body//img[@src]", xpathCtx);
  xmlNodeSetPtr screenNodes = screenXPathObj->nodesetval;
  for (int j = 0; j < screenNodes->nodeNr; j++) {
    //printf("Obrázky %s\n", screenNodes->nodeTab[j]->name);
    imgPair i;
    for (xmlAttrPtr attr = screenNodes->nodeTab[j]->properties; attr != NULL; attr = attr->next) {

      // hledám atributy
      if (xmlStrcasecmp(attr->name, BAD_CAST "src") == 0) {
        // našel jsem hidden input
        i.url.assign((const char*) attr->children->content);
        printf("Obrázky %s\n", attr->children->content);
      } else if ((xmlStrcasecmp(attr->name, BAD_CAST "alt") == 0) || (xmlStrcasecmp(attr->name, BAD_CAST "title") == 0)) {
        printf("Obrázky alt %s\n", attr->children->content);
        i.alt.assign((const char*) attr->children->content);
      }
      imgList.push_back(i);
    }
  }

  //vyčistíme objekt XPath
  xmlXPathFreeObject(screenXPathObj);

  //  // dále do seznamu připojíme i odkazy na prvky typu obrázek TODO
  //  xmlXPathObjectPtr screenXPathObj = xmlXPathEvalExpression(BAD_CAST "//body//a[@src]", xpathCtx);
  //  xmlNodeSetPtr screenNodes = screenXPathObj->nodesetval;
  //  for (int j = 0; j < screenNodes->nodeNr; j++) {
  //    //printf("Obrázky %s\n", screenNodes->nodeTab[j]->name);
  //    imgPair i;
  //    for (xmlAttrPtr attr = screenNodes->nodeTab[j]->properties; attr != NULL; attr = attr->next) {
  //
  //      // hledám atributy
  //      if (xmlStrcasecmp(attr->name, BAD_CAST "src") == 0) {
  //        // našel jsem hidden input
  //        i.url.assign((const char*) attr->children->content);
  //        printf("Obrázky %s\n", attr->children->content);
  //      } else if ((xmlStrcasecmp(attr->name, BAD_CAST "alt") == 0) || (xmlStrcasecmp(attr->name, BAD_CAST "title") == 0)) {
  //        printf("Obrázky alt %s\n", attr->children->content);
  //        i.alt.assign((const char*) attr->children->content);
  //      }
  //      imgList.push_back(i);
  //    }
  //  }


  return imgList;
}


/**
 * Funkce nalezne text v potomcích uzlu node a vrací jej 
 * @param node
 * @param linkText
 */
void Webpage::findDeepText(xmlNode * node, std::string& linkText) {
  if (node->content != NULL) linkText.append((const char*) node->content);

  // nemáme děti, vracím se
  if (node->children == NULL) return;

  // projdeme všechny děti uzlu a hledáme textové informace
  for (xmlNode* n = node->children; n != NULL; n = n->next) {
    findDeepText(n, linkText);
    //    std::string str;
    //    if (n->content != NULL) {
    //      str.assign((const char*) n->content);
    //      linkText.append(str);
    //      //printf("<%s>%s\n", n->name, str.c_str());
    //    }
  }


  //printf("\n");
}


/**
 * Rekurzivní metoda hledá informace z titulků obrázků. Důležité např. při odkazu, který je
 * umístěn na obrázku a okolí neobsahuje textovou informaci.
 * Doporučuje: volání pokud se nenalezne v potomcích žádná textová informace.
 * @param node - uzel, ve kterém hledáme
 * @param linkText - vracený text
 */
void Webpage::findLinkTextFromImgTitles(xmlNode * node, std::string& imgTitle) {
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
      findDeepText(screenNodes->nodeTab[j]->parent, linkText);

      // ořežeme bílé znaky ze začátku a konce
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
 * Rekurzivní metoda pro nalezení prvků typu input type="hidden" a type="submit"
 * @param element
 */
void Webpage::findInputHidden(htmlNodePtr element) {

  for (htmlNodePtr node = element; node != NULL; node = node->next) {
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
          searchUrl.append(attrName).append("=").append(attrValue).append("&");
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
