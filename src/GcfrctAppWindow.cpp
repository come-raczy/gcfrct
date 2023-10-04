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

  m_search = m_refBuilder->get_widget<Gtk::ToggleButton>("search");
  if (nullptr == m_search)
  {
    throw std::runtime_error("No \"search\" object in window.ui");
  }

  m_searchbar = m_refBuilder->get_widget<Gtk::SearchBar>("searchbar");
  if (nullptr == m_searchbar)
  {
    throw std::runtime_error("No \"searchbar\" object in window.ui");
  }

  m_searchentry = m_refBuilder->get_widget<Gtk::SearchEntry>("searchentry");
  if (nullptr == m_searchentry)
  {
    throw std::runtime_error("No \"searchentry\" object in window.ui");
  }

  m_gears = m_refBuilder->get_widget<Gtk::MenuButton>("gears");
  if (nullptr == m_gears)
  {
    throw std::runtime_error("No \"gears\" object in window.ui");
  }

  // Bind settings.
  m_settings = Gio::Settings::create("gcfrct");
  m_settings->bind("transition", m_stack->property_transition_type());

  // Bind properties.
  m_prop_binding = Glib::Binding::bind_property(m_search->property_active(),
    m_searchbar->property_search_mode_enabled(), Glib::Binding::Flags::BIDIRECTIONAL);

  // Connect signal handlers.
  m_searchentry->signal_search_changed().connect(
    sigc::mem_fun(*this, &GcfrctAppWindow::on_search_text_changed));
  m_stack->property_visible_child().signal_changed().connect(
    sigc::mem_fun(*this, &GcfrctAppWindow::on_visible_child_changed));

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

 auto buffer = view->get_buffer();
  try
  {
    char* contents = nullptr;
    gsize length = 0;

    file->load_contents(contents, length);
    buffer->set_text(contents, contents+length);
    g_free(contents);
  }
  catch (const Glib::Error& ex)
  {
    std::cout << "GcfrctAppWindow::open_file_view(\"" << file->get_parse_name()
      << "\"):\n  " << ex.what() << std::endl;
  }

 auto tag = buffer->create_tag();
  m_settings->bind("font", tag->property_font());
  buffer->apply_tag(tag, buffer->begin(), buffer->end());

  m_search->set_sensitive(true);
}

void GcfrctAppWindow::on_search_text_changed()
{
  const auto text = m_searchentry->get_text();
  if (text.empty())
  {
    return;
  }

  auto tab = dynamic_cast<Gtk::ScrolledWindow*>(m_stack->get_visible_child());
  if (nullptr == tab)
  {
    std::cout << "GcfrctAppWindow::on_search_text_changed(): No visible child." << std::endl;
    return;
  }

  auto view = dynamic_cast<Gtk::TextView*>(tab->get_child());
  if (nullptr == view)
  {
    std::cout << "GcfrctAppWindow::on_search_text_changed(): No visible text view." << std::endl;
    return;
  }

  // Very simple-minded search implementation.
  auto buffer = view->get_buffer();
  Gtk::TextIter match_start;
  Gtk::TextIter match_end;
  if (buffer->begin().forward_search(text, Gtk::TextSearchFlags::CASE_INSENSITIVE,
      match_start, match_end))
  {
    buffer->select_range(match_start, match_end);
    view->scroll_to(match_start);
  }
}

void GcfrctAppWindow::on_visible_child_changed()
{
  m_searchbar->set_search_mode(false);
}

