#ifndef MAKE_PONY_H
#define MAKE_PONY_H

#include <gtk/gtk.h>
#include <stdint.h>
#include "nbt.h"
#include "color.h"
#include "pngimg.h"

#define STYLECHARLEN	12
// #define TARGETCOUNT  	70
#define DETAILCHANCE  	15
#define DETAILCOUNT  	4

#define PI  	3.14159f
#define RAD(X)	PI * (X) / 180.0f

#define SECONDSPERDAY	86400

#define UMANE(X)	((X) + 0)
#define LMANE(X)	((X) + STYLECHARLEN)
#define TAIL(X) 	((X) + STYLECHARLEN*2)
#define COLOR(X)	*((X) + STYLECHARLEN*3)

#define STYLECOUNT  	17

#define ADVENTUROUS	0
#define SHOWBOAT	1
#define BOOKWORM	2
#define SPEEDSTER	3
#define ASSERTIVE	4
#define RADICAL	5
#define BUMPKIN	6
#define FLOOFEH	7
#define INSTRUCTOR	8
#define FILLY	9
#define MOON	10
#define TIMID	11
#define BIGMAC	12
#define BOLD	13
#define MECHANIC	14
#define SHOWBOAT2	15
#define MAILCALL	16

#define RACES	3

int at;
// #define PUSHTARGET(R,X,T)	sprintf((R)[at], "%c%s", (T), (X)); at++
#define PUSHSTRING(R,X)	strcpy((R)[at], (X)); at++
#define ADDSTYLE(R, I, S, T, U, C)	strcpy(UMANE((R)[(I)]), (S)); strcpy(LMANE((R)[(I)]), (T)); strcpy(TAIL((R)[(I)]), (U)); COLOR((R)[(I)]) = (C)
#define RIGHT(X,C)	((X)+strlen(X)-(C))
#define WARM(H)	((H) <= 120.0f || (H) > 300.0f)


/*	2.0 upgrade definitions	*/

char ** styles;
char ** races;
char ** target;
uint8_t * data;
int gtk_data_len;
FILE * sterr;
int log_pipe[2];

PNGIMG * preview;
int preview_seed;
GdkPixbuf * preview_buf;
GdkRGBA * clrbtn_hue_color;

typedef struct {
	int seed;
	int use_floofers;
	int use_socks;
	int use_old_colors;
	int eyes_always_color;
	int male;
	int invert;
	int desaturated;
	int white;
	int any_saturation;
	int key; // style
	int detail_count;
	char ** details;
	int hue;
	int wheel;
	int traditional;
	int verbose;
	color * colors;
	color * detail_color;
	color body_color;
} mp_data;

void mp_pick_colors(mp_data * oc);
int mp_construct_nbt(mp_data * oc, unsigned char * data);
void mp_write(mp_data * oc, char * filename);
void update_txt_log();

typedef struct {
	GtkWidget * g_fch_outdir;
	GtkWidget * g_btn_generate;
	GtkWidget * g_btn_seed_random;
	GtkWidget * g_btn_seed_now;
	GtkWidget * g_btn_save;
	GtkWidget * g_entry_seed;
	GtkWidget * g_txt_log;
	GtkWidget * g_img_preview;
	GtkWidget * g_cmb_wheel;
	GtkWidget * g_cmb_saturation;
	GtkWidget * g_cmb_style;
	GtkWidget * g_cmb_socks;
	GtkWidget * g_cmb_male;
	GtkWidget * g_chk_hue;
	GtkWidget * g_chk_old_colors;
	GtkWidget * g_chk_eyes_always_color;
	GtkWidget * g_chk_traditional;
	GtkWidget * g_chk_invert;
	GtkWidget * g_chk_preview;
	GtkWidget * g_chk_verbose;
	GtkWidget * g_chk_thumbnail;
	GtkWidget * g_scl_hue;
	GtkWidget * g_entry_hue;
	GtkWidget * g_entry_thumbnail_dir;
	GtkWidget * g_scrl_log;
	GtkWidget * g_clrbtn_hue;
	
	GtkBuilder * builder;
	GtkWidget * window;
} mp_gtk_widgets;

mp_gtk_widgets * main_widgets;
mp_data * main_oc;

void mp_gtk_build(int * argc, char ** argv[], mp_gtk_widgets ** widgets);
void mp_gtk_destroy(mp_gtk_widgets * widgets);
void mp_data_init(mp_data * oc);
int render_oc(PNGIMG * img, uint8_t * nbt, int nbt_len, mp_data * oc);

#endif
