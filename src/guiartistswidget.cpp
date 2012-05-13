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
 * @file guicoverwidget.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief GTK GUI Album Cover Widget
 */


#include "guiartistswidget.h"

#include <iostream>
#include <cassert>

using namespace DataStorage;

namespace GUI {


  ArtistsWidget::ArtistsWidget(Glib::RefPtr<Gtk::Builder>& builder)
  : selectedArtist("") {
    noimage.set("ui/dir.png");
    builder->get_widget("artistViewPort", artistViewPort);
    builder->get_widget("artistScrolledWindow", artistScrolledWindow);

    builder->get_widget("artistsTab", artistsLabel);
    builder->get_widget("artistsView", artistsView);


    treeModel = Gtk::ListStore::create(columns);


    artistsView->set_model(treeModel);

    // set columns semantic
    artistsView->set_text_column(0);
    artistsView->set_pixbuf_column(1);


    //set item width
    artistsView->set_item_width(70);


    // set spacing
    artistsView->set_row_spacing(0);
    artistsView->set_column_spacing(0);

    artistScrolledWindow->signal_check_resize().connect(sigc::mem_fun(*this, &ArtistsWidget::on_resize3));
    artistScrolledWindow->signal_screen_changed().connect(sigc::mem_fun(*this, &ArtistsWidget::on_resize2));
    artistScrolledWindow->signal_event().connect(sigc::mem_fun(*this, &ArtistsWidget::on_resize));
    artistsView->signal_item_activated().connect(sigc::mem_fun(*this, &ArtistsWidget::on_itemClick));

    Glib::signal_timeout().connect(
            sigc::mem_fun(*this, &ArtistsWidget::checkMPDConnection), 1000);
  }


  ArtistsWidget::~ArtistsWidget() {
  }


  bool ArtistsWidget::checkMPDConnection() {
    if (clientMPD.Connected()) {
      loadArtistsFromMPD();
      loadArtists();
      return false;
    } else {
      return true;
    }
  }


  void ArtistsWidget::loadArtistsFromMPD() {
    // load artists from MPD server
    clientMPD.GetList(artists, MPD_TAG_ARTIST);
    for (int i = 0; i < artists.size(); i++) {
      artistList.push_back(artists[i]);
    }

    // sort
    artistList.sort();
    artistList.unique();
  }


  void ArtistsWidget::loadArtists() {

    // set artist Label
    artistsLabel->set_text("Artists");


    artistsView->unset_model();
    treeModel->clear();
    artists.clear();
    //    artistList.clear();



    std::list<std::string>::iterator it;



    for (it = artistList.begin(); it != artistList.end(); it++) {
      if (it->length() == 0) continue;
      Gtk::TreeModel::Row row = *(treeModel->append());
      row[columns.artist] = *it;
      row[columns.image] = noimage.get_pixbuf();
      row[columns.isArtist] = 1;

    }


    artistsView->set_model(treeModel);
  }


  bool ArtistsWidget::actualSongSortCond(const MPD::Song* s1, const MPD::Song* s2) {
    unsigned int i = 0;
    while ((i < s1->GetTitle().length()) && (i < s2->GetTitle().length())) {
      if (tolower(s1->GetTitle()[i]) < tolower(s2->GetTitle()[i])) return true;
      else if (tolower(s1->GetTitle()[i]) > tolower(s2->GetTitle()[i])) return false;
      ++i;
    }
    if (s1->GetTitle().length() < s2->GetTitle().length()) return true;
    return false;
  }


  void ArtistsWidget::loadArtistsSongs(std::string artist) {

    selectedArtist = artist;

    // set label of tab
    artistsLabel->set_text(selectedArtist);

    Gtk::TreeModel::Path path;
    Gtk::CellRenderer* cell;
    artistsView->get_cursor(path, cell);
    std::cout << "sel path:" << path.to_string();
    //    cell->;

    // clear treemodel
    m_actualSongs.clear();


    // start searching for artist
    clientMPD.StartSearch(1);
    clientMPD.AddSearch(MPD_TAG_ARTIST, artist);
    clientMPD.CommitSearch(m_actualSongs);

    // sort songs
    std::sort(m_actualSongs.begin(), m_actualSongs.end(), actualSongSortCond);

    artistsView->unset_model();
    treeModel->clear();

    // insert return button
    insertReturn();

    for (int i = 0; i < m_actualSongs.size(); i++) {
      Gtk::TreeModel::Row row = *(treeModel->append());
      row[columns.artist] = m_actualSongs[i]->GetTitle();
      Gtk::Image image;
      image.set("ui/icon_smaller.png");
      row[columns.image] = image.get_pixbuf();
      row[columns.isArtist] = 0;
      row[columns.song] = m_actualSongs[i];
    }


    artistsView->set_model(treeModel);
    //scroll to top
    artistScrolledWindow->get_vadjustment()->set_value(0);

  }


  void ArtistsWidget::hide() {
    artistsView->hide();
  }


  void ArtistsWidget::show() {
    artistsView->show();
  }


  bool ArtistsWidget::on_resize(GdkEvent* e) {
//    std::cout << e->type << std::endl;
//    //    if (e->type == GDK_VISIBILITY_NOTIFY) {
//    int width = artistScrolledWindow->get_width();
//    std::cout << "resize: " << width;
//    artistsView->unset_model();
//    artistsView->set_model(treeModel);
//
//    //      loadArtists();
//    //      artistsView->set_();
//    //    }
    return false;
  }


  void ArtistsWidget::on_resize2(Glib::RefPtr<Gdk::Screen> g) {

    std::cout << "resize2";
    artistsView->unset_model();
    artistsView->set_model(treeModel);
  }


  void ArtistsWidget::on_resize3() {

    std::cout << "resize3";
    loadArtists();
  }


  void ArtistsWidget::insertReturn() {
    Gtk::TreeModel::Row row = *(treeModel->append());
    //    row[columns.artist] = "Home";
    row[columns.isArtist] = -1;
    Gtk::Image i;
    //i.set_from_icon_name("gtk-media-pause", Gtk::IconSize::from_name("GTK_ICON_SIZE_BUTTON"));
    i.set("ui/arrowleft.png");
    row[columns.image] = i.get_pixbuf();
  }


  void ArtistsWidget::on_itemClick(Gtk::TreeModel::Path p) {
    Gtk::TreeModel::iterator i = treeModel->get_iter(p);
    Glib::ustring s;
    int isArtist;

    MPD::Song* song;
    i->get_value(0, s);
    i->get_value(2, isArtist);
    i->get_value(3, song);



    // if is artist, load artist's songs
    if (isArtist == 1) {
      loadArtistsSongs(s.c_str());
    } else if (isArtist == 0) {
      // add to queue and play
      clientMPD.AddSong(*song, clientMPD.GetPlaylistLength());
      clientMPD.Play(clientMPD.GetPlaylistLength());
    } else {
      loadArtists();
    }

  }
}
