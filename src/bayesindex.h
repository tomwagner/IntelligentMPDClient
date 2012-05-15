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
 * @file bayesindex.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 * @original sources from http://nbayes.codeplex.com/ (C#)
 */
#ifndef BAYESINDEX_H
#define	BAYESINDEX_H
#include <string>
#include <list>
#include <map>
#include "utils.h"


namespace NaiveBayes {

  class Index {
  public:

    Index();
    virtual ~Index();


    //    void add(std::map<std::string, int> wordCountMap);
    void add(const std::string& s);

    unsigned inline getTokenCount() {
      return m_wordCountMap->size();
    }

    inline std::map<std::string, int> * get() const {
      return m_wordCountMap;
    }

    void clear();

  private:
    std::map<std::string, int> * m_wordCountMap;
    static std::list <std::string> m_forbiddenWordList;

    void insertWordToMap(std::string& s);
    void removeSpecialChar(std::string& s);
    void removeDiacritic(std::string& s);
  };

}
#endif	/* BAYESINDEX_H */

