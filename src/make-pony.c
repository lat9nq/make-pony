/* make-pony.c
 * C
 *
 * Toast Unlimited
 * 24 September 2017
 *
 * -s1506233186 -d1 is a known issue (completely black)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _WIN64
#include <process.h>
#else
#include <unistd.h>
#endif

#include "color.h"
#include "target.h"
#include "make-pony.h"
#include "thumbnailer.h"

int main(int argc, char * argv[]) {

	char filename[255];

	int times = 0;
	int fname_specified = 0;
	long int seed = time(NULL);
	int verbose = 0;
	int help = 0;
	int nogui = 0;

	char * farg = NULL;
	FILE * arg_in = NULL;
	char ** fargv = NULL;
	int stdo = 0;
	sterr = stderr;
	
	data = NULL;
	preview = NULL;
	preview_buf = NULL;
	
	mp_data *oc;
	oc = malloc(sizeof(*oc));
	oc->key = -1;
	oc->use_floofers = -1;
	oc->use_socks = -1;
	oc->male = 0;
	oc->desaturated = -1;
	oc->white = -1;
	oc->detail_count = -1;
	oc->wheel = -1;
	oc->traditional = 0;
	oc->verbose = 0;
	oc->hue = -1;

	char s[255];
	s[0] = 0;

	char ** args = argv;
	int argsc = argc;

	if (argsc != 1) {
		for (int i = 1; i < argsc; i++) {
			if (args[i][0] == '-') {
				switch (args[i][1]) {
					case 's': //seed specify
						if (args[i][2] != 0) {
							seed = atoi(args[i]+2);
						}
						else {
							i++;
							seed = atoi(args[i]);
						}
						// printf("%ld", seed);
						break;
					case 'k': //style _Key specify
						if (args[i][2] != 0) {
							oc->key = atoi(args[i]+2);
						}
						else {
							i++;
							oc->key = atoi(args[i]);
						}
						oc->key %= STYLECOUNT;
						break;
					case 'o': //output file
						if (args[i][2] != 0) {
							strcpy(filename, args[i]+2);
						}
						else {
							i++;
							strcpy(filename, args[i]);
						}
						fname_specified = 1;
						break;
					case 'n':
						nogui = 1;
						break;
					case 'm': //how many?
						times = 1;
						if (args[i][2] != 0) {
							times = atoi(args[i]+2);
						}
						else {
							i++;
							times = atoi(args[i]);
						}
						break;
					case 'c': 
						stdo = 1;
						break;
					case 'x': { //color specify
							  char * arg;
							  //colorgiven = 1;
							  if (args[i][2] != 0) {
								  arg = args[i] + 2;
							  }
							  else {
								  i++;
								  arg = args[i];
							  }
							  sscanf(arg, "%06X",&oc->hue);
						  }
						  break;
					case 'd': //desaturate? 0 - no; 1 - yes; 2 - white
						  if (args[i][2]) {
							  oc->desaturated = atoi(args[i]+2);
						  }
						  else {
							  i++;
							  if (i < argsc && isdigit(args[i][0])) {
								  oc->desaturated = atoi(args[i]);
							  }
							  else {
								  oc->desaturated = 1;
							  }
						  }
						  oc->white = oc->desaturated > 1;
						  oc->desaturated = (!!oc->desaturated) ^ oc->white;
						  break;
					case 'w':
						  if (args[i][2]) {
							  oc->wheel = atoi(args[i]+2);
						  }
						  else {
							  i++;
							  if (i < argsc && isdigit(args[i][0])) {
								  oc->wheel = atoi(args[i]);
							  }
						  }
						  break;
					case 'l':
						  oc->use_socks = 1;
						  break;
					case 'v':
						  oc->verbose = 1;
						  verbose = 1;
						  break;
					case 'g':
						  oc->male = 1;
						  break;
/*					case 'r':
						  oc->separate_hair = 1;
						  break;*/
					case 't':
						  oc->traditional = 1;
						  break;
					case 'h':
						  help = 1;
						  break;
					case '-': {
							  if (strstr(args[i], "--help")) {
								  help = 1;
							  }
						  }
						  break;
					default:
						  help = 1;
						  break;
				}
			}
			else {
				if (!farg) {
					farg = args[i];
					arg_in = fopen(farg, "r");
					fargv = malloc(sizeof(*fargv)*32);
					int j;
					for (j = 0; j < 32; j++) {
						fargv[j] = NULL;
					}
					j = 0;
					while (fgets(s,256,arg_in)) {
						fargv[j] = malloc(sizeof(**fargv)*256);
						strcpy(fargv[j], s);
						printf("%s",fargv[j]);
						j++;
						fargv[j] = NULL;
					}
					args = fargv;
					i = -1;
					argsc = j;
				}
				else {
					fprintf(sterr, "error: specified too many input files\n");
					return 0;
				}
			}
		}
		//strcpy(filename, args[1]);
	}

	if (fargv) {
		for (int i = 0; fargv[i]; i++) {
			free(fargv[i]);
		}
		free(fargv);
		fargv = 0;
	}

	if (help) {
		fprintf(sterr, "usage: %s [OPTIONS]\n", argv[0]);
		fprintf(sterr, "Creates a file compatible with Pony Player Models 2 addon for Garry's Mod 13.\n");
		fprintf(sterr, "By default, the output files take the format \"[TIME]_[SEED]_makepony.dat\",\n");
		fprintf(sterr, "and the default seed is the time since the UNIX epoch.\n");
		fprintf(sterr, "\t-o<file>\toutput to a specific file (incompatible with -m)\n");
		fprintf(sterr, "\t-m<n>\t\tgenerate n files\n\n");
		fprintf(sterr, "\t-l\t\tenable socks\n");
		fprintf(sterr, "\t-w\t\tcolor wheel options [15/56 chance per color]\n");
		fprintf(sterr, "\t\t\t0 -- do not use\n");
		fprintf(sterr, "\t\t\t1 -- use analogous colors [default]\n");
		fprintf(sterr, "\t\t\t2 -- use rectangular/tetradic colors\n");
		fprintf(sterr, "\t-t\t\ttraditional -- turns off body details\n");
		fprintf(sterr, "\t-r\t\tseparate hair from body color\n");
		fprintf(sterr, "\t-d[0-2]\t\tdesaturate\n");
		fprintf(sterr, "\t\t\t0 -- use saturation\n");
		fprintf(sterr, "\t\t\t1 -- disallow saturation\n");
		fprintf(sterr, "\t\t\t2 -- use only white body colors\n");
		fprintf(sterr, "\t-g\t\tforce generate a male\n");
		fprintf(sterr, "\t-k<n>\t\tset a style to generate with (UMane, LMane, Tail)\n");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 0, "ADVENTUROUS", "ADVENTUROUS", "ADVENTUROUS");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 1, "SHOWBOAT", "SHOWBOAT", "SHOWBOAT");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 2, "BOOKWORM", "BOOKWORM", "BOOKWORM");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 3, "SPEEDSTER", "SPEEDSTER", "SPEEDSTER");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 4, "ASSERTIVE", "ASSERTIVE", "ASSERTIVE");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 5, "RADICAL", "NONE", "RADICAL");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 6, "BUMPKIN", "BUMPKIN", "BUMPKIN");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 7, "FLOOFEH", "FLOOFEH", "FLOOFEH");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 8, "INSTRUCTOR", "NONE", "RADICAL");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 9, "FILLY", "ADVENTUROUS", "SHOWBOAT");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 10, "ADVENTUROUS", "MOON", "SHOWBOAT");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 11, "BOOKWORM", "TIMID", "SHOWBOAT");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 12, "ADVENTUROUS", "ADVENTUROUS", "RADICAL");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 13, "BOLD", "BOLD", "RADICAL");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 14, "MECHANIC", "MOON", "ADVENTUROUS");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 15, "SHOWBOAT", "BOOKWORM", "SHOWBOAT");
		fprintf(sterr, "\t\t\t%d -- %s, %s, %s\n", 16, "MAILCALL", "MAILCALL", "MAILCALL");
		fprintf(sterr, "\t-x<XXXXXX>\tspecify a base color to use in HEXADECIMAL\n\n");
		fprintf(sterr, "\t-s<n>\t\tspecify a seed for the random generator\n");
		fprintf(sterr, "\t-c\t\toutput to STDOUT\n\n");
		fprintf(sterr, "\t-v\t\tverbose output\n");
		fprintf(sterr, "\t-h, --help\tthis help text\n");
		fprintf(sterr, "\t-n\t\tdisable GTK+ GUI\n");
		fprintf(sterr, "written by Toast Unlimited<http://steamcommunity.com/id/toastunlimited/>\n");

		return 0;
	}
	long the_time = time(NULL);

	srand(seed);
	oc->seed = seed;

	if (times) {
		FILE * p;
		char * cmd = malloc(sizeof(*cmd)*256);
		char * temp = malloc(sizeof(*temp) * 128);
		temp[0] = 0;
		for (int i = 1 - (argv != args); i < argsc; i++) {
			if (args[i][1] != 'm' && args[i][1] != 's' && args[i][1] != 'o') {
				strcat(temp, " ");
				strcat(temp, args[i]);
			}
			else {
				if (args[i][2] == 0) {
					i++;
				}
			}
		}

		printf("The seed is: %ld\n", seed);

		for (int i = 0; i < times; i++) {
			cmd[0] = 0;
			seed = rand() * rand() & 2147483647;
			sprintf(cmd, "%s -s%ld%s -o%03d_%010lu_makepony.dat" , argv[0], seed, temp, i, seed);
			if (verbose) {
				fprintf(sterr, "%s\n", cmd);
			}
			p = popen(cmd, "r");
			if (!fgets(cmd, 256, p))
				break;
			cmd[strlen(cmd)-1] = 0;
			if (!verbose) {
				printf("%03d_%010lu: %s\n", i, seed, cmd);
			}
			else {
				putchar('\n');
			}
			pclose(p);
		}

		free(temp);
		free(cmd);
		return 0;
	}

	if (!fname_specified) {
		sprintf(filename, "%09ld_%ld_makepony.dat", the_time, seed);
	}

	s[0] = 0;


	target = malloc(sizeof(*target)*TARGETCOUNT);
	for (int i = 0; i < TARGETCOUNT; i++) {
		target[i] = malloc(sizeof(**target)*32);
	}
	importTargets(target);

	races = malloc(sizeof(*races)*RACES);
	for (int i = 0; i < RACES; i++) {
		races[i] = malloc(sizeof(**races)*8);
	}
	at = 0;
	PUSHSTRING(races, "EARTH");
	PUSHSTRING(races, "PEGASUS");
	PUSHSTRING(races, "UNICORN");


	//char ** styles;
	styles = malloc(sizeof(*styles)*STYLECOUNT);

	for (int i = 0; i < STYLECOUNT; i++) {
		styles[i] = calloc(STYLECHARLEN*3+1,sizeof(**styles));
	}

	// #define ADDSTYLE(R, I, S, T, U, C)	strcpy(UMANE((R)[(I)]), (S)); strcpy(LMANE((R)[(I)]), (T)); strcpy(TAIL((R)[(I)]), (U)); COLOR((R)[(I)]) = (C)
	ADDSTYLE(styles, 0, "ADVENTUROUS", "ADVENTUROUS", "ADVENTUROUS", 2);
	ADDSTYLE(styles, 1, "SHOWBOAT", "SHOWBOAT", "SHOWBOAT", 2);
	ADDSTYLE(styles, 2, "BOOKWORM", "BOOKWORM", "BOOKWORM", 5);
	ADDSTYLE(styles, 3, "SPEEDSTER", "SPEEDSTER", "SPEEDSTER", 7);
	ADDSTYLE(styles, 4, "ASSERTIVE", "ASSERTIVE", "ASSERTIVE", 2);
	ADDSTYLE(styles, 5, "RADICAL", "NONE", "RADICAL", 7);
	ADDSTYLE(styles, 6, "BUMPKIN", "BUMPKIN", "BUMPKIN", 3);
	ADDSTYLE(styles, 7, "FLOOFEH", "FLOOFEH", "FLOOFEH", 2);
	ADDSTYLE(styles, 8, "INSTRUCTOR", "NONE", "RADICAL", 2);
	ADDSTYLE(styles, 9, "FILLY", "ADVENTUROUS", "SHOWBOAT", 2);
	ADDSTYLE(styles, 10, "ADVENTUROUS", "MOON", "SHOWBOAT", 2);
	ADDSTYLE(styles, 11, "BOOKWORM", "TIMID", "SHOWBOAT", 5);
	ADDSTYLE(styles, 12, "ADVENTUROUS", "ADVENTUROUS", "RADICAL", 2);
	ADDSTYLE(styles, 13, "BOLD", "BOLD", "RADICAL", 3);
	ADDSTYLE(styles, 14, "MECHANIC", "MOON", "ADVENTUROUS", 2);
	ADDSTYLE(styles, 15, "SHOWBOAT", "BOOKWORM", "SHOWBOAT", 5);
	ADDSTYLE(styles, 16, "MAILCALL", "MAILCALL", "MAILCALL", 1);

	unsigned char * data;
	data = malloc(sizeof(*data)*32768);
	if (!data) {
		fprintf(sterr, "error: could not allocate enough memory to store data\n");
		return 0;
	}
	memset(data, 0, sizeof(*data)*32768);
	int data_len;
	
	
	if (nogui) {
		mp_pick_colors(oc);
		data_len = mp_construct_nbt(oc, data);
		
		if (!stdo) {
			FILE * f = fopen(filename, "wb");
/*	#ifdef _WIN32
			if (!f) {
				char * oldfilename = filename;
				filename = strrchr(filename, '\\')+1;
				fprintf(sterr, "warning: could not write to %s, trying %s\n", oldfilename, filename);
				f = fopen(filename, "wb");
			}
	#endif*/
			if (!f) {
				fprintf(sterr, "error: output file not accessible\n");
				return 0;
			}
			fwrite(data, sizeof(*data), data_len, f);
			printf("Wrote to %s successfully\n", filename);
			fclose(f);
		}
		else {
			fprintf(stdout, "%s", data);
		}
	}
	else {
		srand(time(NULL));
		
		preview = pngimg_init();
		preview_seed = 0;
		
		main_oc = (mp_data *)malloc(sizeof(*main_oc));
		mp_data_init(main_oc);
		
#ifdef _WIN64
		_pipe(log_pipe, 4096, _O_BINARY);
#else
		pipe(log_pipe);
#endif
		sterr = fdopen(log_pipe[1], "w");
		
		mp_gtk_build(&argc, &argv, &main_widgets);
	}
	
	return 0;
}

void mp_gtk_build(int *argc, char ** argv[], mp_gtk_widgets ** w) {
	mp_gtk_widgets * widgets;
	widgets = (mp_gtk_widgets *)malloc(sizeof(*widgets));
	*w = widgets;
	
	gtk_init(argc, argv);
	
	widgets->builder = gtk_builder_new();
	gtk_builder_add_from_file(widgets->builder, "src/make-pony.glade", NULL);
	
	widgets->window = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "window_main"));
	gtk_builder_connect_signals(widgets->builder, NULL);
	
	
	widgets->g_entry_seed = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "entry_seed"));
	widgets->g_entry_hue = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "entry_hue"));
	widgets->g_entry_thumbnail_dir = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "entry_thumbnail_dir"));
	widgets->g_chk_traditional = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "chk_traditional"));
	widgets->g_chk_verbose = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "chk_verbose"));
	widgets->g_chk_hue = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "chk_hue"));
	widgets->g_chk_preview = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "chk_preview"));
	widgets->g_chk_thumbnail = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "chk_thumbnail"));
	widgets->g_cmb_saturation = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "cmb_saturation"));
	widgets->g_cmb_wheel = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "cmb_wheel"));
	widgets->g_cmb_socks = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "cmb_socks"));
	widgets->g_cmb_male = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "cmb_male"));
	widgets->g_cmb_style = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "cmb_style"));
	widgets->g_img_preview = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "img_preview"));
	widgets->g_scl_hue = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "scl_hue"));
	widgets->g_fch_outdir = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "fch_outdir"));
	widgets->g_txt_log = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "txt_log"));
	widgets->g_scrl_log = GTK_WIDGET(gtk_builder_get_object(widgets->builder, "scrl_log"));
	

    g_object_unref(widgets->builder);

    gtk_widget_show(widgets->window);
    gtk_main();
}

void update_txt_log() {
	char *s;
	s = malloc(sizeof(*s) * 4096);
	int i;
	
	char c;
	c = 0;
	i = 0;
	fflush(sterr);
	write(log_pipe[1], &c, 1);
	while (read(log_pipe[0], &c, 1)) {
		if (c == 0)
			break;
		
		s[i] = c;
		i++;
		s[i] = 0;
	}
	
	GtkTextBuffer * buf = gtk_text_view_get_buffer((GtkTextView*)main_widgets->g_txt_log);
	gtk_text_buffer_insert_at_cursor(buf, s, -1);
	
	free(s);
}

void on_txt_log_size_allocate() {
	GtkAdjustment * vadj = gtk_scrolled_window_get_vadjustment((GtkScrolledWindow*)main_widgets->g_scrl_log);
	gtk_adjustment_set_value(vadj, gtk_adjustment_get_upper(vadj));
}

void mp_gtk_destroy(mp_gtk_widgets * widgets) {
	
}

void on_btn_generate_clicked() {
	char text[256];
	int seed;
	
	strncpy(text, gtk_entry_get_text((GtkEntry*)main_widgets->g_entry_seed), 255);
	if (text[0] == '\0') {
		seed = time(NULL);
	}
	else if (!strcmp(text, "random")) {
		seed = rand();
	}
	else {
		seed = atoi(text);
	}
	
	mp_data_init(main_oc);
	main_oc->seed = seed;
	main_oc->desaturated = atoi(gtk_combo_box_get_active_id((GtkComboBox*)main_widgets->g_cmb_saturation));
	main_oc->wheel = atoi(gtk_combo_box_get_active_id((GtkComboBox*)main_widgets->g_cmb_wheel));
	main_oc->use_socks = atoi(gtk_combo_box_get_active_id((GtkComboBox*)main_widgets->g_cmb_socks));
	main_oc->male = atoi(gtk_combo_box_get_active_id((GtkComboBox*)main_widgets->g_cmb_male));
	main_oc->traditional = gtk_toggle_button_get_active((GtkToggleButton*)main_widgets->g_chk_traditional);
	main_oc->verbose = gtk_toggle_button_get_active((GtkToggleButton*)main_widgets->g_chk_verbose);
	if (gtk_toggle_button_get_active((GtkToggleButton*)main_widgets->g_chk_hue))
		main_oc->hue = gtk_range_get_value((GtkRange*)main_widgets->g_scl_hue);
	main_oc->key = atoi(gtk_combo_box_get_active_id((GtkComboBox*)main_widgets->g_cmb_style));
	
	if (data == NULL) {
		data = (uint8_t *)malloc(sizeof(*data)*32768);
	}
	memset(data, 0, 32768);
	
	mp_pick_colors(main_oc);
	gtk_data_len = mp_construct_nbt(main_oc, data);
	
	int show_preview;
	show_preview = gtk_toggle_button_get_active((GtkToggleButton*)main_widgets->g_chk_preview);
	
	if (show_preview) {
		if (preview == NULL) {
			preview = pngimg_init();
		}
		preview_seed = render_oc(preview, data, gtk_data_len, main_oc);
		if (preview_seed < 0) {
			update_txt_log();
			return;
		}
		
		int p_width, p_height;
		int width, height;
		p_width = gtk_widget_get_allocated_width(main_widgets->g_img_preview);
		p_height = gtk_widget_get_allocated_height(main_widgets->g_img_preview);
		width = preview->width;
		height = preview->height;
		
		if (preview_buf == NULL) {
			preview_buf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 1, 8, p_width, p_height);
		}
		
		
		uint8_t * preview_pix = gdk_pixbuf_get_pixels(preview_buf);
		int preview_rowstride = gdk_pixbuf_get_rowstride(preview_buf);
		
		int x, x_2;
		for (int i = 0; i < height; i+=height / p_height) {
			x = i / (height / p_height) * preview_rowstride;
			for (int j = 0; j < width; j+=width / p_width) {
				x_2 = x + j / (width / p_width) * 4;
				preview_pix[x_2 + 0] = preview->pixels[i][j].r;
				preview_pix[x_2 + 1] = preview->pixels[i][j].g;
				preview_pix[x_2 + 2] = preview->pixels[i][j].b;
				preview_pix[x_2 + 3] = preview->pixels[i][j].a;
			}
		}
		gtk_image_set_from_pixbuf((GtkImage*)main_widgets->g_img_preview, preview_buf);
	}
	
	update_txt_log();
}

int render_oc(PNGIMG * img, uint8_t * nbt, int nbt_len, mp_data * oc) {
	//int thumbnail(PNGIMG * canvas, uint8_t * data, uint32_t data_len);
	preview_seed = oc->seed;
	thumbnail(img, nbt, nbt_len);
	return preview_seed;
}

void on_btn_seed_now_clicked() {
	char blank[1] = {0};
	gtk_entry_set_text((GtkEntry*)main_widgets->g_entry_seed, blank);
}

void on_btn_seed_random_clicked() {
	const char * s = "random";
	gtk_entry_set_text((GtkEntry*)main_widgets->g_entry_seed, s);
}

void on_btn_save_clicked() {
	char directory[256];
	char * chooser_dir;
	chooser_dir = gtk_file_chooser_get_filename((GtkFileChooser*)main_widgets->g_fch_outdir);
	if (chooser_dir == NULL) {
		fprintf(sterr, "Please select an output directory...\n");
		update_txt_log();
		return;
	}
	strncpy(directory, chooser_dir, 255);
	
	int length = strlen(directory);
	if (length == 255) {
		fprintf(sterr, "Directory is too long. Pick a different one.\n");
		update_txt_log();
		return;
	}
	
	char filename[256];
	sprintf(filename, "%ld_%d_makepony", (long)time(NULL), main_oc->seed);
	
	char output[1024];
	snprintf(output, 1023, "%s/%s.dat", directory, filename);
	
	int fd;
	fd = open(output, O_WRONLY | O_CREAT, 0664);
	if (data == NULL) {
		fprintf(sterr, "You must generate an OC before clicking save.\n");
		update_txt_log();
		return;
	}
	if (fd == -1) {
		fprintf(sterr, "Error: %s could not be written to.\n", output);
		update_txt_log();
		return;
	}
	write(fd, data, gtk_data_len);
	close(fd);
	
	fprintf(sterr, "Wrote to %s successfully.\n", output);
	
	if (gtk_toggle_button_get_active((GtkToggleButton*)main_widgets->g_chk_thumbnail)) {
		if (preview_seed != main_oc->seed) {
			fprintf(sterr, "Cannot save thumbnail. None generated...\n");
			update_txt_log();
			return;
		}
		
		char thumb_dir[256];
		int thumb_dir_len;
		
		strncpy(thumb_dir, gtk_entry_get_text((GtkEntry*)main_widgets->g_entry_thumbnail_dir), 255);
		thumb_dir_len = strlen(thumb_dir);
		
		if (thumb_dir_len == 255) {
			fprintf(sterr, "error: thumbnail output is inaccessible\n");
			update_txt_log();
			return;
		}
		
		if (thumb_dir[thumb_dir_len - 1] != '/')
			strcat(thumb_dir, "/");
		
		snprintf(output, 1023, "%s/%s%s.png", directory, thumb_dir, filename);
		
		FILE * f;
		f = fopen(output, "w");
		if (!f) {
			fprintf(sterr, "error: thumbnail output is inaccessible\n");
			update_txt_log();
			return;
		}
		if (pngimg_write(preview, f) >= 0) {
			fprintf(sterr, "Wrote to %s successfully.\n", output);
		}
		else {
			fprintf(sterr, "PNG error... unable to write %s.\n", output);
		}
		fclose(f);
	}
	
	update_txt_log();
}

void on_chk_hue_toggled() {
	int active = gtk_toggle_button_get_active((GtkToggleButton*)main_widgets->g_chk_hue);
	gtk_widget_set_sensitive(main_widgets->g_entry_hue, active);
	gtk_widget_set_sensitive(main_widgets->g_scl_hue, active);
}

void on_entry_hue_changed() {
	char text[256];
	int length, valid;
	color c;
	
	strncpy(text, gtk_entry_get_text((GtkEntry*)main_widgets->g_entry_hue), 255);
	length = strlen(text);
	valid = 1;
	
	if (gtk_widget_has_focus(main_widgets->g_entry_hue) && length < 7)
		return;
	else if (!gtk_widget_has_focus(main_widgets->g_entry_hue))
		return;
	
	if (length > 7)
		text[7] = 0;
	
	for (int i = 0; text[i] && i < 7; i++) {
		if (!(isxdigit(text[i]) || text[i] == '#'))
			valid = 0;
	}
	
	if (valid) {
		int r, g, b;
		sscanf(text, "#%02x%02x%02x", &r, &g, &b);
		c.r = (uint8_t)r;
		c.g = (uint8_t)g;
		c.b = (uint8_t)b;
		gtk_range_set_value((GtkRange*)main_widgets->g_scl_hue, hue(&c));
	}
}

void on_scl_hue_value_changed() {
	hsv hsvcolor;
	color c;
	char text[255];
	hsvcolor.s = 1;
	hsvcolor.v = 1;
	hsvcolor.h = gtk_range_get_value((GtkRange*)main_widgets->g_scl_hue);
	hsvToRGB(&hsvcolor, &c);
	sprintf(text, "#%02X%02X%02X", c.r, c.g, c.b);
	
	gtk_entry_set_text((GtkEntry*)main_widgets->g_entry_hue, text);
}

void on_window_main_destroy() {
	gtk_main_quit();
	exit(0);
}

void mp_data_init(mp_data * oc) {
	oc->key = -1;
	oc->use_floofers = -1;
	oc->use_socks = -1;
	oc->male = 0;
	oc->desaturated = -1;
	oc->white = -1;
	oc->detail_count = -1;
	oc->wheel = -1;
	oc->traditional = 0;
	oc->verbose = 0;
	oc->hue = -1;
}

void mp_pick_colors(mp_data * oc) {
	srand(oc->seed);
	
	int clrcount;
	if (oc->key == -1) {
		oc->key = rand() % STYLECOUNT;
		if (oc->traditional) {
			oc->key %= 9;
		}
	} else {
		rand();
	}
	clrcount = COLOR(styles[oc->key]);
	
	fprintf(sterr, "%d: %s\n", oc->seed, UMANE(styles[oc->key]));


	//color * colors;
	oc->colors = malloc(sizeof(*oc->colors)*7);
	
	color color1;
	
	hsv hsvcolor;

	if (oc->hue == -1) {
		oc->hue = rand() % 360;
		hsvcolor.h = oc->hue;
		hsvcolor.s = 1.0f;
		hsvcolor.v = 1.0f;
		hsvToRGB(&hsvcolor, &color1);
	}
	else {
		rand();
	}

	if (oc->male == -1) 
		oc->male = ((oc->hue % 180 < 60) && ((oc->key == BIGMAC) || ((oc->key == ADVENTUROUS) && (rand() & 3))));

	if (oc->verbose) {
		fprintf(sterr, "color1: 0x%02X%02X%02X\n", color1.r, color1.g, color1.b);
		fprintf(sterr, "gender: %s\n", (oc->male) ? "male" : "female");
	}

	// if (!(rand() & 7)) {
	// if (rand() & 0x1) {
	// static_hue = (static_hue + 150) % 360;
	// }
	// else {
	// static_hue = (static_hue + 210) % 360;
	// }
	// }

	float avg_sat = 0.0f;
	float std_dev = 0.0f;

	if (oc->desaturated == -1) {
		oc->desaturated = !(rand() & 15) ;
		oc->white = oc->desaturated && !(rand() & 3);
		oc->desaturated = oc->desaturated ^ oc->white;
		if (oc->traditional) {
			oc->desaturated = 0;
		}
	}
	else {
		rand();
		rand();
	}
	int any_saturation = !oc->desaturated;

	if (oc->verbose) {
		fprintf(sterr, "body is %ssaturated\n", (oc->desaturated) ? "de" : "");
	}

	int r;
	int r2;
	if (oc->use_socks == -1) {
		oc->use_socks = !(rand() % 6);
	}
	else {
		rand();
	}
	if (oc->use_socks && clrcount < 2) {
		clrcount = 2;
	}

	for (int i = 0; i < clrcount; i++) { //get hair colors
		hsvcolor.h = oc->hue;
		hsvcolor.s = 1;//saturation(&colors[0]);
		hsvcolor.v = 1;//value(&colors[0]);

		r = rand();
		r2 = rand();

		if (oc->wheel) {
			if (oc->wheel == 1 || oc->wheel == -1) { // analogous (1/4 if set, 1/8 if not)
				if (!(r & 7)) {
					hsvcolor.h += 360/12;
				}
				else if (!((r & 7) - 1)) {
					hsvcolor.h += 360*11/12;
				}
			}
			else if (oc->wheel == 2) { // rectangular
				if (r2 & 1) {
					if (!(r & 7)) {
						hsvcolor.h += 360/12*2;
					}
					/*else if (!((r & 7) - 1)) {
						hsvcolor.h += 8*360/12;
					}*/
				}
				else { // reverse rectangular
					if (!((r & 7) - 1)) {
						hsvcolor.h += 4*360/12;
					}
					/*else if (!(r & 7)) {
						hsvcolor.h += 10*360/12;
					}*/
				}
			}
			hsvcolor.h %= 360;
		}

		/*if (separate_hair && (r & 7)) {
			float del = ((r % 100) + (r2 % 100)) / 200.0f;
			del = del * 2.0f - 1.0f;
			if (oc->verbose) {
				fprintf(sterr, "\t\tdel: %g\n", del);
			}
			hsvcolor.h = (int)(static_hue + 30 * del + 360) % 360;
		}*/
		if (oc->key == BOOKWORM && i == 1) {
			oc->colors[i] = oc->colors[0];
		}
		else if (r2 & (1/*  + 2 * (hsvcolor.h != static_hue) */)) {
			hsvcolor.s = (!oc->desaturated) ? (rand() & 15) / 15.0f : 0.0f;
			if (oc->desaturated) {
				hsvcolor.v = (!oc->desaturated) ? sqrtf((rand() % 12 + 4) / 15.0f) : sqrtf((rand() & 15) / 15.0f);
			}
		}
		else {
			hsvcolor.v = (!oc->desaturated) ? sqrtf((rand() % 12 + 4) / 15.0f) : sqrtf((rand() & 15) / 15.0f);
			any_saturation = (1 & (hsvcolor.v > 0.0f)) | any_saturation;
		}

		avg_sat += ((!oc->desaturated) * hsvcolor.s + (oc->desaturated) * hsvcolor.v) / pow(2,i);
		hsvToRGB(&hsvcolor,&oc->colors[i]);

		if (oc->verbose) {
			fprintf(sterr, "\tcolors[%d]: 0x%02X%02X%02X\t HSV(%d,%g,%g)\n", i, oc->colors[i].r, oc->colors[i].g, oc->colors[i].b, hsvcolor.h, hsvcolor.s, hsvcolor.v); 
		}
	}

	// avg_sat /= (float)clrcount;
	avg_sat /= 2.0f;

	// Get a value from each mane color to help decide on a body color
	for (int i = 0; i < clrcount; i++) {
		float x;
		// x = (desaturated) * value(&colors[i]) + (!desaturated) * saturation(&colors[i]) - avg_sat;
		x = (((!oc->desaturated) * saturation(&oc->colors[i]) + (oc->desaturated) * value(&oc->colors[i])) / pow(2,i)) - avg_sat / pow(2,i);
		if (oc->verbose) {
			fprintf(sterr, "x[%d] = %g\tvalue: %g\tsat: %g\n", i, x, value(&oc->colors[i]), saturation(&oc->colors[i]));
		}
		x *= x;
		std_dev += x;
	}
	std_dev = sqrtf(std_dev);
	if (oc->verbose) {
		fprintf(sterr,"avg_sat: %g\tstd_dev: %g\n", avg_sat, std_dev);
	}

	//color bodycolor;
	if (!oc->white) {
		hsvcolor.h = oc->hue; // * (0.7 + 0.3 * lightness(&color1)))
		// hsvcolor.s = (!desaturated) ? (avg_sat * 0.8f * (0.4f + 0.6f * fmodf(fabs(-std_dev*1.5f + 1.25f), 1.0f))) : 0.0f;// * !(!(rand()&7));
		// hsvcolor.s = (!desaturated) ? (avg_sat * 0.8f * (0.4f + std_dev * 0.6f)) : 0.0f;// * !(!(rand()&7));
		hsvcolor.s = (!oc->desaturated) ? (avg_sat * sqrt(std_dev)) : 0.0f;// * !(!(rand()&7));
		hsvcolor.v = (oc->desaturated) ? (avg_sat * sqrtf(0.3f + std_dev*0.7f)) : 1.0f;//((rand() % 12) + 4) / 15.0f : 1.0f;
		// c = color1;
		// if (!desaturated) {
		// hsvToRGB(&hsvcolor, &bodycolor);
		// float l = lightness(&bodycolor);
		// l *= l;
		// hsvcolor.s = avg_sat - 0.3f*l;
		// }
		hsvcolor.s = ((float)(hsvcolor.s >= 0)) * hsvcolor.s;
	}
	else {
		hsvcolor.h = 0;
		hsvcolor.s = 0;
		hsvcolor.v = 1;
	}
	hsvToRGB(&hsvcolor, &oc->body_color);


	int details_in_use = 0;

	oc->details = malloc(sizeof(*oc->details)*8);
	for (int i = 0; i < 8; i++) {
		oc->details[i] = malloc(sizeof(**oc->details)*16);
		strcpy(oc->details[i], "NONE");
	}

	char ** available_details = malloc(sizeof(*available_details)*16);
	for (int i = 0; i < 16; i++) {
		available_details[i] = malloc(sizeof(**available_details)*32);
		available_details[i][0] = 0;
	}

	oc->detail_color = malloc(sizeof(*oc->detail_color)*8);
	for (int i = 0; i < 8; i++) {
		oc->detail_color[i].r = 255;
		oc->detail_color[i].g = 255;
		oc->detail_color[i].b = 255;
	}
	oc->use_floofers = (!(rand() % 6)) * (!oc->traditional) * (!oc->use_socks);

	if (!oc->traditional) {
		at = 0;
		PUSHSTRING(available_details, "GRADIENT");
		// PUSHSTRING(available_details, "HOOF_SMALL");
		PUSHSTRING(available_details, "STRIPES");
		PUSHSTRING(available_details, "LINES");
		PUSHSTRING(available_details, "FRECKLES");
		PUSHSTRING(available_details, "MUZZLE");

		// generate colors if using floofers
		if (oc->use_floofers) {
			strcpy(oc->details[details_in_use], "GRADIENT");
			if (any_saturation) {
				oc->detail_color[details_in_use] = color1;
			}
			else {
				oc->detail_color[details_in_use] = oc->colors[0];
			}
			details_in_use += 1;
			strcpy(oc->details[details_in_use], "HOOF_SMALL");
			oc->detail_color[details_in_use] = oc->colors[1 + (oc->key == BOOKWORM || oc->key == SPEEDSTER)];
			details_in_use += 1;
		}
		else {
			PUSHSTRING(available_details, "HOOF_SMALL");
			PUSHSTRING(available_details, "GRADIENT");
			PUSHSTRING(available_details, "HOOF_ROUND_SMALL");
			PUSHSTRING(available_details, "SHARP_HOOVES");
		}

		// generate colors for other details
		for (int i = 0 + 2 * oc->use_floofers; i < DETAILCOUNT; i++) {
			if (!(rand() % DETAILCHANCE)) {
				strcpy(oc->details[details_in_use], available_details[i]);
				oc->detail_color[details_in_use] = oc->colors[rand() % clrcount];
				if (oc->verbose) {
					fprintf(sterr, "detail_color[%d]: 0x%02X%02X%02X\tdetails[%d]: %s\n", 
						details_in_use, 
						oc->detail_color[details_in_use].r, 
						oc->detail_color[details_in_use].g, 
						oc->detail_color[details_in_use].b, 
						details_in_use, 
						oc->details[details_in_use]);
				}
				details_in_use += 1;
			}
		}

	}
}

	// -- BEGIN SYNTHESIZNIG DATA --
int mp_construct_nbt(mp_data * oc, unsigned char * data) {
	hsv hsvcolor;
	color color1;
	hsvcolor.h = oc->hue;
	hsvcolor.s = 1;
	hsvcolor.v = 1;
	hsvToRGB(&hsvcolor, &color1);
	
	
	int data_len;
	data_len = 0;

	char temp[255];
	char s[255];
	int s_len = 0;

	//strcpy(data, "\n{\n");
	*(data) = NBT_GROUP;
	data_len ++;
	*(data + data_len) = 0x00;
	data_len++;
	*(data + data_len) = 0x04;
	data_len++;
	strcpy((char *)(data+data_len), "data");
	data_len += strlen("data");

	for (int i = 0; i < TARGETCOUNT; i++) {
		char * cur = target[i];
		s[0] = 0;
		if (cur[0] == SEP) {
			s_len = addSeparator(cur+1, s);
			putchar('x');
		}
		else if (cur[0] == COL) {
			color c;

			if (!strcmp(cur+1,"body")) {
				// hsvcolor.h = static_hue;
				// hsvcolor.s = (!desaturated) ? (avg_sat * 0.8f * sqrtf(0.5f + std_dev*0.5f) * (0.7 + 0.3 * lightness(&color1))) : 0.0f;// * !(!(rand()&7));
				// hsvcolor.v = (desaturated) ? (avg_sat * 0.8f * sqrtf(0.5f + std_dev*0.5f) * (0.7 + 0.3 * lightness(&color1))) : 1.0f;//((rand() % 12) + 4) / 15.0f : 1.0f;
				// // c = color1;
				// hsvToRGB(&hsvcolor, &c);
				c = oc->body_color;
			}
			else if (strstr(cur+1, "body_detail")) {
				int num = atoi(RIGHT(cur,1));
				c = oc->detail_color[num-1];
			}
			else if (strstr(cur+1, "_phong_")) {
				c.r = 1;
				c.g = 1;
				c.b = 1;
			}
			else if (strstr(cur+1, "socks_new_model")) {
				if (strstr(cur+1, "1")) {
					c = oc->colors[0];
				}
				else if (strstr(cur+1, "2")) {
					c = oc->colors[1];
				}
				else {
					c.r = 255;
					c.g = 255;
					c.b = 255;
				}
			}
			else if (strstr(cur+1, "horn") && oc->desaturated) {
				c = oc->colors[0];
			}
			else if (strstr(cur+1, "mane_") == cur + 1 || strstr(cur+1, "tail_") == cur + 1) {
				//printf("%s\n", RIGHT(cur+1,1));
				int num = 2 * (strstr(cur+1, "detail_") != NULL);
				num += atoi(RIGHT(cur,1));

				// if ((key == BIGMAC) && (!strcmp(cur+1, "tail_detail_color_1"))) {
				// num = 2;
				// }
				switch (oc->key) {
					case BIGMAC:
					case BUMPKIN:
					case INSTRUCTOR:
					case ASSERTIVE:
						if (strstr(cur+1, "_detail_color_")) {
							num -= 1;
						}
						else if (oc->key == INSTRUCTOR && strstr(cur+1, "mane_color_2")){
							num -= 1;
						}
					break;
				}

				c = oc->colors[num-1];

				if (!strcmp(cur+1, "mane_color_2")) {
					switch (oc->key) {
						case BOOKWORM:
						case ASSERTIVE:
						case BUMPKIN:
							c = oc->colors[0];
						break;
					}
				}
				if (strstr(cur+1, "_color_2") && (oc->key == TIMID) && !strstr(cur+1, "detail")) {
					c = oc->colors[4];
				}

				hsvcolor.h = hue(&c);
				hsvcolor.s = saturation(&c);
				hsvcolor.v = value(&c);
				hsvToRGB(&hsvcolor, &c);
			}
			else if (strstr(cur+1, "eye_") == cur + 1) {
				if (strstr(cur+1, "effect") || strstr(cur+1, "hole")) {
					c.r = 0;
					c.g = 0;
					c.b = 0;
				}
				// else if (strstr(cur+1, "reflection")) {
				// c.r = 255;
				// c.g = 255;
				// c.b = 255;
				// }
				else if (strstr(cur+1, "iris1")) {
					if (oc->desaturated) {
						hsvcolor.h = 0;
						hsvcolor.s = 0.0f;
						hsvcolor.v = 0.5f * (1.0f-value(&oc->body_color));
						hsvToRGB(&hsvcolor, &c);
					}
					else {
						c = color1;
					}
				}
				else if (strstr(cur+1, "iris2")) {
					c = oc->body_color;
				}
				else if (strstr(cur+1, "irisline2")) {
					c = color1;
					hsvcolor.h = hue(&c);
					hsvcolor.s = 0.1f * !oc->desaturated;
					hsvcolor.v = 0.9f + 0.1 * !oc->desaturated;
					hsvToRGB(&hsvcolor, &c);
				}
				else if (strstr(cur+1, "irisline1")) {
					c = color1;
					hsvcolor.h = hue(&c);
					hsvcolor.s = 0.5f * !oc->desaturated;
					hsvcolor.v = 0.5f + 0.5f * !oc->desaturated;
					hsvToRGB(&hsvcolor, &c);
				}
				else {
					c.r = 255;
					c.g = 255;
					c.b = 255;
				}
			}
			else if (strstr(cur+1, "mouth_")) {
				hsvcolor.h = 327;
				hsvcolor.s = 0.39f;
				hsvcolor.v = 1;
				hsvToRGB(&hsvcolor, &c);
			}
			else if (strstr(cur+1, "tongue_")) {
				/*hsvcolor.h = (static_hue + 45 ) % 360;
				*/
				hsvcolor.h = 27;
				hsvcolor.s = 1;
				hsvcolor.v = 1;
				hsvToRGB(&hsvcolor, &c);
			}
			else if (strstr(cur+1, "eyelash") || strstr(cur + 1, "eyebrow")) {
				hsvcolor.h = 0;
				hsvcolor.s = 0;
				hsvcolor.v = 0;
				hsvToRGB(&hsvcolor, &c);
			}
			else if (strstr(cur+1, "teeth")) {
				c.r = 255;
				c.g = 255;
				c.b = 255;
			}

			s_len = addColor(cur+1, &c, 1, s);
		}
		else if (target[i][0] == STR) {
			strcpy(temp, "NONE");

			if (strstr(cur+1, "url")) {
				temp[0] = 0;
			}
			else if (strstr(cur+1, "mane_") == cur + 1) {
				strcpy(temp, UMANE(styles[oc->key]));
			}
			else if (strstr(cur+1, "manelower_") == cur + 1) {
				strcpy(temp, LMANE(styles[oc->key]));
			}
			else if (strstr(cur+1, "tail_") == cur + 1) {
				strcpy(temp, TAIL(styles[oc->key]));
			}
			else if (!strcmp(cur+1, "race")) {
				strcpy(temp, races[rand() % RACES]);
			}
			else if (strstr(cur+1, "body_detail")) {
				int num = atoi(RIGHT(cur,1));
				strcpy(temp, oc->details[num-1]);
			}
			else if (!strcmp(cur+1, "gender")) {
				strcpy(temp, (oc->male) ? "ADULT" : "FILLY");
			}
			else if (!strcmp(cur+1, "eyelash") && !oc->male) {
				strcpy(temp, "DEFAULT");
			}
			else if (strstr(cur+1, "lightwarp")) {
				strcpy(temp, "SFM_PONY");
			}

			s_len = addString(cur+1, temp, 1, s);
		}
		else if (target[i][0] == BOOL) {
			int x = 0;

			x = (strstr(cur+1, "inherit") != NULL);
			x = (strstr(cur+1, "lines") != NULL) || x;
			x = (strstr(cur+1, "hoof_") && oc->use_floofers) || x;
			x = (strstr(cur+1, "weapon_hide")) || x;
			x = (strstr(cur+1, "new_male_muzzle")) || x;
			x = (strstr(cur+1, "socks_model_new") && oc->use_socks) || x;

			s_len = addBool(cur+1, x, 1, s);
		}
		else if (target[i][0] == VAL || target[i][0] == INT) {
			float x = 1.0f;

			if (!strcmp(cur+1, "hoof_fluffers_strength")) {
				x = 0.8f;
			}
			else if (strstr(cur+1, "bump")) {
				x = 0.0f;
			}
			else if (strstr(cur+1,"_phong_")) {
				if (strstr(cur+1,"_boost")) {
					x = 0.09f;
				}
				else if (strstr(cur+1,"_middle")) {
					x = 5.0f;
				}
				else if (strstr(cur+1,"_exponent")) {
					x = 3.0f;
				}
				else if (strstr(cur+1,"_front")) {
					x = 1.0f;
				}
				else if (strstr(cur+1,"_sliding")) {
					x = 10.0f;
				}
			}
			else if (strstr(cur+1, "_wing_")) {
				x = 0.0f;
				if (strstr(cur+1, "_size")) {
					x = 1.0f;
				}
			}
			else if (strstr(cur+1, "eye") && !strstr(cur+1, "hole")) {
				x = 1;
				if (strstr(cur + 1, "rotation")) {
					x = 0;
				}
			}
			else if (strstr(cur+1, "hole")) {
				x = 0.75f;
				if (strstr(cur+1, "shift")) {
					x = 0;
				}
				else if (strstr(cur+1, "width") || strstr(cur+1, "height")) {
					x = 1;
				}
			}
			else if (strstr(cur+1, "rotat")) {
				x = 0;
			}

			if (target[i][0] == VAL)
				s_len = addValue(cur+1, x, 1, s);
			else
				s_len = addInt(cur+1, (int)x, 1, s);
		}
		memcpy(data + data_len, s, s_len);
		data_len += s_len;
	}
	//data[strlen(data)-2] = 0;
	//strcat(data, "\n}\n");
	*(data + data_len) = 0x00;
	data_len++;
	*(data + data_len) = 0x0a;
	data_len++;
	
	return data_len;
}
