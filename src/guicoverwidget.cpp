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
 * @file guicoverwidget.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief GTK GUI Album Cover Widget
 */


#include "guicoverwidget.h"

#include <iostream>
#include <cassert>

using namespace DataStorage;

namespace GUI {


  CoverWidget::CoverWidget(Glib::RefPtr<Gtk::Builder>& builder) :
  currentPosition(0),
  it(NULL),
  coverList(NULL) {
    builder->get_widget("coverEventBox", coverEventBox);
    builder->get_widget("albumCover", cover);
    coverWidth = 200;
    coverHeight = 200;

    //feedback
    builder->get_widget("coverRight", coverRight);
    builder->get_widget("coverWrong", coverWrong);

    coverRight->signal_clicked().connect(sigc::mem_fun(*this, &CoverWidget::rightFeedback));
    coverWrong->signal_clicked().connect(sigc::mem_fun(*this, &CoverWidget::wrongFeedback));

    //class
    builder->get_widget("coverClass", coverClass);

    // mouse scrolling
    coverEventBox->signal_scroll_event().connect(sigc::mem_fun(*this, &CoverWidget::on_slideshowScroll));

  }


  CoverWidget::~CoverWidget() {
  }


  void CoverWidget::updateCoverWidget() {
    if (coverList == NULL) return;
    if (!coverList->empty()) {
      if (currentPosition == 0)
        showCover(it->second);
    }
  }


  void CoverWidget::showCover(SAlbum * s) {
    std::ostringstream position;
    int albumListLen = coverList->size();
    position << (currentPosition % (albumListLen) + 1);
    position << "/";
    position << coverList->size();


    checkIfICanVote();

    checkClassOfObject(s);

    if (!s->img->getLocalPath().empty()) {
      cover->set(s->img->resize(coverWidth, coverHeight));
    }


  }


  void CoverWidget::setCoverWidget(std::map<std::string, SAlbum *>* list) {
    if (list == NULL) return;

    coverList = list;
    it = coverList->begin();

    if (it->second == NULL) return;

    // check for voting
    //    checkIfICanVote();

    // show slide
    if (!coverList->empty())
      showCover(it->second);
    else
      clearCover();
  }


  void CoverWidget::clearCover() {
    currentPosition = 0;
    showLoading();

    // clear classification
    coverClass->set_from_icon_name("gtk-yes", Gtk::ICON_SIZE_SMALL_TOOLBAR);
  }


  //  void CoverWidget::hide() {
  //    CoverWidget.hide();
  //  }
  //
  //
  //  void CoverWidget::show() {
  //    CoverWidget.show();
  //  }


  void CoverWidget::showLoading() {
    cover->set_from_icon_name("gtk-no", Gtk::ICON_SIZE_DIALOG);
  }


  void CoverWidget::run() {

    if (!slideshowStarted) {
      slideshowStarted = true;
      // plan cyclic update
      updateTimeout = Glib::signal_timeout().connect(
              sigc::mem_fun(*this, &CoverWidget::slideLeft), 5000);
    }
  }


  void CoverWidget::stop() {
    if (slideshowStarted) {
      slideshowStarted = false;
    }
  }


  bool CoverWidget::slideLeft() {
    if (slideshowStarted) {
      on_next();
      return true;
    } else {
      return false;
    }

  }


  void CoverWidget::on_next() {
    if (it->second == NULL) return;
    if (coverList->empty()) return;
    it++;
    currentPosition++;

    if (it == coverList->end()) it = coverList->begin();
    showCover(it->second);
  }


  void CoverWidget::on_prev() {
    if (it->second == NULL) return;
    if (coverList->empty()) return;
    if (it == coverList->begin()) it = coverList->end();
    it--;
    if (currentPosition == 0) currentPosition = coverList->size();
    currentPosition--;

    showCover(it->second);

  }


  void CoverWidget::rightFeedback() {
    if (it->second == NULL) return;

    if (!it->second->canIVote()) return;
    it->second->voteRight();
    checkIfICanVote();
    // detect new class of object
    checkClassOfObject(it->second);
  }


  void CoverWidget::wrongFeedback() {
    if (it->second == NULL) return;

    if (!it->second->canIVote()) return;
    it->second->voteWrong();
    checkIfICanVote();
    // detect new class of object
    checkClassOfObject(it->second);
  }


  void CoverWidget::checkIfICanVote() {
    if (it->second == NULL) return;

    if (it->second->canIVote()) {
      coverRight->set_sensitive(true);
      coverWrong->set_sensitive(true);
    } else {
      coverRight->set_sensitive(false);
      coverWrong->set_sensitive(false);
    }
  }


  void CoverWidget::checkClassOfObject(SAlbum * s) {
    if (s->objectclass == NaiveBayes::firstClass) {
      coverClass->set_from_icon_name("gtk-yes", Gtk::ICON_SIZE_SMALL_TOOLBAR);
    } else if (s->objectclass == NaiveBayes::secondClass) {
      coverClass->set_from_icon_name("gtk-close", Gtk::ICON_SIZE_SMALL_TOOLBAR);
    } else {
      coverClass->set_from_icon_name("gtk-help", Gtk::ICON_SIZE_SMALL_TOOLBAR);
    }
  }


  bool CoverWidget::on_slideshowScroll(GdkEventScroll * s) {

    if (s->direction == GDK_SCROLL_DOWN) {
      on_next();
    } else if (s->direction == GDK_SCROLL_UP) {
      on_prev();
    }
  }
}
