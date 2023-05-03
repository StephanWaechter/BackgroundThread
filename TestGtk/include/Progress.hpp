#pragma once
#include <gtkmm.h>

namespace BackgroundThread
{
	namespace GtkDemo
	{
		class Progress : public Gtk::Grid
		{
		public:
			Progress(std::string const & label)
			{
				m_Label.set_text(label);
				m_Button.set_child(Gtk::Label("X"));
				attach(m_Label, 0, 0);
				attach(m_ProgressBar, 1, 0);
				attach(m_Button, 2, 0);
			}

			void set_fraction(double fraction)
			{
				m_ProgressBar.set_fraction(fraction);
			}

			auto get_AbortButtonClicked()
			{
				return m_Button.signal_clicked();
			}

		private:
			Gtk::Label m_Label;
			Gtk::ProgressBar m_ProgressBar;
			Gtk::Button m_Button;
		};
	}
}