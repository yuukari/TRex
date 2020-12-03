#include <libbip.h>

#ifndef __TREX__
#define __TREX__

#define SPRITE_DINO_IDLE 0
#define SPRITE_DINO_STEP_1 1
#define SPRITE_DINO_STEP_2 2
#define SPRITE_DINO_JUMP 3

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

#define UPDATE_SCREEN_SPEED 85

struct app_data_t
{
	void* ret_f;
	Elf_proc_* proc;

	unsigned int current_screen;
	unsigned int menu_item;
	unsigned int vibration;

	unsigned int score;
	unsigned int hi_score;

	unsigned int speed;
	unsigned int speed_counter;

	unsigned int current_sprite;
	unsigned int is_jump;
	unsigned int jump_state;
	int jump_shift;
	int jump_force;
	unsigned int is_damaged;

	unsigned int is_cactus_show;
	unsigned int cactus_preshow;
	unsigned int cactus_type;
	int cactus_x;

	int cloud_x;
	unsigned int cloud_y;

	int ground_x;

	unsigned int pause_counter;
	unsigned int pause_frame;
};

struct save_data_t {
	unsigned int score;
	unsigned int vibration;
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