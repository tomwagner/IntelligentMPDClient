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
 * @file guicoverwidget.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief GTK GUI Album Cover Widget
 */

#ifndef COVERWIDGET_H
#define	COVERWIDGET_H
#include <gtkmm.h>
#include "storage.h"
#include <map>
#include "salbum.h"

using namespace DataStorage;

namespace GUI {

  class CoverWidget {
  public:
    CoverWidget(Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~CoverWidget();

    void setCoverWidget(std::map<std::string, SAlbum *>* articlesList);
    void hide();
    void show();

    void updateCoverWidget();
    void showCover(SAlbum * a);
    void clearCover();
    void showLoading();
    void on_slideshowStart();
    void run();
    void stop();
    bool slideLeft();
  private:
    int currentPosition;

    bool slideshowStarted;
    std::string loadingIcon;

    int coverWidth;
    int coverHeight;

    std::map<std::string, SAlbum *>* coverList;
    std::map<std::string, SAlbum *>::iterator it;
  protected:
    sigc::connection updateTimeout;
    Gtk::EventBox * coverEventBox;
    Gtk::Image * cover;

    // feedback
    Gtk::Button * coverRight;
    Gtk::Button * coverWrong;

    void rightFeedback();
    void wrongFeedback();
    void checkIfICanVote();

    void on_next();
    void on_prev();
    bool on_slideshowScroll(GdkEventScroll * s);

    // classification
    Gtk::Image * coverClass;
    void checkClassOfObject(SAlbum * s);


  };
}
#endif

