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
 * @file SSource.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief 
 */

#ifndef SSOURCE_H
#define	SSOURCE_H
#include <cstring>
#include "stext.h"
#include "simage.h"


namespace DataStorage {

  class SSource : public SPicture {
  public:

    SSource() : m_sourceName("") {
    };

    virtual ~SSource() {
    };

    inline void setSourceName(std::string name) {
      m_sourceName = name;
    }

    inline std::string getSourceName() const {
      return m_sourceName;
    }

  protected:
    std::string m_sourceName;

  private:

  };
}
#endif

