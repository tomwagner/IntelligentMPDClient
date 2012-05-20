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
 * @file guimainwindow.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief GTK Main Window
 */
#include "guimainwindow.h"
//#include <cairomm/cairomm.h>


//GUI::MainWindow * gui = new GUI::MainWindow();

namespace GUI {

  MainWindow* MainWindow::instance = NULL;


  MainWindow::MainWindow() {
    std::cout << "GUI constructor" << std::endl;

    // set default icon settings
    Glib::RefPtr<Gtk::Settings> settings = Gtk::Settings::get_default();
    settings->property_gtk_button_images() = true;


    // load Glade UI
    Glib::RefPtr<Gtk::Builder> builder;
    try {
      builder = Gtk::Builder::create_from_file("ui/mainWindow.glade");
    } catch (const Glib::FileError & ex) {
      std::cerr << ex.what() << std::endl;
    }


    // select widgets
    builder->get_widget("mainWindow", mainWindow);
    // menu widgets
    builder->get_widget("connect", connect);
    builder->get_widget("restart", restart);
    restart->hide();
    builder->get_widget("disconnect", disconnect);
    builder->get_widget("info", info);
    info->hide();
    builder->get_widget("update", update);
    builder->get_widget("quit", quit);
    builder->get_widget("showFeedbackButtons", showFeedbackButtons);
    builder->get_widget("fullscreen", fullscreen);
    builder->get_widget("sourceSettings", sourceSettings);
    builder->get_widget("progSettings", progSettings);
    builder->get_widget("remoteSync", remoteSync);
    if (Config::GetInstance()->isRemoteStorageEnabled()) {
      remoteSync->set_active(true);
    }
    builder->get_widget("enableAgents", enableAgents);
    Glib::RefPtr<Gtk::Action> a = Gtk::ToggleAction::create();

    a->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::setAgentSwitch));
    enableAgents->set_related_action(a);
    
    if (Config::GetInstance()->isAgentsEnabled()) {
      enableAgents->set_active(true);
    }
    builder->get_widget("about", about);


    builder->get_widget("songLabel", songLabel);

    builder->get_widget("albumCover", albumCover);
    builder->get_widget("songArtist", songArtist);
    builder->get_widget("songName", songName);
    builder->get_widget("songGenre", songGenre);
    builder->get_widget("songAlbum", songAlbum);
    builder->get_widget("songBitrate", songBitrate);
    builder->get_widget("songWebpage", songWebpage);
    builder->get_widget("statusBar", statusBar);

    // artist widget
    builder->get_widget("artistTitle", artistTitle);
    builder->get_widget("artistAbout", artistAbout);
    builder->get_widget("artistSourceIcon", artistSourceIcon);
    builder->get_widget("artistSourceName", artistSourceName);
    builder->get_widget("artistSourceUrl", artistSourceUrl);

    // feedback buttons
    builder->get_widget("coverRight", coverRight);
    builder->get_widget("coverWrong", coverWrong);
    builder->get_widget("artistRight", artistRight);
    builder->get_widget("artistWrong", artistWrong);
    builder->get_widget("articleRight", articleRight);
    builder->get_widget("articleWrong", articleWrong);
    builder->get_widget("slideRight", slideRight);
    builder->get_widget("slideWrong", slideWrong);


    // about dialog
    builder->get_widget("aboutDialog", aboutDialog);

    aboutDialog->set_program_name("Intelligent MPD Client");
    aboutDialog->set_version("0.2");
    aboutDialog->set_website("http://www.impc.cz");
    std::vector<Glib::ustring> authors;
    authors.push_back("Tomas Wagner - xwagne01@stud.fit.vutbr.cz");
    aboutDialog->set_authors(authors);
    Glib::RefPtr<Gdk::Pixbuf> iconAbout = Gdk::Pixbuf::create_from_file("ui/icon.png", 128, 128, true);
    aboutDialog->set_logo(iconAbout);


    about->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::showAboutDialog));
    //    aboutDialog->signal_button_press_event()->connect(sigc::mem_fun(*this, &MainWindow::quit));
    // vytvoříme status iconu
    if (Glib::file_test("ui/icon.png", Glib::FILE_TEST_EXISTS)) {
      Glib::RefPtr<Gtk::StatusIcon> trayIcon = Gtk::StatusIcon::create_from_file("ui/icon.png");
      trayIcon->set_name("Intelligent MPD client");
      trayIcon->set_visible(true);
    }

    showFeedbackButtons->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::feedbackSwitch));
    fullscreen->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::fullscreenSwitch));


    // set name for songLabel an try set CSS style
    songLabel->set_name("songLabel");


    //Cairo

    //mainWindow->signal_event().connect(sigc::mem_fun(*this, &MainWindow::onExposeEvent));


    // CSS
    Glib::RefPtr<Gtk::CssProvider> provider = Gtk::CssProvider::get_default();
    Glib::RefPtr<Gdk::Display> display = Gdk::Display::get_default();

    Gtk::StyleContext::add_provider_for_screen(display->get_default_screen(), provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    try {
      provider->load_from_file(Gio::File::create_for_path("theme/default.css"));
    } catch (Glib::Error e) {
      std::cout << e.what() << std::endl;
    }



    //Inicializace oken
    sourcesWindow = new Sources();

    sourceSettings->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::showSourcesWindow));

    settingsWindow = new Settings();

    progSettings->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::showSettingsWindow));

    remoteSync->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::setRemoteSync));


    connect->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_connect));
    disconnect->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_disconnect));
    info->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_info_mpd));
    update->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_update_mpd_db));

    // Widgets initialization
    ///////////////////////


    // Control widget
    builder->get_widget("timeScale", timeScale);
    builder->get_widget("timeEventBox", timeEventBox);
    builder->get_widget("volumeScale", volumeScale);
    builder->get_widget("first", first);
    builder->get_widget("back", back);
    builder->get_widget("play", play);
    builder->get_widget("playImage", playImage);
    builder->get_widget("pauseImage", pauseImage);
    builder->get_widget("stop", stop);
    builder->get_widget("next", next);
    builder->get_widget("last", last);

    first->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_first));
    back->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_back));
    play->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_playOrPause));
    stop->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_stop));
    next->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_next));
    last->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_last));

    // connect signals to scale widgets
    timeEventBox->add_events(Gdk::BUTTON_PRESS_MASK);
    timeEventBox->signal_button_press_event().connect(sigc::mem_fun(*this, &MainWindow::setTime));
    volumeScale->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::setVolume));


    MPD::Client::GetInstance()->UpdateStatus();

    // article widget
    articlesWidget = new ArticlesWidget(builder);

    // slideshow widget
    slideshowWidget = new SlideshowWidget(builder);
    slideshowWidget->showLoading();


    // artists widget
    artistsWidget = new ArtistsWidget(builder);


    // cover widget
    coverWidget = new CoverWidget(builder);

    // update player
    updateGUI();
    // plan cyclic update fo 500 mili seconds
    updateTimeout = Glib::signal_timeout().connect(
            sigc::mem_fun(*this, &MainWindow::updateGUI), 1000);
  }


  MainWindow::~MainWindow() {
    // delete widgets
    delete articlesWidget;
    delete slideshowWidget;
    delete artistsWidget;
  }


  void MainWindow::showDialogs() {
    if (dialogs.empty()) return;

    while (!dialogs.empty()) {
      Dialog d = dialogs.top();
      if (d.type.compare("error") == 0) {
        showErrorDialog(d.message);
      } else if (d.type.compare("info") == 0) {
        showInfoDialog(d.message);
      }
      dialogs.pop();
    }
  }


  bool MainWindow::onExposeEvent(GdkEvent *event) {
    std::cout << event->type << std::endl;
    if (event->type == GDK_WINDOW_STATE) {
      std::cout << "expose evend" << std::endl;
      Cairo::RefPtr<Cairo::Context> windowContext = mainWindow->get_window()->create_cairo_context();

      windowContext->set_operator(Cairo::OPERATOR_XOR);

      windowContext->set_source_rgba(100, 100, 100, 0.1);
      windowContext->paint();
    }
    //windowContext->paint_with_alpha(0.1);

    // this is just for testing if I can paint ANYTHING onto the transparent window: 
    //    windowContext->set_operator(Cairo::OPERATOR_ADD);
    //    windowContext->set_line_width(10.0);
    //    windowContext->move_to(0, 0);
    //    windowContext->line_to(100, 100);
    //    windowContext->stroke();
    //    mainWindow->show_all_children(true);
    //    GDK_EVENT_STOP;
    return false;
  }


  void MainWindow::showSourcesWindow() {
    sourcesWindow->show();
  }


  void MainWindow::showSettingsWindow() {
    settingsWindow->show();
  }


  void MainWindow::setRemoteSync() {
    if (remoteSync->get_active()) {
      Config::GetInstance()->setRemoteStorageEnabled(true);
    } else {
      Config::GetInstance()->setRemoteStorageEnabled(false);
    }
  }


  void MainWindow::setAgentSwitch() {
    if (enableAgents->get_active()) {
      Config::GetInstance()->setAgentsEnabled(true);
    } else {
      Config::GetInstance()->setAgentsEnabled(false);
    }
  }


  void MainWindow::showErrorDialog(std::string text) {
    // if window is not realized, we accumulate dialogs
    if (!mainWindow->get_realized()) {
      Dialog d;
      d.type = "error";
      d.message = text;

      dialogs.push(d);
      return;
    }

    Gtk::MessageDialog dialog(*this->getWindow(), text.c_str(), false, Gtk::MESSAGE_ERROR);
    dialog.run();
  }


  void MainWindow::showInfoDialog(std::string text) {
    // if window is not realized, we accumulate dialogs
    if (!mainWindow->get_realized()) {
      Dialog d;
      d.type = "info";
      d.message = text;

      dialogs.push(d);
      return;
    }

    Gtk::MessageDialog dialog(*this->getWindow(), text.c_str(), false, Gtk::MESSAGE_INFO);
    dialog.run();
  }


  void MainWindow::showAboutDialog() {
    aboutDialog->show_all();
  }


  void MainWindow::fullscreenSwitch() {
    // pokud je zaškrtnuto, zmenšíme
    if (fullscreen->get_active()) {
      mainWindow->fullscreen();
    } else { //zvětšíme
      mainWindow->unfullscreen();
    }
  }


  /**
   * Method to update GUI. We call it every one second.
   */
  bool MainWindow::updateGUI() {

    // we show accumulated dialogs
    showDialogs();

    // update client status
    MPD::Client::GetInstance()->UpdateStatus();

    if (MPD::Client::GetInstance()->Connected()) {
      MPD::PlayerState s = MPD::Client::GetInstance()->GetState();

      if (s == MPD::psPlay) {
        play->set_active(true);
        play->set_image(*pauseImage);

        // update time scale widget
        setTimeScale(MPD::Client::GetInstance()->GetElapsedTime(), MPD::Client::GetInstance()->GetTotalTime());

        // we set default volume
        volumeScale->set_value(MPD::Client::GetInstance()->GetVolume());

        MPD::Song song = MPD::Client::GetInstance()->GetCurrentSong();
        if (!song.Empty()) {
          // update text widgets
          setAlbum(song.GetAlbum());
          setArtist(song.GetArtist());
          setSongLabel(song.GetArtist() + " - " + song.GetTitle());
          setGenre(song.GetGenre());
          setTitle(song.GetTitle());
        }

        std::stringstream bitrate;
        bitrate << MPD::Client::GetInstance()->GetBitrate();
        bitrate << " kbit/s";

        songBitrate->set_text(bitrate.str());

        // load new info to widgets
        //        std::cout << "updateArticlesWidget()" << std::endl;
        articlesWidget->updateArticlesWidget();
        slideshowWidget->updateSlideshowWidget();
        //        coverWidget->updateCoverWidget();

        //        Glib::RefPtr<Gdk::Pixbuf> p = Gdk::Pixbuf::create_from_file("ui/bg_test2.jpg");
        //        p = p->add_alpha(1, 0, 255, 0);
        //p = p->scale_simple(100,100,Gdk::INTERP_BILINEAR);

      } else if (s == MPD::psPause) {
        play->set_active(false);
        play->set_image(*playImage);
      } else if (s == MPD::psStop) {
        on_stop();
      }
      //      return true;
    }
  }


  Gtk::Window * MainWindow::getWindow() const {
    return mainWindow;
  }


  void MainWindow::setAlbumCover(std::string filename) {
    if (Glib::file_test(filename.c_str(), Glib::FILE_TEST_EXISTS))
      albumCover->set(filename.c_str());
  }


  void MainWindow::setAlbumCover(const Glib::RefPtr<Gdk::Pixbuf> p) {
    albumCover->set(p);
  }


  void MainWindow::setSongLabel(std::string name) {
    if (!songLabel) return;
    songLabel->set_label(name.c_str());
  }


  void MainWindow::setArtist(std::string name) {
    if (!songArtist) return;
    songArtist->set_label(name.c_str());
  }


  void MainWindow::setTitle(std::string name) {
    if (!songName) return;
    songName->set_label(name.c_str());
  }


  void MainWindow::setAlbum(std::string name) {
    if (!songAlbum) return;
    songAlbum->set_label(name.c_str());
  }


  void MainWindow::setGenre(std::string length) {
    if (!songGenre) return;
    songGenre->set_label(length.c_str());
  }


  void MainWindow::setBitrate(std::string genre) {
    if (!songBitrate) return;
    songBitrate->set_label(genre.c_str());
  }


  void MainWindow::setWebpage(std::string url) {
    if (!songWebpage) return;
    // create markup url
    std::string mUrl = "<a href=\"" + url + "\">" + url + "</a>";
    songWebpage->set_markup(mUrl);
  }


  void MainWindow::setArtistAbout(std::string text) {
    if (!artistAbout) return;
    artistAbout->set_markup(text.c_str());
  }


  void MainWindow::setArtistTitle(std::string title) {
    if (!artistTitle) return;
    artistTitle->set_label(title.c_str());
  }


  void MainWindow::setArtistSource(std::string iconpath, std::string name, std::string url) {
    if ((!artistSourceIcon) || (!artistSourceName) || (!artistSourceUrl)) return;
    artistSourceIcon->set(iconpath.c_str());
    artistSourceName->set_label(name.c_str());

    // create markup url
    std::string mUrl = "<a href=\"" + url + "\">" + url + "</a>";
    artistSourceUrl->set_markup(mUrl);
  }


  void MainWindow::setStatusBar(std::string text) {
    if (!statusBar) return;
    statusBar->push(text.c_str());
  }


  void MainWindow::on_back() {
    MPD::Client::GetInstance()->Prev();
  }


  void MainWindow::on_playOrPause() {

    if (play->get_active()) {
      MPD::Client::GetInstance()->Play();
      //      play->set_active(true);
      play->set_image(*pauseImage);

    } else {
      MPD::Client::GetInstance()->Pause(true);
      //      play->set_active(false);
      play->set_image(*playImage);
      setStatusBar(_("IMPC Paused"));
    }
  }


  void MainWindow::on_stop() {
    MPD::Client::GetInstance()->Stop();
    if (play->get_active()) {

      MPD::Client::GetInstance()->Pause(true);

      // set play icon
      play->set_active(false);
      play->set_image(*playImage);
    }
    setAlbum("");
    setArtist("");
    setBitrate("");
    setGenre("");
    setTimeScale(0, 0);
    setTitle("");

    setStatusBar(_("IMPC Stopped"));
  }


  void MainWindow::on_next() {
    MPD::Client::GetInstance()->Next();
  }


  void MainWindow::on_connect() {

    MPD::Client::GetInstance()->Connect();

    //set sensitive on controls
    first->set_sensitive(true);
    back->set_sensitive(true);
    play->set_sensitive(true);
    stop->set_sensitive(true);
    next->set_sensitive(true);
    last->set_sensitive(true);
    volumeScale->set_sensitive(true);
    timeScale->set_sensitive(true);


    if (MPD::Client::GetInstance()->Connected()) {
      setStatusBar(_("IMPC Connected"));
    }

  }


  void MainWindow::on_disconnect() {
    //set sensitive on controls
    first->set_sensitive(false);
    back->set_sensitive(false);
    play->set_sensitive(false);
    stop->set_sensitive(false);
    next->set_sensitive(false);
    last->set_sensitive(false);
    volumeScale->set_sensitive(false);
    timeScale->set_sensitive(false);

    setStatusBar(_("IMPC Disconnected"));

    MPD::Client::GetInstance()->Disconnect();
  }


  void MainWindow::on_update_mpd_db() {
    MPD::Client::GetInstance()->UpdateDirectory("");
  }


  void MainWindow::on_info_mpd() {
    //    MPD::TagList l;
    //    MPD::Client::GetInstance()->GetList
  }


  void MainWindow::setVolume(double vol) {
    MPD::Client::GetInstance()->SetVolume(vol);
  }


  bool MainWindow::setTime(GdkEventButton * e) {

    if (MPD::Client::GetInstance()->isPlaying()) {
      double fraction = e->x / timeScale->get_width() * MPD::Client::GetInstance()->GetTotalTime();
      MPD::Client::GetInstance()->Seek(fraction);
      setTimeScale(fraction, MPD::Client::GetInstance()->GetTotalTime());
    }
    return true;
  }


  void MainWindow::setTimeScale(double elapsedTime, double totalTime) {
    timeScale->set_fraction(elapsedTime / totalTime);
    timeScale->set_text(MPD::Client::GetInstance()->GetCurrentSong().GetLength(elapsedTime) + " / " + MPD::Client::GetInstance()->GetCurrentSong().GetLength(totalTime));
  }


  void MainWindow::feedbackSwitch() {
    if (!showFeedbackButtons->get_active()) {
      coverRight->hide();
      coverWrong->hide();
      artistRight->hide();
      artistWrong->hide();
      articleRight->hide();
      articleWrong->hide();
      slideRight->hide();
      slideWrong->hide();
    } else {
      coverRight->show();
      coverWrong->show();
      artistRight->show();
      artistWrong->show();
      articleRight->show();
      articleWrong->show();
      slideRight->show();
      slideWrong->show();
    }
  }


  void MainWindow::on_first() {
    MPD::Client::GetInstance()->PlayID(0);
  }


  void MainWindow::on_last() {
    if (MPD::Client::GetInstance()->GetPlaylistLength() == 0) return;

    MPD::Client::GetInstance()->PlayID(MPD::Client::GetInstance()->GetPlaylistLength());
  }


}
