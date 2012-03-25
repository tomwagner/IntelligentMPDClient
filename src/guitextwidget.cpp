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
 * @file textwidget.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief GTK TextWidget for viewing text informations.
 */

#include "guitextwidget.h"

namespace GUI {


  TextWidget::TextWidget() {

    vbox.add(title);
    vbox.add(text);
    vbox.add(button);
    
//    title.set_label("ahoj");
//    text.set_label("xxx");
    
    vbox.show();
    add(vbox);
    show();
    
  }


  TextWidget::~TextWidget() {
  }


  void TextWidget::setText(std::string input) {
    text.set_label(input);
  }


}
