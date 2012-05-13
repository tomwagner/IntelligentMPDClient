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
 * @file guimainwindow.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief GTK Main Window
 */


#ifndef MAINWINDOW_H
#define	MAINWINDOW_H

#include <gtkmm-3.0/gtkmm.h>
#include <gtkmm-3.0/gtkmm/volumebutton.h>
#include <glibmm/fileutils.h>
#include <glibmm/ustring.h>

#include "mpdclient.h"
#include "utils.h"

#include "guisources.h"
#include "guisettings.h"
#include "guiarticleswidget.h"
#include "guislideshowwidget.h"
#include "guiartistswidget.h"
#include "guitextwidget.h"
#include "guicoverwidget.h"
#include "clientsettings.h"

#include <stack>
namespace GUI {

  typedef struct {
    std::string type;
    std::string message;
  } Dialog;

  class MainWindow {
  public:
    MainWindow();
    virtual ~MainWindow();


    void setAlbumCover(std::string filepath);
    void setAlbumCover(const Glib::RefPtr<Gdk::Pixbuf> p);
    void setSongLabel(std::string name);
    void setArtist(std::string name);
    void setTitle(std::string name);
    void setGenre(std::string name);
    void setBitrate(std::string length);
    void setAlbum(std::string name);
    void setWebpage(std::string markupWebpage);


    void setStatusBar(std::string text);
    bool updateGUI();
    void updatePlayer0(MPD::Client *, MPD::StatusChanges changed, void *);
    Gtk::Window * getWindow() const;


    // artist widget
    void setArtistAbout(std::string);
    void setArtistTitle(std::string);
    void setArtistSource(std::string iconPath, std::string name, std::string url);


    void fullscreenSwitch();
    void feedbackSwitch();

    // sources window
    void showSourcesWindow();

    // settings windo
    void showSettingsWindow();

    // about dialog
    void showAboutDialog();

    // error dialog
    void showErrorDialog(std::string text);
    void showInfoDialog(std::string text);

    // control widget
    void setVolume(double vol);
    //    void setTime(double time);

    bool setTime(GdkEventButton * button);
    void setTimeScale(double elapsedTime, double totalTime);
    void showDialogs();
  private:

    Gtk::Widget * tray_menu;
    bool onExposeEvent(GdkEvent * event);
  protected:
    // gui dialogs notification

    std::stack<Dialog> dialogs;


    sigc::connection updateTimeout;

    // GUI Widgets
  public:
    ArticlesWidget* articlesWidget;
    ArtistsWidget * artistsWidget;
    SlideshowWidget* slideshowWidget;
    CoverWidget* coverWidget;
  protected:
    Gtk::Window* mainWindow;

    Gtk::Label* songLabel;
    Gtk::Label* songArtist;
    Gtk::Label* songName;
    Gtk::Label* songGenre;
    Gtk::Label* songAlbum;
    Gtk::Label* songWebpage;
    Gtk::Label* songBitrate;

    Gtk::Image* albumCover;
    Gtk::Statusbar* statusBar;


    // menu widgets
    Gtk::ImageMenuItem * connect;
    Gtk::ImageMenuItem * restart;
    Gtk::ImageMenuItem * disconnect;
    Gtk::ImageMenuItem * info;
    Gtk::ImageMenuItem * update;
    Gtk::ImageMenuItem * quit;
    Gtk::CheckMenuItem * showFeedbackButtons;
    Gtk::CheckMenuItem * fullscreen;
    Gtk::ImageMenuItem * sourceSettings;
    Gtk::ImageMenuItem * progSettings;
    Gtk::ImageMenuItem * about;



    // Control widget
    Gtk::ProgressBar * timeScale;
    Gtk::EventBox * timeEventBox;
    Glib::RefPtr<Gtk::Adjustment> timeAdj;
    Gtk::VolumeButton * volumeScale;

    Gtk::Button * first;
    Gtk::Button * back;
    Gtk::ToggleButton * play;
    Gtk::Image * playImage;
    Gtk::Image * pauseImage;
    Gtk::Button * stop;
    Gtk::Button * next;
    Gtk::Button * last;

    Gtk::Image * playIcon;


    // artist widget
    Gtk::Label* artistTitle;
    Gtk::Label* artistAbout;
    Gtk::Image* artistSourceIcon;
    Gtk::Label* artistSourceName;
    Gtk::Label* artistSourceUrl;


    // feedback widgets
    Gtk::Button * coverRight;
    Gtk::Button * coverWrong;
    Gtk::Button * artistRight;
    Gtk::Button * artistWrong;
    Gtk::Button * articleRight;
    Gtk::Button * articleWrong;
    Gtk::Button * slideRight;
    Gtk::Button * slideWrong;


    // dialog about
    Gtk::AboutDialog * aboutDialog;
    Gtk::ButtonBox * aboutDialogClose;

    // okno sources
    Gtk::Window * sourcesWindow;
    Gtk::Box * sourcesList;

    // okno settings
    Gtk::Window * settingsWindow;

    // events
    void on_first();
    void on_back();
    void on_playOrPause();
    void on_stop();
    void on_next();
    void on_last();

    void on_connect();
    void on_disconnect();
    void on_update_mpd_db();
    void on_info_mpd();
  };
}
extern Gtk::Main * kit;
extern GUI::MainWindow * gui;
#endif	/* GUI_H */

