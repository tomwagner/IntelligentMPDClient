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
 * @brief Class implements access to HTTP protocol.
 */

#define DEBUG 0
#define DEBUGDOWNLOADED 0
#include <fstream>

#include "http.h"
#include <iostream>

namespace utils {


  HTTP::HTTP() : httpTimeout(0),
  headers(NULL) {


    // init curl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    // accept max 5 redirs
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
#if DEBUG    
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
#endif

    // follow redirections
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

    // create cookie file


    // cookies file
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "/tmp/impc_cookies.txt");

    // create head log
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &writeText);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headLog);


    // FAKE BROWSER BEHAVIOR
    // setting chrome header identification
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 5.1) AppleWebKit/535.11 (KHTML, like Gecko) Chrome/17.0.963.83 Safari/535.11");
    headers = curl_slist_append(headers, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    //TODO curl_slist_append(headers, "Accept-Encoding:gzip,deflate,sdch");
    curl_slist_append(headers, "Accept-Language: en-US,en;q=0.8");
    curl_slist_append(headers, "Accept-Charset: utf-8;q=0.7,*;q=0.3");
    curl_slist_append(headers, "Connection: keep-alive");
    curl_slist_append(headers, "Cache-Control:max-age=0");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  }


  HTTP::~HTTP() {

    // clean header list
    curl_slist_free_all(headers);
    // cleanup curl
    curl_easy_cleanup(curl);
    curl_global_cleanup();
  }


  void HTTP::setMaxRequestTimeout(int seconds) {
    httpTimeout = seconds;
  }


  void HTTP::wait() {
    if (httpTimeout == 0) return;

    // sleep for seconds
    sleep(rand() % httpTimeout);
  }


  /**
   * Escape input string for using in url.
   * @param s
   * @return 
   */
  std::string HTTP::escape(std::string s) {
    char *cs = curl_easy_escape(0, s.c_str(), s.length());
    std::string result(cs);
    curl_free(cs);
    return result;
  }


  /**
   * HTTP GET Method
   * @param url - url to download with GET params
   * @param &buffer - returned text output
   * @return redirect url
   */
  std::string HTTP::GET(std::string url, std::string& buffer) throw (InputException) {
    char errorBuffer[CURL_ERROR_SIZE];
    std::string redirect = url;
    char *ct = NULL;

    // wait for timeout
    wait();

    // error detection
    CURLcode result;

    if (curl) {

      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_HEADER, 0);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeText);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);


      // vyšleme požadavek curl
      result = curl_easy_perform(curl);

      if (CURLE_OK == result) {

        // ask for the content-type
        result = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
        if ((CURLE_OK == result) && ct) {
#if DEBUG 
          printf("HTTP: We received Content-Type: %s\n", ct);
#endif
        }
        // ask for redirection
        result = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &ct);
        if ((CURLE_OK == result) && ct) {
#if DEBUG
          printf("HTTP: We were redirected to: %s\n", ct);
#endif    
          redirect.assign(ct);
        }
      }

#if DEBUGDOWNLOADED
      std::cout << "DOWNLOADED:" << buffer << std::endl;
#endif 
      // exception on error
      if (result != CURLE_OK) {
        InputException e("HTTP GET: Curl Error: " + std::string(errorBuffer));
        throw e;
      }
    }

    return redirect;
  }


  /**
   * HTTP POST Method
   * @param url - url to download without POST params
   * @param params - POST params
   * @param &buffer - returned text output
   * @return redirect url
   */
  std::string HTTP::POST(std::string url, std::string params, std::string& output) throw (InputException) {
    char errorBuffer[CURL_ERROR_SIZE];
    std::string http_code;
    std::string redirect = url;
    char *ct = NULL;

    // wait for timeout
    wait();

    // error detection
    CURLcode result;

    if (curl) {
      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_HEADER, 0);
      /**
       * Post Redirection - if we cant set this, the next redir is GET method
      0 -> do not set any behavior
      1 -> follow redirect with the same type of request only for 301 redirects.
      2 -> follow redirect with the same type of request only for 302 redirects.
      3 -> follow redirect with the same type of request both for 301 and 302 redirects.*/
      curl_easy_setopt(curl, CURLOPT_POSTREDIR, 3);

      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeText);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);
      //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

      // POST data
      curl_easy_setopt(curl, CURLOPT_POST, 1);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());

      //curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

      // send request
      result = curl_easy_perform(curl);


      if (CURLE_OK == result) {


        // ask for the content-type
        result = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
        printf("result %d", result);
        if ((CURLE_OK == result)) {
#if DEBUG
          printf("HTTP: We received Content-Type: %s\n", ct);
#endif
        }
        // ask for redirection
        result = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &ct);
        if ((CURLE_OK == result) && ct) {
#if DEBUG
          printf("HTTP: We were redirected to: %s\n", ct);
#endif
          redirect.assign(ct);
        }

      }

      // exception on request error
      if (result != CURLE_OK) {
        InputException e("HTTP POST: Curl Error: " + std::string(errorBuffer));
        throw e;
      }
    }

    return redirect;
  }


  bool HTTP::POSTFile(std::string url, std::string filePath, std::string& output) throw (InputException) {
    char errorBuffer[CURL_ERROR_SIZE];
    std::string http_code;
    std::string redirect = url;
    char *ct = NULL;

    // wait for timeout
    wait();

    // error detection
    CURLcode result;

    //open connection
    CURL * curlPost = curl_easy_init();

    struct curl_httppost *post = NULL;
    struct curl_httppost *last = NULL;

    if (curlPost) {
      curl_formadd(&post, &last, CURLFORM_COPYNAME, "file", CURLFORM_FILE, filePath.c_str(), CURLFORM_END);
      //Specify the API Endpoint
      curl_easy_setopt(curlPost, CURLOPT_URL, url.c_str());
      //Specify the HTTP Method
      result = curl_easy_setopt(curlPost, CURLOPT_HTTPPOST, post);

      //      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
      //      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      //      curl_easy_setopt(curl, CURLOPT_HEADER, 1);
      //      /**
      //       * Post Redirection - if we cant set this, the next redir is GET method
      //      0 -> do not set any behavior
      //      1 -> follow redirect with the same type of request only for 301 redirects.
      //      2 -> follow redirect with the same type of request only for 302 redirects.
      //      3 -> follow redirect with the same type of request both for 301 and 302 redirects.*/
      //            curl_easy_setopt(curl, CURLOPT_POSTREDIR, 3);
      //
      curl_easy_setopt(curlPost, CURLOPT_WRITEFUNCTION, &writeText);
      curl_easy_setopt(curlPost, CURLOPT_WRITEDATA, &output);
      //      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

      // send request
      result = curl_easy_perform(curlPost);

      // clean      
      curl_easy_cleanup(curlPost);

      // exception on request error
      if (result != CURLE_OK) {
        InputException e("HTTP POST File: Curl Error: " + std::string(errorBuffer));
        throw e;
      }
    }
    curl_formfree(post);
    return true;
  }


  /**
   * Method returns filename from url or empty string
   * @param url
   * @return 
   */
  std::string HTTP::getFilenameFromUrl(std::string url) {
    std::size_t found;
    found = url.find_last_of("/\\");
    if (found != std::string::npos)
      return url.substr(found + 1);
    else return "";
  }


  /**
   * Curl text write function.
   * @param data
   * @param size
   * @param nmemb
   * @param buffer
   * @return 
   */
  int HTTP::writeText(char *data, std::size_t size, std::size_t nmemb, std::string * buffer) {
    // What we will return
    int result = 0;

    buffer->append(data, size * nmemb);

    result = size * nmemb;


    return result;
  }


  /**
   * Curl binary write function.
   * @param data
   * @param size
   * @param nmemb
   * @param stream
   * @return 
   */
  std::size_t HTTP::writeBinary(void *data, std::size_t size, std::size_t nmemb, FILE * stream) {
    std::size_t written = fwrite(data, size, nmemb, stream);
    return written;
  }


  /**
   * Function download image to path with optional filename
   * @param url - url to image
   * @param path - saving path
   * @param filename - without extension
   * @return filepath
   */
  std::string HTTP::GETImage(std::string url, std::string path, std::string filename) throw (InputException) {
    char errorBuffer[CURL_ERROR_SIZE];
    CURLcode result; // error detection
    std::string filePath;

    if (url.substr(0, 7).compare("http://") != 0) {
      InputException e("HTTP GET Image: URL ERROR - detected non absolute url: ");
      throw e;
      filePath = "";
      return filePath;
    }


    // create filepath
    filePath = path.c_str();

    if (filename.empty()) {
      filePath += getFilenameFromUrl(url);
    } else {
      filePath += filename + getExtension(url);
    }


    // check if image doesn't exist
    std::ifstream ifile(filePath.c_str());
    if (ifile.is_open()) {

      // detect if is binary or ascii file
      int c;
      while ((c = ifile.get()) != EOF && c <= 127);
      if (c == EOF) {
        // is ascii file, error
        ifile.close();
        filePath = "";
        return filePath;
      }
      // OK, binary file, return path
      ifile.close();
      return filePath;
    }

    // wait for timeout
    wait();


#if DEBUG
    std::cout << "url:" << url << " ,Filename:" << filePath.c_str() << std::endl;
#endif

    // open file for binary write
    FILE* fp = fopen(filePath.c_str(), "wb");

    if (curl) {

      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
      curl_easy_setopt(curl, CURLOPT_URL, url.data());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeBinary);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

      // send request
      result = curl_easy_perform(curl);

      // close file
      fclose(fp);


      if (CURLE_OK == result) {

        char * ct;
        // ask for the content-type
        result = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
        //        printf("result %d", result);

        if ((CURLE_OK == result)) {
          //          printf("HTTP: We received Content-Type: %s\n", ct);
          std::string r;
          r.assign(ct);

          if (r.find("html") != std::string::npos) {
            //            InputException e("HTTP GET Image: Content is not an image.");
            //            throw e;
            filePath = "";
            return filePath;
          }
        }
      }

      // e
      if (result != CURLE_OK) {
        InputException e("HTTP GET Image: Curl Error: " + std::string(errorBuffer));
        throw e;
      }
    }

    return filePath;
  }


  /**
   * Method returns log of Head from HTTM communication.
   * @return 
   */
  std::string & HTTP::getHeadLog() {
    return headLog;
  }


  std::string HTTP::getExtension(std::string & url) {
    return url.substr(url.length() - 4, 4);
  }
}
