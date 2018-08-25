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
  char *one_file = NULL;
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
      close(fd);
      one_file = stat_file;
      read_stat(info, one_file);
      for (int i = 0; i < 6; i++)
        txt[i] = utf8_fix(info[i]);
      gtk_clist_append(GTK_CLIST(clist), txt);
      process_num++;
    }
  }
  closedir(dir);
}

void get_modules_info(void) {
  FILE *fp;
  char modules_info[1024];
  char *line = NULL;
  char info[3][1024];
  gchar *txt[3];
  int pos = 0;

  /* Set clist column title */
  gtk_clist_set_column_title(GTK_CLIST(clist2), 0, "Module name");
  gtk_clist_set_column_title(GTK_CLIST(clist2), 1, "Memory use");
  gtk_clist_set_column_title(GTK_CLIST(clist2), 2, "Used Times");

  /* Set clist column width */
  gtk_clist_set_column_width(GTK_CLIST(clist2), 0, 250);
  gtk_clist_set_column_width(GTK_CLIST(clist2), 1, 150);
  gtk_clist_set_column_width(GTK_CLIST(clist2), 2, 150);
  gtk_clist_column_titles_show(GTK_CLIST(clist2));

  fp = fopen("/proc/modules", "r");

  while ((line = fgets(modules_info, sizeof(modules_info), fp)) != NULL) {
    /* Read modules name */
    for (pos = 0; pos < 1024; pos++) {
      if (line[pos] == ' ')
        break;
    }
    line[pos] = '\0';
    strcpy(info[0], line);
    pos++;
    line += pos;

    /* Read modules memory use */
    for (pos = 0; pos < 1024; pos++) {
      if (line[pos] == ' ')
        break;
    }
    line[pos] = '\0';
    strcpy(info[1], line);
    pos++;
    line += pos;
    
    /* Read modules name */
    for (pos = 0; pos < 1024; pos++) {
      if (line[pos] == ' ')
        break;
    }
    line[pos] = '\0';
    strcpy(info[2], line);

    for (int i = 0; i < 3; i++)
      txt[i] = utf8_fix(info[i]);
    
    gtk_clist_append(GTK_CLIST(clist2), txt);
  }
  fclose(fp);
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
  GtkWidget *label3;
  GtkWidget *scrolled_window;
  GtkWidget *frame;
  GtkWidget *cpu_use;
  GtkWidget *mem_use;
  GtkWidget *swap_use;
  GtkWidget *button1;
  GtkWidget *button2;
  GtkWidget *button3;
  GtkWidget *fixed;
  GtkWidget *image;

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
  label1 = gtk_label_new("");
  label2 = gtk_label_new("");
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
  set_label_fontsize(label, "12");
  gtk_container_add(GTK_CONTAINER(frame), label);

  label = gtk_label_new(title_buf);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);


  /*
   * Page 3: Memory info
   */
  sprintf(title_buf, "Memory");
  vbox = gtk_vbox_new(FALSE, 0);

  /* Create frame to show curve of memory use */
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 5);
  mem_use = gtk_frame_new("Memory Use");
  gtk_container_set_border_width(GTK_CONTAINER(mem_use), 5);
  gtk_widget_set_size_request(mem_use, 520, 250);
  gtk_box_pack_start(GTK_BOX(hbox), mem_use, TRUE, FALSE, 5);

  hbox = gtk_hbox_new(FALSE, 0);
  label1 = gtk_label_new("");
  label2 = gtk_label_new("");
  gtk_box_pack_start(GTK_BOX(hbox), label1, TRUE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(hbox), label2, TRUE, FALSE, 5);
  g_timeout_add(1000, (GtkFunction)get_memory_ratio, (gpointer)label1);
  g_timeout_add(1000, (GtkFunction)get_memory_fraction, (gpointer)label2);
  gtk_widget_set_size_request(hbox, 550, 20);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 5);

  /* Draw Memory use curve */
  mem_curve = gtk_drawing_area_new();
  gtk_widget_set_size_request(mem_curve, 0, 0);
  g_signal_connect(G_OBJECT(mem_curve), "expose_event", G_CALLBACK(mem_curve_callback), NULL);
  gtk_container_add(GTK_CONTAINER(mem_use), mem_curve);

  /* Create frame to show curve of swap use */
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 5);
  swap_use = gtk_frame_new("Swap Use");
  gtk_container_set_border_width(GTK_CONTAINER(swap_use), 5);
  gtk_widget_set_size_request(swap_use, 520, 250);
  gtk_box_pack_start(GTK_BOX(hbox), swap_use, TRUE, FALSE, 5);

  hbox = gtk_hbox_new(FALSE, 0);
  label1 = gtk_label_new("");
  label2 = gtk_label_new("");
  gtk_box_pack_start(GTK_BOX(hbox), label1, TRUE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(hbox), label2, TRUE, FALSE, 5);
  g_timeout_add(1000, (GtkFunction)get_swap_ratio, (gpointer)label1);
  g_timeout_add(1000, (GtkFunction)get_swap_fraction, (gpointer)label2);
  gtk_widget_set_size_request(hbox, 550, 20);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, FALSE, 5);

  /* Draw Swap use curve */
  swap_curve = gtk_drawing_area_new();
  gtk_widget_set_size_request(swap_curve, 0, 0);
  g_signal_connect(G_OBJECT(swap_curve), "expose_event", G_CALLBACK(swap_curve_callback), NULL);
  gtk_container_add(GTK_CONTAINER(swap_use), swap_curve);

  label = gtk_label_new(title_buf);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);


  /*
   * Page 4: Modules
   */
  sprintf(title_buf, "Modules");
  vbox = gtk_vbox_new(FALSE, 0);
  
  /* Create scrolled window for modules info */
  scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_set_size_request(scrolled_window, 550, 500);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  /* Create list with 3 columus */
  clist2 = gtk_clist_new(3);
  get_modules_info();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), clist2);
  gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 5);

  /* Create buttons */
  fixed = gtk_fixed_new();
  button1 = gtk_button_new_with_label("Refresh");
  g_signal_connect(G_OBJECT(button1), "clicked", G_CALLBACK(refresh_modules), NULL);
  gtk_widget_set_size_request(button1, 80, 30);
  gtk_fixed_put(GTK_FIXED(fixed), button1, 450, 0);
  gtk_box_pack_start(GTK_BOX(vbox), fixed, FALSE, FALSE, 5);

  label = gtk_label_new(title_buf);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);


  /*
   * Page 5: System info
   */
  sprintf(title_buf, "System");
  vbox = gtk_vbox_new(FALSE, 10);

  frame = gtk_frame_new("System Information");
  label1 = gtk_label_new("");
  gtk_container_add(GTK_CONTAINER(frame), label1);
  g_timeout_add(1000, (GtkFunction)get_sys_info, (gpointer)label1);
  gtk_widget_set_size_request(frame, 550, 300);
  gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, FALSE, 5);

  frame = gtk_frame_new("Network");
  label2 = gtk_label_new("");
  gtk_container_add(GTK_CONTAINER(frame), label2);
  g_timeout_add(2000, (GtkFunction)get_network_info, (gpointer)label2);
  gtk_widget_set_size_request(frame, 550, 180);
  gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, FALSE, 0);

  frame = gtk_frame_new("Disk");
  label3 = gtk_label_new("");
  gtk_container_add(GTK_CONTAINER(frame), label3);
  g_timeout_add(2000, (GtkFunction)get_disk_info, (gpointer)label3);
  gtk_widget_set_size_request(frame, 550, 180);
  gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, FALSE, 0);

  label = gtk_label_new(title_buf);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);


  /*
   * Page 5: Author info
   */
  sprintf(title_buf, "About");
  vbox = gtk_vbox_new(FALSE, 0);

  frame = gtk_frame_new("About Author");
  hbox = gtk_hbox_new(FALSE, 0);
  sprintf(buffer1, "\n\n\n\n%15s: %-25s\n\n%15s: %-25s\n\n%15s: %-25s\n\n\n\n", "Auther", "zxcpyp", "Github", "zxc479773533", "e-mail", "zxc479773533@gmail.com");
  label = gtk_label_new(buffer1);
  set_label_fontsize(label, "14");
  gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, FALSE, 5);
  image = gtk_image_new_from_file(AUTHOR);
  gtk_box_pack_start(GTK_BOX(hbox), image, TRUE, FALSE, 5);
  gtk_container_add(GTK_CONTAINER(frame), hbox);
  gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, FALSE, 5);

  frame = gtk_frame_new("About Version");
  sprintf(buffer1, "\n\nVersion v1.2.0\n\n\
Copyright (C) 2018 Pan Yue\n\
School of Computer Science and Technology\n\
Huazhong University of Science and Technology\n\n\n");
  label = gtk_label_new(buffer1);
  set_label_fontsize(label, "14");
  gtk_container_add(GTK_CONTAINER(frame), label);
  gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, FALSE, 5);

  label = gtk_label_new("For HUST Operating System Curriculum Design 2018 Summer");
  gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, FALSE, 5);

  label = gtk_label_new(title_buf);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), vbox, label);

  /* GTK show */
  gtk_widget_show_all(top_window);
  gtk_main();

  return 0;
}


/********************CALLBACKS********************/


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
  int ret;
  if (now_pid != NULL) {
    ret = kill(atoi(now_pid), SIGKILL);
    if (ret == -EPERM) {
      popup_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      popup_label = gtk_label_new("You need root privilege\n   to kill this process!");
      set_label_fontsize(popup_label, "14");
      gtk_widget_set_size_request(popup_window, 300, 180);
      gtk_container_add(GTK_CONTAINER(popup_window), popup_label);
      gtk_window_set_title(GTK_WINDOW(popup_window), "ERROR!");
      gtk_widget_show_all(popup_window);
    }
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
  gtk_clist_select_row(GTK_CLIST(clist), 0, 0);
  return;
}

/*
 * cpu_curve_callback - Refresh the CPU use curve once every second
 */
gboolean cpu_curve_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
  static int flag = 0;
  draw_cpu_curve((gpointer)widget);
  if (flag == 0) {
    g_timeout_add(1000, (GtkFunction)draw_cpu_curve, (gpointer)widget);
    flag = 1;
  }
  return TRUE;
}

/*
 * cpu_curve_callback - Refresh the Memory use curve once every second
 */
gboolean mem_curve_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
  static int flag = 0;
  draw_mem_curve((gpointer)widget);
  if (flag == 0) {
    g_timeout_add(1000, (GtkFunction)draw_mem_curve, (gpointer)widget);
    flag = 1;
  }
  return TRUE;
}

/*
 * swap_curve_callback - Refresh the Swap use curve once every second
 */
gboolean swap_curve_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
  static int flag = 0;
  draw_swap_curve((gpointer)widget);
  if (flag == 0) {
    g_timeout_add(1000, (GtkFunction)draw_swap_curve, (gpointer)widget);
    flag = 1;
  }
  return TRUE;
}

/*
 * refresh_modules - Refresh the modules info in clist
 */
void refresh_modules(void) {
  gtk_clist_freeze(GTK_CLIST(clist2));
  gtk_clist_clear(GTK_CLIST(clist2));
  get_modules_info();
  gtk_clist_thaw(GTK_CLIST(clist2));
  gtk_clist_select_row(GTK_CLIST(clist2), 0, 0);
  return;
}

/********************LOOPS********************/

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
  gdk_draw_rectangle(cpu_curve->window, gc, TRUE, 15, 30, 480, 200);

  /* Draw background lines */
  color.red = 0;
  color.green = 20000;
  color.blue = 0;
  gdk_gc_set_rgb_fg_color(gc, &color);
  for (int i = 30; i <= 220; i += 20)
    gdk_draw_line(cpu_curve->window, gc, 15, i, 495, i);
  for (int i = 15; i <= 480; i += 20)
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
                  15 + i * 4, 230 - 200 * cpu_ratio_data[draw_pos % 120],
                  15 + (i + 1) * 4, 230 - 200 * cpu_ratio_data[(draw_pos + 1) % 120]);
    draw_pos++;
    if (draw_pos == 120)
      draw_pos = 0;
  }

  /* Reset the color */
  color.red = 25000;
	color.green = 25000;
	color.blue = 25000;
	gdk_gc_set_rgb_fg_color(gc, &color);

  /* To loop this function, it must return TRUE */
  return TRUE;
}

/*
 * draw_mem_curve - Draw Memory use curve
 */
gboolean draw_mem_curve(gpointer widget) {
  GtkWidget *mem_curve = (GtkWidget *)widget;
  GdkColor color;
  GdkGC *gc = mem_curve->style->fg_gc[GTK_WIDGET_STATE(widget)];
  static int flag = 0;
  static int now_pos = 0;
  int draw_pos = 0;

  /* Darw background */
  color.red = 0;
  color.green = 0;
  color.blue = 0;
  gdk_gc_set_rgb_fg_color(gc, &color);
  gdk_draw_rectangle(mem_curve->window, gc, TRUE, 15, 10, 480, 200);

  /* Draw background lines */
  color.red = 0;
  color.green = 20000;
  color.blue = 0;
  gdk_gc_set_rgb_fg_color(gc, &color);
  for (int i = 10; i <= 230; i += 20)
    gdk_draw_line(mem_curve->window, gc, 15, i, 495, i);
  for (int i = 15; i <= 480; i += 20)
    gdk_draw_line(mem_curve->window, gc, i + mem_curve_start, 10, i + mem_curve_start, 210);

  /* Settle memory curve start position to make it live */
  mem_curve_start -= 4;
  if (mem_curve_start == 0)
    mem_curve_start = 20;

  /* Initial data */
  if (flag == 0) {
    for (int i = 0; i < 120; i++) {
      mem_ratio_data[i] = 0;
      flag = 1;
    }
  }

  /* Add data */
  mem_ratio_data[now_pos] = mem_ratio / 100;
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
    gdk_draw_line(mem_curve->window, gc,
                  15 + i * 4, 210 - 200 * mem_ratio_data[draw_pos % 120],
                  15 + (i + 1) * 4, 210 - 200 * mem_ratio_data[(draw_pos + 1) % 120]);
    draw_pos++;
    if (draw_pos == 120)
      draw_pos = 0;
  }

  /* Reset the color */
  color.red = 25000;
	color.green = 25000;
	color.blue = 25000;
	gdk_gc_set_rgb_fg_color(gc, &color);

  /* To loop this function, it must return TRUE */
  return TRUE;
}

/*
 * draw_swap_curve - Draw Swap use curve
 */
gboolean draw_swap_curve(gpointer widget) {
  GtkWidget *swap_curve = (GtkWidget *)widget;
  GdkColor color;
  GdkGC *gc = swap_curve->style->fg_gc[GTK_WIDGET_STATE(widget)];
  static int flag = 0;
  static int now_pos = 0;
  int draw_pos = 0;

  /* Darw background */
  color.red = 0;
  color.green = 0;
  color.blue = 0;
  gdk_gc_set_rgb_fg_color(gc, &color);
  gdk_draw_rectangle(swap_curve->window, gc, TRUE, 15, 10, 480, 200);

  /* Draw background lines */
  color.red = 0;
  color.green = 20000;
  color.blue = 0;
  gdk_gc_set_rgb_fg_color(gc, &color);
  for (int i = 10; i <= 230; i += 20)
    gdk_draw_line(swap_curve->window, gc, 15, i, 495, i);
  for (int i = 15; i <= 480; i += 20)
    gdk_draw_line(swap_curve->window, gc, i + mem_curve_start, 10, i + mem_curve_start, 210);

  /* Settle memory curve start position to make it live */
  swap_curve_start -= 4;
  if (swap_curve_start == 0)
    swap_curve_start = 20;

  /* Initial data */
  if (flag == 0) {
    for (int i = 0; i < 120; i++) {
      swap_ratio_data[i] = 0;
      flag = 1;
    }
  }

  /* Add data */
  swap_ratio_data[now_pos] = swap_ratio / 100;
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
    gdk_draw_line(swap_curve->window, gc,
                  15 + i * 4, 210 - 200 * swap_ratio_data[draw_pos % 120],
                  15 + (i + 1) * 4, 210 - 200 * swap_ratio_data[(draw_pos + 1) % 120]);
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
   * cpu(total) = user+nice+system+idle
   * pcpu = 100 *(total – idle) / total
   * total =total(t2) – total(t1)
   * idle =idle(t2) – idle(t1)
   */
  /* Data holder */
  static long old_idle, old_total;
  static int flag = 0;

  long user, nice, system, idle, total;
  float total_diff, idle_diff;
  char cpu[10], buffer[256], cpu_ratio_char[256];
  int fd;
  fd = open("/proc/stat", O_RDONLY);
  read(fd, buffer, sizeof(buffer));
  close(fd);
  sscanf(buffer, "%s %ld %ld %ld %ld", cpu, &user, &nice, &system, &idle);

  /* First */
  if (flag == 0) {
    flag = 1;
    old_idle = idle;
    old_total = user + nice + system + idle;
    cpu_ratio = 0;
  }
  /* Others */
  else {
    total = user + nice + system + idle;
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

/*
 * get_cpu_mhz - Get CPU freqency from /proc/cpuinfo line 8
 */
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

/*
 * get_memory_ratio - Get memory use ratio in /proc/meminfo
 */
gboolean get_memory_ratio(gpointer label) {
  int fd;
  char mem_buf[1024];
  char mem_total_char[1024];
  char mem_free_char[1024];
  char mem_ratio_char[1024];
  char *pos = NULL;
  int i;
  fd = open("/proc/meminfo", O_RDONLY);
  read(fd, mem_buf, sizeof(mem_buf));
  close(fd);

  /* Read memory total */
  i = 0;
  pos = strstr(mem_buf, "MemTotal");
  while (*pos != ':')
    pos++;
  pos += 1;
  while (*pos == ' ')
    pos++;

  while (*pos != ' ') {
    mem_total_char[i] = *pos;
    i++;
    pos++;
  }
  mem_total_char[i] = '\0';
  mem_total = atof(mem_total_char) / (1024 * 1024);

  /* Read memory free */
  i = 0;
  pos = strstr(mem_buf, "MemFree");
  while (*pos != ':')
    pos++;
  pos += 1;
  while (*pos == ' ')
    pos++;

  while (*pos != ' ') {
    mem_free_char[i] = *pos;
    i++;
    pos++;
  }
  mem_free_char[i] = '\0';
  mem_free = atof(mem_free_char) / (1024 * 1024);

  /* Get memory use ratio */
  mem_ratio = 100 - (mem_free / mem_total) * 100;
  sprintf(mem_ratio_char, "Memory usage: %0.1f%%", mem_ratio);
  gtk_label_set_text(GTK_LABEL(label), mem_ratio_char);
  return TRUE;
}

/*
 * get_memory_fraction - Set memory use fraction
 */
gboolean get_memory_fraction(gpointer label) {
  char mem_fraction[1024];
  sprintf(mem_fraction, "%0.2f / %0.2f GB", mem_total - mem_free, mem_total);
  gtk_label_set_text(GTK_LABEL(label), mem_fraction);
  return TRUE;
}

/*
 * get_swap_ratio - Get sawp use ratio in /proc/meminfo
 */
gboolean get_swap_ratio(gpointer label) {
  int fd;
  char swap_buf[1024];
  char swap_total_char[1024];
  char swap_free_char[1024];
  char swap_ratio_char[1024];
  char *pos = NULL;
  int i;
  fd = open("/proc/meminfo", O_RDONLY);
  read(fd, swap_buf, sizeof(swap_buf));
  close(fd);

  /* Read swap total */
  i = 0;
  pos = strstr(swap_buf, "SwapTotal");
  while (*pos != ':')
    pos++;
  pos += 1;
  while (*pos == ' ')
    pos++;

  while (*pos != ' ') {
    swap_total_char[i] = *pos;
    i++;
    pos++;
  }
  swap_total_char[i] = '\0';
  swap_total = atof(swap_total_char) / (1024 * 1024);

  /* Read swap free */
  i = 0;
  pos = strstr(swap_buf, "SwapFree");
  while (*pos != ':')
    pos++;
  pos += 1;
  while (*pos == ' ')
    pos++;

  while (*pos != ' ') {
    swap_free_char[i] = *pos;
    i++;
    pos++;
  }
  swap_free_char[i] = '\0';
  swap_free = atof(swap_free_char) / (1024 * 1024);

  /* Get swap use ratio */
  swap_ratio = 100 - (swap_free / swap_total) * 100;
  sprintf(swap_ratio_char, "Swap usage: %0.1f%%", swap_ratio);
  gtk_label_set_text(GTK_LABEL(label), swap_ratio_char);
  return TRUE;
}

/*
 * get_swap_fraction - Set swap use fraction
 */
gboolean get_swap_fraction(gpointer label) {
  char swap_fraction[1024];
  sprintf(swap_fraction, "%0.2f / %0.2f GB", swap_total - swap_free, swap_total);
  gtk_label_set_text(GTK_LABEL(label), swap_fraction);
  return TRUE;
}

/*
 * get_sys_info - Set system information
 */
gboolean get_sys_info(gpointer label) {
  int fd;
  FILE *fp;
  int i, j;
  char buffer[1024];
  char *pos = NULL;
  char host_name[128];
  char os_name[128];
  int os_type;
  char kernel_version[128];
  char gcc_version[128];
  int setup_time;
  int uphour, upminute, upsecond;

  /* Get host name */
  fp = fopen("/etc/hostname", "r");
  fgets(host_name, sizeof(host_name), fp);
  fclose(fp);

  /* Get os name */
  memset(buffer, 0, sizeof(buffer));
  fd = open("/etc/issue", O_RDONLY);
  read(fd, buffer, sizeof(buffer));
  close(fd);
  for (i = 0; i < 1024; i++) {
    if (buffer[i] == '\\')
      break;
  }
  buffer[i - 1] = '\0';
  strcpy(os_name, buffer);

  /* Get os type */
  os_type = sizeof(char *) * 8;

  /* Get kernel version */
  fp = fopen("/proc/sys/kernel/osrelease", "r");
  fgets(kernel_version, sizeof(kernel_version), fp);
  fclose(fp);

  /* Get gcc version */
  memset(buffer, 0, sizeof(buffer));
  fd = open("/proc/version", O_RDONLY);
  read(fd, buffer, sizeof(buffer));
  close(fd);
  pos = strstr(buffer, "gcc version");
  for (i = 0, j = 0; i < 1024; i++) {
    if (*pos == ' ')
      j++;
    if (j == 2)
      break;
    pos++;
  }
  pos++;
  for (i = 0; i < 1024; i++) {
    if (pos[i] == ' ')
      break;
    gcc_version[i] = pos[i];
  }
  gcc_version[i] = '\0';

  /* Get setuo time */
  memset(buffer, 0, sizeof(buffer));
  fd = open("/proc/uptime", O_RDONLY);
  read(fd, buffer, sizeof(buffer));
  close(fd);
  for (i = 0; i < 1024; i++) {
    if (buffer[i] == ' ')
      break;
  }
  buffer[i] = '\0';
  setup_time = atoi(buffer);
  upsecond = setup_time % 60;
  upminute = (setup_time / 60) % 60;
  uphour = setup_time / 3600;

  sprintf(buffer, "Hostname:          %s\n\
OS Name:           %s\n\n\
OS Type:             %d-bit\n\n\
Kernel Version:    %s\n\
GCC Version:       %s\n\n\
Uptime:              %02d:%02d:%02d",
          host_name, os_name, os_type, kernel_version, gcc_version, uphour, upminute, upsecond);
  
  gtk_label_set_text(GTK_LABEL(label), buffer);
  set_label_fontsize((GtkWidget *)label, "13");

  return TRUE;
}

/*
 * get_network_info - Get network information in /proc/net/dev
 */
gboolean get_network_info(gpointer label) {
  /*
   * Network dev file format:
   * 
   * Inter-|   Receive                                                |  Transmit
   *  face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
   *   dev1: ...
   *   dev2: ...
   *   ...
   */
  FILE *fp;
  long r_byte, s_byte;
  long receive_byte, send_byte;
  float receive_diff, send_diff;
  static long old_receive, old_send;
  static int flag = 0;
  char buffer[256];
  char *pos;

  fp = fopen("/proc/net/dev", "r");
  fgets(buffer, sizeof(buffer), fp);
  fgets(buffer, sizeof(buffer), fp);

  receive_byte = 0;
  send_byte = 0;

  while ((pos = fgets(buffer, sizeof(buffer), fp)) != NULL) {
    while (*pos != ':')
      pos++;
    pos++;
    sscanf(pos, "%ld %*d %*d %*d %*d %*d %*d %*d %ld", &r_byte, &s_byte);
    receive_byte += r_byte;
    send_byte += s_byte;
  }
  fclose(fp);

  if (flag == 0) {
    old_receive = receive_byte;
    old_send = send_byte;
    receive_diff = 0;
    send_diff = 0;
    flag = 1;
  }
  else {
    receive_diff = receive_byte - old_receive;
    send_diff = send_byte - old_send;
    old_receive = receive_byte;
    old_send = send_byte;
  }

  /* Count speed (KB) */
  receive_speed = receive_diff / (1024 * 2);
  send_speed = send_diff / (1024 * 2);

  sprintf(buffer, "    Network Speed:\n\n\
\tUpload speed: %7.1f KB/s\tDownload speed: %7.1f KB/s\n\n\
\t  Total upload: %7.1f MB\t\t  Total download: %7.1f MB",
          send_speed, receive_speed, send_byte / (1024.0 * 1024.0), receive_byte / (1024.0 * 1024.0));
  gtk_label_set_text(GTK_LABEL(label), buffer);
  set_label_fontsize((GtkWidget *)label, "13");

  return TRUE;
}

/*
 * get_disk_info - Get network information in /proc/diskstat
 */
gboolean get_disk_info(gpointer label) {
  /*
   * Disk status file format:
   * 
   * 8 num sdx rd_ios rd_merges rd_sectors rd_ticks wr_ios wr_merges wr_sectors \
   *  wr_ticks in_flight io_ticks time_in_queue
   * 
   * read speed = (Δrd_sectors/Δt) * (block_size / 1024)
   * write speed = (Δwr_sectors/Δt) * (block_size / 1024)
   * Here block_size is 512
   */
  FILE *fp;
  long rd_sector, wr_sector;
  long rd_sectors, wr_sectors;
  float rd_diff, wr_diff;
  static long old_rd_sectors, old_wr_sectors;
  static int flag = 0;
  char buffer[256];
  char *pos;

  fp = fopen("/proc/diskstats", "r");

  rd_sectors = 0;
  wr_sectors = 0;

  while((pos = fgets(buffer, sizeof(buffer), fp)) != NULL) {
    sscanf(buffer, "%*d %*d %*s %*d %*d %d %*d %*d %*d %d", &rd_sector, &wr_sector);
    rd_sectors += rd_sector;
    wr_sectors += wr_sector;
  }
  fclose(fp);

  if (flag == 0) {
    old_rd_sectors = rd_sectors;
    old_wr_sectors = wr_sectors;
    rd_diff = 0;
    wr_diff = 0;
    flag = 1;
  }
  else {
    rd_diff = rd_sectors - old_rd_sectors;
    wr_diff = wr_sectors - old_wr_sectors;
    old_rd_sectors = rd_sectors;
    old_wr_sectors = wr_sectors;
  }

  /* Count speed (MB) */
  read_speed = (rd_diff * 512) / (2 * 1024 * 1024);
  write_speed = (wr_diff * 512) / (2 * 1024 * 1024);

  sprintf(buffer, "Disk I/O Speed:\n\n\
\tRead speed: %7.1f MB/s\t\tWrite speed: %7.1f MB/s\n\n\
\t  Total Read: %7.1f GB\t\t  Total Write: %7.1f GB",
          read_speed, write_speed, (rd_sectors * 512) / (1024.0 * 1024.0 * 1024.0), (wr_sectors * 512) / (1024.0 * 1024.0 * 1024.0));
  gtk_label_set_text(GTK_LABEL(label), buffer);
  set_label_fontsize((GtkWidget *)label, "13");

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

/*
 * set_label_fontsize - To set font size in a label
 * 
 * Referencing from: https://blog.csdn.net/gl_ding/article/details/4939355
 */
void set_label_fontsize(GtkWidget *label, char *fontsize) {
  PangoFontDescription *desc_info = pango_font_description_from_string(fontsize);
  gtk_widget_modify_font(label, desc_info);
  pango_font_description_free(desc_info);
}