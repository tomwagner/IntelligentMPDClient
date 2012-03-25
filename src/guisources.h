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
 * @file guisources.h
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief GTK Sources Window for GUI setting sources to client.
 */

#ifndef SOURCESWINDOW_H
#define	SOURCESWINDOW_H
#include "global.h"
#include <locale>
#include <glibmm-2.4/glibmm/i18n.h>
#include <gtkmm-3.0/gtkmm.h>
#include <jsoncpp/json/json.h>
#include "clientsettings.h"

namespace GUI {

  class Sources : public Gtk::Window {
  public:
    Sources(ClientSettings * settings);
    virtual ~Sources();

  protected:
    ClientSettings * clientSettings;
    //Signal handlers:
    void load();
    void save();
    void onClose();

    void addRow();
    void removeRow();

    // events
    bool onKeyPressEvent(GdkEventKey *event);

    void treeviewcolumn_validated_on_cell_data(Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter);
    void cellrenderer_validated_on_editing_started(Gtk::CellEditable* cell_editable, const Glib::ustring& path);
    void cellrenderer_validated_on_edited(const Glib::ustring& path_string, const Glib::ustring& new_text);

    //Tree model columns:

    class ModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:

      ModelColumns() {
        add(active);
        add(url);
      }

      Gtk::TreeModelColumn<bool> active;
      Gtk::TreeModelColumn<Glib::ustring> url;
    };

    ModelColumns columns;

    //Child widgets:
    Gtk::VBox vbox;

    Gtk::ScrolledWindow scrolledWindow;
    Gtk::TreeView treeView;
    Glib::RefPtr<Gtk::ListStore> treeModel;
    Glib::RefPtr<Gtk::TreeModelSort> modelSort;

    Gtk::HButtonBox buttonBox;
    Gtk::Button addBtn;
    Gtk::Button removeBtn;
    Gtk::Button closeBtn;

    //For the validated column:
    //You could also use a CellRendererSpin or a CellRendererProgress:
    bool m_validate_retry;
    Gtk::CellRendererText m_cellrenderer_validated;
    Gtk::TreeView::Column m_treeviewcolumn_validated;
    Glib::ustring m_invalid_text_for_retry;

  };
}
#endif	/* SOURCESWINDOW_H */

