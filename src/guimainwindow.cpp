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

namespace GUI {


  MainWindow::MainWindow(MPD::Client *client, ClientSettings * settings) :
  clientMPD(client),
  clientSettings(settings) {

    //clientMPD = client;
    // načteme šablonu pro hlavní okno aplikace
    Glib::RefPtr<Gtk::Builder> builder;
    try {
      builder = Gtk::Builder::create_from_file("ui/mainWindow.glade");
    } catch (const Glib::FileError & ex) {
      std::cerr << ex.what() << std::endl;
    }


    // vybereme widgety
    builder->get_widget("mainWindow", mainWindow);
    builder->get_widget("imageWidget", picture);
    builder->get_widget("songLabel", songLabel);

    builder->get_widget("songArtist", songArtist);
    builder->get_widget("songName", songName);
    builder->get_widget("songGenre", songGenre);
    builder->get_widget("songAlbum", songAlbum);
    builder->get_widget("songLength", songLength);
    builder->get_widget("songWebpage", songWebpage);

    builder->get_widget("statusBar", statusBar);
    builder->get_widget("textLabel", textLabel);
    //builder->get_widget("hBox", hBox);


    // menu widgety
    builder->get_widget("quit", quit);
    builder->get_widget("fullscreen", fullscreen);
    builder->get_widget("sourceSettings", sourceSettings);
    builder->get_widget("progSettings", progSettings);
    builder->get_widget("about", about);

    // about dialog
    builder->get_widget("aboutDialog", aboutDialog);
    //builder->get_widget("aboutDialog", aboutDialogClose);
    //    builder->get_widget("settingsWindow", settingsWindow);

    aboutDialog->set_program_name("Intelligent MPD Client");
    aboutDialog->set_version("0.1");
    aboutDialog->set_website("http://www.impc.cz");
    std::vector<Glib::ustring> authors;
    authors.push_back("Bc. Tomáš Wagner - xwagne01@stud.fit.vutbr.cz");
    aboutDialog->set_authors(authors);
    aboutDialog->set_license("GNU General Public License, version 3");
    Glib::RefPtr<Gdk::Pixbuf> iconAbout = Gdk::Pixbuf::create_from_file("ui/icon.png");
    aboutDialog->set_logo(iconAbout);


    about->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::showAboutDialog));
    //aboutDialog->signal_button_press_event()->connect(sigc::mem_fun(*this, &MainWindow::quit));
    // vytvoříme status iconu
    GtkStatusIcon * si = gtk_status_icon_new_from_file("ui/icon.png");
    gtk_status_icon_set_name(si, "Intelligent MPD client");
    gtk_status_icon_set_visible(si, true);



    // nastavení obrázku na pozadí
    //    Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file("ui/bg_test.jpg");
    //    Glib::RefPtr<Gdk::Pixmap> pixmap;
    //    Glib::RefPtr<Gdk::Bitmap> mask;
    //    pixbuf->add_alpha(true, 255, 255, 255);
    //    pixbuf->render_pixmap_and_mask(pixmap, mask, 0);
    //
    //    Glib::RefPtr<Gtk::Style> style = mainWindow->get_style()->copy();
    //
    //    style->set_bg_pixmap(Gtk::STATE_NORMAL, pixmap);
    //    style->set_bg_pixmap(Gtk::STATE_ACTIVE, pixmap);
    //    style->set_bg_pixmap(Gtk::STATE_PRELIGHT, pixmap);
    //    style->set_bg_pixmap(Gtk::STATE_SELECTED, pixmap);
    //    style->set_bg_pixmap(Gtk::STATE_INSENSITIVE, pixmap);
    //    mainWindow->set_style(style);




    //mainWindow->signal_event().connect(sigc::mem_fun(*this, &MainWindow::onExposeEvent));
    fullscreen->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::fullscreenSwitch));

    quit->signal_activate().connect(&gtk_main_quit);

    //mainWindow->set_decorated(false);


    //    Gdk::Color color;
    //    color.set("FFAAFF");
    //    mainWindow->get_style()->set_base(Gtk::STATE_NORMAL, color);
    //    mainWindow->get_style()->
    mainWindow->show_all();
    //    image->render_to_drawable(mainWindow->get_window(), mainWindow->get_style()->get_black_gc(),
    //            0, 0, 0, 0, image->get_width(), image->get_height(), // draw the whole image (from 0,0 to the full width,height) at 100,80 in the window
    //            Gdk::RGB_DITHER_NONE, 0, 0);


    //    gtk_widget_set_style(GTK_WIDGET(mainWindow), GTK_STYLE(style));
    //gtk_window_set_position(GTK_WINDOW(mainWindow), GTK_WIN_POS_CENTER_ALWAYS);
    //gtk_container_set_border_width(GTK_CONTAINER(mainWindow), 14);
    //    if (Transient == TRUE)
    //      gtk_window_set_transient_for(GTK_WINDOW(mainWindow), GTK_WINDOW(mainWindow));


    songLabel->set_name("songLabel");

    // CSS
    Glib::RefPtr<Gtk::CssProvider> provider = Gtk::CssProvider::create();
    Glib::RefPtr<Gdk::Display> display = Gdk::Display::get_default();
    //Glib::RefPtr<Gdk::Display> screen = Gdk::Display::open(display->get_name());

    Gtk::StyleContext::add_provider_for_screen(display->get_default_screen(), provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    try {
      provider->load_from_file(Gio::File::create_for_path("theme/default.css"));
      //provider->load_from__data("@import url (\"theme/default.css\");\n");
    } catch (Glib::Error e) {
      std::cout << e.what() << std::endl;
    }





    // zaregistrujeme posluchače pro GUI
    clientMPD->addListener(*this);

    //Inicializace oken
    sourcesWindow = new Sources(clientSettings);

    sourceSettings->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::showSourcesWindow));

    settingsWindow = new Settings(clientMPD, clientSettings);

    progSettings->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::showSettingsWindow));

    //    test = new TextWidget();
    //
    //    test->setText("Ahoj, tohle je nový widget");
    //    hBox->add(*test);


    // Inicializace Widgetů


    // Control widget
    builder->get_widget("first", first);
    builder->get_widget("back", back);
    builder->get_widget("play", play);
    builder->get_widget("playIcon", playIcon);
    builder->get_widget("stop", stop);
    builder->get_widget("next", next);
    builder->get_widget("last", last);

    back->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_back));
    play->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_playOrPause));
    stop->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_stop));
    next->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_next));


    // slideshow widget
    builder->get_widget("slide", slide);


//    // aktualizujeme GUI, pokud se právě přehravá
    //mpd_state state = clientMPD->getClientState();
//    if (state == MPD_STATE_PLAY) {
//      playerEvent();
//      play->set_active(true);
//    } else if (state == MPD_STATE_PAUSE) {
//      playerEvent();
//    }



    TextWidget * textWidget = new TextWidget();
    // test html v labelu
    textWidget->setText(std::string("The Beatles were an iconic rock group from Liverpool, England. They are frequently cited as the most commercially successful and critically acclaimed band in modern history, with innovative music, a cultural impact that helped define the 1960s and an enormous influence on music that is still felt today. Currently, The Beatles are one of the two musical acts to sell more than 1 billion records, with only Elvis Presley having been able to achieve the same feat. <a href=\"http://www.seznam.cz\" title=\"odkaz titulek\">Odkaz</a>"));

    //    hBox->add(*textWidget);

    textLabel->set_markup("The Beatles were an iconic rock group from Liverpool, England. They are frequently cited as the most commercially successful and critically acclaimed band in modern history, with innovative music, a cultural impact that helped define the 1960s and an enormous influence on music that is still felt today. Currently, The Beatles are one of the two musical acts to sell more than 1 billion records, with only Elvis Presley having been able to achieve the same feat. <a href=\"http://www.seznam.cz\" title=\"odkaz titulek\">Odkaz</a>");
  }


  MainWindow::~MainWindow() {

  }


  void MainWindow::showSourcesWindow() {
    sourcesWindow->show();
  }


  void MainWindow::showSettingsWindow() {
    settingsWindow->show();
  }


  void MainWindow::showErrorDialog(std::string text) {
    Gtk::MessageDialog dialog(*this->getWindow(), text.c_str(), false, Gtk::MESSAGE_ERROR);
    dialog.run();
  }


  void MainWindow::showInfoDialog(std::string text) {
    Gtk::MessageDialog dialog(*this->getWindow(), text.c_str(), false, Gtk::MESSAGE_INFO);
    dialog.run();
  }


  bool MainWindow::onExposeEvent(GdkEventExpose *event) {

    std::cout << "expose evend" << std::endl;
    Cairo::RefPtr<Cairo::Context> windowContext = mainWindow->get_window()->create_cairo_context();

    windowContext->set_operator(Cairo::OPERATOR_XOR);

    //    windowContext->set_source_rgba(255, 255, 255, 0.6);
    //    
    //    windowContext->paint_with_alpha(1.0);

    // this is just for testing if I can paint ANYTHING onto the transparent window: 
    //    windowContext->set_operator(Cairo::OPERATOR_ADD);
    //    windowContext->set_line_width(10.0);
    //    windowContext->move_to(0, 0);
    //    windowContext->line_to(100, 100);
    //    windowContext->stroke();
    mainWindow->show_all_children(true);

    return true;
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
   * Metoda pro překreslení GUI při aktualizaci skladby
   * v MPD serveru
   */
  void MainWindow::playerEvent() {
    std::cout << "jsem v události" << std::endl;
    MPD::Song song;

    // zjistíme aktuální přehrávanou skladbu
    clientMPD->getCurrentSong(song);

    //aktualizujeme widgety
    setSongLabel(song.getArtistAndSongName());

    setSongArtist(song.artist());
    setSongName(song.title());
    setSongAlbum(song.album());
    setSongGenre(song.genre());
    setSongLength(song.getDurationInString());
    setSongWebpage("<a href=\"http://www.impc.cz\">IMPC</a>");

    // aktualizujeme stavovou lištu
    std::string x("Právě hrajeme: ");
    x.append(song.uri());
    setStatusBarTitle(x.data());

    // TODO nastavení obalu
    setCover("ui/794121.jpg");
    slide->set("ui/bg_test2.jpg");
    //    std::string webHTML;
    //    utils::loadStringFromFile("test/lastfm_beatles_text", webHTML);
    //    HTMLParser page = HTMLParser(webHTML);
    //    std::list<std::string> l = page.getParagraphList();
    //    setTextLabel(l.front());
  }


  Gtk::Window * MainWindow::getWindow() {
    return mainWindow;
  }


  void MainWindow::setCover(std::string filename) {
    picture->set(filename.c_str());
  }


  void MainWindow::setSongLabel(std::string name) {
    if (!songLabel) return;
    songLabel->set_label(name.c_str());
  }


  void MainWindow::setSongArtist(std::string name) {
    if (!songArtist) return;
    songArtist->set_label(name.c_str());
  }


  void MainWindow::setSongName(std::string name) {
    if (!songName) return;
    songName->set_label(name.c_str());
  }


  void MainWindow::setSongAlbum(std::string name) {
    if (!songAlbum) return;
    songAlbum->set_label(name.c_str());
  }


  void MainWindow::setSongGenre(std::string length) {
    if (!songGenre) return;
    songGenre->set_label(length.c_str());
  }


  void MainWindow::setSongLength(std::string genre) {
    if (!songLength) return;
    songLength->set_label(genre.c_str());
  }


  void MainWindow::setSongWebpage(std::string markupWebpage) {
    if (!songWebpage) return;
    songWebpage->set_markup(markupWebpage.c_str());
  }


  void MainWindow::setTextLabel(std::string text) {
    if (!textLabel) return;
    textLabel->set_label(text.c_str());
  }


  void MainWindow::setStatusBarTitle(std::string text) {
    if (!statusBar) return;
    statusBar->push(text.c_str());
  }


  void MainWindow::on_back() {
    clientMPD->previousSong();
  }


  void MainWindow::on_playOrPause() {

    if (play->get_active()) {
      playIcon->set_from_icon_name(GTK_STOCK_MEDIA_PAUSE, Gtk::ICON_SIZE_BUTTON);
      clientMPD->play();
    } else {
      playIcon->set_from_icon_name(GTK_STOCK_MEDIA_PLAY, Gtk::ICON_SIZE_BUTTON);
      clientMPD->pause();
    }
  }


  void MainWindow::on_stop() {
    clientMPD->stop();
  }


  void MainWindow::on_next() {
    clientMPD->nextSong();
  }



}
