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

#include "GcfrctAppWindow.hpp"
#include <stdexcept>

GcfrctAppWindow::GcfrctAppWindow(BaseObjectType* cobject,
  const Glib::RefPtr<Gtk::Builder>& refBuilder)
: Gtk::ApplicationWindow(cobject),
  m_refBuilder(refBuilder)
{
}

//static
GcfrctAppWindow* GcfrctAppWindow::create()
{
  // Load the Builder file and instantiate its widgets.
  auto refBuilder = Gtk::Builder::create_from_resource("/gcfrct/application/ui/gcfrct.ui");
  //auto refBuilder = Gtk::Builder::create_from_file("ui/gcfrct.ui");

  auto window = Gtk::Builder::get_widget_derived<GcfrctAppWindow>(refBuilder, "gcfrct_app_window");
  if (!window)
    throw std::runtime_error("No \"gcfrct_app_window\" object in gcfrct.ui");

  return window;
}

void GcfrctAppWindow::open_file_view(const Glib::RefPtr<Gio::File>& /* file */)
{
}
