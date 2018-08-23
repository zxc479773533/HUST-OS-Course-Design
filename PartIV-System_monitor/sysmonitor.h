/*
 * HUST OS Design - Part IV
 * 
 * System Monitor - Header
 * 
 * Created by zxcpyp at 2018-08-21
 * 
 * Github: zxc479773533
 */

#include "../lib/zxcpyp_sys.h"
#include <dirent.h>
#include <gtk/gtk.h>

/* Global variables */
char *now_pid = NULL;     /* Hold the pid clicked by mouse */
gint process_num = 0;    /* Hold the process num */
GtkWidget *clist;
GtkWidget *entry;

/* Callback functions */
void select_row_callback(GtkWidget *clist, gint row, gint column, GdkEventButton *event, gpointer data);
void search_proc(GtkButton *button,gpointer data);
void kill_proc(void);
void refresh_proc(void);

/* Assist functions */
void scroll_to_line(gpointer scrolled_window, gint line_num, gint to_line_index);