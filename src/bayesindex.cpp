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
 * GNU General Public License for more detailm_text.
 *
 * You should have received a copy of the GNU General Public License
 * along with rrv.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file bayesindex.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Indexing storage for bayesian classificator.
 * @original sources from http://nbayes.codeplex.com/ (C#)
 */

#include "bayesindex.h"

namespace NaiveBayes {


  Index::Index() : m_wordCountMap(new std::map<std::string, int>) {
  }


  Index::~Index() {
  }

  //  void Index::add(std::map<std::string, int> m_wordCountMap) {
  //    std::map<std::string, int>::iterator it;
  //    std::map<std::string, int>::iterator fpos;
  //    // add all words to local map
  //    for (it = m_wordCountMap.begin(); it != m_wordCountMap.end(); it++) {
  //      fpos = m_m_wordCountMap->find(it->first);
  //
  //      if (fpos == m_m_wordCountMap->end()) {
  //        m_m_wordCountMap->insert(std::pair<std::string, int>(it->first, 1));
  //      } else {
  //        fpos->second += it->second;
  //      }
  //    }
  //  }


  void Index::removeSpecialChar(std::string& s) {
    utils::replace(s, ".", "");
    utils::replace(s, ",", "");
    utils::replace(s, ";", "");
    utils::replace(s, ":", "");
    utils::replace(s, "?", "");
    utils::replace(s, "\"", "");
    utils::replace(s, "'", "");
    utils::replace(s, "?", "");
    utils::replace(s, "’", "");
    utils::replace(s, "(", "");
    utils::replace(s, ")", "");
    utils::replace(s, "[", "");
    utils::replace(s, "]", "");
    utils::replace(s, "@", "");
    utils::replace(s, "$", "");
    utils::replace(s, "©", "");
    utils::replace(s, "*", "");
  }


  /**
   * Function removes czech diacritic from text
   * @param s
   */
  void Index::removeDiacritic(std::string& s) {
    utils::replace(s, "ě", "e");
    utils::replace(s, "š", "s");
    utils::replace(s, "č", "c");
    utils::replace(s, "ř", "r");
    utils::replace(s, "ž", "z");
    utils::replace(s, "ý", "y");
    utils::replace(s, "á", "a");
    utils::replace(s, "í", "i");
    utils::replace(s, "é", "e");
    utils::replace(s, "ú", "u");
    utils::replace(s, "ů", "u");
  }


  void Index::insertWordToMap(std::string& word) {

    // insert words into asociation array <word, count>
    if (m_wordCountMap->find(word) == m_wordCountMap->end()) {
      m_wordCountMap->insert(std::pair<std::string, int>(word, 1));
    } else {
      m_wordCountMap->find(word)->second++;
    }
  }


  /**
   * Method add string to bayes index
   * @param string
   */
  void Index::add(const std::string & s) {

    std::vector<std::string> words = utils::explode(" ", s);
    std::vector<std::string>::iterator it;


    for (it = words.begin(); it < words.end(); it++) {

      removeSpecialChar(*it);
      //removeDiacritic(*it);

      //      // remove small words // FOR TESTING NOW
      //      if ((*it).length() <= 3) {
      //        words.erase(it);
      //        continue;
      //      }


      // if word is empty, erase it
      if ((*it).empty()) {
        words.erase(it);
        continue;
      }

      *it = utils::toLower(*it);

#if DEBUG
      std::cout << "'" + *it + "'" << std::endl;
#endif
      insertWordToMap(*it);
    }
  }


  void Index::clear() {
    m_wordCountMap->clear();
  }

}