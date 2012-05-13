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
 * @file simage.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */
#include "simage.h"
#include "clientsettings.h"
#define DEBUG 0
using namespace utils;

namespace DataStorage {


  SImage::SImage() : m_url(new SText),
  m_image(new Gtk::Image),
  m_http(new HTTP),
  m_localPath("") {
  }


  SImage::~SImage() {
    delete m_http;
    delete m_url;
    delete m_image;
  }


  void SImage::setUrl(std::string text) {
    m_url->set(text);

    downloadImage();
    
    // load image to gtk image object
    m_image->set(m_localPath);
  }


  Glib::RefPtr<Gdk::Pixbuf> SImage::resize(int width, int height) {
    if (m_localPath.empty()) return m_image->get_pixbuf();
    if (m_image->get_pixbuf() == 0) return m_image->get_pixbuf();

    int newWidth;
    int newHeight;
    double ratio;
#if DEBUG
    std::cout << "old width" << getWidth() << "height" << getHeight() << std::endl;
#endif

    if (getWidth() > width) {
      ratio = (double) width / (double) getWidth();
      newWidth = width;
      newHeight = getHeight() * ratio;
    } else if (getHeight() > height) {
      ratio = (double) height / (double) getHeight();
      newHeight = height;
      newWidth = getWidth() * ratio;
    } else {
      // no scale needed
      return m_image->get_pixbuf();
    }
    //      ratio = (double) getWidth() / (double) width;
    //      newWidth = getWidth();
    //    }
#if DEBUG
    std::cout << "ratio" << ratio << std::endl;
#endif

#if DEBUG
    std::cout << "new width" << newWidth << "height" << newHeight << std::endl;
#endif
    return m_image->get_pixbuf()->scale_simple(newWidth, newHeight, Gdk::INTERP_BILINEAR);
  }


  void SImage::downloadImage() {
    try {
      m_localPath = m_http->GETImage(m_url->getText(), clientSettings->getTempPath(), m_url->getHash());
    } catch (InputException &e) {
      std::cerr << "SImage - InputExeption:" << e.what() << std::endl;
    } catch (ClientException &e) {
      std::cerr << e.what() << std::endl;
    }
  }
}