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

#ifndef ARTISTSWIDGET_H
#define	ARTISTSWIDGET_H
#include <gtkmm.h>
#include "storage.h"
#include <list>
#include "sarticle.h"
#include "mpdclient.h"

using namespace DataStorage;

namespace GUI {

  class ArtistsWidget {
  public:
    ArtistsWidget(Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~ArtistsWidget();
    void show();
    void hide();
    void reload();
  private:

    bool checkMPDConnection();
    void loadArtists();
    void insertReturn();
    void loadArtistsSongs(std::string artist);

    static bool actualSongSortCond(const MPD::Song* s1, const MPD::Song* s2);
  protected:
    std::string selectedArtist;
    MPD::TagList artists;
    std::list<std::string> artistList;
    MPD::SongList m_actualSongs;

    Gtk::Viewport * artistViewPort;
    Gtk::ScrolledWindow * artistScrolledWindow;
    Gtk::Label * artistsLabel;
    Gtk::IconView * artistsView;
    Glib::RefPtr<Gtk::ListStore> treeModel;

    Gtk::Image noimage;


    void loadArtistsFromMPD();

    class ArtistColumns : public Gtk::TreeModel::ColumnRecord {
    public:

      ArtistColumns() {
        add(artist);
        add(image);
        add(isArtist);
        add(song);
      }

      Gtk::TreeModelColumn<Glib::ustring> artist;
      Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > image;
      Gtk::TreeModelColumn<int> isArtist;
      Gtk::TreeModelColumn<MPD::Song*> song;
    };

    ArtistColumns columns;

    bool on_resize(GdkEvent*);
    void on_resize2(Glib::RefPtr<Gdk::Screen> g);
    void on_resize3();
    void on_itemClick(Gtk::TreeModel::Path p);
  };
}
#endif

