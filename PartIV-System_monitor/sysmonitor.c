/*
 * HUST OS Design - Part IV
 * 
 * System Monitor - Implementation
 * 
 * Created by zxcpyp at 2018-08-21
 * 
 * Github: zxc479773533
 */

#include "sysmonitor.h"

/*
 * read_stat - Read data from /proc/pid/stat
 */
void read_stat(char (*info)[1024], char *stat_file) {
  /*
   * stat file format:
   * 
   * pid (name) status ppid ......(13 data) priority (4 data) memory
   */
  int pos;

  /* Get pid */
  for (pos = 0; pos < 1024; pos++) {
    if (stat_file[pos] == ' ')
      break;
  }
  stat_file[pos] = '\0';
  strcpy(info[0], stat_file);
  stat_file += pos;
  stat_file += 2;

  /* Get name */
  for (pos = 0; pos < 1024; pos++) {
    if (stat_file[pos] == ')')
      break;
  }
  stat_file[pos] = '\0';
  strcpy(info[1], stat_file);
  stat_file += pos;
  stat_file += 2;

  /* Get status */
  for (pos = 0; pos < 1024; pos++) {
    if (stat_file[pos] == ' ')
      break;
  }
  stat_file[pos] = '\0';
  strcpy(info[3], stat_file);
  stat_file += pos;
  stat_file += 1;

  /* Get ppid */
  for (pos = 0; pos < 1024; pos++) {
    if (stat_file[pos] == ' ')
      break;
  }
  stat_file[pos] = '\0';
  strcpy(info[2], stat_file);
  stat_file += pos;
  stat_file += 1;

  /* Get priority */
  int i;
  for (i = 0, pos = 0; pos < 1024; pos++) {
    if (stat_file[pos] == ' ')
      i++;
    if (i == 13)
      break;
  }
  stat_file[pos] = '\0';
  stat_file += pos;
  stat_file += 1;
  for (pos = 0; pos < 1024; pos++) {
    if (stat_file[pos] == ' ')
      break;
  }
  stat_file[pos] = '\0';
  strcpy(info[4], stat_file);
  stat_file += pos;
  stat_file += 1;

  /* Get memory use */
  for (i = 0, pos = 0; pos < 1024; pos++) {
    if (stat_file[pos] == ' ')
      i++;
    if (i == 4)
      break;
  }
  stat_file[pos] = '\0';
  stat_file += pos;
  stat_file += 1;
  for (pos = 0; pos < 1024; pos++) {
    if (stat_file[pos] == ' ')
      break;
  }
  stat_file[pos] = '\0';
  char buf[1024];
  sprintf(buf, "%d KB\0", abs(atoi(stat_file)) / 1024);
  strcpy(info[5], buf);
}

/*
 * get_process_info - Get process info in /proc and add into clist
 */
void get_process_info(void) {
  DIR *dir;
  struct dirent *dir_info;
  int fd;
  char pid_file[1024];
  char stat_file[1024];
  char info[6][1024];
  gchar *txt[6];

  /* Set clist column title */
  gtk_clist_set_column_title(GTK_CLIST(clist), 0, "PID");
  gtk_clist_set_column_title(GTK_CLIST(clist), 1, "Name");
  gtk_clist_set_column_title(GTK_CLIST(clist), 2, "PPID");
  gtk_clist_set_column_title(GTK_CLIST(clist), 3, "State");
  gtk_clist_set_column_title(GTK_CLIST(clist), 4, "Priority");
  gtk_clist_set_column_title(GTK_CLIST(clist), 5, "Memory use");

  /* Set clist column width */
  gtk_clist_set_column_width(GTK_CLIST(clist), 0, 75);
  gtk_clist_set_column_width(GTK_CLIST(clist), 1, 125);
  gtk_clist_set_column_width(GTK_CLIST(clist), 2, 75);
  gtk_clist_set_column_width(GTK_CLIST(clist), 3, 75);
  gtk_clist_set_column_width(GTK_CLIST(clist), 4, 75);
  gtk_clist_set_column_width(GTK_CLIST(clist), 5, 125);
  gtk_clist_column_titles_show(GTK_CLIST(clist));

  /* Read proc info */
  dir = opendir("/proc");
  process_num = 0;
  while (dir_info = readdir(dir)) {
    /*
     * If start with number, then read it
     */
    if ((dir_info->d_name)[0] >= '0' && ((dir_info->d_name)[0]) <= '9') {
      sprintf(pid_file, "/proc/%s/stat", dir_info->d_name);
      fd = open(pid_file, O_RDONLY);
      read(fd, stat_file, 1024);
      read_stat(info, stat_file);
      for (int i = 0; i < 6; i++)
        txt[i] = utf8_fix(info[i]);
      close(fd);
      gtk_clist_append(GTK_CLIST(clist), txt);
      process_num++;
    }
  }
  closedir(dir);
}

/*
 * get_cpu_info - get cpu information from /proc/cpuinfo
 */
void get_cpu_info(char *cpu_name,
                  char *cpu_addr_digit,
                  char *cpu_cache_size,
                  char *cpu_core) {
  /*
   * cpuinfo file format:
   * 
   * model name: line 5
   * cache size: line 9
   * cpu cores: line 13
   * address sizes: line 25
   */
  int fd;
  char info_buf[2048];  /* Here 1024 is too small */
  char info_str[1024];
  char *pos = NULL;
  int i;
  fd = open("/proc/cpuinfo", O_RDONLY);
  read(fd, info_buf, sizeof(info_buf));
  close(fd);

  /* Read CPU Name */
  i = 0;
  pos = strstr(info_buf, "model name");
  while (*pos != ':')
    pos++;
  pos += 2;

  while (*pos != '\n') {
    info_str[i] = *pos;
    i++;
    pos++;
  }
  info_str[i] = '\0';
  strcpy(cpu_name, info_str);

  /* Read Cache size */
  i = 0;
  pos = strstr(info_buf, "cache size");
  while (*pos != ':')
    pos++;
  pos += 2;
  while (*pos != '\n') {
    info_str[i] = *pos;
    i++;
    pos++;
  }
  info_str[i] = '\0';
  strcpy(cpu_cache_size, info_str);

  /* Read CPU Cores */
  i = 0;
  pos = strstr(info_buf, "cpu cores");
  while (*pos != ':')
    pos++;
  pos += 2;
  while (*pos != '\n') {
    info_str[i] = *pos;
    i++;
    pos++;
  }
  info_str[i] = '\0';
  strcpy(cpu_core, info_str);

  /* Read Addressing digital */
  i = 0;
  pos = strstr(info_buf, "address sizes");
  while (*pos != ':')
    pos++;
  pos += 2;
  while (*pos != '\n') {
    info_str[i] = *pos;
    i++;
    pos++;
  }
  info_str[i] = '\0';
  strcpy(cpu_addr_digit, info_str);
}


int main(int argc, char **argv) {
  gtk_init(&argc, &argv);
  /* GTK widgets */
  GtkWidget *top_window;
  GtkWidget *notebook;
  GtkWidget *hbox;
  GtkWidget *vbox;
  GtkWidget *label;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *scrolled_window;
  GtkWidget *frame;
  GtkWidget *cpu_use;
  GtkWidget *button1;
  GtkWidget *button2;
  GtkWidget *button3;

  /* Save page title */
  char title_buf[1024];

  /* Buffer to use */
  char buffer1[1024];
  char cpu_name[1024];
  char cpu_addr_digit[1024];
  char cpu_cache_size[1024];
  char cpu_core[1024];

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


  /*
   * Page 1: Process manage
   */
  sprintf(title_buf, "Process");
  vbox = gtk_vbox_new(FALSE, 0);

  /* Create scrolled window for process info */
  scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_set_size_request(scrolled_window, 550, 500);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  /* Create list with 6 columus */
  clist = gtk_clist_new(6);
  get_process_info();
  gtk_signal_connect(GTK_OBJECT(clist), "select_row", GTK_SIGNAL_FUNC(select_row_callback), NULL);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), clist);
  gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 5);

  /* Create buttons */
  hbox = gtk_hbox_new(FALSE, 10);
  entry = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(entry), 0);
  button1 = gtk_button_new_with_label("Search by PID");
  button2 = gtk_button_new_with_label("Kill");
  button3 = gtk_button_new_with_label("Refresh");
  g_signal_connect(G_OBJECT(button1), "clicked", G_CALLBACK(search_proc), scrolled_window);
  g_signal_connect(G_OBJECT(button2), "clicked", G_CALLBACK(kill_proc), NULL);
  g_signal_connect(G_OBJECT(button3), "clicked", G_CALLBACK(refresh_proc), NULL);
  gtk_widget_set_size_request(entry, 200, 30);
  gtk_widget_set_size_request(button1, 120, 30);
  gtk_widget_set_size_request(button2, 80, 30);
  gtk_widget_set_size_request(button3, 80, 30);
  gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(hbox), button1, FALSE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(hbox), button2, FALSE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(hbox), button3, FALSE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
  label = gtk_label_new(title_buf);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);


  /*
   * Page 2: CPU info
   */
  sprintf(title_buf, "CPU");
  vbox = gtk_vbox_new(FALSE, 0);

  /* Creat frame to show curve of CPU use */
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 5);
  cpu_use = gtk_frame_new("CPU Use");
  gtk_container_set_border_width(GTK_CONTAINER(cpu_use), 5);
  gtk_widget_set_size_request(cpu_use, 520, 300);
  gtk_box_pack_start(GTK_BOX(hbox), cpu_use, TRUE, FALSE, 5);

  hbox = gtk_hbox_new(FALSE, 0);
  label1 = gtk_label_new("Test 1");
  label2 = gtk_label_new("Test 2");
  gtk_box_pack_start(GTK_BOX(hbox), label1, TRUE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(hbox), label2, TRUE, FALSE, 5);
  g_timeout_add(1000, (GtkFunction)get_cpu_ratio, (gpointer)label1);
  g_timeout_add(1000, (GtkFunction)get_cpu_mhz, (gpointer)label2);
  gtk_widget_set_size_request(hbox, 550, 30);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 5);

  /* Draw CPU use curve */
  cpu_curve = gtk_drawing_area_new();
  gtk_widget_set_size_request(cpu_curve, 0, 0);
  g_signal_connect(G_OBJECT(cpu_curve), "expose_event", G_CALLBACK(cpu_curve_callback), NULL);
  gtk_container_add(GTK_CONTAINER(cpu_use), cpu_curve);

  /* Creat frame to show CPU info */
  frame = gtk_frame_new("CPU Information");
  gtk_widget_set_size_request(frame, 500, 200);
  gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 5);
  get_cpu_info(cpu_name, cpu_addr_digit, cpu_cache_size, cpu_core);
  sprintf(buffer1, "CPU Type and Frequency :\n%s\n\nAddressing digit : %s\n\nCache size : %s\n\nCPU Core : %s\n",
                    cpu_name, cpu_addr_digit, cpu_cache_size, cpu_core);
  label = gtk_label_new(buffer1);
  PangoFontDescription *desc_info = pango_font_description_from_string("12");
  gtk_widget_modify_font(label, desc_info);
  pango_font_description_free(desc_info);
  gtk_container_add(GTK_CONTAINER(frame), label);

  label = gtk_label_new(title_buf);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);


  /*
   * Page 3: Memory info
   */
  sprintf(title_buf, "Memory");
  vbox = gtk_vbox_new(FALSE, 0);
  button1 = gtk_button_new_with_label("Page 3");
  gtk_container_add(GTK_CONTAINER(vbox), button1);
  label = gtk_label_new(title_buf);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);


  /*
   * Page 4: Network
   */
  sprintf(title_buf, "Network");
  vbox = gtk_vbox_new(FALSE, 0);
  button1 = gtk_button_new_with_label("Page 4");
  gtk_container_add(GTK_CONTAINER(vbox), button1);
  label = gtk_label_new(title_buf);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);


  /*
   * Page 5: System info
   */
  sprintf(title_buf, "System");
  vbox = gtk_vbox_new(FALSE, 0);
  button1 = gtk_button_new_with_label("Page 5");
  gtk_container_add(GTK_CONTAINER(vbox), button1);
  label = gtk_label_new(title_buf);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);

  /* GTK show */
  gtk_widget_show_all(top_window);
  gtk_main();

  return 0;
}


/********************LOOPS********************/


/*
 * select_row_callback - Callback for select_row
 */
void select_row_callback(GtkWidget *clist, gint row, gint column, GdkEventButton *event, gpointer data) {
	gtk_clist_get_text(GTK_CLIST(clist), row, 0, &now_pid);
  gtk_entry_set_text(GTK_ENTRY(entry), (gchar *)now_pid);
  return;
}

/*
 * select_row_callback - Search process in GTK clist
 */
void search_proc(GtkButton *button, gpointer data) {
  const gchar *entry_txt;
  gchar *txt;
  gint ret, i = 0;
  entry_txt = gtk_entry_get_text(GTK_ENTRY(entry));
  while ((ret = gtk_clist_get_text(GTK_CLIST(clist), i, 0, &txt)) != 0) {
    if (!strcmp(entry_txt, txt))
      break;
    i++;
  }
  gtk_clist_select_row(GTK_CLIST(clist), i, 0);
  scroll_to_line(data, process_num, i);
  return;
}

/*
 * select_row_callback - Kill process clicked
 */
void kill_proc(void) {
  char buf[20];
  if (now_pid != NULL) {
    sprintf(buf, "kill -9 %s", now_pid);
    system(buf);
  }
  return;
}

/*
 * refresh_proc - Refresh the process info in clist
 */
void refresh_proc(void) {
  gtk_clist_freeze(GTK_CLIST(clist));
  gtk_clist_clear(GTK_CLIST(clist));
  get_process_info();
  gtk_clist_thaw(GTK_CLIST(clist));
  return;
}

/*
 * cpu_curve_callback - Refresh the CPU use curve once every second
 */
gboolean cpu_curve_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
  draw_cpu_curve((gpointer)widget);
  g_timeout_add(1000, (GtkFunction)draw_cpu_curve, (gpointer)widget);
  return TRUE;
}


/********************CALLBACKS********************/

/*
 * draw_cpu_curve - Draw CPU curve
 */
gboolean draw_cpu_curve(gpointer widget) {
  GtkWidget *cpu_curve = (GtkWidget *)widget;
  GdkColor color;
  GdkGC *gc = cpu_curve->style->fg_gc[GTK_WIDGET_STATE(widget)];
  static int flag = 0;
  static int now_pos = 0;
  int draw_pos = 0;

  /* Darw background */
  color.red = 0;
  color.green = 0;
  color.blue = 0;
  gdk_gc_set_rgb_fg_color(gc, &color);
  gdk_draw_rectangle(cpu_curve->window, gc, TRUE, 20, 30, 480, 200);

  /* Draw background lines */
  color.red = 0;
  color.green = 20000;
  color.blue = 0;
  gdk_gc_set_rgb_fg_color(gc, &color);
  for (int i = 30; i <= 220; i += 20)
    gdk_draw_line(cpu_curve->window, gc, 20, i, 500, i);
  for (int i = 20; i <= 500; i += 20)
    gdk_draw_line(cpu_curve->window, gc, i + cpu_curve_start, 30, i + cpu_curve_start, 230);
  
  /* Settle cpu curve start position to make it live */
  cpu_curve_start -= 4;
  if (cpu_curve_start == 0)
    cpu_curve_start = 20;

  /* Initial data */
  if (flag == 0) {
    for (int i = 0; i < 120; i++) {
      cpu_ratio_data[i] = 0;
      flag = 1;
    }
  }

  /* Add data */
  cpu_ratio_data[now_pos] = cpu_ratio / 100;
  now_pos++;
  if (now_pos == 120)
    now_pos = 0;
    
  /* Draw lines */
  color.red = 0;
	color.green = 65535;
	color.blue = 0;
	gdk_gc_set_rgb_fg_color(gc, &color);
  draw_pos = now_pos;
  for (int i = 0; i < 119; i++) {
    gdk_draw_line(cpu_curve->window, gc,
                  20 + i * 4, 230 - 170 * cpu_ratio_data[draw_pos % 120],
                  20 + (i + 1) * 4, 230 - 170 * cpu_ratio_data[(draw_pos + 1) % 120]);
    draw_pos++;
    if (draw_pos == 120)
      draw_pos = 0;
  }

  /* Reset the color */
  color.red = 0;
	color.green = 0;
	color.blue = 0;
	gdk_gc_set_rgb_fg_color(gc, &color);

  /* To loop this function, it must return TRUE */
  return TRUE;
}

/*
 * get_cpu_ratio - Get CPU use ratio from /proc/stat
 */
gboolean get_cpu_ratio(gpointer label) {
  /*
   * stat file format
   * 
   * cpu user nice system idle iowait
   * 
   * t1, t2: Two near moments
   * cpu(total) = user+nice+system+idle+iowait
   * pcpu = 100 *(total – idle) / total
   * total =total(t2) – total(t1)
   * idle =idle(t2) – idle(t1)
   */
  /* Data holder */
  static long old_idle, old_total;
  static int flag = 0;

  long user, nice, system, idle, iowait, total;
  long total_diff, idle_diff;
  char cpu[10], buffer[256], cpu_ratio_char[256];
  int fd;
  fd = open("/proc/stat", O_RDONLY);
  read(fd, buffer, sizeof(buffer));
  close(fd);
  sscanf(buffer, "%s %ld %ld %ld %ld %ld", cpu, &user, &nice, &system, &idle, &iowait);

  /* First */
  if (flag == 0) {
    flag = 1;
    old_idle = idle;
    old_total = user + nice + system + idle + iowait;
    cpu_ratio = 0;
  }
  /* Others */
  else {
    total = user + nice + system + idle + iowait;
    total_diff = total - old_total;
    idle_diff = idle - old_idle;
    cpu_ratio = 100 * (total_diff - idle_diff) / total_diff;
    total = old_total;
    idle = old_idle;
  }
  sprintf(cpu_ratio_char, "CPU usage: %0.1f%%", cpu_ratio);
  gtk_label_set_text(GTK_LABEL(label), cpu_ratio_char);
  return TRUE;
}

gboolean get_cpu_mhz(gpointer label) {
  int fd;
  char info_buf[1024];
  char info_str[1024];
  char cpu_freq_char[256];
  char *pos = NULL;
  int i;
  fd = open("/proc/cpuinfo", O_RDONLY);
  read(fd, info_buf, sizeof(info_buf));
  close(fd);

  /* Read CPU mhz */
  i = 0;
  pos = strstr(info_buf, "cpu MHz");
  while (*pos != ':')
    pos++;
  pos += 2;

  while (*pos != '\n') {
    info_str[i] = *pos;
    i++;
    pos++;
  }
  info_str[i] = '\0';
  strcpy(cpu_freq, info_str);
  sprintf(cpu_freq_char, "CPU frequency: %s MHz", cpu_freq);
  gtk_label_set_text(GTK_LABEL(label), cpu_freq_char);
  return TRUE;
}

/********************ASSISTS********************/


/*
 * utf8_fix - To settle waring: Invalid UTF-8 string passed to pango_layout_set_text()
 * 
 * Referencing from: https://stackoverflow.com/questions/43753260/pango-warning-invalid-utf-8-string-passed-to-pango-layout-set-text-in-gtk
 */
char* utf8_fix(char *c) {
  return g_locale_to_utf8(c, -1, NULL, NULL, NULL);
}

/*
 * scroll_to_line - To set the scroll window position
 * 
 * Modified from: https://my.oschina.net/plumsoft/blog/79950
 */
void scroll_to_line(gpointer scrolled_window, gint line_num, gint to_line_index) {
  GtkAdjustment *adj;
  gdouble lower_value, upper_value, page_size, max_value, line_height, to_value;
  adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
  lower_value = gtk_adjustment_get_lower(adj);
  upper_value = gtk_adjustment_get_upper(adj);
  page_size = gtk_adjustment_get_page_size(adj);
  max_value = upper_value - page_size;
  line_height = upper_value / line_num;
  to_value = line_height * to_line_index;
  if (to_value < lower_value)
    to_value = lower_value;
  if (to_value > max_value)
    to_value = max_value;
  gtk_adjustment_set_value(adj, to_value);
  return;
}