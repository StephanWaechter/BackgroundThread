#pragma once
#include <gtkmm.h>

namespace GtkTest
{
	namespace Views
	{
		class Thumb : public Gtk::Button
		{
		public:
			Thumb(Glib::ustring const& label)
			{
				set_label(label);
				set_margin(10);
				set_hexpand(true);
				set_vexpand(true);
				set_size_request(-1, 40);
			}
		};
	}
}