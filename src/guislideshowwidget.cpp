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

#define DEBUG 1

namespace GUI {

  const int slideshowSlideshowTimeout = 3000;


  SlideshowWidget::SlideshowWidget(Glib::RefPtr<Gtk::Builder>& builder) :
  currentArtist(NULL),
  slideshowStarted(false),
  slideWidth(100),
  slideHeight(100),
  loadingIcon("ui/loader.gif"),
  it(NULL),
  currentPosition(0),
  m_presentation(true) {
    builder->get_widget("slideshowTab", slideshowTab);
    builder->get_widget("slideshowWidget", slideshowWidget);
    builder->get_widget("slideTitle", slideTitle);
    builder->get_widget("slide", slide);
    builder->get_widget("slideEventBox", slideEventBox);
    builder->get_widget("slideRelevance", slideRelevance);
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

    setActive(false);
  }


  SlideshowWidget::~SlideshowWidget() {
  }


  void SlideshowWidget::updateSlideshowWidget() {
    if (currentArtist == NULL) return;
    if (slideList == NULL) return;
    if (!slideList->empty()) {
      if (currentPosition == 0) {
        it = slideList->begin();
      }
      slideshowSize = getListSize();
      showSlide();
    }
  }


  unsigned SlideshowWidget::getListSize() {
    // if is no presentation mode set, return all elements size
    if (!m_presentation) return slideList->size();


    // presentation mode enabled, show only valid objects
    unsigned len = 0;
    std::map<std::string, SSlide *>::iterator p;
    for (p = slideList->begin(); p != slideList->end(); p++) {
      if ((p->second->objectclass == NaiveBayes::firstClass) || (p->second->objectclass == NaiveBayes::unknownClass)) len++;
    }
    return len;
  }


  void SlideshowWidget::showSlide() {
    std::ostringstream position;


    if (slideshowSize != 0) {
      position << (currentPosition % slideshowSize + 1);
      position << "/";
      position << slideshowSize;
    }

    // activate widget
    setActive(true);
    checkIfICanVote();


    checkClassOfObject(it->second);
    if (!it->second->img->title.empty())
      slideTitle->set_label(it->second->img->title);
    else if (!it->second->img->alt.empty())
      slideTitle->set_label(it->second->img->alt);
    else if (!it->second->img->context.empty() && it->second->img->context.length() < 100)
      slideTitle->set_label(it->second->img->context);


    slideshowTab->set_label("Slideshow [" + position.str() + "]");
    if (!it->second->getLocalPath().empty()) {
      slide->set(it->second->img->resize(slideWidth, slideHeight));
      if (Glib::file_test(it->second->getLocalPath(), Glib::FILE_TEST_EXISTS))
        slideSourceIcon->set(it->second->resize(16, 16));
    }

    // set relevance label
    setRelevance(it->second->relevance);

    slideSourceName->set_label(it->second->getName());

    // convert to html specialchars
    std::string url = it->second->getSourceName();
    utils::htmlSpecialChars(url);

    slideSourceUrl->set_markup("<a href=\"" + url + "\">" + url + "</a>");
  }


  void SlideshowWidget::setActive(bool b) {
    slideNext->set_sensitive(b);
    slidePrev->set_sensitive(b);

    slideRight->set_sensitive(b);
    slideWrong->set_sensitive(b);
    slideSlideshow->set_sensitive(b);
  }


  void SlideshowWidget::setSlideshowWidget(SArtist * artist) {

    currentArtist = artist;
    slideList = &currentArtist->images;
    it = currentArtist->images.begin();

    // check for voting
    //    checkIfICanVote();

    // show slide
    if (!slideList->empty())
      showSlide();
    else
      clearSlide();
  }


  void SlideshowWidget::clearSlide() {
    currentPosition = 0;

    setActive(false);

    slideshowTab->set_label("Slideshow");
    slideTitle->set_label("");
    showLoading();

    slideRelevance->set_label("");

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

    // if presenattion mode enabled, skip second class
    if (m_presentation) {
      // search for data with first class
      while ((it->second->objectclass == NaiveBayes::secondClass) && (slideshowSize > 0)) {//|| (it->second->objectclass == NaiveBayes::unknownClass)
        it++;
        if (it == slideList->end()) it = slideList->begin();
      }
    }

    showSlide();
  }


  void SlideshowWidget::on_prev() {
    if (it->second == NULL) return;
    if (it == slideList->begin()) it = slideList->end();
    it--;
    if (currentPosition == 0) currentPosition = slideList->size();
    currentPosition--;


    // if presenattion mode enabled, skip second class
    if (m_presentation) {
      // search for data with first class
      if (it->second == NULL) return;
      while ((it->second->objectclass == NaiveBayes::secondClass) && (slideshowSize > 0)) { //|| (it->second->objectclass == NaiveBayes::unknownClass)
        it--;
        if (it->second == NULL) it = slideList->end();
      }
    }

    if (it->second == NULL) return;
    showSlide();

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

    // if presentation enabled, show next after wrong feedback
    if (m_presentation) {
      // show me next relevant
      on_next();
    }
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
    // set relevance label
    setRelevance(s->relevance);
  }


  bool SlideshowWidget::on_slideshowScroll(GdkEventScroll * s) {

    if (s->direction == GDK_SCROLL_DOWN) {
      on_next();
    } else if (s->direction == GDK_SCROLL_UP) {
      on_prev();
    }
  }


  void SlideshowWidget::setRelevance(float rel) {
    std::ostringstream relevance;
    float precent = rel * 100;
    relevance << std::fixed << std::setprecision(2) << precent << '%';


    slideRelevance->set_label(relevance.str());
  }


  void SlideshowWidget::enablePresentationMode() {
    m_presentation = true;
    showSlide();
  }


  void SlideshowWidget::disablePresentationMode() {
    m_presentation = false;
    showSlide();
  }
}