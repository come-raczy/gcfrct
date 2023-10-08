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

#include "GcfrctApplication.hpp"
#include "GcfrctApplicationWindow.hpp"
#include "GcfrctPreferences.hpp"
#include <iostream>
#include <exception>

GcfrctApplication::GcfrctApplication()
: Gtk::Application("gcfrct.application", Gio::Application::Flags::HANDLES_OPEN)
{
}

Glib::RefPtr<GcfrctApplication> GcfrctApplication::create()
{
  return Glib::make_refptr_for_instance<GcfrctApplication>(new GcfrctApplication());
}

Gtk::ApplicationWindow* GcfrctApplication::createApplicationWindow()
{
  auto * window = GcfrctApplicationWindow::create();

  // Make sure that the application runs for as long this window is still open.
  add_window(*window);

  // A window can be added to an application with Gtk::Application::add_window()
  // or Gtk::Window::set_application(). When all added windows have been hidden
  // or removed, the application stops running (Gtk::Application::run() returns()),
  // unless Gio::Application::hold() has been called.

  // Delete the window when it is hidden.
  window->signal_hide().connect([window](){ delete window; });

  return window;
}

void GcfrctApplication::on_startup()
{
  // Call the base class's implementation.
  Gtk::Application::on_startup();

  // Add actions and keyboard accelerators for the menu.
  add_action("preferences", sigc::mem_fun(*this, &GcfrctApplication::onActionPreferences));
  add_action("quit", sigc::mem_fun(*this, &GcfrctApplication::onActionQuit));
  set_accel_for_action("app.quit", "<Ctrl>Q");
}

void GcfrctApplication::on_activate()
{
  try
  {
    // The application has been started, so let's show a window.
    auto * window = createApplicationWindow();
    window->present();
  }
  // If create_window() throws an exception (perhaps from Gtk::Builder),
  // no window has been created, no window has been added to the application,
  // and therefore the application will stop running.
  catch (const Glib::Error& ex)
  {
    std::cerr << "GcfrctApplication::on_activate(): " << ex.what() << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "GcfrctApplication::on_activate(): " << ex.what() << std::endl;
  }
}

void GcfrctApplication::onHideWindow(Gtk::Window* window)
{
  delete window;
}

void GcfrctApplication::onActionPreferences()
{
  try
  {
    auto * prefs_dialog = GcfrctPreferences::create(*get_active_window());
    prefs_dialog->present();

    // Delete the dialog when it is hidden.
    prefs_dialog->signal_hide().connect([prefs_dialog](){ delete prefs_dialog; });
  }
  catch (const Glib::Error& ex)
  {
    std::cerr << "ExampleApplication::onActionPreferences(): " << ex.what() << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "ExampleApplication::onActionPreferences(): " << ex.what() << std::endl;
  }
}

void GcfrctApplication::onActionQuit()
{
  // Gio::Application::quit() will make Gio::Application::run() return,
  // but it's a crude way of ending the program. The window is not removed
  // from the application. Neither the window's nor the application's
  // destructors will be called, because there will be remaining reference
  // counts in both of them. If we want the destructors to be called, we
  // must remove the window from the application. One way of doing this
  // is to hide the window. See comment in create_window().
  auto windows = get_windows();
  for (auto * window : windows)
  {
    window->hide();
  }

  // Not really necessary, when Gtk::Widget::hide() is called, unless
  // Gio::Application::hold() has been called without a corresponding call
  // to Gio::Application::release().
  quit();
}
