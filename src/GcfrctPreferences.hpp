#ifndef GCFRCT_PREFERENCES_HPP
#define GCFRCT_PREFERENCES_HPP

#include <gtkmm.h>

class GcfrctPreferences : public Gtk::Dialog
{
public:
  GcfrctPreferences(BaseObjectType* cobject,
    const Glib::RefPtr<Gtk::Builder>& refBuilder);

  static GcfrctPreferences* create(Gtk::Window& parent);

protected:
  Glib::RefPtr<Gtk::Builder> m_refBuilder;
  Glib::RefPtr<Gio::Settings> m_settings;
  Gtk::FontButton* m_font;
  Gtk::ComboBoxText* m_transition;
};

#endif /* GCFRCT_PREFERENCES_HPP */
