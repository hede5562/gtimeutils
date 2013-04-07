#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include <glib/gi18n.h>

gboolean running;
GTimer *timer;
GtkWidget *timer_display;

gboolean update_progress_bar (void) {
	gchar output[100];
	gint hours, minutes, seconds = -1;
	gulong gulong;

	seconds = g_timer_elapsed (timer, &gulong);
	hours = seconds / 3600;
	seconds -= 3600 * hours;
	minutes = seconds / 60;
	seconds -= 60 * minutes;
	sprintf(output, "%02d:%02d:%02d", hours, minutes, seconds);

	gtk_label_set_text(GTK_LABEL(timer_display), output);
	return TRUE;
}

gboolean keypress (GtkWidget *widget, GdkEventKey *event) {
	guint(g) = event->keyval;

	if((g == GDK_KEY_space)) {
		if(running == FALSE) {
			g_timer_start(timer);
			running = TRUE;
			return TRUE;
		} else {
			g_timer_stop(timer);
			running = FALSE;
			return TRUE;
		}
	}
	return FALSE;
}

void about_dialog_close (GtkWidget *about_dialog) {
	gtk_widget_destroy(GTK_WIDGET(about_dialog));
}

void about_dialog_open (void) {
	GtkWidget *about_dialog;
	gchar *license_trans;

	const gchar *authors[] = {"Jente", "Calvin Morrison", NULL};
	const gchar *license[] = {
		N_("Gstopwatch is free software: you can redistribute it and/or modify "
		   "it under the terms of the GNU General Public License as published by "
		   "the Free Software Foundation, either version 3 of the License, or "
		   "(at your option) any later version."),
		N_("Gstopwatch is distributed in the hope that it will be useful "
		   "but WITHOUT ANY WARRANTY; without even the implied warranty of "
		   "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
		   "GNU General Public License for more details."),
		N_("You should have received a copy of the GNU General Public License "
		   "along with this program. If not, see <http://www.gnu.org/licenses/>.")
	};
	license_trans = g_strjoin ("\n\n", _(license[0]), _(license[1]), _(license[2]), NULL);

	about_dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG (about_dialog), "Gstopwatch");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG (about_dialog), "A simple stopwatch, written in GTK3"),
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG (about_dialog), "Copyright \xc2\xa9 2013 Jente");
	gtk_about_dialog_set_license(GTK_ABOUT_DIALOG (about_dialog), license_trans);
	gtk_about_dialog_set_wrap_license(GTK_ABOUT_DIALOG (about_dialog), TRUE);
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG (about_dialog), authors);
	gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG (about_dialog), "GitHub repository");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG (about_dialog), "https://github.com/Unia/gstopwatch");
	gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG (about_dialog), "clocks");

	g_signal_connect (GTK_DIALOG(about_dialog), "response", G_CALLBACK(about_dialog_close), NULL);

	gtk_widget_show (about_dialog);
}

int main (int argc, char *argv[]) {
	GtkWidget *window, *box, *box_buttons, *button_about;

	gtk_init(&argc, &argv);

	timer_display = gtk_label_new("");
	button_about = gtk_button_new_from_stock("gtk-about");
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	box_buttons = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_box_pack_start(GTK_BOX(box), timer_display, TRUE, TRUE, 5);
	gtk_container_add(GTK_CONTAINER(box), box_buttons);
	gtk_box_pack_end(GTK_BOX(box_buttons), button_about, FALSE, FALSE, 5);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW(window), "Gstopwatch");
	gtk_window_set_default_icon_name("clocks");
	gtk_container_add(GTK_CONTAINER(window), box);
	gtk_widget_show_all(window);

	timer = g_timer_new();
	g_timer_stop(timer);

	g_timeout_add_full(G_PRIORITY_HIGH, 50, (GSourceFunc) update_progress_bar, NULL, NULL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(keypress), window);
	g_signal_connect (button_about, "clicked", G_CALLBACK (about_dialog_open), NULL);

	gtk_main();
	g_timer_destroy(timer);
}
