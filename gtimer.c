/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * gtimer
 * Copyright (C) 2013 Jente (jthidskes at outlook dot com)
 *
 * gtimer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gtimer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>
#include <canberra.h>
#include <glib/gprintf.h>
#include <libnotify/notify.h>
#include <stdlib.h>

enum {
    STARTED,
    PAUSED,
    STOPPED
};

GdkColor color;
ca_context *sound;
GtkStatusIcon *tray_icon;
static gboolean start_on_run = FALSE;
const gchar *entry_text = "Notification text";
gint state = STOPPED, hours = 0, minutes = 0, seconds = 0;
GtkWidget *timer_display, *hbox1, *entry, *button_timer, *button_reset, *spin_seconds, *spin_minutes, *spin_hours;

static GOptionEntry entries[] = {
	{ "seconds", 's', 0, G_OPTION_ARG_INT, &seconds, "Specify seconds to count down from", NULL },
	{ "minutes", 'm', 0, G_OPTION_ARG_INT, &minutes, "Specify minutes to count down from", NULL },
	{ "hours", 'u', 0, G_OPTION_ARG_INT, &hours, "Specify hours to count down from", NULL },
	{ "text", 't', 0, G_OPTION_ARG_STRING, &entry_text, "Set an alternative notification text", NULL },
	{ "run", 'r', 0, G_OPTION_ARG_NONE, &start_on_run, "Immediately start the countdown", NULL },
	{ NULL },
};

void button_timer_stop (void) {
	gdk_color_parse("#C73333", &color);
	gtk_widget_modify_fg(button_timer, GTK_STATE_NORMAL, &color);
	gtk_button_set_label(GTK_BUTTON(button_timer), "Stop");
}

void button_timer_start (gboolean start) {
	gdk_color_parse("#67953C", &color);
	gtk_widget_modify_fg(button_timer, GTK_STATE_NORMAL, &color);
	if(start)
		gtk_button_set_label(GTK_BUTTON(button_timer), "Start");
	else
		gtk_button_set_label(GTK_BUTTON(button_timer), "Continue");
}

void notify (void) {
	NotifyNotification *notify;
	GError *error_notify = NULL;

	entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
	if(g_strcmp0(entry_text, "Notification text...") != 0)
		notify = notify_notification_new(entry_text, NULL, "clocks");
	else
		notify = notify_notification_new("Time is up!", NULL, "clocks");
	notify_notification_set_category(notify, "GTimeUtils");
	notify_notification_set_urgency(notify, NOTIFY_URGENCY_NORMAL);
	notify_notification_show(notify, &error_notify);
	if(error_notify)
		g_fprintf(stderr, "Can not initialize notification: %s\n", error_notify->message);

	ca_context_play(sound, 0, CA_PROP_APPLICATION_NAME, "Gtimer", CA_PROP_EVENT_ID, "complete-copy", CA_PROP_MEDIA_ROLE, "notification",  CA_PROP_APPLICATION_ICON_NAME, "clocks", CA_PROP_CANBERRA_CACHE_CONTROL, "never", NULL);
}

void counter (void) {
	gchar *markup, tray_text[120], output[100];

	seconds = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_seconds));
	minutes = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_minutes));
	hours = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_hours));
	seconds += 60 * minutes;
	seconds += 3600 * hours;

	if(seconds == 0) {
		notify();
		button_timer_start(TRUE);
		gtk_widget_set_sensitive(button_reset, FALSE);
		gtk_widget_set_sensitive(entry, TRUE);
		state = STOPPED;
		start_on_run = FALSE;
		if(gtk_status_icon_is_embedded(tray_icon) == TRUE) {
			gtk_status_icon_set_tooltip_text(tray_icon, NULL);
			gtk_status_icon_set_visible(tray_icon, FALSE);
		}
#ifdef DEBUG
		g_fprintf(stdout, "Timer completed!\n");
#endif
	} else {
		seconds -= 1;
		hours = seconds / 3600;
		seconds -= 3600 * hours;
		minutes = seconds / 60;
		seconds -= 60 * minutes;
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_seconds), seconds);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_minutes), minutes);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_hours), hours);
		sprintf(output, "%02d:%02d:%02d", hours, minutes, seconds);

		gtk_label_set_text(GTK_LABEL(timer_display), output);
		markup = g_markup_printf_escaped("<span font=\"48\" weight=\"heavy\"><tt>%s</tt></span>", output);
		gtk_label_set_markup(GTK_LABEL(timer_display), markup);
		g_free (markup);

		if(gtk_status_icon_is_embedded(tray_icon) == TRUE) {
			sprintf(tray_text, "Gtimer is running: %s", output);
			gtk_status_icon_set_tooltip_text(tray_icon, tray_text);
		}
#ifdef DEBUG
		g_fprintf(stdout, "Seconds: %02d   Minutes: %02d   Hours: %02d\n", seconds, minutes, hours);
#endif
	}
}

gboolean timer_function (void) {
	if(state == STARTED || start_on_run)
		counter();
	return TRUE;
}

void on_timer_button_clicked (void) {
	if(state == STOPPED) {
		button_timer_stop();
		gtk_widget_set_sensitive(button_reset, TRUE);
		gtk_widget_set_sensitive(entry, FALSE);
		if(gtk_status_icon_is_embedded(tray_icon) == FALSE) /*this doesn't add up. re-check this with a proper system tray*/
			gtk_status_icon_set_visible(tray_icon, TRUE);
		state = STARTED;
	} else if(state == PAUSED) {
		button_timer_stop();
		state = STARTED;
	} else if(state == STARTED) {
		button_timer_start(FALSE);
		state = PAUSED;
	}
}

void on_reset_button_clicked (void) {
	if(state == PAUSED || state == STARTED) {
		state = STOPPED;
		button_timer_start(TRUE);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_seconds), 0);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_minutes), 0);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_hours), 0);
		gtk_widget_set_sensitive(button_reset, FALSE);
		gtk_widget_set_sensitive(entry, TRUE);
		if(gtk_status_icon_is_embedded(tray_icon) == TRUE) {
			gtk_status_icon_set_tooltip_text(tray_icon, NULL);
			gtk_status_icon_set_visible(tray_icon, FALSE);
		}
	}
}

int main (int argc, char *argv[]) {
	GError *error_parsearg = NULL;
	GOptionContext *context;
	GtkWidget *window, *vbox, *hbox2;
#ifdef GTK3
	GtkAdjustment *sadj, *madj, *hadj;
#endif
#ifdef GTK2
	GtkObject *sadj, *madj, *hadj;
#endif
	gchar *markup;

	gtk_init(&argc, &argv);
	notify_init("Gtimer");
	ca_context_create(&sound);

	context = g_option_context_new("- a simple countdown timer");
	g_option_context_add_main_entries(context, entries, NULL);
	if (!g_option_context_parse (context, &argc, &argv, &error_parsearg)) {
		g_fprintf (stderr, "%s\n", error_parsearg->message);
		exit(1);
	}

	tray_icon = gtk_status_icon_new_from_icon_name("clocks");
	if(gtk_status_icon_is_embedded(tray_icon) == FALSE) { /*this doesn't add up. re-check this with a proper system tray*/
		gtk_status_icon_set_visible(tray_icon, FALSE);
		gtk_status_icon_set_title(tray_icon, "Gtimer");
		gtk_status_icon_set_name(tray_icon, "Gtimer");
	}

#ifdef GTK3
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#endif
#ifdef GTK2
	vbox = gtk_vbox_new(FALSE, 5);
	hbox1 = gtk_hbox_new(FALSE, 0);
	hbox2 = gtk_hbox_new(FALSE, 0);
#endif

	gtk_box_set_spacing(GTK_BOX(hbox2), 5);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);

	timer_display = gtk_label_new(NULL);
	markup = g_markup_printf_escaped("<span font=\"48\" weight=\"heavy\"><tt>%s</tt></span>", "HH:MM:SS");
	gtk_label_set_markup(GTK_LABEL(timer_display), markup);
	g_free (markup);

#ifdef GTK3
	sadj = gtk_adjustment_new(0, 0, 60, 1, 1, 1);
	madj = gtk_adjustment_new(0, 0, 60, 1, 1, 1);
	hadj = gtk_adjustment_new(0, 0, 24, 1, 1, 1);
	spin_seconds = gtk_spin_button_new(sadj, 1, 0);
	spin_minutes = gtk_spin_button_new(madj, 1, 0);
	spin_hours = gtk_spin_button_new(hadj, 24, 0);
#endif
#ifdef GTK2
	sadj = gtk_adjustment_new(0, 0, 60, 1, 1, 0);
	madj = gtk_adjustment_new(0, 0, 60, 1, 1, 0);
	hadj = gtk_adjustment_new(0, 0, 24, 1, 1, 0);
	spin_seconds = gtk_spin_button_new(GTK_ADJUSTMENT(sadj), 1, 0);
	spin_minutes = gtk_spin_button_new(GTK_ADJUSTMENT(madj), 1, 0);
	spin_hours = gtk_spin_button_new(GTK_ADJUSTMENT(hadj), 1, 0);
#endif

	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spin_seconds), TRUE);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_seconds), seconds);
	g_object_set (spin_seconds, "shadow-type", GTK_SHADOW_IN, NULL);
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spin_minutes), TRUE);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_minutes), minutes);
	g_object_set (spin_minutes, "shadow-type", GTK_SHADOW_IN, NULL);
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spin_hours), TRUE);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_hours), hours);
	g_object_set (spin_hours, "shadow-type", GTK_SHADOW_IN, NULL);

	entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry), entry_text);
	
	button_timer = gtk_button_new();
	button_timer_start(TRUE);
	button_reset = gtk_button_new_with_label("Reset");
	gtk_widget_set_sensitive(button_reset, FALSE);

	gtk_box_pack_start(GTK_BOX(hbox1), spin_hours, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox1), spin_minutes, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox1), spin_seconds, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), timer_display, FALSE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox2), button_timer, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox2), button_reset, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(vbox), hbox1);
	gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, TRUE, 5);
	gtk_container_add(GTK_CONTAINER(vbox), hbox2);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW(window), "Gtimer");
	gtk_window_set_default_icon_name("clocks");
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show_all(window);

	g_timeout_add_seconds(1, (GSourceFunc)timer_function, NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(button_timer, "clicked", G_CALLBACK(on_timer_button_clicked), NULL);
	g_signal_connect(button_reset, "clicked", G_CALLBACK(on_reset_button_clicked), NULL);

	gtk_main();
	notify_uninit();
	ca_context_destroy(sound);
	g_option_context_free(context); 
}
