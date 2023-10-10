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

#include "GcfrctApplicationWindow.hpp"
#include <stdexcept>
#include <iostream>
#include <set>

GcfrctApplicationWindow::GcfrctApplicationWindow(BaseObjectType* cobject,
  const Glib::RefPtr<Gtk::Builder>& builder)
: Gtk::ApplicationWindow(cobject)
{
  stack_ = builder->get_widget<Gtk::Stack>("stack");
  if (nullptr == stack_)
  {
    throw std::runtime_error("No \"stack\" object in window.ui");
  }
  stack_->set_halign(Gtk::Align::FILL);

  m_search = builder->get_widget<Gtk::ToggleButton>("search");
  if (nullptr == m_search)
  {
    throw std::runtime_error("No \"search\" object in window.ui");
  }

  m_gears = builder->get_widget<Gtk::MenuButton>("gears");
  if (nullptr == m_gears)
  {
    throw std::runtime_error("No \"gears\" object in window.ui");
  }

  // Bind settings.
  m_settings = Gio::Settings::create("gcfrct");
  m_settings->bind("transition", stack_->property_transition_type());

  // Connect signal handlers.
  stack_->property_visible_child().signal_changed().connect(
    sigc::mem_fun(*this, &GcfrctApplicationWindow::onVisibleChildChanged));

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

  // Set the window icon.
  Gtk::IconTheme::get_for_display(get_display())->add_resource_path("/gcfrct/application/icons");
  set_icon_name("gcfrct");
}

//static
GcfrctApplicationWindow* GcfrctApplicationWindow::create()
{
  // Load the Builder file and instantiate its widgets.
  auto builder = Gtk::Builder::create_from_resource("/gcfrct/application/ui/GcfrctApplicationWindow.ui");

  auto * window = Gtk::Builder::get_widget_derived<GcfrctApplicationWindow>(builder, "GcfrctApplicationWindow");
  if (nullptr == window)
  {
    throw std::runtime_error("No \"GcfrctApplicationWindow\" object in GcfrctApplicationWindow.ui");
  }
  // add the tabs
  window->addImageTab();
  window->addPrimaryTab();
  window->addSecondaryTab();
  window->addTiltTab();


  return window;
}

void GcfrctApplicationWindow::addImageTab()
{
  const Glib::ustring tabName = "Image";
  auto * sideBar = Gtk::make_managed<Gtk::ScrolledWindow>();
  auto * sideBarBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
  for (unsigned i = 0; i < 3; ++i)
  {
    const Glib::ustring::value_type mask = 'A' + i;
    Glib::ustring text = Glib::ustring::sprintf("Mask %c", mask);
    //auto * frame = Gtk::make_managed<Gtk::Frame>(text);
    auto * frame = Gtk::make_managed<Gtk::Frame>();
    frame->set_label(text);
    frame->set_margin(10);
    frame->set_label_align(Gtk::Align::END);
    auto * label = Gtk::make_managed<Gtk::Label>(text);
    frame->set_child(*label);
    std::cerr << "Adding label " << text << ": " << label->property_width_chars() <<  std::endl;
    sideBarBox->append(*frame);
  }
  sideBar->set_child(*sideBarBox);
  auto * sideBarFrame = Gtk::make_managed<Gtk::Frame>("Side Bar --------------------------");
  sideBarFrame->set_child(*sideBar);
  auto * contentArea = Gtk::make_managed<Gtk::ScrolledWindow>();
  auto * contentAreaFrame = Gtk::make_managed<Gtk::Frame>("Content Area");
  contentAreaFrame->set_halign(Gtk::Align::FILL);
  contentArea->set_halign(Gtk::Align::FILL);
  contentAreaFrame->set_child(*contentArea);
  auto * imageBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 20);
  const auto SPACING = 10;
  const auto MARGIN = 10;
  imageBox->set_spacing(SPACING);
  imageBox->set_margin(MARGIN);
  sideBarFrame->set_halign(Gtk::Align::FILL);
  imageBox->set_halign(Gtk::Align::FILL);
  imageBox->append(*sideBarFrame);
  imageBox->append(*contentAreaFrame);
  imageBox->set_expand(true);
  stack_->add(*imageBox, tabName, tabName);
}

void GcfrctApplicationWindow::addPrimaryTab()
{
  addAdjustmentTab("Primary");
}

void GcfrctApplicationWindow::addSecondaryTab()
{
  addAdjustmentTab("Secondary");
}

void GcfrctApplicationWindow::addTiltTab()
{
  addAdjustmentTab("Tilt");
}

void GcfrctApplicationWindow::addAdjustmentTab(const Glib::ustring &tabName)
{
  auto * scrolled = Gtk::make_managed<Gtk::ScrolledWindow>();
  scrolled->set_expand(true);
  auto * view = Gtk::make_managed<Gtk::TextView>();   
  view->set_editable(false);
  view->set_cursor_visible(false);                            
  scrolled->set_child(*view);
  scrolled->set_halign(Gtk::Align::FILL);
  scrolled->set_valign(Gtk::Align::FILL);
  scrolled->set_margin(10);
  auto * frame = Gtk::make_managed<Gtk::Frame>(tabName);
  frame->set_child(*scrolled);
  frame->set_halign(Gtk::Align::FILL);
  frame->set_valign(Gtk::Align::FILL);
  frame->set_margin(10);
  stack_->add(*frame, tabName, tabName);
}

void GcfrctApplicationWindow::onVisibleChildChanged()
{
	std::cout << "GcfrctApplicationWindow::onVisibleChildChanged()" << std::endl;
}

void GcfrctApplicationWindow::onRevealChildChanged()
{
	std::cout << "GcfrctApplicationWindow::onRevealChildChanged()" << std::endl;
}

