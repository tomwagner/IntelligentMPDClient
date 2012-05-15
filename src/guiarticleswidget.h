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

#ifndef ARTICLESWIDGET_H
#define	ARTICLESWIDGET_H
#include <gtkmm.h>
#include <cstring>
#include "storage.h"
#include <map>
#include "sarticle.h"
#include "utils.h"

using namespace DataStorage;

namespace GUI {

  class ArticlesWidget {
  public:
    ArticlesWidget(Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~ArticlesWidget();

    void setArticlesWidget(SArtist * currentArtist);
    void updateArticlesWidget();
    void hide();
    void show();

    void clearArticlesWidget();

  private:
    SArtist * currentArtist;
    int currentPosition;
    bool slideshowStarted;
    void showArticle(SArticle *);

    void removeMarkup(std::string &s);

    std::map<std::string, SArticle *>* articlesList;
    std::map<std::string, SArticle *>::iterator it;
  protected:
    sigc::connection updateTimeout;
    Gtk::Label * articleTab;
    Gtk::Box * articlesWidget;
    Gtk::ScrolledWindow * articleWindow;
    Gtk::Label * articleTitle;
    Gtk::Label * articleAbout;
    Gtk::Image * articleSourceIcon;
    Gtk::Label * articleSourceName;
    Gtk::Label * articleSourceUrl;


    // slideshow
    Gtk::ToggleButton * articleSlideshow;
    Gtk::Image * articleSlideshowPlay;
    Gtk::Image * slideshowIconPause;
    Gtk::Button * articleNext;
    Gtk::Button * articlePrev;

    void on_slideshowStart();
    void on_next();
    void on_prev();

    void run();
    void stop();
    bool slideLeft();

    // feedback
    Gtk::Button * articleRight;
    Gtk::Button * articleWrong;

    void rightFeedback();
    void wrongFeedback();
    void checkIfICanVote();

    // classification
    Gtk::Image * articleClass;
    void checkClassOfObject(SArticle * s);

  };
}
#endif

