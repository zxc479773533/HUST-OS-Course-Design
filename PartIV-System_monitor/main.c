/*
 * HUST OS Design - Part IV
 * 
 * System Monitor - main
 * 
 * Created by zxcpyp at 2018-08-21
 * 
 * Github: zxc479773533
 */

#include <stdio.h>
#include <gtk/gtk.h>

int main(int argc, char **argv) {
  gtk_init(&argc, &argv);
  /* GTK widgets */
  GtkWidget *top_window;
  GtkWidget *notebook;
  GtkWidget *hbox;
  GtkWidget *vbox;
  GtkWidget *table;
  GtkWidget *label;
  GtkWidget *button1;

  /* Save page title */
  char title_buf[128];

  /* Create top window */
  top_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(top_window), "System monitor");
  gtk_widget_set_size_request(top_window, 600, 700);
  g_signal_connect(G_OBJECT(top_window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
  gtk_container_set_border_width(GTK_CONTAINER(top_window), 10);

  /* Create notebook */
  notebook = gtk_notebook_new();
  gtk_container_add(GTK_CONTAINER(top_window), notebook);
  gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);

  /* Page 1: Process manage */
  sprintf(title_buf, "Process");
  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  button1 = gtk_button_new_with_label("Page 1");
  gtk_container_add(GTK_CONTAINER(vbox), button1);
  label = gtk_label_new(title_buf);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);

  /* Page 2: CPU info */
  sprintf(title_buf, "CPU");
  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  button1 = gtk_button_new_with_label("Page 2");
  gtk_container_add(GTK_CONTAINER(vbox), button1);
  label = gtk_label_new(title_buf);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);

  /* Page 3: Memory info */
  sprintf(title_buf, "Memory");
  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  button1 = gtk_button_new_with_label("Page 3");
  gtk_container_add(GTK_CONTAINER(vbox), button1);
  label = gtk_label_new(title_buf);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);

  /* Page 4: Network */
  sprintf(title_buf, "Network");
  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  button1 = gtk_button_new_with_label("Page 4");
  gtk_container_add(GTK_CONTAINER(vbox), button1);
  label = gtk_label_new(title_buf);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);

  /* Page 5: System info */
  sprintf(title_buf, "System");
  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  button1 = gtk_button_new_with_label("Page 5");
  gtk_container_add(GTK_CONTAINER(vbox), button1);
  label = gtk_label_new(title_buf);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);

  /* GTK show */
  gtk_widget_show_all(top_window);
  gtk_main();
}