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
 * @file utils.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief File contains utilities etc. for parsing strings and so on.
 */

#ifndef UTILS_H
#define	UTILS_H

#include <fstream>
#include <algorithm>
#include <fcntl.h>
#include <sstream>
#include <vector>
#include <cstring>


namespace utils {

  template <size_t N> void replace(std::string &s, const char (&from)[N], const char *to) {
    size_t to_len = strlen(to);
    for (size_t i = 0; (i = s.find(from, i)) != std::string::npos; i += to_len)
      s.replace(i, N - 1, to);
  }

  void loadStringFromFile(const char * filename, std::string &content);
  void saveStringToFile(const char * filename, std::string &content);
  void setupCERRedirection(void);
  void redirectStdErrToNull(void);
  void restoreStdErr(void);
  std::string toUpper(std::string word);
  std::string toLower(std::string word);
  std::string trim(const std::string& pString);
  bool ci_stringCompare(const std::string& str1, const std::string& str2);
  /**
   * Case insensitive string contains
   * @param string - string
   * @param contains - substring
   * @return 
   */
  bool ci_stringContains(const std::string& str1, const std::string& str2);
  std::string removeMultipleWhiteSpaces(std::string s);
  std::string intToString(int number);
  std::vector<std::string> explode(const std::string &delimiter, const std::string &str);
  int distance(const std::string source, const std::string target);
  void htmlSpecialChars(std::string &s);
  void removeTabulators(std::string &s);
}


#endif

