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
 * utf8_fix - To settle waring: Invalid UTF-8 string passed to pango_layout_set_text()
 */
char* utf8_fix(char *c) {
  return g_locale_to_utf8(c, -1, NULL, NULL, NULL);
}

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
    }
  }
  closedir(dir);
}


int main(int argc, char **argv) {
  gtk_init(&argc, &argv);
  /* GTK widgets */
  GtkWidget *top_window;
  GtkWidget *notebook;
  GtkWidget *hbox;
  GtkWidget *vbox;
  GtkWidget *label;
  GtkWidget *scrolled_window;
  GtkWidget *button1;
  GtkWidget *button2;
  GtkWidget *button3;

  /* Save page title */
  char title_buf[1024];

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
  g_signal_connect(G_OBJECT(button1), "clicked", G_CALLBACK(search_proc), NULL);
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
  button1 = gtk_button_new_with_label("Page 2");
  gtk_container_add(GTK_CONTAINER(vbox), button1);
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

/********************CALLBACKS********************/

/*
 * select_row_callback - Callback for select_row
 */
void select_row_callback(GtkWidget *clist, gint row, gint column, GdkEventButton *event, gpointer data) {
	gtk_clist_get_text(GTK_CLIST(clist), row, 0, &now_pid);
  gtk_entry_set_text(GTK_ENTRY(entry), (gchar *)now_pid);
  return;
}

void search_proc(void) {
  const gchar *entry_txt;
  entry_txt = gtk_entry_get_text(GTK_ENTRY(entry));
  return;
}

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