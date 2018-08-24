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
GtkWidget *popup_window;
GtkWidget *popup_label;
char *now_pid = NULL;       /* Hold the pid clicked by mouse */
gint process_num = 0;       /* Hold the Process num */
GtkWidget *clist;           /* Hold the Process info */
GtkWidget *clist2;          /* Hold the Modules info */
GtkWidget *entry;           /* Hold the input */
float cpu_ratio = 0;        /* Hold the CPU use ratio */
float cpu_ratio_data[120];  /* Hold the CPU use ratio histories */
char cpu_freq[1024];        /* Hold the CPU frequence */
float mem_total = 0;        /* Hold the Memory total size */
float mem_free = 0;         /* Hold the Memory free size */
float mem_ratio = 0;        /* Hold the Memory use ratio */
float mem_ratio_data[120];  /* Hold the Memory use ratio histories */
float swap_total = 0;       /* Hold the Swap total size */
float swap_free = 0;        /* Hold the Swap free size */
float swap_ratio = 0;       /* Hold the Swap use ratio */
float swap_ratio_data[120]; /* Hold the Swap use ratio histories */
GtkWidget *cpu_curve;       /* Hold the CPU use curve */
GtkWidget *mem_curve;       /* Hold the Memory use curve */
GtkWidget *swap_curve;      /* Hold the Swap use curve */
int cpu_curve_start = 20;   /* Hold the CPU use curve start position */
int mem_curve_start = 20;   /* Hold the CPU use curve start position */
int swap_curve_start = 20;  /* Hold the CPU use curve start position */


/* Callback functions */
void select_row_callback(GtkWidget *clist, gint row, gint column, GdkEventButton *event, gpointer data);
void search_proc(GtkButton *button,gpointer data);
void kill_proc(void);
void refresh_proc(void);
gboolean cpu_curve_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data);
gboolean mem_curve_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data);
gboolean swap_curve_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data);
void refresh_modules(void);

/* Loop functions */
gboolean draw_cpu_curve(gpointer widget);
gboolean draw_mem_curve(gpointer widget);
gboolean draw_swap_curve(gpointer widget);
gboolean get_cpu_ratio(gpointer label);
gboolean get_cpu_mhz(gpointer label);
gboolean get_memory_ratio(gpointer label);
gboolean get_memory_fraction(gpointer label);
gboolean get_swap_ratio(gpointer label);
gboolean get_swap_fraction(gpointer label);
gboolean get_sys_info(gpointer label);

/* Assist functions */
char *utf8_fix(char *c);
void scroll_to_line(gpointer scrolled_window, gint line_num, gint to_line_index);