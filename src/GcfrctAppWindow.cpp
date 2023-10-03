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
#include <iostream>

GcfrctAppWindow::GcfrctAppWindow(BaseObjectType* cobject,
  const Glib::RefPtr<Gtk::Builder>& refBuilder)
: Gtk::ApplicationWindow(cobject),
  m_refBuilder(refBuilder),
  m_stack(nullptr),
  m_gears(nullptr)
{
  m_stack = m_refBuilder->get_widget<Gtk::Stack>("stack");
  if (nullptr == m_stack)
  {
    throw std::runtime_error("No \"stack\" object in window.ui");
  }
  m_gears = m_refBuilder->get_widget<Gtk::MenuButton>("gears");
  if (nullptr == m_gears)
  {
    throw std::runtime_error("No \"gears\" object in window.ui");
  }

  // Bind settings.
  m_settings = Gio::Settings::create("gcfrct");
  m_settings->bind("transition", m_stack->property_transition_type());

  // Connect the menu to the MenuButton m_gears.
  // (The connection between action and menu item is specified in gears_menu.ui.)
  auto menu_builder = Gtk::Builder::create_from_resource("/gcfrct/application/ui/gears_menu.ui");
  auto menu = menu_builder->get_object<Gio::MenuModel>("menu");
  if (nullptr == menu)
  {
    throw std::runtime_error("No \"menu\" object in gears_menu.ui");
  }

  m_gears->set_menu_model(menu);
}

//static
GcfrctAppWindow* GcfrctAppWindow::create()
{
  // Load the Builder file and instantiate its widgets.
  auto refBuilder = Gtk::Builder::create_from_resource("/gcfrct/application/ui/gcfrct.ui");

  auto * window = Gtk::Builder::get_widget_derived<GcfrctAppWindow>(refBuilder, "gcfrct_app_window");
  if (nullptr == window)
  {
    throw std::runtime_error("No \"gcfrct_app_window\" object in gcfrct.ui");
  }

  return window;
}

void GcfrctAppWindow::open_file_view(const Glib::RefPtr<Gio::File>& file)
{
  const Glib::ustring basename = file->get_basename();

  auto * scrolled = Gtk::make_managed<Gtk::ScrolledWindow>();
  scrolled->set_expand(true);
  auto * view = Gtk::make_managed<Gtk::TextView>();
  view->set_editable(false);
  view->set_cursor_visible(false);
  scrolled->set_child(*view);
  m_stack->add(*scrolled, basename, basename);

  try
  {
    char* contents = nullptr;
    gsize length = 0;

    file->load_contents(contents, length);
    view->get_buffer()->set_text(contents, contents+length);
    g_free(contents);
  }
  catch (const Glib::Error& ex)
  {
    std::cout << "ExampleAppWindow::open_file_view(\"" << file->get_parse_name()
      << "\"):\n  " << ex.what() << std::endl;
  }
}
