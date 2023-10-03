#include "GcfrctPreferences.hpp"
#include "GcfrctAppWindow.hpp"
#include <array>
#include <stdexcept>

namespace
{
struct TransitionTypeStruct
{
  Glib::ustring id;   // Value of "transition" key in Gio::Settings
  Glib::ustring text; // Text in the DropDown list
};

const std::array<TransitionTypeStruct, 3> transitionTypes =
{
  TransitionTypeStruct{"none",             "None"},
  TransitionTypeStruct{"crossfade",        "Fade"},
  TransitionTypeStruct{"slide-left-right", "Slide"}
};

} // anonymous namespace

GcfrctPreferences::GcfrctPreferences(BaseObjectType* cobject,
  const Glib::RefPtr<Gtk::Builder>& refBuilder)
: Gtk::Dialog(cobject),
  m_refBuilder(refBuilder),
  m_font(nullptr),
  m_transition(nullptr)
{
  m_font = m_refBuilder->get_widget<Gtk::FontDialogButton>("font");
  if (nullptr == m_font)
  {
    throw std::runtime_error("No \"font\" object in prefs.ui");
  }

  m_transition = m_refBuilder->get_widget<Gtk::DropDown>("transition");
  if (nullptr == m_transition)
  {
    throw std::runtime_error("No \"transition\" object in prefs.ui");
  }

  //m_settings->bind("font", m_font->property_font());
  //m_settings->bind("transition", m_transition->property_active_id());

  // DropDown for transition type.
  auto string_list = Gtk::StringList::create();
  for (const auto& transitionType : transitionTypes)
  {
    string_list->append(transitionType.text);
  }

  m_transition->set_model(string_list);

  m_settings = Gio::Settings::create("gcfrct");

  // Connect preference properties to the Gio::Settings.
#if HAS_GIO_SETTINGS_BIND_WITH_MAPPING
  m_settings->bind<Glib::ustring, Pango::FontDescription>("font",
    m_font->property_font_desc(), Gio::Settings::BindFlags::DEFAULT,
    [](const auto& font) { return Pango::FontDescription(font); },
    [](const auto& fontdesc) { return fontdesc.to_string(); }
  );
  m_settings->bind<Glib::ustring, unsigned int>("transition",
    m_transition->property_selected(), Gio::Settings::BindFlags::DEFAULT,
    [](const auto& transition) { return map_from_ustring_to_int(transition); },
    [](const auto& pos) { return map_from_int_to_ustring(pos); }
  );
#else
  // This is easier when g_settings_bind_with_mapping() is
  // wrapped in a Gio::Settings method.
  m_settings->signal_changed("font").connect(
    sigc::mem_fun(*this, &GcfrctPreferences::on_font_setting_changed));
  m_font->property_font_desc().signal_changed().connect(
    sigc::mem_fun(*this, &GcfrctPreferences::on_font_selection_changed));

  m_settings->signal_changed("transition").connect(
    sigc::mem_fun(*this, &GcfrctPreferences::on_transition_setting_changed));
  m_transition->property_selected().signal_changed().connect(
    sigc::mem_fun(*this, &GcfrctPreferences::on_transition_selection_changed));

  // Synchronize the preferences dialog with m_settings.
  on_font_setting_changed("font");
  on_transition_setting_changed("transition");
#endif
}

//static
GcfrctPreferences* GcfrctPreferences::create(Gtk::Window& parent)
{
  // Load the Builder file and instantiate its widgets.
  auto refBuilder = Gtk::Builder::create_from_resource("/gcfrct/application/ui/preferences.ui");

  auto * dialog = Gtk::Builder::get_widget_derived<GcfrctPreferences>(refBuilder, "prefs_dialog");
  if (nullptr == dialog)
  {
    throw std::runtime_error("No \"prefs_dialog\" object in prefs.ui");
  }

  dialog->set_transient_for(parent);

  return dialog;
}

#if HAS_GIO_SETTINGS_BIND_WITH_MAPPING
std::optional<unsigned int> GcfrctPreferences::map_from_ustring_to_int(const Glib::ustring& transition)
{
  for (std::size_t i = 0; i < transitionTypes.size(); ++i)
  {
    if (transitionTypes[i].id == transition)
    {
      return i;
    }
  }
  return std::nullopt;
}

std::optional<Glib::ustring> GcfrctPreferences::map_from_int_to_ustring(const unsigned int& pos)
{
  if (pos >= transitionTypes.size())
  {
    return std::nullopt;
  }
  return transitionTypes[pos].id;
}
#else
void GcfrctPreferences::on_font_setting_changed(const Glib::ustring& /* key */)
{
  const auto font_setting = m_settings->get_string("font");
  const auto font_button = m_font->get_font_desc().to_string();
  if (font_setting != font_button)
    m_font->set_font_desc(Pango::FontDescription(font_setting));
}

void GcfrctPreferences::on_font_selection_changed()
{
  const auto font_setting = m_settings->get_string("font");
  const auto font_button = m_font->get_font_desc().to_string();
  if (font_setting != font_button)
    m_settings->set_string("font", font_button);
}

void GcfrctPreferences::on_transition_setting_changed(const Glib::ustring& /* key */)
{
  const auto transition_setting = m_settings->get_string("transition");
  const auto transition_button = transitionTypes[m_transition->get_selected()].id;
  if (transition_setting != transition_button)
  {
    for (std::size_t i = 0; i < transitionTypes.size(); ++i)
    {
      if (transitionTypes[i].id == transition_setting)
      {
        m_transition->set_selected(i);
        break;
      }
    }
  }
}

void GcfrctPreferences::on_transition_selection_changed()
{
  const auto pos = m_transition->get_selected();
  if (pos >= transitionTypes.size())
    return;
  const auto transition_setting = m_settings->get_string("transition");
  const auto transition_button = transitionTypes[pos].id;
  if (transition_setting != transition_button)
    m_settings->set_string("transition", transition_button);
}
#endif

