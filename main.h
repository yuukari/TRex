#include <libbip.h>

#ifndef __TREX__
#define __TREX__

/* Sprite indexes */

#define SPRITE_DINO_IDLE 0
#define SPRITE_DINO_STEP_1 1
#define SPRITE_DINO_STEP_2 2
#define SPRITE_DINO_JUMP 0
#define SPRITE_DINO_DAMAGED 3

#define SPRITE_NIGHT_DINO_IDLE 33
#define SPRITE_NIGHT_DINO_STEP_1 20
#define SPRITE_NIGHT_DINO_STEP_2 21
#define SPRITE_NIGHT_DINO_JUMP 33
#define SPRITE_NIGHT_DINO_DAMAGED 22

#define SPRITE_CACTUS_BIG 4
#define SPRITE_CACTUS_SMALL 5
#define SPRITE_CACTUS_BIG_PRESHOW 15
#define SPRITE_CACTUS_SMALL_PRESHOW 16

#define SPRITE_NIGHT_CACTUS_BIG 23
#define SPRITE_NIGHT_CACTUS_SMALL 24
#define SPRITE_NIGHT_CACTUS_BIG_PRESHOW 28
#define SPRITE_NIGHT_CACTUS_SMALL_PRESHOW 29

#define SPRITE_BIRD_FLY_1 17
#define SPRITE_BIRD_FLY_2 18
#define SPRITE_BIRD_PRESHOW 19

#define SPRITE_NIGHT_BIRD_FLY_1 30
#define SPRITE_NIGHT_BIRD_FLY_2 31
#define SPRITE_NIGHT_BIRD_PRESHOW 32

#define SPRITE_CLOUD 6
#define SPRITE_NIGHT_CLOUD 25

#define SPRITE_GROUND 8
#define SPRITE_NIGHT_GROUND 27

#define SPRITE_RESTART_BUTTON 7
#define SPRITE_NIGHT_RESTART_BUTTON 26

/* ENUMs */

#define JUMP_SMALL 0
#define JUMP_BIG 1
#define JUMP_FALLING 2

#define CACTUS_SMALL 0
#define CACTUS_BIG 1

#define SCREEN_START 0
#define SCREEN_GAME 1
#define SCREEN_SETTINGS 2
#define SCREEN_PAUSE 3
#define SCREEN_GAME_OVER 4

#define MENU_ITEM_PLAY 0
#define MENU_ITEM_SETTINGS 1

#define MAP_TYPE_CLASSIC 0
#define MAP_TYPE_DAY 1
#define MAP_TYPE_NIGHT 2

#ifdef BipEmulator
	#define UPDATE_SCREEN_SPEED 110
#else
	#define UPDATE_SCREEN_SPEED 85
#endif

struct app_data_t
{
	void* ret_f;
	Elf_proc_* proc;

	unsigned short current_screen;
	unsigned short menu_item;
	unsigned short vibration;

	unsigned int score;
	unsigned int hi_score;
	unsigned short is_record;

	unsigned short speed;
	unsigned int speed_counter;

	unsigned short map_type;
	unsigned short is_night;
	unsigned int night_counter;
	unsigned short night_text_delay;
	unsigned short night_text_show;

	unsigned int current_sprite;
	unsigned int is_jump;
	unsigned int jump_state;
	int jump_shift;
	int jump_force;
	unsigned short is_damaged;

	int cactus_x;
	unsigned short is_cactus_show;
	unsigned short cactus_preshow;
	unsigned short cactus_type;

	int cloud_x;
	unsigned int cloud_y;

	int bird_x;
	unsigned int bird_y;
	unsigned short is_bird_show;
	unsigned short bird_preshow;
	unsigned short bird_frame;
	unsigned short bird_sptite;

	int ground_x;

	unsigned int pause_counter;
	unsigned int pause_frame;
};

struct save_data_t {
	unsigned int score;
	unsigned short vibration;
	unsigned short map_type;
};

void 	init_app(void* return_screen);
void 	set_initial_values();
void 	show_screen();
void	touch_handler(void* gesture);
void 	keypress_handler();
void 	update_screen();

void	start_game();
void 	handle_collision();
void 	draw_start_screen();
void	draw_game_screen();
void	draw_pause_screen();
void	draw_game_over_screen();
void	draw_settings_screen();

#endif