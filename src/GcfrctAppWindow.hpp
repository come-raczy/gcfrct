/*
 * Copyright (C) 2023  Come Raczy
 *
 * This file is part of gcfrct:
 * Guided Colimation For Ritchey-Chretien Telescopes.
 *
 * Gcfrct is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Gcfrct is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along
 * with gcfrct. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef GCFRCT_APP_WINDOW_HPP
#define GCFRCT_APP_WINDOW_HPP

#include <gtkmm.h>

class GcfrctAppWindow : public Gtk::ApplicationWindow
{
public:
  GcfrctAppWindow(BaseObjectType* cobject,
    const Glib::RefPtr<Gtk::Builder>& refBuilder);

  static GcfrctAppWindow* create();

  void open_file_view(const Glib::RefPtr<Gio::File>& file);

protected:
  // Signal handlers
  void on_search_text_changed();
  void on_visible_child_changed();
  void on_find_word(const Gtk::Button* button);
  void on_reveal_child_changed();

  void update_words();
  void update_lines();

  Glib::RefPtr<Gtk::Builder> m_refBuilder;
  Glib::RefPtr<Gio::Settings> m_settings;
  Gtk::Stack* m_stack {nullptr};
  Gtk::ToggleButton* m_search;
  Gtk::SearchBar* m_searchbar;
  Gtk::SearchEntry* m_searchentry;
  Gtk::MenuButton* m_gears;
  Gtk::Revealer* m_sidebar {nullptr};
  Gtk::ListBox* m_words {nullptr};
  Gtk::Label* m_lines {nullptr};
  Gtk::Label* m_lines_label {nullptr};
  Glib::RefPtr<Glib::Binding> m_binding_search_enabled;
  Glib::RefPtr<Glib::Binding> m_binding_lines_visible;
};

#endif /* GCFRCT_APP_WINDOW_HPP */
