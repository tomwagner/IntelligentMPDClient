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
 * @file utils.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief File contains utilities etc. for parsing strings and so on.
 */

#include <sys/param.h>
#include <iostream>
#include "utils.h"
int nullfd;
int stderrfd;

namespace utils {


  /**
   * Funkce načte soubor filename do stringu content
   * @param filename
   * @param content
   */
  void loadStringFromFile(const char * filename, std::string &content) {
    std::string line;
    std::ifstream in(filename);
    while (std::getline(in, line))
      content += line;
    in.close();
  }


  void saveStringToFile(const char * filename, std::string &content) {
    std::ofstream out(filename);

    if (!out) {
      std::cerr << "Cannot open file.\n";
    }

    // write
    out << content;

    out.close();
  }


  /**
   * Funkce pro možnost skrytí chybového hlášení posílaného na 
   * std:err.
   */
  void setupCERRedirection(void) {
    int fail = 0, localDebug = 0;

    nullfd = open("/dev/null", (O_RDWR | O_NONBLOCK), 0);
    stderrfd = dup(STDERR_FILENO);

    if (nullfd == -1) {
      std::cerr << "Nemohu otevřít /dev/null!" << std::endl;
      fail = 1;
    }
    if (stderrfd == -1) {
      std::cerr << "Nemohu provést dup stderr!" << std::endl;
      fail = 1;
    }
    if (!fail && localDebug) {
      std::cerr << "Dup úspěšný" << std::endl;
    }
  }


  /**
   * Funkce přesměruje std::err výstup do /dev/null
   */
  void redirectStdErrToNull(void) {
    if ((nullfd != -1) && (stderrfd != -1)) {
      dup2(nullfd, STDERR_FILENO);
    }
  }


  /**
   * Funkce obnoví funkčnost std::err výstupu
   */
  void restoreStdErr(void) {
    if ((nullfd != -1) && (stderrfd != -1)) {
      dup2(stderrfd, STDERR_FILENO);
    }
  }


  /**
   * Metoda zkonvertuje slovo na velká písmena
   * @param input
   * @return 
   */
  std::string toUpper(std::string input) {
    std::transform(input.begin(), input.end(), input.begin(), ::toupper);
    return input;
  }


  /**
   * Metoda zkonvertuje slovo na malá písmena
   * @param input
   * @return 
   */
  std::string toLower(std::string input) {
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    return input;
  }


  std::string trim(const std::string& pString) {
    const std::string& pWhitespace = " \t\n";
    const size_t beginStr = pString.find_first_not_of(pWhitespace);
    if (beginStr == std::string::npos) {
      // no content
      return "";
    }

    const size_t endStr = pString.find_last_not_of(pWhitespace);
    const size_t range = endStr - beginStr + 1;

    return pString.substr(beginStr, range);
  }


  bool ci_stringCompare(const std::string& str1, const std::string& str2) {
    if (str1.size() != str2.size()) {
      return false;
    }
    for (std::string::const_iterator c1 = str1.begin(), c2 = str2.begin(); c1 != str1.end(); ++c1, ++c2) {
      if (tolower(*c1) != tolower(*c2)) {
        return false;
      }
    }
    return true;
  }


  bool ci_stringContains(const std::string& string, const std::string& contains) {
    std::string::const_iterator c1, c2;

    for (c1 = string.begin(), c2 = contains.begin(); c1 != string.end(); ++c1) {
      if (tolower(*c1) != tolower(*c2)) {
        c2 = contains.begin();
      } else {
        ++c2;
        // substring found
        if (c2 == contains.end()) {
          return true;
        }
      }
    }
    return false;
  }


  /**
   * Metoda odstraní vícenásobnou mezeru z řetězce
   * @param s
   * @return 
   */
  std::string removeMultipleWhiteSpaces(std::string s) {
    std::string search = "  ";
    size_t index;

    // na nalezeném indexu odebereme jednu mezeru
    while ((index = s.find(search)) != std::string::npos) {
      s.erase(index, 1);
    }

    return s;
  }


  std::string intToString(int number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
  }


  std::vector<std::string> explode(const std::string &delimiter, const std::string &str) {
    std::vector<std::string> arr;

    int strleng = str.length();
    int delleng = delimiter.length();
    if (delleng == 0)
      return arr; //no change

    int i = 0;
    int k = 0;
    while (i < strleng) {
      int j = 0;
      while (i + j < strleng && j < delleng && str[i + j] == delimiter[j])
        j++;
      //found delimiter
      if (j == delleng) {
        arr.push_back(str.substr(k, i - k));
        i += delleng;
        k = i;
      } else {
        i++;
      }
    }
    arr.push_back(str.substr(k, i - k));
    return arr;
  }


  void stripHtmlTags(std::string &s) {
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
  }

  void removeTabulators(std::string &s) {
    s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
    s.erase(std::remove(s.begin(), s.end(), '\t'), s.end());
  }
  
  
  void htmlSpecialChars(std::string &s) {
    replace(s, "&", "&amp;");
    replace(s, "'", "&#039;");
    replace(s, "\"", "&quot;");
    replace(s, " ", "&nbsp;");
  }


  /**
   * Levenstein by Anders Sewerin Johansen
   * @url http://www.merriampark.com/ldcpp.htm
   * @param source
   * @param target
   * @return 
   */
  int distance(const std::string source, const std::string target) {

    // Step 1

    const int n = source.length();
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

      const char s_i = source[i - 1];

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
          if (source[i - 2] != t_j) trans++;
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

