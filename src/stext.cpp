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
 * @file stext.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */
#include <vector>

#include "stext.h"
#include "utils.h"

#include <iostream>

#define DEBUG 0

namespace DataStorage {


  SText::SText() : m_text(""), m_hash("") {
    hashWrapper = new md5wrapper();

    calculateHash();
  }


  SText::SText(const SText& orig) {

  }


  SText::~SText() {
    delete hashWrapper;
  }


  void SText::set(std::string text) {
    m_text = text;

    calculateHash();
  }


  void SText::calculateHash() {
    m_hash = hashWrapper->getHashFromString(m_text);
  }


//  std::string SText::toLower(std::string& text) {
//    std::transform(text.begin(), text.end(), text.begin(), ::tolower);
//  }
//

  SText& SText::toLower() {
    std::string lower;
    SText result;
    std::transform(m_text.begin(), m_text.end(), lower.begin(), ::tolower);
    result.set(lower);
    result;
  }


  void SText::trim() {
    const std::string& pWhitespace = " \t";
    const size_t beginStr = m_text.find_first_not_of(pWhitespace);

    if (beginStr == std::string::npos) return;

    const size_t endStr = m_text.find_last_not_of(pWhitespace);
    const size_t range = endStr - beginStr + 1;

    m_text = m_text.substr(beginStr, range);

    calculateHash();
  }


  std::string SText::getTextWithoutHtml() {
    std::string s = m_text;
    bool erase = 0;
    for (size_t i = s.find("<"); i != std::string::npos; i = s.find("<")) {
      size_t j = s.find(">", i) + 1;
      s.replace(i, j - i, "");
    }
    replace(s, "&#039;", "'");
    replace(s, "&amp;", "&");
    replace(s, "&quot;", "\"");
    replace(s, "&nbsp;", " ");
    for (size_t i = 0; i < s.length(); ++i) {
      if (erase) {
        s.erase(s.begin() + i);
        erase = 0;
      }
      if (s[i] == 13) // ascii code for windows line ending, get rid of this shit
      {
        s[i] = '\n';
        erase = 1;
      } else if (s[i] == '\t')
        s[i] = ' ';
    }
    return s;
  }


  /**
   * Levenstein by Anders Sewerin Johansen
   * @url http://www.merriampark.com/ldcpp.htm
   * @param source
   * @param target
   * @return 
   */
  int SText::distance(const std::string target) {

    // Step 1

    const int n = m_text.length();
    const int m = target.length();
    if (n == 0) {
      return m;
    }
    if (m == 0) {
      return n;
    }

    // Good form to declare a TYPEDEF

    typedef std::vector< std::vector<int> > Tmatrix;

    Tmatrix matrix(n + 1);

    // Size the vectors in the 2.nd dimension. Unfortunately C++ doesn't
    // allow for allocation on declaration of 2.nd dimension of vec of vec

    for (int i = 0; i <= n; i++) {
      matrix[i].resize(m + 1);
    }

    // Step 2

    for (int i = 0; i <= n; i++) {
      matrix[i][0] = i;
    }

    for (int j = 0; j <= m; j++) {
      matrix[0][j] = j;
    }

    // Step 3

    for (int i = 1; i <= n; i++) {

      const char s_i = m_text[i - 1];

      // Step 4

      for (int j = 1; j <= m; j++) {

        const char t_j = target[j - 1];

        // Step 5

        int cost;
        if (s_i == t_j) {
          cost = 0;
        } else {
          cost = 1;
        }

        // Step 6

        const int above = matrix[i - 1][j];
        const int left = matrix[i][j - 1];
        const int diag = matrix[i - 1][j - 1];
        int cell = MIN(above + 1, MIN(left + 1, diag + cost));

        // Step 6A: Cover transposition, in addition to deletion,
        // insertion and substitution. This step is taken from:
        // Berghel, Hal ; Roach, David : "An Extension of Ukkonen's 
        // Enhanced Dynamic Programming ASM Algorithm"
        // (http://www.acm.org/~hlb/publications/asm/asm.html)

        if (i > 2 && j > 2) {
          int trans = matrix[i - 2][j - 2] + 1;
          if (m_text[i - 2] != t_j) trans++;
          if (s_i != target[j - 2]) trans++;
          if (cell > trans) cell = trans;
        }

        matrix[i][j] = cell;
      }
    }

    // Step 7

    return matrix[n][m];
  }


}