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

#include "global.h"
#include "gtkmm-3.0/gtkmm.h"
#include "mpdlistener.h"
#include "mpdclient.h"
#include "utils.h"

#include "guisources.h"
#include "guisettings.h"
#include "guitextwidget.h"
#include "clientsettings.h"

namespace GUI {

  class MainWindow : public MPD::Listener {
  public:
    MainWindow(MPD::Client * clientMPD, ClientSettings * clientSettings);
    virtual ~MainWindow();
    void show();
    void setCover(std::string filename);
    void redrawCover();
    void setSongLabel(std::string name);
    void setSongArtist(std::string name);
    void setSongName(std::string name);
    void setSongGenre(std::string name);
    void setSongLength(std::string length);
    void setSongAlbum(std::string name);
    void setSongWebpage(std::string markupWebpage);
    void setTextLabel(std::string artistName);
    void setStatusBarTitle(std::string text);
    void playerEvent();
    Gtk::Window * getWindow();


    void fullscreenSwitch();

    // sources window
    void showSourcesWindow();

    // settings windo
    void showSettingsWindow();

    // about dialog
    void showAboutDialog();

    // error dialog
    void showErrorDialog(std::string text);
    void showInfoDialog(std::string text);

  private:
    GtkWidget * tray_menu;
    bool onExposeEvent(GdkEventExpose *event);
    int width;
    int height;
  protected:
    TextWidget *test;
    Gtk::Window* mainWindow;
    Gtk::Box * hBox;
    Gtk::Label* songLabel;
    Gtk::Label* songArtist;
    Gtk::Label* songName;
    Gtk::Label* songGenre;
    Gtk::Label* songAlbum;
    Gtk::Label* songWebpage;
    Gtk::Label* songLength;
    Gtk::Label* textLabel;
    Gtk::Image* picture;
    Gtk::Statusbar* statusBar;
    MPD::Client* clientMPD;
    ClientSettings* clientSettings;

    // Widgety klienta
    //TextWidget textWidget;

    // menu widgety
    Gtk::ImageMenuItem * quit;
    Gtk::CheckMenuItem * fullscreen;
    Gtk::ImageMenuItem * sourceSettings;
    Gtk::ImageMenuItem * progSettings;
    Gtk::ImageMenuItem * about;

    // dialog about
    Gtk::AboutDialog * aboutDialog;
    Gtk::ButtonBox * aboutDialogClose;

    // okno sources
    Gtk::Window * sourcesWindow;
    Gtk::Box * sourcesList;

    // okno settings
    Gtk::Window * settingsWindow;

    // Control widget
    Gtk::Button * first;
    Gtk::Button * back;
    Gtk::ToggleButton * play;
    Gtk::Button * stop;
    Gtk::Button * next;
    Gtk::Button * last;
    
    Gtk::Image * playIcon;
    
    // slideshow widget
    Gtk::Image* slide;

    
    // events
    void on_back();
    void on_playOrPause();
    void on_stop();
    void on_next();
  };
}

#endif	/* GUI_H */

