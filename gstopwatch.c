#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib.h>

gboolean running;
GtkWidget *window, *box, *timer_display;
GTimer *timer;

gboolean update_progress_bar (void) {
	gulong gulong;
	gdouble time_elapsed;
	char *output;

	time_elapsed = g_timer_elapsed (timer, &gulong);
	sprintf(output, "%.2f", time_elapsed);

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

int main (int argc, char *argv[]) {
	gtk_init (&argc, &argv);

	timer_display = gtk_label_new("");

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_box_pack_start(GTK_BOX (box), timer_display, FALSE, FALSE, 5);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_container_add (GTK_CONTAINER (window), box);
	gtk_widget_show_all (window);

	timer = g_timer_new ();
	g_timer_stop(timer);

	g_timeout_add_full(G_PRIORITY_HIGH, 50, (GSourceFunc) update_progress_bar, NULL, NULL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(keypress), window);

	gtk_main ();
}
