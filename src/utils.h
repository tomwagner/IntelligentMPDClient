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

#include "global.h"
#include <fstream>
#include <algorithm>
#include <fcntl.h>
#include <sstream>

namespace utils {

  void loadStringFromFile(const char * filename, std::string &content);
  void setupCERRedirection(void);
  void redirectStdErrToNull(void);
  void restoreStdErr(void);
  std::string toUpper(std::string word);
  std::string trim(const std::string& pString);
  bool ci_stringCompare(const std::string& str1, const std::string& str2);
  bool ci_stringContains(const std::string& str1, const std::string& str2);
  std::string removeMultipleWhiteSpaces(std::string s);
  std::string intToString(int number);
}


#endif

