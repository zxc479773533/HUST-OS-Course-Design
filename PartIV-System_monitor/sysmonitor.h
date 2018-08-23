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
char *now_pid = NULL;       /* Hold the pid clicked by mouse */
gint process_num = 0;       /* Hold the process num */
GtkWidget *clist;           /* Hold the process info */
GtkWidget *entry;           /* Hold the input */
float cpu_ratio;            /* Hold the CPU use ratio */
float cpu_ratio_data[120];  /* Hold the CPU use ratio histories */
char cpu_freq[1024];        /* Hold the CPU frequence */
GtkWidget *cpu_curve;       /* Hold the CPU use curve */
int cpu_curve_start = 20;   /* Hold the CPU use curve start position */


/* Callback functions */
void select_row_callback(GtkWidget *clist, gint row, gint column, GdkEventButton *event, gpointer data);
void search_proc(GtkButton *button,gpointer data);
void kill_proc(void);
void refresh_proc(void);
gboolean cpu_curve_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data);

/* Loop functions */
gboolean draw_cpu_curve(gpointer widget);
gboolean get_cpu_ratio(gpointer label);
gboolean get_cpu_mhz(gpointer label);

/* Assist functions */
char *utf8_fix(char *c);
void scroll_to_line(gpointer scrolled_window, gint line_num, gint to_line_index);