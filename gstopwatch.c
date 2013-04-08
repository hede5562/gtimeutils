/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * gstopwatch
 * Copyright (C) 2013 Jente (jthidskes at outlook dot com)
 *
 * gstopwatch is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gstopwatch is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>

enum {
    STARTED,
    PAUSED,
    STOPPED
};

enum {
    N_LAP,
    /*SPLIT,*/
    TIME,
    N_COLUMNS
};

GTimer *timer;
gchar output[100];
gint state = STOPPED, lap = 0, hours, minutes; /*split_hours, split_minutes, prev_hours, prev_minutes;*/
gdouble seconds; /*split_seconds, prev_seconds;*/
GtkWidget *timer_display, *button_stopwatch, *button_funcs, *tree;
GtkListStore *liststore;
GtkTreeSelection *selection;
GtkTreeIter selection_iter, iter;

gboolean stopwatch (void) {
	gchar *markup;
	gulong gulong;

	seconds = g_timer_elapsed (timer, &gulong);
	hours = seconds / 3600;
	seconds -= 3600 * hours;
	minutes = seconds / 60;
	seconds -= 60 * minutes;
	sprintf(output, "%02d:%02d:%.2f", hours, minutes, seconds);

	gtk_label_set_text(GTK_LABEL(timer_display), output);
	markup = g_markup_printf_escaped("<span font=\"48\" weight=\"heavy\"><tt>%s</tt></span>", output);
	gtk_label_set_markup(GTK_LABEL(timer_display), markup);
	g_free (markup);
	return TRUE;
}

void add_lap (void) {
	/*gchar split[100];*/
	lap++;
	/*split_seconds = seconds - prev_seconds;
	prev_seconds = seconds;
	if(split_seconds > 60) {
		split_minutes = split_seconds / 60;
		prev_minutes = minutes;
	}
	sprintf(split, "%02d:%.2f", split_minutes, split_seconds);*/
	gtk_list_store_append(GTK_LIST_STORE(liststore), &iter);
	gtk_list_store_set(GTK_LIST_STORE(liststore), &iter, N_LAP, lap, /*SPLIT, split, */TIME, output, -1);
	gtk_tree_selection_select_iter(gtk_tree_view_get_selection(GTK_TREE_VIEW(tree)), &iter);
}

void on_timer_button_clicked (void) {
	if(state == STOPPED) {
		gtk_button_set_label(GTK_BUTTON(button_stopwatch), "Stop");
		gtk_widget_set_sensitive(GTK_WIDGET(button_funcs), TRUE);
		gtk_button_set_label(GTK_BUTTON(button_funcs), "Lap");
		g_timer_start(timer);
		state = STARTED;
	} else if(state == PAUSED) {
		gtk_button_set_label(GTK_BUTTON(button_stopwatch), "Stop");
		gtk_button_set_label(GTK_BUTTON(button_funcs), "Lap");
		g_timer_continue(timer);
		state = STARTED;
	} else if(state == STARTED) {
		gtk_button_set_label(GTK_BUTTON(button_stopwatch), "Continue");
		gtk_widget_set_sensitive(GTK_WIDGET(button_funcs), TRUE);
		gtk_button_set_label(GTK_BUTTON(button_funcs), "Reset");
		g_timer_stop(timer);
		state = PAUSED;
	}
}

void on_funcs_button_clicked (void) {
	if(state == STARTED)
		add_lap();
	else if(state == PAUSED) {
		g_timer_start(timer);
		g_timer_stop(timer);
		gtk_widget_set_sensitive(GTK_WIDGET(button_funcs), FALSE);
		gtk_button_set_label(GTK_BUTTON(button_funcs), "Reset");
		gtk_button_set_label(GTK_BUTTON(button_stopwatch), "Start");
		gtk_list_store_clear(GTK_LIST_STORE(liststore));
		state = STOPPED;
	}
}

int main (int argc, char *argv[]) {
	GtkWidget *window, *notebook, *label_stopwatch, *vbox_stopwatch, *hbox_stopwatch, *scroll, *label_timer, *vbox_timer, *button_timer;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;

	gtk_init(&argc, &argv);

	notebook = gtk_notebook_new();
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
	
	vbox_stopwatch = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	hbox_stopwatch = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	label_stopwatch = gtk_label_new("Stopwatch");
	timer_display = gtk_label_new("");
	button_stopwatch = gtk_button_new_with_label("Start");
	button_funcs = gtk_button_new_with_label("Reset");
	gtk_widget_set_sensitive(button_funcs, FALSE);
	scroll = gtk_scrolled_window_new (NULL, NULL);
	g_object_set (scroll, "shadow-type", GTK_SHADOW_IN, NULL);

	vbox_timer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	label_timer = gtk_label_new("Timer");
	button_timer = gtk_button_new_with_label("Start");

	tree = gtk_tree_view_new();
	liststore = gtk_list_store_new(N_COLUMNS, G_TYPE_INT, /*G_TYPE_STRING, */G_TYPE_STRING);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(tree), TRUE);
	gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(liststore));

	column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column, "Lap");

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer, "text", N_LAP, NULL);
	gtk_tree_view_column_set_expand(column, TRUE);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	/*renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Split", renderer, "text", SPLIT, NULL);
	gtk_tree_view_column_set_expand(column, TRUE);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);*/

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Time", renderer, "text", TIME, NULL);
	gtk_tree_view_column_set_expand(column, TRUE);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
	gtk_tree_view_set_enable_search(GTK_TREE_VIEW(tree), FALSE);

	gtk_box_pack_start(GTK_BOX(vbox_stopwatch), timer_display, FALSE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox_stopwatch), button_stopwatch, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox_stopwatch), button_funcs, TRUE, TRUE, 5);
	gtk_container_add(GTK_CONTAINER(vbox_stopwatch), hbox_stopwatch);
	gtk_container_add(GTK_CONTAINER(scroll), tree);
	gtk_box_pack_start(GTK_BOX(vbox_stopwatch), scroll, TRUE, TRUE, 5);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox_stopwatch, label_stopwatch);

	gtk_box_pack_start(GTK_BOX(vbox_timer), button_timer, TRUE, TRUE, 5);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox_timer, label_timer);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW(window), "Gstopwatch");
	gtk_window_set_default_icon_name("clocks");
	gtk_container_add(GTK_CONTAINER(window), notebook);
	gtk_widget_show_all(window);

	timer = g_timer_new();
	g_timer_stop(timer);

	g_timeout_add_full(G_PRIORITY_HIGH, 50, (GSourceFunc) stopwatch, NULL, NULL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(button_stopwatch, "clicked", G_CALLBACK(on_timer_button_clicked), NULL);
	g_signal_connect(button_funcs, "clicked", G_CALLBACK(on_funcs_button_clicked), NULL);

	gtk_main();
	g_timer_destroy(timer);
}
