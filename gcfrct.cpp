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

int main(int argc, char* argv[])
{
  // Since this example is running uninstalled, we have to help it find its
  // schema. This is *not* necessary in a properly installed application.
  Glib::setenv ("GSETTINGS_SCHEMA_DIR", ".", false);

  auto application = GcfrctApplication::create();

  // Start the application, showing the initial window,
  // and opening extra views for any files that it is asked to open,
  // for instance as a command-line parameter.
  // run() will return when the last window has been closed.
  return application->run(argc, argv);
}
