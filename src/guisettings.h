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
 * @file guisettings.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief GTK Settings Window for GUI setting client parameters.
 */

#ifndef GUISETTINGS_H
#define	GUISETTINGS_H

#include <gtkmm-3.0/gtkmm.h>
#include <glibmm-2.4/glibmm/i18n.h>
#include "mpdclient.h"
#include "clientsettings.h"
namespace GUI {

  class Settings : public Gtk::Window {
  public:
    Settings();
    virtual ~Settings();
  private:
    bool mpdSettingChanged;
  protected:

    // signal handlers
    void onAuthToggle();
    void onConfDirSelect();
    void onTempDirSelect();

    void onMPDSettingsChanged();

    void onSave();
    void onClose();

    bool onKeyPressEvent(GdkEventKey * event);

    Gtk::VBox winVbox;

    // frame MPD
    Gtk::VBox vbox;
    Gtk::Frame frameMPD;
    Gtk::Label frameMPDLabel;
    Gtk::Table tableMPD;

    Gtk::Label hostLabel;
    Gtk::Entry host;
    Gtk::Label portLabel;
    Gtk::SpinButton port;
    Gtk::Label passLabel;
    Gtk::Entry pass;
    Gtk::CheckButton auth;

    // frame prog
    Gtk::VBox vbox1;
    Gtk::Frame frameProg;
    Gtk::Label frameProgLabel;
    Gtk::Table tableProg;

    // temp
    Gtk::Label tempLabel;
    Gtk::FileChooserButton tempDir;


    Gtk::ButtonBox bbox;
    Gtk::Button save;
    Gtk::Button close;


  };
}
#endif	/* GUISETTINGS_H */

