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
 * @file http.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Class for implementation access to HTTP protocol.
 */

#ifndef HTTP_H
#define	HTTP_H

#include "global.h"
#include "ClientException.h"
#include <curl/curl.h>

namespace utils {

  class HTTP {
  public:
    HTTP();
    ~HTTP();
    void GET(std::string url, std::string& output) throw (InputException);
    void GETImage(std::string url, std::string tmpURL) throw (InputException);
    void POST(std::string url, std::string params, std::string& output) throw (InputException);

    std::string escapeUrl(std::string url);
  private:
    CURL *curl;
    
    
    static size_t writeBinary(void *ptr, size_t size, size_t nmemb, FILE *stream);
    static int writeText(char *data, size_t size, size_t nmemb, std::string * buffer);
    std::string getFilenameFromUrl(std::string url);
  };
}
#endif	/* HTTP_H */

