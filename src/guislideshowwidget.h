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

#ifndef GUISLIDESHOWWIDGET_H
#define	GUISLIDESHOWWIDGET_H
#include <gtkmm.h>
#include <cairomm/cairomm.h>
#include <list>
#include "sartist.h"
#include "utils.h"



using namespace DataStorage;

namespace GUI {

  class SlideshowWidget {
  public:
    SlideshowWidget(Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~SlideshowWidget();

    void setSlideshowWidget(SArtist* currentArtist);
    void updateSlideshowWidget();

    void hide();
    void show();

    void showLoading();
    void clearSlide();
    void run();
    void stop();


  private:
    SArtist * currentArtist;
    bool slideshowStarted;
    std::string loadingIcon;

    int slideWidth;
    int slideHeight;

    bool slideLeft();
    void showSlide(SSlide * s);

    std::map<std::string, SSlide *>* slideList;
    std::map<std::string, SSlide *>::iterator it;
  protected:
    sigc::connection updateTimeout;

    Gtk::Label * slideshowTab;
    Gtk::Box * slideshowWidget;
    Gtk::Label * slideTitle;
    Gtk::Image * slide;
    Gtk::EventBox * slideEventBox;
    Gtk::Image * slideSourceIcon;
    Gtk::Label * slideSourceName;
    Gtk::Label * slideSourceUrl;
    Gtk::ToggleButton * slideSlideshow;
    Gtk::Image * slideSlideshowPlay;
    Gtk::Image * slideshowIconPause;
    Gtk::Button * slideNext;
    Gtk::Button * slidePrev;

    void on_slideshowStart();
    void on_next();
    void on_prev();
    bool on_expose(GdkEvent * e);
    bool on_slideshowScroll(GdkEventScroll * s);

    Gtk::Box * slideBox;
    Gtk::Label * slideText;

    // feedback
    Gtk::Button * slideRight;
    Gtk::Button * slideWrong;


    void rightFeedback();
    void wrongFeedback();
    void checkIfICanVote();

    // classification
    Gtk::Image * slideClass;
    void checkClassOfObject(SSlide * s);
    void setActive(bool b);
  private:
    int currentPosition;


  };
}
#endif

