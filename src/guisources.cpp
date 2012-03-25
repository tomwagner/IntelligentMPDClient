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
 * @file guisources.cpp
 * @author Tomas Wagner (xwagne01@stud.fit.vutbr.cz)
 * @brief GTK Sources Window for GUI setting sources to client.
 */
#include "guisources.h"

namespace GUI {

  using std::sprintf;
  using std::strtol;


  Sources::Sources(ClientSettings * settings) :
  clientSettings(settings),
  m_validate_retry(false) {
    // nastavíme titulek okna
    set_title(_("Metadata source"));

    set_border_width(5);
    // nastavíme min velikost
    set_default_size(400, 200);
    // vycentrujeme
    set_position(Gtk::WIN_POS_CENTER);

    set_modal(true);

    addBtn.set_label("gtk-add");
    addBtn.set_use_stock(true);

    removeBtn.set_label("gtk-remove");
    removeBtn.set_use_stock(true);

    closeBtn.set_label("gtk-close");
    closeBtn.set_use_stock(true);

    // přidáme vert. kontejner do okna
    add(vbox);

    // vytvoříme scrolled window, očekáváme větší množství zdrojů
    scrolledWindow.add(treeView);

    // posouvátka ukážeme až to bude potřeba
    scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    vbox.pack_start(scrolledWindow);
    vbox.pack_start(buttonBox, Gtk::PACK_SHRINK);

    addBtn.signal_clicked().connect(sigc::mem_fun(*this,
            &Sources::addRow));
    removeBtn.signal_clicked().connect(sigc::mem_fun(*this,
            &Sources::removeRow));

    buttonBox.pack_start(addBtn, Gtk::PACK_SHRINK);
    buttonBox.pack_start(removeBtn, Gtk::PACK_SHRINK);
    buttonBox.pack_start(closeBtn, Gtk::PACK_SHRINK);

    buttonBox.set_border_width(5);
    buttonBox.set_layout(Gtk::BUTTONBOX_END);
    closeBtn.signal_clicked().connect(sigc::mem_fun(*this,
            &Sources::onClose));

    //Create the Tree model:
    treeModel = Gtk::ListStore::create(columns);
    treeView.set_model(treeModel);

    //Fill the TreeView's model
    //    Gtk::TreeModel::Row row = *(treeModel->append());
    //    row[columns.active] = true;
    //    row[columns.url] = "http://www.last.fm";
    //
    //
    //    row = *(treeModel->append());
    //    row[columns.active] = true;
    //    row[columns.url] = "http://www.musicbrainz.com";
    //
    //
    //    row = *(treeModel->append());
    //
    //    row[columns.active] = true;
    //    row[columns.url] = "http://www.seznam.cz";


    //Add the TreeView's view columns:
    //We use the *_editable convenience methods for most of these,
    //because the default functionality is enough:
    treeView.append_column_editable("Active", columns.active);
    treeView.append_column_editable("URL", columns.url);

    // povolíme možnost řazení
    treeView.get_column(0)->set_sort_column(columns.active);
    treeView.get_column(1)->set_sort_column(columns.url);

    // implicitní řazení dle url
    treeView.get_column(1)->set_sort_indicator(true);

    //    modelSort = Gtk::TreeModelSort::create(treeModel);
    //
    //    modelSort->set_sort_column(columns.active, Gtk::SORT_ASCENDING);
    //    treeView.set_model(modelSort);


    //For this column, we create the CellRenderer ourselves, and connect our own
    //signal handlers, so that we can validate the data that the user enters, and
    //control how it is displayed.
    //    m_treeviewcolumn_validated.set_title("validated (<10)");
    //    m_treeviewcolumn_validated.pack_start(m_cellrenderer_validated);
    //    m_TreeView.append_column(m_treeviewcolumn_validated);
    //
    //    //Tell the view column how to render the model values:
    //    m_treeviewcolumn_validated.set_cell_data_func(m_cellrenderer_validated,
    //            sigc::mem_fun(*this,
    //            &Sources::treeviewcolumn_validated_on_cell_data));
    //
    //    //Make the CellRenderer editable, and handle its editing signals:
    //    m_cellrenderer_validated.property_editable() = true;
    //
    //    m_cellrenderer_validated.signal_editing_started().connect(
    //            sigc::mem_fun(*this,
    //            &Sources::cellrenderer_validated_on_editing_started));
    //
    //        m_cellrenderer_validated.signal_edited().connect(sigc::mem_fun(*this,
    //                &Sources::cellrenderer_validated_on_edited));

    //If this was a CellRendererSpin then you would have to set the adjustment:
    //m_cellrenderer_validated.property_adjustment() = m_spin_adjustment;

    // načteme zdroje
    load();

    // registrace událostí
    add_events(Gdk::KEY_PRESS_MASK);
    signal_key_press_event().connect(sigc::mem_fun(*this,
            &Sources::onKeyPressEvent));

    show_all_children();
  }


  Sources::~Sources() {
  }


  bool Sources::onKeyPressEvent(GdkEventKey *event) {
    if (event->keyval == GDK_KEY_Escape) {
      //close the window, when the 'esc' key is pressed
      hide();
    }

    //returning true, cancels the propagation of the event
    return true;
  }


  void Sources::load() {
    // načteme seznam zdrojů
    std::list<source*> sourcesList = clientSettings->getSourcesList();

    // projdeme všechny zdroje a uložíme nastavení do widgetu
    std::list<source*>::iterator it;
    for (it = sourcesList.begin(); it != sourcesList.end(); it++) {
      Gtk::TreeModel::Row row = *(treeModel->append());
      row[columns.active] = (*it)->active;
      row[columns.url] = (*it)->url;
    }
  }


  void Sources::save() {
    // vymažeme seznam zdrojů
    clientSettings->initSourcesList();

    // projdeme všechny položky widgetu a uložíme je
    Gtk::TreeModel::Children children = treeModel->children();
    for (Gtk::TreeModel::Children::iterator iter = children.begin(); iter != children.end(); ++iter) {
      Gtk::TreeModel::Row modelRow = *iter;
      Glib::ustring url = modelRow[columns.url];
      clientSettings->insertSource((bool) modelRow[columns.active], url.c_str());
    }
  }


  void Sources::addRow() {
    Gtk::TreeModel::Row row = *(treeModel->append());

    row[columns.active] = true;
    row[columns.url] = "http://";
    treeView.expand_all();
  }


  void Sources::removeRow() {
    Glib::RefPtr<Gtk::TreeSelection> treeSelection = treeView.get_selection();

    if (treeSelection) {
      Gtk::TreeModel::iterator sorted_iter = treeSelection->get_selected();
      if (sorted_iter) {
        //Gtk::TreeModel::iterator iter = modelSort->convert_iter_to_child_iter(sorted_iter);
        treeModel->erase(sorted_iter);
        //        Gtk::MessageDialog dialog(*this, "Zdroj vymazán!", false, Gtk::MESSAGE_INFO);
        //        dialog.run();
      }
    }
  }


  void Sources::onClose() {
    save();
    hide();
  }


  //  void Sources::treeviewcolumn_validated_on_cell_data(
  //          Gtk::CellRenderer* /* renderer */,
  //          const Gtk::TreeModel::iterator& iter) {
  //    //Get the value from the model and show it appropriately in the view:
  //    if (iter) {
  //      Gtk::TreeModel::Row row = *iter;
  //      int model_value = row[m_Columns.m_col_number_validated];
  //
  //      //This is just an example.
  //      //In this case, it would be easier to use append_column_editable() or
  //      //append_column_numeric_editable()
  //      char buffer[32];
  //      sprintf(buffer, "%d", model_value);
  //
  //      Glib::ustring view_text = buffer;
  //      m_cellrenderer_validated.property_text() = view_text;
  //    }
  //  }
  //
  //
  //  void Sources::cellrenderer_validated_on_editing_started(
  //          Gtk::CellEditable* cell_editable, const Glib::ustring& /* path */) {
  //    //Start editing with previously-entered (but invalid) text, 
  //    //if we are allowing the user to correct some invalid data. 
  //    if (m_validate_retry) {
  //      //This is the CellEditable inside the CellRenderer. 
  //      Gtk::CellEditable* celleditable_validated = cell_editable;
  //
  //      //It's usually an Entry, at least for a CellRendererText:
  //      Gtk::Entry* pEntry = dynamic_cast<Gtk::Entry*> (celleditable_validated);
  //      if (pEntry) {
  //        pEntry->set_text(m_invalid_text_for_retry);
  //        m_validate_retry = false;
  //        m_invalid_text_for_retry.clear();
  //      }
  //    }
  //
  //  }
  //
  //
  //  void Sources::cellrenderer_validated_on_edited(
  //          const Glib::ustring& path_string,
  //          const Glib::ustring& new_text) {
  //    Gtk::TreePath path(path_string);
  //
  //    //Convert the inputed text to an integer, as needed by our model column:
  //    char* pchEnd = 0;
  //    int new_value = strtol(new_text.c_str(), &pchEnd, 10);
  //
  //    if (new_value > 10) {
  //      //Prevent entry of numbers higher than 10.
  //
  //      //Tell the user:
  //      Gtk::MessageDialog dialog(*this,
  //              "The number must be less than 10. Please try again.",
  //              false, Gtk::MESSAGE_ERROR);
  //      dialog.run();
  //
  //      //Start editing again, with the bad text, so that the user can correct it.
  //      //A real application should probably allow the user to revert to the
  //      //previous text.
  //
  //      //Set the text to be used in the start_editing signal handler:
  //      m_invalid_text_for_retry = new_text;
  //      m_validate_retry = true;
  //
  //      //Start editing again:
  //      m_TreeView.set_cursor(path, m_treeviewcolumn_validated,
  //              m_cellrenderer_validated, true /* start_editing */);
  //    } else {
  //      //Get the row from the path:
  //      Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
  //      if (iter) {
  //        Gtk::TreeModel::Row row = *iter;
  //
  //        //Put the new value in the model:
  //        row[m_Columns.m_col_number_validated] = new_value;
  //      }
  //    }
  //  }

}