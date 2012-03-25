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
 * @file http.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief Class for implementation access to HTTP protocol.
 */

#define DEBUG 0

#include "http.h"

namespace utils {


  HTTP::HTTP() {
    // inicializujeme curl
    curl = curl_easy_init();

    // nastavíme curl header na prohlížeč
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-GB; rv:1.8.1.6) Gecko/20070725 Firefox/2.0.0.6");

    // povolíme max. 5 přesměrování
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
  }


  HTTP::~HTTP() {
    // vyčístíme curl
    curl_easy_cleanup(curl);
  }


  std::string HTTP::escapeUrl(std::string url) {
    std::string esc = curl_easy_escape(curl, url.c_str(), url.length());
    return esc;
  }


  void HTTP::GET(std::string url, std::string& buffer) throw (InputException) {
    char errorBuffer[CURL_ERROR_SIZE];

    // chybová detekce
    CURLcode result;

    if (curl) {
      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_HEADER, 0);
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeText);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

      // vyšleme požadavek curl
      result = curl_easy_perform(curl);



      // Pokud jsme neuspěli, vracíme výjimku s chybou
      if (result != CURLE_OK) {
        InputException e("HTTP GET: Curl Error: " + std::string(errorBuffer));
        throw e;
      }
    }
  }


  void HTTP::POST(std::string url, std::string params, std::string& output) throw (InputException) {
    char errorBuffer[CURL_ERROR_SIZE];

    // error detection
    CURLcode result;
    
    if (curl) {
      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_HEADER, 0);
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeText);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);

      // POST data
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());

      // send request
      result = curl_easy_perform(curl);
      
      // exception on request error
      if (result != CURLE_OK) {
        InputException e("HTTP POST: Curl Error: " + std::string(errorBuffer));
        throw e;
      }
    }
  }


  std::string HTTP::getFilenameFromUrl(std::string url) {
    size_t found;
    found = url.find_last_of("/\\");
    if (found != std::string::npos)
      return url.substr(found + 1);
    else return "";
  }


  int HTTP::writeText(char *data, size_t size, size_t nmemb, std::string * buffer) {
    // What we will return
    int result = 0;

    // Is there anything in the buffer?
    if (buffer != NULL) {
      // Append the data to the buffer
      buffer->append(data, size * nmemb);

      // How much did we write?
      result = size * nmemb;
    }

    return result;
  }


  size_t HTTP::writeBinary(void *data, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(data, size, nmemb, stream);
    return written;
  }


  void HTTP::GETImage(std::string url, std::string path) throw (InputException) {
    char errorBuffer[CURL_ERROR_SIZE];

    // chybová detekce
    CURLcode result;

    std::string filePath;

    //TODO TMP složka
    filePath.assign(path.c_str());


    filePath.append(getFilenameFromUrl(url));
#if DEBUG
    std::cout << "url:" << url << " ,Filename:" << filePath.c_str() << std::endl;
#endif

    // otevřeme výstupní soubor pro binární zápis
    FILE* fp = fopen(filePath.c_str(), "wb");

    if (curl) {

      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
      curl_easy_setopt(curl, CURLOPT_URL, url.data());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeBinary);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

      // vyšleme požadavek curl
      result = curl_easy_perform(curl);

      fclose(fp);

      // Pokud jsme neuspěli, vracíme výjimku s chybou
      if (result != CURLE_OK) {
        InputException e("HTTP GET: Curl Error: " + std::string(errorBuffer));
        throw e;
      }
    }
  }

}
