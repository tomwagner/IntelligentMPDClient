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
 * @file guisettings.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief GTK Settings Window for GUI setting client parameters.
 */

#include "guisettings.h"

namespace GUI {


  Settings::Settings() :
  mpdSettingChanged(false) {
    // nastavíme vlastnosti okna
    set_title(_("Settings"));
    set_size_request(400, 200);
    set_resizable(false);
    set_modal(true);
    set_keep_above(true);
    set_position(Gtk::WIN_POS_CENTER);

    winVbox.add(frameMPD);

    // nastavíme rámec
    frameMPD.set_shadow_type(Gtk::SHADOW_IN);
    frameMPD.set_border_width(10);
    frameMPDLabel.set_use_markup(true);
    frameMPDLabel.set_markup("<b>MPD</b>");
    frameMPD.set_label_widget(frameMPDLabel);

    // do rámce vbox
    frameMPD.add(vbox);

    // do kontejneru přidáme tabulku
    vbox.pack_start(tableMPD, Gtk::PACK_START, 0);

    //
    tableMPD.set_border_width(6);
    tableMPD.set_row_spacings(6);
    tableMPD.set_spacings(6);

    // host
    hostLabel.set_label(_("Host:"));
    hostLabel.set_alignment(1.0, 0.5);
    tableMPD.attach(hostLabel, 0, 1, 0, 1);
    host.set_text(Config::GetInstance()->getHost());
    tableMPD.attach(host, 1, 2, 0, 1);

    //port
    portLabel.set_label(_("Port:"));
    portLabel.set_alignment(1.0, 0.5);
    tableMPD.attach(portLabel, 0, 1, 1, 2);
    // nastavíme rozsahy
    port.set_adjustment(Gtk::Adjustment::create(Config::GetInstance()->getPort(), 1.0, 65535.0, 1.0, 5.0, 0.0));
    port.set_progress_pulse_step(1.0);
    tableMPD.attach(port, 1, 2, 1, 2);

    // authentification
    auth.set_label(_("Use Authentication"));
    auth.signal_toggled().connect(sigc::mem_fun(*this, &Settings::onAuthToggle));
    tableMPD.attach(auth, 1, 2, 2, 3);

    passLabel.set_label(_("Password:"));
    passLabel.set_alignment(1.0, 0.5);
    tableMPD.attach(passLabel, 0, 1, 3, 4);
    pass.set_visibility(false);
    pass.set_invisible_char('*');
    pass.set_sensitive(false);
    pass.set_text(Config::GetInstance()->getPassword());
    tableMPD.attach(pass, 1, 2, 3, 4);

    // pokud heslo není prázdné, odcheckujeme
    if (!Config::GetInstance()->getPassword().empty()) {
      auth.set_active(true);
    }

    // zaregistruji události na změnu nastavení mpd
    host.signal_changed().connect(sigc::mem_fun(*this, &Settings::onMPDSettingsChanged));
    pass.signal_changed().connect(sigc::mem_fun(*this, &Settings::onMPDSettingsChanged));
    port.signal_changed().connect(sigc::mem_fun(*this, &Settings::onMPDSettingsChanged));
    auth.signal_style_updated().connect(sigc::mem_fun(*this, &Settings::onMPDSettingsChanged));



    // button box
    bbox.set_border_width(10);
    bbox.set_spacing(10);
    bbox.set_layout(Gtk::BUTTONBOX_END);

    // přidáme rámec do okna
    winVbox.add(frameProg);
    frameProg.set_shadow_type(Gtk::SHADOW_IN);
    frameProg.set_border_width(10);
    frameProgLabel.set_use_markup(true);
    frameProgLabel.set_markup(_("<b>Client Settings</b>"));
    frameProg.set_label_widget(frameProgLabel);

    // tabulka
    frameProg.add(tableProg);
    tableProg.set_border_width(6);
    tableProg.set_row_spacings(6);
    tableProg.set_spacings(6);

    //temp
    tempLabel.set_label("Temp dir:");
    tempLabel.set_alignment(1.0, 0.5);
    tableProg.attach(tempLabel, 0, 1, 1, 2);
    tableProg.attach(tempDir, 1, 2, 1, 2);
    //nastavení dialogu pro výběr
    tempDir.set_title("Select Temp Directory");
    //vybíráme celou složku
    tempDir.set_action(Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    tempDir.signal_current_folder_changed().connect(sigc::mem_fun(*this, &Settings::onTempDirSelect));

    // pokusíme se nastavit temp path
    if (!tempDir.set_current_folder(Config::GetInstance()->getTempPath())) {
      Gtk::MessageDialog dialog(*this,
              "Error in Temp path doesn't exist.",
              false, Gtk::MESSAGE_ERROR);
      dialog.run();

      // zobrazíme nastavení
      show();
    }

    // nastavení tlačítek okna
    // save
    save.set_label("gtk-save");
    save.set_use_stock(true);
    save.signal_clicked().connect(sigc::mem_fun(*this, &Settings::onSave));
    bbox.add(save);

    // close
    close.set_label("gtk-close");
    close.set_use_stock(true);
    close.signal_clicked().connect(sigc::mem_fun(*this, &Settings::onClose));
    bbox.add(close);
    winVbox.pack_end(bbox, Gtk::PACK_END, 0);


    // přidáme widget do okna
    add(winVbox);

    // zobrazíme widgety
    winVbox.show_all();


    // registrace událostí
    add_events(Gdk::KEY_PRESS_MASK);
    signal_key_press_event().
            connect(sigc::mem_fun(*this, &Settings::onKeyPressEvent));
  }


  Settings::~Settings() {
  }


  void Settings::onMPDSettingsChanged() {
    mpdSettingChanged = true;
  }


  bool Settings::onKeyPressEvent(GdkEventKey *event) {
    if (event->keyval == GDK_KEY_Escape) {
      //close the window, when the 'esc' key is pressed
      hide();
    }

    //returning true, cancels the propagation of the event
    return true;
  }


  /**
   * Metoda povolí zadávání hesla po zaškrtnutí checkboxu v GUI
   */
  void Settings::onAuthToggle() {
    if (auth.get_active() == true) {
      pass.set_sensitive(true);
    } else {
      pass.set_sensitive(false);
    }
  }


  void Settings::onTempDirSelect() {
    Config::GetInstance()->setTempPath(tempDir.get_current_folder() + "/");
  }


  void Settings::onSave() {
    // pokud se změnilo nastavení konfigurace připojení k MPD
    if (mpdSettingChanged) {
      // odpojíme
      MPD::Client::GetInstance()->Disconnect();

      MPD::Client::GetInstance()->SetHostname(host.get_text());
      MPD::Client::GetInstance()->SetPassword(pass.get_text());
      MPD::Client::GetInstance()->SetPort(port.get_value());

      Config::GetInstance()->setHost(host.get_text());
      Config::GetInstance()->setPort((int) port.get_value());

      // if autentification needed
      Config::GetInstance()->setPassword(pass.get_text());
      if (auth.get_active() == false)
        Config::GetInstance()->setPassword("");
    }

    Config::GetInstance()->setTempPath(tempDir.get_current_folder() + "/");

    hide();

    Config::GetInstance()->saveSettings();
  }


  void Settings::onClose() {
    hide();
  }

}
