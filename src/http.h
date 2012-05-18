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

#include "ClientException.h"
#include "clientsettings.h"
#include <cstdlib>
#include <curl/curl.h>


namespace utils {

  class HTTP {
  public:
    HTTP();
    ~HTTP();

    void setMaxRequestTimeout(int seconds);


    std::string GET(std::string url, std::string& output) throw (InputException);
    std::string GETImage(std::string url, std::string tmpURL, std::string filename = "") throw (InputException);
    std::string POST(std::string url, std::string params, std::string& output) throw (InputException);

    bool POSTFile(std::string url, std::string filePath, std::string& output) throw (InputException);


    static std::string escape(std::string s);
    std::string getFilenameFromUrl(std::string url);
  private:
    CURL *curl;
    curl_slist * headers;
    int httpTimeout;
    std::string headLog;
    
    void init();
    void clean();

    void wait();

    static size_t writeBinary(void *ptr, std::size_t size, std::size_t nmemb, FILE * stream);
    static int writeText(char *data, std::size_t size, std::size_t nmemb, std::string * buffer);

    std::string & getHeadLog();
    std::string getExtension(std::string & url);
  };
}
#endif	/* HTTP_H */

