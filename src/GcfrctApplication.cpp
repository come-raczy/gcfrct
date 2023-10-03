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
#include "GcfrctAppWindow.hpp"
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

GcfrctAppWindow* GcfrctApplication::create_appwindow()
{
  auto * appwindow = GcfrctAppWindow::create();

  // Make sure that the application runs for as long this window is still open.
  add_window(*appwindow);

  // A window can be added to an application with Gtk::Application::add_window()
  // or Gtk::Window::set_application(). When all added windows have been hidden
  // or removed, the application stops running (Gtk::Application::run() returns()),
  // unless Gio::Application::hold() has been called.

  // Delete the window when it is hidden.
  appwindow->signal_hide().connect([appwindow](){ delete appwindow; });

  return appwindow;
}

void GcfrctApplication::on_startup()
{
  // Call the base class's implementation.
  Gtk::Application::on_startup();

  // Add actions and keyboard accelerators for the menu.
  add_action("preferences", sigc::mem_fun(*this, &GcfrctApplication::on_action_preferences));
  add_action("quit", sigc::mem_fun(*this, &GcfrctApplication::on_action_quit));
  set_accel_for_action("app.quit", "<Ctrl>Q");
}

void GcfrctApplication::on_activate()
{
  try
  {
    // The application has been started, so let's show a window.
    auto * appwindow = create_appwindow();
    appwindow->present();
  }
  // If create_appwindow() throws an exception (perhaps from Gtk::Builder),
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

void GcfrctApplication::on_open(const Gio::Application::type_vec_files& files,
  const Glib::ustring& /* hint */)
{
  // The application has been asked to open some files,
  // so let's open a new view for each one.
  GcfrctAppWindow* appwindow = nullptr;
  auto windows = get_windows();
  if (!windows.empty())
  {
    appwindow = dynamic_cast<GcfrctAppWindow*>(windows[0]);
  }

  try
  {
    if (nullptr == appwindow)
    {
      appwindow = create_appwindow();
    }

    for (const auto& file : files)
    {
      appwindow->open_file_view(file);
    }

    appwindow->present();
  }
  catch (const Glib::Error& ex)
  {
    std::cerr << "GcfrctApplication::on_open(): " << ex.what() << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "GcfrctApplication::on_open(): " << ex.what() << std::endl;
  }
}


void GcfrctApplication::on_hide_window(Gtk::Window* window)
{
  delete window;
}

void GcfrctApplication::on_action_preferences()
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
    std::cerr << "ExampleApplication::on_action_preferences(): " << ex.what() << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cerr << "ExampleApplication::on_action_preferences(): " << ex.what() << std::endl;
  }
}

void GcfrctApplication::on_action_quit()
{
  // Gio::Application::quit() will make Gio::Application::run() return,
  // but it's a crude way of ending the program. The window is not removed
  // from the application. Neither the window's nor the application's
  // destructors will be called, because there will be remaining reference
  // counts in both of them. If we want the destructors to be called, we
  // must remove the window from the application. One way of doing this
  // is to hide the window. See comment in create_appwindow().
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
