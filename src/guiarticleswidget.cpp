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


  ArticlesWidget::ArticlesWidget(Glib::RefPtr<Gtk::Builder>& builder) :
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
    builder->get_widget("articleNext", articleNext);
    articleNext->set_name("articleNext");
    builder->get_widget("articlePrev", articlePrev);

    articleNext->signal_clicked().connect(sigc::mem_fun(*this, &ArticlesWidget::on_next));
    articlePrev->signal_clicked().connect(sigc::mem_fun(*this, &ArticlesWidget::on_prev));

    //feedback
    builder->get_widget("articleRight", articleRight);
    builder->get_widget("articleWrong", articleWrong);

    articleRight->signal_clicked().connect(sigc::mem_fun(*this, &ArticlesWidget::rightFeedback));
    articleWrong->signal_clicked().connect(sigc::mem_fun(*this, &ArticlesWidget::wrongFeedback));

    // classification info
    builder->get_widget("articleClass", articleClass);
  }


  ArticlesWidget::~ArticlesWidget() {
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


  void ArticlesWidget::setArticlesWidget(std::map<std::string, SArticle *>* list) {
    articlesList = list;
    it = articlesList->begin();

    checkIfICanVote();

    if (!articlesList->empty())
      showArticle(it->second);
    else
      clearArticlesWidget();
  }


  void ArticlesWidget::clearArticlesWidget() {
    articleTab->set_label("Articles");
    
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
    if (articlesList == NULL) return;
    if (!articlesList->empty()) {
      if (it->second == NULL)
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
    checkIfICanVote();
    checkClassOfObject(it->second);
  }


  void ArticlesWidget::wrongFeedback() {
    if (it->second == NULL) return;

    if (!it->second->canIVote()) return;
    it->second->voteWrong();
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
