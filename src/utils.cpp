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
    const std::string& pWhitespace = " \t";
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


  /**
   * Vrátí true pokud řetězec str1 obsahuje podřetězec str2. 
   * Funkce je Case insensitive.
   * @param string - řetěžec
   * @param contains - hledaný podřetězec
   * @return 
   */
  bool ci_stringContains(const std::string& string, const std::string& contains) {
    std::string::const_iterator c1, c2;

    for (c1 = string.begin(), c2 = contains.begin(); c1 != string.end(); ++c1) {
      if (tolower(*c1) != tolower(*c2)) {
        c2 = contains.begin();
      } else {
        ++c2;
        // pokud jsem našel podřetězec, vracím true
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
}

