#include "target.h"

void importTargetsSimple(char ** target) {
	target_at = 0;
	
	PUSHTARGET(target, "body", COL);
	PUSHTARGET(target, "mane_color_1", COL);
	PUSHTARGET(target, "mane_color_2", COL);
	PUSHTARGET(target, "mane_detail_color_1", COL);
	PUSHTARGET(target, "mane_detail_color_2", COL);
	PUSHTARGET(target, "mane_detail_color_3", COL);
	PUSHTARGET(target, "mane_detail_color_4", COL);
	PUSHTARGET(target, "mane_detail_color_5", COL);
	PUSHTARGET(target, "tail_color_1", COL);
	PUSHTARGET(target, "tail_color_2", COL);
	PUSHTARGET(target, "tail_detail_color_1", COL);
	PUSHTARGET(target, "tail_detail_color_2", COL);
	PUSHTARGET(target, "tail_detail_color_3", COL);
	PUSHTARGET(target, "tail_detail_color_4", COL);
	PUSHTARGET(target, "tail_detail_color_5", COL);
	PUSHTARGET(target, "horn_detail_color", COL);
	PUSHTARGET(target, "body_detail_1", STR);
	PUSHTARGET(target, "body_detail_2", STR);
	PUSHTARGET(target, "body_detail_3", STR);
	PUSHTARGET(target, "body_detail_4", STR);
	PUSHTARGET(target, "body_detail_5", STR);
	PUSHTARGET(target, "body_detail_6", STR);
	PUSHTARGET(target, "body_detail_7", STR);
	PUSHTARGET(target, "body_detail_8", STR);
	PUSHTARGET(target, "body_detail_color_1", COL);
	PUSHTARGET(target, "body_detail_color_2", COL);
	PUSHTARGET(target, "body_detail_color_3", COL);
	PUSHTARGET(target, "body_detail_color_4", COL);
	PUSHTARGET(target, "body_detail_color_5", COL);
	PUSHTARGET(target, "body_detail_color_6", COL);
	PUSHTARGET(target, "body_detail_color_7", COL);
	PUSHTARGET(target, "body_detail_color_8", COL);
	PUSHTARGET(target, "eye_iris1", COL);
	PUSHTARGET(target, "eye_iris2", COL);
	PUSHTARGET(target, "eye_irisline1", COL);
	PUSHTARGET(target, "eye_irisline2", COL);
	PUSHTARGET(target, "eye_reflection", COL);
	PUSHTARGET(target, "eye_effect", COL);
	PUSHTARGET(target, "eye_type", STR);
	PUSHTARGET(target, "eye_derp", BOOL);
	PUSHTARGET(target, "eye_lines", BOOL);
	PUSHTARGET(target, "cmark", BOOL);
	PUSHTARGET(target, "lips_color_inherit", BOOL);
	PUSHTARGET(target, "nose_color_inherit", BOOL);
	PUSHTARGET(target, "separate_horn", BOOL);
	PUSHTARGET(target, "socks_model", BOOL);
	PUSHTARGET(target, "socks", BOOL);
	PUSHTARGET(target, "socks_model_new", BOOL);
	PUSHTARGET(target, "socks_model", BOOL);
	PUSHTARGET(target, "gender", STR);
	PUSHTARGET(target, "suit", STR);
	PUSHTARGET(target, "mane_new", STR);
	PUSHTARGET(target, "manelower_new", STR);
	PUSHTARGET(target, "tail_new", STR);
	PUSHTARGET(target, "race", STR);
	PUSHTARGET(target, "eyelash", STR);
	PUSHTARGET(target, "body_lightwarp_texture", STR);
	PUSHTARGET(target, "weight", VAL);
	PUSHTARGET(target, "necksize", VAL);
	PUSHTARGET(target, "tailsize", VAL);
	PUSHTARGET(target, "legssize", VAL);
	PUSHTARGET(target, "ponysize", VAL);
	PUSHTARGET(target, "body_phong_boost", VAL); //0.09
	PUSHTARGET(target, "body_phong_middle", VAL); //5
	PUSHTARGET(target, "body_phong_exponent", VAL); //3
	PUSHTARGET(target, "body_phong_front", VAL); //1
	PUSHTARGET(target, "body_phong_sliding", VAL); //10
	PUSHTARGET(target, "body_phong_tint", COL);
	PUSHTARGET(target, "hoof_fluffers", BOOL);
	PUSHTARGET(target, "hoof_fluffers_strength", VAL);
}

