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
 * @file simage.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */

#ifndef SIMAGE_H
#define	SIMAGE_H

#include <string>
#include "stext.h"
#include "sfeedback.h"
#include "http.h"
#include "clientsettings.h"
#include <gtkmm.h>

using namespace utils;


namespace DataStorage {

  class SImage {
  public:
    std::string title;
    std::string alt;
    std::string context;

    SImage();
    virtual ~SImage();

    void setUrl(std::string text);

    inline SText * getUrl() const {
      return m_url;
    };

    inline std::string getLocalPath() const {
      return m_localPath;
    };

    /**
     * Method checks if object contains local image.
     * @return bool
     */
    inline bool isImage() const {
      if (m_localPath.length() == 0) return false;
      return true;
    }

    inline const std::string getHash() const {
      return m_url->getHash();
    };

    int getWidth() const {
      if (m_image->get_pixbuf() == 0)
        return 0;
      else
        return m_image->get_pixbuf()->get_width();
    }

    int getHeight() const {
      if (m_image->get_pixbuf() == 0)
        return 0;
      else
        return m_image->get_pixbuf()->get_height();
    }

    Glib::RefPtr<Gdk::Pixbuf> resize(int width, int height);

  protected:
    SText * m_url;
    std::string m_localPath;


  private:
    HTTP * m_http;
    Gtk::Image * m_image;
    void downloadImage();
  };

  class SPicture : public SImage {
  public:

    SPicture() : m_name("") {
    };

    virtual ~SPicture() {
    };

    inline void setName(std::string text) {
      m_name = text;
    }

    inline std::string getName() const {
      return m_name;
    };

  protected:
    std::string m_name;

  };
}
#endif

