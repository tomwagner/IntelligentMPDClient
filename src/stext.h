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
 * @file stext.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */

#ifndef STEXT_H
#define	STEXT_H
#include <string.h>
#include <cstring>
#include "hashlib/hashlibpp.h"
#include <algorithm>
#include <map>
#include <vector>
#include "utils.h"
#include <sys/param.h>


namespace DataStorage {

  class SText {
  public:
    SText();
    SText(const SText& orig);
    virtual ~SText();


    void set(std::string text);

    inline std::string getHash() const {
      return m_hash;
    };

    inline std::string getText() const {
      return m_text;
    };

    std::string getTextWithoutHtml();
    std::map<std::string, int> getWordsCountMap();
    int distance(const std::string target);

    //    std::string SText::toLower(std::string& text);
    SText& toLower();
    void trim();
  protected:
    std::string m_text;
    std::string m_hash;

  private:
    hashwrapper * hashWrapper;
    void calculateHash();


    std::map<std::string, int> wordCountMap;

    template <size_t N> void replace(std::string &s, const char (&from)[N], const char *to) {
      size_t to_len = strlen(to);
      for (size_t i = 0; (i = s.find(from, i)) != std::string::npos; i += to_len)
        s.replace(i, N - 1, to);
    }


  };
}
#endif

