/*
 * HUST OS Design - Part I
 * 
 * Linux Basics - Fork demo using GTK+ 3.0
 * 
 * Created by zxcpyp at 2018-07-17
 * 
 * Github: zxc479773533
 */

#include "../zxcpyplib/zxcpyp_sys.h"

#include <wait.h>
#include <gtk/gtk.h>

#define WINDOW_WIDTH 450
#define WINDOW_HEIGHT 300
#define RE_FEQ 250

/*
 * refresh_time - Refresh the time in window1
 */
gboolean refresh_time(gpointer label) {
  time_t times;
  struct tm *time_buf;
  time(&times);
  time_buf = localtime(&times);

  /* Get now time */
  gchar *text_day = g_strdup_printf("<span font_desc='48'>%04d-%02d-%02d</span>", \
    1900 + time_buf->tm_year, 1 + time_buf->tm_mon, time_buf->tm_mday);
  gchar *text_time = g_strdup_printf("<span font_desc='32'>%02d:%02d:%02d</span>", \
    time_buf->tm_hour, time_buf->tm_min, time_buf->tm_sec);
  gchar *text_data = g_strdup_printf("\n%s\n\n%s\n", text_day, text_time);

  gtk_label_set_markup(GTK_LABEL(label), text_data);
  return TRUE;
}

/*
 * refresh_num - Refresh the num in window2
 */
gboolean refresh_num(gpointer label) {
  static int num = 0;
  gchar *text_num = g_strdup_printf("<span font_desc='128'>%d</span>", num++);
  if (num == 10)
    num = 0;
  gtk_label_set_markup(GTK_LABEL(label), text_num);
  return TRUE;
}

/*
 * refresh_sum - Refresh the num in window3
 */
gboolean refresh_sum(gpointer label) {
  static int sum = 0;
  static int add = 1;

  gchar *text_old = g_strdup_printf("<span font_desc='48'>%d+%d=</span>", sum, add);
  sum += add++;
  gchar *text_new = g_strdup_printf("<span font_desc='32'>%d</span>", sum);
  if (add == 1000) {
    sum = 0;
    add = 1;
  }

  gchar *text_data = g_strdup_printf("\n%s\n\n%s\n", text_old, text_new);
  gtk_label_set_markup(GTK_LABEL(label), text_data);
  return TRUE;
}

int main(int argc, char **argv) {
  int pid1, pid2;
  int wait_tmp;

  switch(pid1 = fork()) {
  
  case -1:
    err_exit("Fork child 1");

  /* Child pid 1 - Show time */
  case 0:
    gtk_init(&argc, &argv);
    GtkWidget *window1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window1), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_title(GTK_WINDOW(window1), "Window1: Show Time");

    /* Window1 inside */
    GtkWidget *label1 = gtk_label_new(NULL);
    gtk_container_add(GTK_CONTAINER(window1), label1);
    /*
     * g_timeout_add - Sets a function to be called at regular intervals,
     * with the default priority, G_PRIORITY_DEFAULT.
     * 
     * 1000: the time between calls to the function, in milliseconds
     */
    gint s1 = g_timeout_add(RE_FEQ, refresh_time, (void *)label1);

    gtk_widget_set_size_request(window1, WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_widget_show_all(window1);
    gtk_main();
    printf("Window 1 Closed!\n");
    exit(0);

  default:
    switch(pid2 = fork()) {
    
    case -1:
      err_exit("Fork child 2");
    
    /* Child pid 2 - Show 0 to 9 */
    case 0:
      gtk_init(&argc, &argv);
      GtkWidget *window2 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      g_signal_connect(G_OBJECT(window2), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
      gtk_window_set_title(GTK_WINDOW(window2), "Window2: Show 0 ~ 9");

      /* Window2 inside */
      GtkWidget *label2 = gtk_label_new(NULL);
      gtk_container_add(GTK_CONTAINER(window2), label2);
      gint s2 = g_timeout_add(RE_FEQ, refresh_num, (void *)label2);

      gtk_widget_set_size_request(window2, WINDOW_WIDTH, WINDOW_HEIGHT);
      gtk_widget_show_all(window2);
      gtk_main();
      printf("Window 2 Closed!\n");
      exit(0);

    /* Parent pid - Add 1 to 1000 */
    default:
      gtk_init(&argc, &argv);
      GtkWidget *window3 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      g_signal_connect(G_OBJECT(window3), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
      gtk_window_set_title(GTK_WINDOW(window3), "Window3: Show Add 1 to 1000");

      /* Window3 inside */
      GtkWidget *label3 = gtk_label_new(NULL);
      gtk_container_add(GTK_CONTAINER(window3), label3);
      gint s3 = g_timeout_add(RE_FEQ, refresh_sum, (void *)label3);

      gtk_widget_set_size_request(window3, WINDOW_WIDTH, WINDOW_HEIGHT);
      gtk_widget_show_all(window3);
      gtk_main();
      /* Wait for child pid */
      printf("Window 3 Closed!\n");
      waitpid(pid1, &wait_tmp, 0);
      waitpid(pid1, &wait_tmp, 0);
      exit(0);
    }
  }
}