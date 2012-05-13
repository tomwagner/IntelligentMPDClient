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
 * @file guislideshowwidget.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief GTK GUI Link Widget
 */

#ifndef GUILINKWIDGET_H
#define	GUILINKWIDGET_H
#include <gtkmm.h>
#include <list>

namespace GUI {

  typedef struct {
    std::string linkIcon;
    std::string linkUrl;
    std::string linkText;
  } Link;

  class LinkWidget {
  public:
    LinkWidget();
    virtual ~LinkWidget();
  private:
    std::list<Link> linkList;
    std::list<Link>::iterator it;

  };
}
#endif	/* GUILINKWIDGET_H */

