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
#include <set>

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

  m_sidebar = m_refBuilder->get_widget<Gtk::Revealer>("sidebar");
  if (nullptr == m_sidebar)
  {
    throw std::runtime_error("No \"sidebar\" object in window.ui");
  }

  m_words = m_refBuilder->get_widget<Gtk::ListBox>("words");
  if (nullptr == m_words)
  {
    throw std::runtime_error("No \"words\" object in window.ui");
  }

  m_lines = m_refBuilder->get_widget<Gtk::Label>("lines");
  if (nullptr == m_lines)
  {
    throw std::runtime_error("No \"lines\" object in window.ui");
  }

  m_lines_label = m_refBuilder->get_widget<Gtk::Label>("lines_label");
  if (nullptr == m_lines_label)
  {
    throw std::runtime_error("No \"lines_label\" object in window.ui");
  }

  // Bind settings.
  m_settings = Gio::Settings::create("gcfrct");
  m_settings->bind("transition", m_stack->property_transition_type());
  m_settings->bind("show-words", m_sidebar->property_reveal_child());

  // Bind properties.
  m_binding_search_enabled = Glib::Binding::bind_property(m_search->property_active(),
    m_searchbar->property_search_mode_enabled(), Glib::Binding::Flags::BIDIRECTIONAL);

  // Connect signal handlers.
  m_searchentry->signal_search_changed().connect(
    sigc::mem_fun(*this, &GcfrctAppWindow::on_search_text_changed));
  m_stack->property_visible_child().signal_changed().connect(
    sigc::mem_fun(*this, &GcfrctAppWindow::on_visible_child_changed));
  m_sidebar->property_reveal_child().signal_changed().connect(
    sigc::mem_fun(*this, &GcfrctAppWindow::on_reveal_child_changed));

  // Connect the menu to the MenuButton m_gears.
  // (The connection between action and menu item is specified in gears_menu.ui.)
  auto menu_builder = Gtk::Builder::create_from_resource("/gcfrct/application/ui/gears_menu.ui");
  auto menu = menu_builder->get_object<Gio::MenuModel>("menu");
  if (nullptr == menu)
  {
    throw std::runtime_error("No \"menu\" object in gears_menu.ui");
  }

  m_gears->set_menu_model(menu);
  add_action(m_settings->create_action("show-words"));

  // Bind the "visible" property of m_lines to the win.show-lines action, to
  // the "Lines" menu item and to the "visible" property of m_lines_label.
  add_action(Gio::PropertyAction::create("show-lines", m_lines->property_visible()));
  m_binding_lines_visible = Glib::Binding::bind_property(m_lines->property_visible(),
    m_lines_label->property_visible());
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
  update_words();
  update_lines();
}

void GcfrctAppWindow::on_search_text_changed()
{
  const auto text = m_searchentry->get_text();
  if (text.empty())
  {
    return;
  }

  auto * tab = dynamic_cast<Gtk::ScrolledWindow*>(m_stack->get_visible_child());
  if (nullptr == tab)
  {
    std::cout << "GcfrctAppWindow::on_search_text_changed(): No visible child." << std::endl;
    return;
  }

  auto * view = dynamic_cast<Gtk::TextView*>(tab->get_child());
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
  update_words();
  update_lines();
}

void GcfrctAppWindow::on_find_word(const Gtk::Button* button)
{
  m_searchentry->set_text(button->get_label());
}

void GcfrctAppWindow::on_reveal_child_changed()
{
  update_words();
}

void GcfrctAppWindow::update_words()
{
  auto * tab = dynamic_cast<Gtk::ScrolledWindow*>(m_stack->get_visible_child());
  if (nullptr ==tab)
  {
    return;
  }

  auto * view = dynamic_cast<Gtk::TextView*>(tab->get_child());
  if (nullptr == view)
  {
    std::cout << "GcfrctAppWindow::update_words(): No visible text view." << std::endl;
    return;
  }
  auto buffer = view->get_buffer();

  // Find all words in the text buffer.
  std::set<Glib::ustring> words;
  auto start = buffer->begin();
  Gtk::TextIter end;
  while (start)
  {
    while (start && !start.starts_word())
    {
      ++start;
    }

    if (!start)
    {
      break;
    }

    end = start;
    end.forward_word_end();
    if (start == end)
    {
      break;
    }

    auto word = buffer->get_text(start, end, false);
    words.insert(word.lowercase());
    start = end;
  }

  // Remove old children from the ListBox.
  while (auto * child = m_words->get_first_child())
  {
    m_words->remove(*child);
  }

  // Add new child buttons, one per unique word.
  for (const auto& word : words)
  {
    auto * row = Gtk::make_managed<Gtk::Button>(word);
    row->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this,
      &GcfrctAppWindow::on_find_word), row));
    m_words->append(*row);
  }
}

void GcfrctAppWindow::update_lines()
{
  auto * tab = dynamic_cast<Gtk::ScrolledWindow*>(m_stack->get_visible_child());
  if (nullptr == tab)
  {
    return;
  }

  auto * view = dynamic_cast<Gtk::TextView*>(tab->get_child());
  if (nullptr == view)
  {
    std::cout << "GcfrctAppWindow::update_lines(): No visible text view." << std::endl;
    return;
  }
  auto buffer = view->get_buffer();

  int count = 0;
  auto iter = buffer->begin();
  while (iter)
  {
    ++count;
    if (!iter.forward_line())
    {
      break;
    }
  }
  m_lines->set_text(Glib::ustring::format(count));
}

