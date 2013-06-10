/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * gstopwatch.c
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
#include <glib/gi18n.h>

enum {
    STARTED,
    PAUSED,
    STOPPED
};

enum {
    N_LAP,
    TIME,
    N_COLUMNS
};

GTimer *stopwatch;
gchar output[100];
gint state = STOPPED, laps = 0, hours = 0, minutes = 0;
gdouble seconds = 0;
GtkWidget *stopwatch_display, *button_stopwatch, *button_funcs, *tree;
GtkListStore *liststore;
GtkTreeSelection *selection;
GtkTreeIter selection_iter, iter;
GdkColor color;

void counter (gboolean counting) {
	gchar *markup;

	if(counting) {
		hours = seconds / 3600;
		seconds -= 3600 * hours;
		minutes = seconds / 60;
		seconds -= 60 * minutes;
		sprintf(output, "%02d:%02d:%.2f", hours, minutes, seconds);
	} else
		sprintf(output, "00:00:0,00");

	gtk_label_set_text (GTK_LABEL (stopwatch_display), output);
	markup = g_markup_printf_escaped ("<span font=\"48\" weight=\"heavy\"><tt>%s</tt></span>", output);
	gtk_label_set_markup (GTK_LABEL (stopwatch_display), markup);
	g_free (markup);
}

gboolean stopwatch_function (void) {
	gulong gulong;

	if(state == STARTED) {
		seconds = g_timer_elapsed (stopwatch, &gulong);
		counter (TRUE);
	} else if(state == STOPPED)
		counter (FALSE);
	return TRUE;
}

void add_lap (void) {
	laps++;

	gtk_list_store_append (GTK_LIST_STORE (liststore), &iter);
	gtk_list_store_set (GTK_LIST_STORE (liststore), &iter, N_LAP, laps, TIME, output, -1);
	gtk_tree_selection_select_iter (gtk_tree_view_get_selection(GTK_TREE_VIEW(tree)), &iter);
}

void on_stopwatch_button_clicked (void) {
	if(state == STOPPED) {
		gdk_color_parse ("#C73333", &color);
		gtk_widget_modify_fg (GTK_WIDGET (button_stopwatch), GTK_STATE_NORMAL, &color);
		gtk_button_set_label (GTK_BUTTON (button_stopwatch), _("Stop"));
		gtk_widget_set_sensitive (GTK_WIDGET (button_funcs), TRUE);
		gtk_button_set_label (GTK_BUTTON (button_funcs), _("Lap"));
		g_timer_start (stopwatch);
		state = STARTED;
	} else if(state == PAUSED) {
		gdk_color_parse ("#C73333", &color);
		gtk_widget_modify_fg (GTK_WIDGET (button_stopwatch), GTK_STATE_NORMAL, &color);
		gtk_button_set_label (GTK_BUTTON (button_stopwatch), _("Stop"));
		gtk_button_set_label (GTK_BUTTON (button_funcs), _("Lap"));
		g_timer_continue (stopwatch);
		state = STARTED;
	} else if(state == STARTED) {
		gdk_color_parse ("#67953C", &color);
		gtk_widget_modify_fg (GTK_WIDGET (button_stopwatch), GTK_STATE_NORMAL, &color);
		gtk_button_set_label (GTK_BUTTON (button_stopwatch), _("Continue"));
		gtk_widget_set_sensitive (GTK_WIDGET (button_funcs), TRUE);
		gtk_button_set_label (GTK_BUTTON (button_funcs), _("Reset"));
		g_timer_stop (stopwatch);
		state = PAUSED;
	}
}

void on_funcs_button_clicked (void) {
	if(state == STARTED)
		add_lap();
	else if(state == PAUSED) {
		g_timer_start (stopwatch);
		g_timer_stop (stopwatch);
		laps = 0;
		state = STOPPED;
		gtk_widget_set_sensitive (GTK_WIDGET (button_funcs), FALSE);
		gtk_button_set_label (GTK_BUTTON (button_funcs), _("Reset"));
		gtk_button_set_label (GTK_BUTTON (button_stopwatch), _("Start"));
		gtk_list_store_clear (GTK_LIST_STORE (liststore));
	}
}

int main (void) {
 	GtkWidget *window, *vbox, *hbox, *scroll;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
	
	gtk_init (NULL, NULL);

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
	stopwatch_display = gtk_label_new ("");
	button_stopwatch = gtk_button_new_with_label (_("Start"));
	gdk_color_parse ("#67953C", &color);
	gtk_widget_modify_fg (GTK_WIDGET (button_stopwatch), GTK_STATE_NORMAL, &color);
	button_funcs = gtk_button_new_with_label (_("Reset"));
	gtk_widget_set_sensitive (button_funcs, FALSE);
	scroll = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll), GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	liststore = gtk_list_store_new (N_COLUMNS, G_TYPE_INT, G_TYPE_STRING);
	tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL (liststore));

	column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title (column, _("Lap"));

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start (column, renderer, TRUE);
	gtk_tree_view_column_set_attributes (column, renderer, "text", N_LAP, NULL);
	gtk_tree_view_column_set_expand (column, TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes (_("Time"), renderer, "text", TIME, NULL);
	gtk_tree_view_column_set_expand (column, TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
	gtk_tree_view_set_enable_search (GTK_TREE_VIEW (tree), FALSE);

	gtk_box_pack_start (GTK_BOX (vbox), stopwatch_display, FALSE, TRUE, 5);
	gtk_box_pack_start (GTK_BOX (hbox), button_stopwatch, TRUE, TRUE, 5);
	gtk_box_pack_start (GTK_BOX (hbox), button_funcs, TRUE, TRUE, 5);
	gtk_container_add (GTK_CONTAINER (vbox), hbox);
	gtk_container_add (GTK_CONTAINER (scroll), tree);
	gtk_box_pack_start (GTK_BOX (vbox), scroll, TRUE, TRUE, 0);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "Gstopwatch");
	gtk_window_set_default_icon_name ("clocks");
	gtk_container_add (GTK_CONTAINER (window), vbox);
	gtk_widget_show_all (window);

	stopwatch = g_timer_new();
	g_timer_stop (stopwatch);

	g_timeout_add_full (G_PRIORITY_HIGH, 50, (GSourceFunc) stopwatch_function, NULL, NULL);
	g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect (button_stopwatch, "clicked", G_CALLBACK (on_stopwatch_button_clicked), NULL);
	g_signal_connect (button_funcs, "clicked", G_CALLBACK (on_funcs_button_clicked), NULL);

	gtk_main();
	g_timer_destroy (stopwatch);
	return 0;
}

