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

enum {
    STARTED,
    PAUSED,
    STOPPED
};

GTimer *timer;
ca_context *c;
GdkColor color;
GtkAdjustment *sadj, *madj, *hadj;
gint elapsed, seconds, minutes, hours, state = STOPPED;
GtkWidget *button_timer, *button_reset, *spin_seconds, *spin_minutes, *spin_hours;

void button_timer_stop (void) {
	gdk_color_parse("#C73333", &color);
	gtk_widget_modify_fg(GTK_WIDGET(button_timer), GTK_STATE_NORMAL, &color);
	gtk_button_set_label(GTK_BUTTON(button_timer), "Stop");
}

void button_timer_start (gboolean start) {
	gdk_color_parse("#67953C", &color);
	gtk_widget_modify_fg(GTK_WIDGET(button_timer), GTK_STATE_NORMAL, &color);
	if(start)
		gtk_button_set_label(GTK_BUTTON(button_timer), "Start");
	else
		gtk_button_set_label(GTK_BUTTON(button_timer), "Continue");
}

void notify (void) {
	GError *error_notify = NULL;
	NotifyNotification *gtimer_notify;

	gtimer_notify = notify_notification_new("Time is up!", NULL, "clocks");
	notify_notification_set_category(gtimer_notify, "GTimeUtils");
	notify_notification_set_urgency(gtimer_notify, NOTIFY_URGENCY_NORMAL);
	notify_notification_show(gtimer_notify, &error_notify);
	if(error_notify)
		g_fprintf(stderr, "Can not initialize notification: %s\n", error_notify->message);

	ca_context_play(c, 0, CA_PROP_APPLICATION_NAME, "Gtimer", CA_PROP_EVENT_ID, "complete-copy", CA_PROP_MEDIA_ROLE, "notification",  CA_PROP_APPLICATION_ICON_NAME, "clocks", CA_PROP_CANBERRA_CACHE_CONTROL, "never", NULL);
}

void counter (void) {
	seconds = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_seconds));
	minutes = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_minutes));
	hours = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_hours));
	seconds += 60 * minutes;
	seconds += 3600 * hours;

	if(seconds == 0) {
		notify();
		button_timer_start(TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(button_reset), FALSE);
		state = STOPPED;
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
#ifdef DEBUG
		g_fprintf(stdout, "Seconds: %02d   Minutes: %02d   Hours: %02d\n", seconds, minutes, hours);
#endif
	}
}

gboolean timer_function (void) {
	if(state == STARTED)
		counter();
	return TRUE;
}

void on_timer_button_clicked (void) {
	if(state == STOPPED) {
		button_timer_stop();
		gtk_widget_set_sensitive(GTK_WIDGET(button_reset), TRUE);
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
	}
}

int main (void) {
	GtkWidget *window, *vbox, *hbox1, *hbox2;

	gtk_init(NULL, NULL);
	notify_init("Gtimer");
	ca_context_create(&c);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	sadj = gtk_adjustment_new(0, 0, 60, 1, 1, 1);
	madj = gtk_adjustment_new(0, 0, 60, 1, 1, 1);
	hadj = gtk_adjustment_new(0, 0, 24, 1, 1, 1);
	spin_seconds = gtk_spin_button_new(sadj, 1, 0);
	spin_minutes = gtk_spin_button_new(madj, 1, 0);
	spin_hours = gtk_spin_button_new(hadj, 24, 0);
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spin_seconds), TRUE);
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spin_minutes), TRUE);
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spin_hours), TRUE);

	button_timer = gtk_button_new();
	button_timer_start(TRUE);
	button_reset = gtk_button_new_with_label("Reset");
	gtk_widget_set_sensitive(GTK_WIDGET(button_reset), FALSE);

	gtk_box_pack_start(GTK_BOX(hbox1), spin_hours, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox1), spin_minutes, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox1), spin_seconds, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox2), button_timer, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox2), button_reset, TRUE, TRUE, 5);
	gtk_container_add(GTK_CONTAINER(vbox), hbox1);
	gtk_container_add(GTK_CONTAINER(vbox), hbox2);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW(window), "Gtimer");
	gtk_window_set_default_icon_name("clocks");
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show_all(window);

	g_timeout_add_seconds(1, (GSourceFunc) timer_function, NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(button_timer, "clicked", G_CALLBACK(on_timer_button_clicked), NULL);
	g_signal_connect(button_reset, "clicked", G_CALLBACK(on_reset_button_clicked), NULL);

	gtk_main();
	notify_uninit();
	ca_context_destroy(c);
}
