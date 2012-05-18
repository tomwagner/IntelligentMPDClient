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
 * @brief GTK GUI Articles Widget
 */

#include "guiarticleswidget.h"

#include <iostream>
#include <cassert>

#define DEBUG 0

using namespace DataStorage;

namespace GUI {

  const int articleSlideshowTimeout = 3000;


  ArticlesWidget::ArticlesWidget(Glib::RefPtr<Gtk::Builder>& builder) :
  currentArtist(NULL),
  currentPosition(0),
  it(NULL) {
    builder->get_widget("articleTab", articleTab);
    builder->get_widget("articleWindow", articleWindow);
    builder->get_widget("articlesWidget", articlesWidget);
    builder->get_widget("articleTitle", articleTitle);
    builder->get_widget("articleAbout", articleAbout);
    builder->get_widget("articleSourceIcon", articleSourceIcon);
    builder->get_widget("articleSourceName", articleSourceName);
    builder->get_widget("articleSourceUrl", articleSourceUrl);


    // slideshow
    slideshowIconPause = new Gtk::Image;
    slideshowIconPause->set_from_icon_name("gtk-media-pause", Gtk::ICON_SIZE_SMALL_TOOLBAR);
    builder->get_widget("articleSlideshow", articleSlideshow);
    builder->get_widget("articleSlideshowPlay", articleSlideshowPlay);
    builder->get_widget("articleNext", articleNext);
    articleNext->set_name("articleNext");
    builder->get_widget("articlePrev", articlePrev);

    articleSlideshow->signal_clicked().connect(sigc::mem_fun(*this, &ArticlesWidget::on_slideshowStart));
    articleNext->signal_clicked().connect(sigc::mem_fun(*this, &ArticlesWidget::on_next));
    articlePrev->signal_clicked().connect(sigc::mem_fun(*this, &ArticlesWidget::on_prev));

    //feedback
    builder->get_widget("articleRight", articleRight);
    builder->get_widget("articleWrong", articleWrong);

    articleRight->signal_clicked().connect(sigc::mem_fun(*this, &ArticlesWidget::rightFeedback));
    articleWrong->signal_clicked().connect(sigc::mem_fun(*this, &ArticlesWidget::wrongFeedback));

    // classification info
    builder->get_widget("articleClass", articleClass);

    clearArticlesWidget();
  }


  ArticlesWidget::~ArticlesWidget() {
    delete slideshowIconPause;
  }


  void ArticlesWidget::removeMarkup(std::string &s) {
    utils::replace(s, "&amp;", "&");
  }


  void ArticlesWidget::showArticle(SArticle * a) {

    std::ostringstream position;
    int articlesListLen = articlesList->size();
    position << (currentPosition % (articlesListLen) + 1);
    position << "/";
    position << articlesList->size();

    setActive(true);

    checkIfICanVote();
    checkClassOfObject(a);

    articleTab->set_label("Articles [" + position.str() + "]");

    std::string content = a->text->getText();
    removeMarkup(content);

    articleTitle->set_label(a->title->getText());
    articleAbout->set_label(content);


    if (!a->getLocalPath().empty()) {
      // source labels
      if (Glib::file_test(a->getLocalPath(), Glib::FILE_TEST_EXISTS))
        articleSourceIcon->set(a->resize(16, 16));
    }
    articleSourceName->set_label(a->getName());

    // convert to html specialchars
    std::string url = a->getSourceName();
    utils::htmlSpecialChars(url);

    articleSourceUrl->set_markup("<a href=\"" + url + "\">" + url + "</a>");
  }


  void ArticlesWidget::setArticlesWidget(SArtist * artist) {
    currentArtist = artist;
    articlesList = &currentArtist->articles;
    it = articlesList->begin();

    //    checkIfICanVote();
    if (!articlesList->empty())
      showArticle(it->second);
    else
      clearArticlesWidget();
  }


  void ArticlesWidget::setActive(bool b) {
    articleNext->set_sensitive(b);
    articlePrev->set_sensitive(b);

    articleRight->set_sensitive(b);
    articleWrong->set_sensitive(b);
    articleSlideshow->set_sensitive(b);
  }


  void ArticlesWidget::clearArticlesWidget() {
    // set position to 0
    currentPosition = 0;
    
    articleTab->set_label("Articles");

    setActive(false);

    articleTitle->set_label("");
    articleAbout->set_label("");

    // clear sources
    articleSourceIcon->set_from_icon_name("gtk-missing-image", Gtk::ICON_SIZE_SMALL_TOOLBAR);
    articleSourceName->set_label("");
    articleSourceUrl->set_markup("");


    // clear classification
    articleClass->set_from_icon_name("gtk-yes", Gtk::ICON_SIZE_SMALL_TOOLBAR);
  }


  void ArticlesWidget::updateArticlesWidget() {
    if (currentArtist == NULL) return;
    if (articlesList == NULL) return;
    if (!articlesList->empty()) {
      if (currentPosition == 0)
        it = articlesList->begin();
      showArticle(it->second);
    }
  }


  void ArticlesWidget::hide() {
    articlesWidget->hide();
  }


  void ArticlesWidget::show() {
    articlesWidget->show();
  }


  void ArticlesWidget::run() {
    if (!slideshowStarted) {
      slideshowStarted = true;
      // plan cyclic update
      updateTimeout = Glib::signal_timeout().connect(
              sigc::mem_fun(*this, &ArticlesWidget::slideLeft), articleSlideshowTimeout);
    }
  }


  bool ArticlesWidget::slideLeft() {
    if (slideshowStarted) {
      on_next();
      return true;
    } else {
      return false;
    }
  }


  void ArticlesWidget::stop() {
    if (slideshowStarted) {
      slideshowStarted = false;
    }
  }


  void ArticlesWidget::on_slideshowStart() {
    if (articleSlideshow->get_active()) {
      run();
      articleSlideshow->set_image(*slideshowIconPause);
    } else {
      stop();
      articleSlideshow->set_image(*articleSlideshowPlay);
    }
  }


  void ArticlesWidget::on_next() {
    if (it->second == NULL) return;

    it++;
    currentPosition++;

    if (it == articlesList->end()) it = articlesList->begin();


    showArticle(it->second);
  }


  void ArticlesWidget::on_prev() {
    if (it->second == NULL) return;

    if (it == articlesList->begin()) it = articlesList->end();
    it--;
    if (currentPosition == 0) currentPosition = articlesList->size();
    currentPosition--;


    showArticle(it->second);

  }


  void ArticlesWidget::rightFeedback() {
    if (it->second == NULL) return;

    if (!it->second->canIVote()) return;
    it->second->voteRight();

    // set current slide as right
    std::string text = it->second->text->getText();
    currentArtist->addAsRight(text);
    std::string title = it->second->title->getText();
    currentArtist->addAsRight(title);

    // reclassificate all data
    currentArtist->classificateArtist();

    checkIfICanVote();
    checkClassOfObject(it->second);
  }


  void ArticlesWidget::wrongFeedback() {
    if (it->second == NULL) return;

    if (!it->second->canIVote()) return;
    it->second->voteWrong();

    // set current slide as right
    std::string text = it->second->text->getText();
    currentArtist->addAsWrong(text);
    std::string title = it->second->title->getText();
    currentArtist->addAsWrong(title);

    // reclassificate all data
    currentArtist->classificateArtist();

    checkIfICanVote();
    checkClassOfObject(it->second);
  }


  void ArticlesWidget::checkIfICanVote() {
    if (it->second == NULL) return;

    if (it->second->canIVote()) {
      articleRight->set_sensitive(true);
      articleWrong->set_sensitive(true);
    } else {
      articleRight->set_sensitive(false);
      articleWrong->set_sensitive(false);
    }
  }


  void ArticlesWidget::checkClassOfObject(SArticle * s) {
    if (s->objectclass == NaiveBayes::firstClass) {
      articleClass->set_from_icon_name("gtk-yes", Gtk::ICON_SIZE_SMALL_TOOLBAR);
    } else if (s->objectclass == NaiveBayes::secondClass) {
      articleClass->set_from_icon_name("gtk-close", Gtk::ICON_SIZE_SMALL_TOOLBAR);
    } else {
      articleClass->set_from_icon_name("gtk-help", Gtk::ICON_SIZE_SMALL_TOOLBAR);
    }
  }
}
