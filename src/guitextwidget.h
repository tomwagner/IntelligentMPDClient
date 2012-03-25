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
 * @file textwidget.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief GTK TextWidget for viewing text informations.
 */

#ifndef TEXTWIDGET_H
#define	TEXTWIDGET_H
#include "global.h"
#include <gtkmm-3.0/gtkmm.h>
namespace GUI {

  class TextWidget : public Gtk::Bin {
  public:
    TextWidget();
    virtual ~TextWidget();

    void setText(std::string input);
  private:
  protected:
    Gtk::VBox vbox;
    Gtk::Label title;
    Gtk::Label text;
    Gtk::Button button;

  };
}
#endif	/* TEXTWIDGET_H */

