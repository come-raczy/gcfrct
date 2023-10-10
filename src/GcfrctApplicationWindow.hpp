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

#ifndef GCFRCT_APPLICATION_WINDOW_HPP
#define GCFRCT_APPLICATION_WINDOW_HPP

#include <gtkmm.h>

class GcfrctApplicationWindow : public Gtk::ApplicationWindow
{
public:
  GcfrctApplicationWindow(BaseObjectType* cobject,
    const Glib::RefPtr<Gtk::Builder>& builder);

  static GcfrctApplicationWindow* create();

protected:
  // Signal handlers
  void onVisibleChildChanged();
  void onRevealChildChanged();

private:
  void addImageTab();
  void addPrimaryTab();
  void addSecondaryTab();
  void addTiltTab();
  void addAdjustmentTab(const Glib::ustring &tabName);

  Gtk::Stack* stack_ {nullptr};

  Glib::RefPtr<Gio::Settings> m_settings;
  Gtk::ToggleButton* m_search;
  Gtk::MenuButton* m_gears;
  Glib::RefPtr<Glib::Binding> m_binding_search_enabled;
  Glib::RefPtr<Glib::Binding> m_binding_lines_visible;
};

#endif /* GCFRCT_APPLICATION_WINDOW_HPP */
