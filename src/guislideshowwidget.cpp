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
 * @brief GTK GUI Slideshow Widget
 */

#include "guislideshowwidget.h"
#include <iostream>

#define DEBUG 0

namespace GUI {

  const int slideshowSlideshowTimeout = 3000;


  SlideshowWidget::SlideshowWidget(Glib::RefPtr<Gtk::Builder>& builder) :
  slideshowStarted(false),
  slideWidth(100),
  slideHeight(100),
  loadingIcon("ui/loader.gif"),
  it(NULL) {
    builder->get_widget("slideshowTab", slideshowTab);
    builder->get_widget("slideshowWidget", slideshowWidget);
    builder->get_widget("slideTitle", slideTitle);
    builder->get_widget("slide", slide);
    builder->get_widget("slideEventBox", slideEventBox);
    builder->get_widget("slideSourceIcon", slideSourceIcon);
    builder->get_widget("slideSourceName", slideSourceName);
    builder->get_widget("slideSourceUrl", slideSourceUrl);
    builder->get_widget("slideSlideshow", slideSlideshow);
    builder->get_widget("slideSlideshowPlay", slideSlideshowPlay);
    slideshowIconPause = new Gtk::Image;
    slideshowIconPause->set_from_icon_name("gtk-media-pause", Gtk::ICON_SIZE_SMALL_TOOLBAR);
    builder->get_widget("slideNext", slideNext);
    builder->get_widget("slidePrev", slidePrev);

    // classification info
    builder->get_widget("slideClass", slideClass);

    // mouse scrolling
    slideEventBox->signal_scroll_event().connect(sigc::mem_fun(*this, &SlideshowWidget::on_slideshowScroll));
    slideEventBox->signal_event().connect(sigc::mem_fun(*this, &SlideshowWidget::on_expose));

    // control buttons actions
    slideSlideshow->signal_clicked().connect(sigc::mem_fun(*this, &SlideshowWidget::on_slideshowStart));
    slideNext->signal_clicked().connect(sigc::mem_fun(*this, &SlideshowWidget::on_next));
    slidePrev->signal_clicked().connect(sigc::mem_fun(*this, &SlideshowWidget::on_prev));

    //feedback
    builder->get_widget("slideRight", slideRight);
    builder->get_widget("slideWrong", slideWrong);

    slideRight->signal_clicked().connect(sigc::mem_fun(*this, &SlideshowWidget::rightFeedback));
    slideWrong->signal_clicked().connect(sigc::mem_fun(*this, &SlideshowWidget::wrongFeedback));
  }


  SlideshowWidget::~SlideshowWidget() {
  }


  void SlideshowWidget::updateSlideshowWidget() {
    if (slideList == NULL) return;
    if (!slideList->empty()) {
      if (it->second == NULL)
        it = slideList->begin();
      showSlide(it->second);
    }
  }


  void SlideshowWidget::showSlide(SSlide * s) {
    std::ostringstream position;
    int slideListLen = slideList->size();
    position << (currentPosition % (slideListLen) + 1);
    position << "/";
    position << slideList->size();


    checkIfICanVote();

    checkClassOfObject(s);
    if (!s->img->title.empty())
      slideTitle->set_label(s->img->title);
    else if (!s->img->alt.empty())
      slideTitle->set_label(s->img->alt);
    else if (!s->img->context.empty() && s->img->context.length() < 100)
      slideTitle->set_label(s->img->context);


    slideshowTab->set_label("Slideshow [" + position.str() + "]");
    if (!s->getLocalPath().empty()) {
      slide->set(s->img->resize(slideWidth, slideHeight));
      if (Glib::file_test(s->getLocalPath(), Glib::FILE_TEST_EXISTS))
        slideSourceIcon->set(s->resize(16, 16));
    }
    slideSourceName->set_label(s->getName());

    // convert to html specialchars
    std::string url = s->getSourceName();
    utils::htmlSpecialChars(url);

    slideSourceUrl->set_markup("<a href=\"" + url + "\">" + url + "</a>");

  }


  void SlideshowWidget::setSlideshowWidget(SArtist* artist) {
    currentArtist = artist;
    slideList = &currentArtist->images;
    it = slideList->begin();

    // check for voting
    checkIfICanVote();

    // show slide
    if (!slideList->empty())
      showSlide(it->second);
    else
      clearSlide();
  }


  void SlideshowWidget::clearSlide() {
    slideshowTab->set_label("Slideshow");
    slideTitle->set_label("");
    showLoading();

    // clear sources
    slideSourceIcon->set_from_icon_name("gtk-missing-image", Gtk::ICON_SIZE_SMALL_TOOLBAR);
    slideSourceName->set_label("");
    slideSourceUrl->set_markup("");

    // clear classification
    slideClass->set_from_icon_name("gtk-yes", Gtk::ICON_SIZE_SMALL_TOOLBAR);
  }


  void SlideshowWidget::hide() {
    slideshowWidget->hide();
  }


  void SlideshowWidget::show() {
    slideshowWidget->show();
  }


  void SlideshowWidget::showLoading() {
    if (Glib::file_test(loadingIcon.c_str(), Glib::FILE_TEST_EXISTS))
      slide->set(Gdk::PixbufAnimation::create_from_file(loadingIcon.c_str()));
  }


  void SlideshowWidget::on_slideshowStart() {
    if (slideSlideshow->get_active()) {
      run();
      slideSlideshow->set_image(*slideshowIconPause);
    } else {
      stop();
      slideSlideshow->set_image(*slideSlideshowPlay);
    }
}


void SlideshowWidget::run() {

  if (!slideshowStarted) {
    slideshowStarted = true;
    // plan cyclic update
    updateTimeout = Glib::signal_timeout().connect(
            sigc::mem_fun(*this, &SlideshowWidget::slideLeft), slideshowSlideshowTimeout);
  }
}


void SlideshowWidget::stop() {
  if (slideshowStarted) {
    slideshowStarted = false;
  }
}


bool SlideshowWidget::slideLeft() {
  if (slideshowStarted) {
    on_next();
    return true;
  } else {
    return false;
  }
}


void SlideshowWidget::on_next() {
  if (it->second == NULL) return;
  it++;
  currentPosition++;

  if (it == slideList->end()) it = slideList->begin();

  showSlide(it->second);
}


void SlideshowWidget::on_prev() {
  if (it->second == NULL) return;
  if (it == slideList->begin()) it = slideList->end();
  it--;
  if (currentPosition == 0) currentPosition = slideList->size();
  currentPosition--;

  showSlide(it->second);

}


bool SlideshowWidget::on_expose(GdkEvent * e) {
  slideWidth = slideEventBox->get_width();
  slideHeight = slideEventBox->get_height();

  return false;
}


void SlideshowWidget::rightFeedback() {
  if (it->second == NULL) return;

  if (!it->second->canIVote()) return;
  it->second->voteRight();
  checkIfICanVote();

  // set current slide as right
  currentArtist->addAsRight(it->second->img->title);
  currentArtist->addAsRight(it->second->img->alt);
  currentArtist->addAsRight(it->second->img->context);

  // reclassificate all data
  currentArtist->classificateArtist();
  // detect new class of object
  checkClassOfObject(it->second);
}


void SlideshowWidget::wrongFeedback() {
  if (it->second == NULL) return;

  if (!it->second->canIVote()) return;
  it->second->voteWrong();
  checkIfICanVote();

  currentArtist->addAsWrong(it->second->img->title);
  currentArtist->addAsWrong(it->second->img->alt);
  currentArtist->addAsWrong(it->second->img->context);

  // reclassificate all data
  currentArtist->classificateArtist();
  // detect new class of object
  checkClassOfObject(it->second);
}


void SlideshowWidget::checkIfICanVote() {
  if (it->second == NULL) return;

  if (it->second->canIVote()) {
    slideRight->set_sensitive(true);
    slideWrong->set_sensitive(true);
  } else {
    slideRight->set_sensitive(false);
    slideWrong->set_sensitive(false);
  }
}


void SlideshowWidget::checkClassOfObject(SSlide * s) {
  if (s->objectclass == NaiveBayes::firstClass) {
    slideClass->set_from_icon_name("gtk-yes", Gtk::ICON_SIZE_SMALL_TOOLBAR);
  } else if (s->objectclass == NaiveBayes::secondClass) {
    slideClass->set_from_icon_name("gtk-close", Gtk::ICON_SIZE_SMALL_TOOLBAR);
  } else {
    slideClass->set_from_icon_name("gtk-help", Gtk::ICON_SIZE_SMALL_TOOLBAR);
  }
}


bool SlideshowWidget::on_slideshowScroll(GdkEventScroll * s) {

  if (s->direction == GDK_SCROLL_DOWN) {
    on_next();
  } else if (s->direction == GDK_SCROLL_UP) {
    on_prev();
  }
}
}