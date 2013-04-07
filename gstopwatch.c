#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include <glib/gi18n.h>

gchar output[100];
gint lap = 0;
GTimer *timer;
gboolean running;
GtkWidget *timer_display, *tree, *button_delete;
GtkListStore *liststore;
GtkTreeSelection *selection;
GtkTreeIter selection_iter, iter;

enum {
    N_LAP,
	TIME,
    N_COLUMNS
};

gboolean update_progress_bar (void) {
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

void on_list_selection_changed (void) {
	GtkTreeModel *model;

	if (gtk_tree_selection_get_selected(selection, &model, &selection_iter))
		gtk_widget_set_sensitive(button_delete, TRUE);
	else
		gtk_widget_set_sensitive(button_delete, FALSE);
}

void on_delete_button_clicked (void) {
	gtk_list_store_remove (GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (tree))), &selection_iter);
	lap--;
}

void on_lap_button_clicked (void) {
	GtkTreePath *path;

	lap++;
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, N_LAP, lap, TIME, output, -1);
	gtk_tree_selection_select_iter(gtk_tree_view_get_selection(GTK_TREE_VIEW(tree)), &iter);
	if(selection) {
		if(gtk_tree_selection_get_selected(selection, NULL, &iter) ) {
			path = gtk_tree_model_get_path(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)), &iter);
			gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(tree), path, NULL, FALSE, 0, 0);
			gtk_tree_path_free(path);
		}
	}
}

int main (int argc, char *argv[]) {
	GtkWidget *window, *vbox, *hbox, *expander, *ebox, *button_about, *button_lap;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;

	gtk_init(&argc, &argv);

	timer_display = gtk_label_new("");
	expander = gtk_expander_new("Laps");
	button_about = gtk_button_new_from_stock("gtk-about");
	button_lap = gtk_button_new_from_stock("gtk-save");
	button_delete = gtk_button_new_from_stock("gtk-delete");
	gtk_widget_set_sensitive(button_delete, FALSE);

	/* setup the tree view widget */
	tree = gtk_tree_view_new();
	liststore = gtk_list_store_new(N_COLUMNS, G_TYPE_INT, G_TYPE_STRING);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(tree), TRUE);
	gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(liststore));

	column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column, "Lap");

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer, "text", N_LAP, NULL);
	gtk_tree_view_column_set_sort_column_id(column, N_LAP);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Time", renderer, "text", TIME, NULL);
	gtk_tree_view_column_set_sort_column_id(column, TIME);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	ebox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_box_pack_start(GTK_BOX(vbox), timer_display, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(ebox), tree, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(ebox), button_delete, FALSE, FALSE, 5);
	gtk_container_add(GTK_CONTAINER(expander), ebox);
	gtk_box_pack_start(GTK_BOX(vbox), expander, TRUE, TRUE, 5);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);
	gtk_box_pack_start(GTK_BOX(hbox), button_about, FALSE, FALSE, 5);
	gtk_box_pack_end(GTK_BOX(hbox), button_lap, FALSE, FALSE, 5);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW(window), "Gstopwatch");
	gtk_window_set_default_icon_name("clocks");
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show_all(window);

	timer = g_timer_new();
	g_timer_stop(timer);

	g_timeout_add_full(G_PRIORITY_HIGH, 50, (GSourceFunc) update_progress_bar, NULL, NULL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(keypress), window);
	g_signal_connect (G_OBJECT (selection), "changed", G_CALLBACK (on_list_selection_changed), NULL);
	g_signal_connect(button_about, "clicked", G_CALLBACK(about_dialog_open), NULL);
	g_signal_connect(button_delete, "clicked", G_CALLBACK(on_delete_button_clicked), NULL);
	g_signal_connect(button_lap, "clicked", G_CALLBACK(on_lap_button_clicked), NULL);

	gtk_main();
	g_timer_destroy(timer);
}
