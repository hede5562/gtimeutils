/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ggui_time_elem.c
 * Copyright (C) 2013 Jente Hidskes <jthidskes@outlook.com>
 *
 * gtimeutils is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gtimeutils is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <gtk/gtk.h>
#include <canberra.h>
#include <glib/gi18n.h>
#include <glib/gprintf.h>
#include <libnotify/notify.h>
#include <stdlib.h>

enum {
    STARTED,
    PAUSED,
    STOPPED
};

static ca_context *sound;
static gboolean start_on_run = FALSE;
static const gchar *entry_text = N_("Notification text");
static gint state = STOPPED;

struct gui {
	GtkWidget *timer_display;
	GtkWidget *entry;
	GtkWidget *button_timer;
	GtkWidget *button_reset;
} gui_elem;

struct gui_timer {
	GtkWidget *spin_seconds;
	GtkWidget *spin_minutes;
	GtkWidget *spin_hours;
} gui_time_elem;

struct timer {
	gint hours;
	gint minutes;
	gint seconds;
} time_elem;

static GOptionEntry entries[] = {
	{ "seconds", 's', 0, G_OPTION_ARG_INT, &time_elem.seconds, N_("Specify seconds to count down from"), NULL },
	{ "minutes", 'm', 0, G_OPTION_ARG_INT, &time_elem.minutes, N_("Specify minutes to count down from"), NULL },
	{ "hours", 'u', 0, G_OPTION_ARG_INT, &time_elem.hours, N_("Specify hours to count down from"), NULL },
	{ "text", 't', 0, G_OPTION_ARG_STRING, &entry_text, N_("Set an alternative notification text"), NULL },
	{ "run", 'r', 0, G_OPTION_ARG_NONE, &start_on_run, N_("Immediately start the countdown"), NULL },
	{ NULL },
};

static void reset_display (void) {
	gchar *markup;

	markup = g_markup_printf_escaped ("<span font=\"48\" weight=\"heavy\"><tt>%s</tt></span>", _("HH:MM:SS"));
	gtk_label_set_markup (GTK_LABEL (gui_elem.timer_display), markup);
	g_free (markup);
}

static void notify (void) {
	NotifyNotification *notify;
	GError *error_notify = NULL;

	entry_text = gtk_entry_get_text (GTK_ENTRY (gui_elem.entry));
	if(g_strcmp0 (entry_text, "Notification text") != 0)
		notify = notify_notification_new (entry_text, NULL, "clocks");
	else
		notify = notify_notification_new (_("Time is up!"), NULL, "clocks");
	notify_notification_set_category (notify, "GTimeUtils");
	notify_notification_set_urgency (notify, NOTIFY_URGENCY_NORMAL);
	notify_notification_show (notify, &error_notify);
	if(error_notify)
		g_fprintf (stderr, "Can not initialize notification: %s\n", error_notify->message);

	ca_context_play(sound, 0, CA_PROP_APPLICATION_NAME, "Gtimer", CA_PROP_EVENT_ID, "complete-copy", CA_PROP_MEDIA_ROLE, "notification",  CA_PROP_APPLICATION_ICON_NAME, "clocks", CA_PROP_CANBERRA_CACHE_CONTROL, "never", NULL);
}

static void counter (void) {
	gchar *markup, output[100];

	time_elem.seconds = gtk_spin_button_get_value (GTK_SPIN_BUTTON (gui_time_elem.spin_seconds));
	time_elem.minutes = gtk_spin_button_get_value (GTK_SPIN_BUTTON (gui_time_elem.spin_minutes));
	time_elem.hours = gtk_spin_button_get_value (GTK_SPIN_BUTTON (gui_time_elem.spin_hours));
	time_elem.seconds += 60 * time_elem.minutes;
	time_elem.seconds += 3600 * time_elem.hours;

	if(time_elem.seconds == 0) {
		notify();
		gtk_button_set_label (GTK_BUTTON (gui_elem.button_timer), _("Start"));
		gtk_widget_set_sensitive (gui_elem.button_reset, FALSE);
		gtk_widget_set_sensitive (gui_elem.entry, TRUE);
		state = STOPPED;
		start_on_run = FALSE;
	} else {
		time_elem.seconds -= 1;
		time_elem.hours = time_elem.seconds / 3600;
		time_elem.seconds -= 3600 * time_elem.hours;
		time_elem.minutes = time_elem.seconds / 60;
		time_elem.seconds -= 60 * time_elem.minutes;
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (gui_time_elem.spin_seconds), time_elem.seconds);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (gui_time_elem.spin_minutes), time_elem.minutes);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (gui_time_elem.spin_hours), time_elem.hours);
		sprintf (output, "%02d:%02d:%02d", time_elem.hours, time_elem.minutes, time_elem.seconds);

		gtk_label_set_text (GTK_LABEL (gui_elem.timer_display), output);
		markup = g_markup_printf_escaped ("<span font=\"48\" weight=\"heavy\"><tt>%s</tt></span>", output);
		gtk_label_set_markup (GTK_LABEL (gui_elem.timer_display), markup);
		g_free (markup);
	}
}

static gboolean timer_function (void) {
	if(state == STARTED || start_on_run)
		counter();
	return TRUE;
}

static void on_timer_button_clicked (void) {
	if(state == STOPPED) {
		gtk_button_set_label (GTK_BUTTON (gui_elem.button_timer), _("Stop"));
		gtk_widget_set_sensitive (gui_elem.button_reset, TRUE);
		gtk_widget_set_sensitive (gui_elem.entry, FALSE);
		state = STARTED;
	} else if(state == PAUSED) {
		gtk_button_set_label (GTK_BUTTON (gui_elem.button_timer), _("Stop"));
		state = STARTED;
	} else if(state == STARTED) {
		gtk_button_set_label (GTK_BUTTON (gui_elem.button_timer), _("Continue"));
		state = PAUSED;
	}
}

static void on_reset_button_clicked (void) {
	if(state == PAUSED || state == STARTED) {
		state = STOPPED;
		gtk_button_set_label (GTK_BUTTON (gui_elem.button_timer), _("Start"));
		reset_display();
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (gui_time_elem.spin_seconds), 0);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (gui_time_elem.spin_minutes), 0);
		gtk_spin_button_set_value (GTK_SPIN_BUTTON (gui_time_elem.spin_hours), 0);
		gtk_widget_set_sensitive (gui_elem.button_reset, FALSE);
		gtk_widget_set_sensitive (gui_elem.entry, TRUE);
	}
}

int main (int argc, char *argv[]) {
	GError *error_parsearg = NULL;
	GOptionContext *context;
	GtkWidget *window, *vbox, *hbox1, *hbox2;
	GtkAdjustment *sadj, *madj, *hadj;

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	gtk_init (&argc, &argv);
	notify_init ("Gtimer");
	ca_context_create (&sound);

	context = g_option_context_new (_("- a simple countdown timer"));
	g_option_context_add_main_entries (context, entries, NULL);
#ifdef ENABLE_NLS
	g_option_context_set_translation_domain (context, GETTEXT_PACKAGE);
#endif
	if (!g_option_context_parse (context, &argc, &argv, &error_parsearg)) {
		g_fprintf (stderr, "%s\n", error_parsearg->message);
		exit(1);
	}

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
	hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

	gtk_box_set_spacing (GTK_BOX (hbox2), 5);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);

	gui_elem.timer_display = gtk_label_new (NULL);
	reset_display();

	sadj = gtk_adjustment_new (0, 0, 60, 1, 1, 0);
	madj = gtk_adjustment_new (0, 0, 60, 1, 1, 0);
	hadj = gtk_adjustment_new (0, 0, 24, 1, 1, 0);
	gui_time_elem.spin_seconds = gtk_spin_button_new (sadj, 1, 0);
	gui_time_elem.spin_minutes = gtk_spin_button_new (madj, 1, 0);
	gui_time_elem.spin_hours = gtk_spin_button_new (hadj, 1, 0);

	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (gui_time_elem.spin_seconds), TRUE);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (gui_time_elem.spin_seconds), time_elem.seconds);
	g_object_set (gui_time_elem.spin_seconds, "shadow-type", GTK_SHADOW_IN, NULL);
	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (gui_time_elem.spin_minutes), TRUE);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (gui_time_elem.spin_minutes), time_elem.minutes);
	g_object_set (gui_time_elem.spin_minutes, "shadow-type", GTK_SHADOW_IN, NULL);
	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (gui_time_elem.spin_hours), TRUE);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (gui_time_elem.spin_hours), time_elem.hours);
	g_object_set (gui_time_elem.spin_hours, "shadow-type", GTK_SHADOW_IN, NULL);

	gui_elem.entry = gtk_entry_new();
	gtk_entry_set_text (GTK_ENTRY (gui_elem.entry), entry_text);

	gui_elem.button_timer = gtk_button_new();
	gtk_button_set_label (GTK_BUTTON (gui_elem.button_timer), _("Start"));
	gui_elem.button_reset = gtk_button_new_with_label (_("Reset"));
	gtk_widget_set_sensitive (gui_elem.button_reset, FALSE);

	gtk_box_pack_start (GTK_BOX (hbox1), gui_time_elem.spin_hours, TRUE, FALSE, 5);
	gtk_box_pack_start (GTK_BOX (hbox1), gui_time_elem.spin_minutes, TRUE, FALSE, 5);
	gtk_box_pack_start (GTK_BOX (hbox1), gui_time_elem.spin_seconds, TRUE, FALSE, 5);
	gtk_box_pack_start (GTK_BOX (vbox), gui_elem.timer_display, FALSE, TRUE, 5);
	gtk_box_pack_start (GTK_BOX (hbox2), gui_elem.button_timer, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox2), gui_elem.button_reset, TRUE, TRUE, 0);
	gtk_container_add (GTK_CONTAINER (vbox), hbox1);
	gtk_box_pack_start (GTK_BOX (vbox), gui_elem.entry, FALSE, TRUE, 5);
	gtk_container_add (GTK_CONTAINER (vbox), hbox2);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "Gtimer");
	gtk_container_add (GTK_CONTAINER (window), vbox);
	gtk_widget_show_all (window);

	g_timeout_add_seconds (1, (GSourceFunc) timer_function, NULL);
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect (gui_elem.button_timer, "clicked", G_CALLBACK (on_timer_button_clicked), NULL);
	g_signal_connect (gui_elem.button_reset, "clicked", G_CALLBACK (on_reset_button_clicked), NULL);

	gtk_main();
	notify_uninit();
	ca_context_destroy (sound);
	g_option_context_free (context);
	return 0;
}
