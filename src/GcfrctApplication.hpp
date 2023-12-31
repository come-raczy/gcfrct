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

#ifndef GCFRCT_APPLICATION_HPP
#define GCFRCT_APPLICATION_HPP

#include <gtkmm.h>

class GcfrctApplication: public Gtk::Application
{
protected:
  GcfrctApplication();

public:
  static Glib::RefPtr<GcfrctApplication> create();

protected:
  // Override default signal handlers:
  void on_activate() override;

private:
  //GcfrctAppWindow* create_appwindow();
  Gtk::ApplicationWindow* createApplicationWindow();
  static void onHideWindow(Gtk::Window* window);
  void on_startup();
  void onActionPreferences();
  void onActionQuit();
};

#endif /* GCFRCT_APPLICATION_HPP */
