#include "main.h"

unsigned short dino_collider_top[20] = { 28, 24, 22, 20, 20, 22, 24, 26, 26, 28, 41, 43, 43, 43, 43, 43, 43, 43, 43, 41 };
unsigned short dino_collider_bottom[20] = { 27, 29, 31, 33, 35, 43, 43, 37, 35, 37, 43, 43, 31, 28, 23, 23, 15, 15, 11, 11 };

unsigned short cactus_big_collider[12] = { 34, 34, 34, 34, 46, 46, 46, 46, 36, 36, 36, 36 };
unsigned short cactus_small_collider[8] = { 24, 24, 33, 33, 33, 28, 28, 28 };

unsigned short bird_fly_1_collider_top[21] = { 22, 24, 26, 28, 30, 30, 26, 36, 34, 32, 30, 28, 26, 24, 20, 18, 18, 18, 18, 18, 18 };
unsigned short bird_fly_1_collider_bottom[21] = { 16, 16, 16, 16, 16, 16, 18, 20, 22, 24, 26, 26, 26, 26, 26, 26, 26, 24, 24, 24, 20 };
unsigned short bird_fly_2_collider_top[21] = { 22, 24, 26, 28, 30, 30, 26, 22, 22, 22, 22, 22, 22, 22, 20, 18, 18, 18, 18, 18, 18 };
unsigned short bird_fly_2_collider_bottom[21] = { 16, 16, 16, 16, 16, 16, 18, 20, 36, 34, 30, 28, 26, 26, 26, 26, 26, 24, 24, 24, 20 };

struct regmenu_ screen_data = { 55,	1, 0, touch_handler, keypress_handler, update_screen, 0, show_screen, 0, 0 };

struct app_data_t** app_data_p;
struct app_data_t* app_data;

struct save_data_t data;
struct save_data_t* data_p;

//#define DEBUG_COLLISION

int main(int return_screen, char** argv) {
	init_app((void*)return_screen);
}

void init_app(void* return_screen) {
	#ifdef BipEmulator
		app_data_p = (app_data_t**)get_ptr_temp_buf_2();
	#else
		app_data_p = get_ptr_temp_buf_2();
	#endif

	if ((return_screen == *app_data_p) && get_var_menu_overlay()) {
		app_data = *app_data_p;
		#ifdef BipEmulator
			*app_data_p = (app_data_t*)NULL;
		#else
			*app_data_p = NULL;
		#endif
		reg_menu(&screen_data, 0);
		*app_data_p = app_data;
	} else {
		reg_menu(&screen_data, 0);
		*app_data_p = (struct app_data_t*)pvPortMalloc(sizeof(struct app_data_t));
		app_data = *app_data_p;
		_memclr(app_data, sizeof(struct app_data_t));

		#ifdef BipEmulator
			app_data->proc = (Elf_proc_*)return_screen;
		#else
			app_data->proc = return_screen;
		#endif
		if (return_screen && app_data->proc->elf_finish)
			app_data->ret_f = app_data->proc->elf_finish;
		else
			app_data->ret_f = show_watchface;

		set_initial_values();
	}

	show_screen();
}

void set_initial_values() {
	app_data->current_screen = SCREEN_START;
	app_data->menu_item = MENU_ITEM_PLAY;

	data_p = &data;
	ElfReadSettings(app_data->proc->index_listed, data_p, 0, sizeof(data));

	app_data->hi_score = data_p->score;
	app_data->vibration = data_p->vibration;
	app_data->map_type = data_p->map_type;

	_srand(get_tick_count());
}

void show_screen() {
	load_font();

	set_display_state_value(8, 1);
	set_display_state_value(4, 1);
	set_display_state_value(2, 1);

	draw_start_screen();

	set_update_period(1, UPDATE_SCREEN_SPEED);
}

void keypress_handler() {
	if (app_data->current_screen == SCREEN_GAME) {
		app_data->current_screen = SCREEN_PAUSE;
		app_data->pause_counter = 0;
		app_data->pause_frame = 0;

		draw_pause_screen();

		set_update_period(1, UPDATE_SCREEN_SPEED);
	} else if (app_data->current_screen == SCREEN_SETTINGS) {
		app_data->current_screen = SCREEN_START;
		draw_start_screen();
	} else {
		set_display_state_value(8, 0);
		set_display_state_value(4, 0);
		set_display_state_value(2, 0);

		show_menu_animate(app_data->ret_f, (unsigned int)show_screen, ANIMATE_RIGHT);
	}
};

void update_screen() {
	switch (app_data->current_screen) {
		case SCREEN_GAME:
			if (app_data->is_jump) {
				if (app_data->jump_state != JUMP_FALLING) {
					app_data->jump_shift += app_data->jump_force;
					app_data->jump_force = app_data->jump_force / 2;

					if (app_data->jump_state == JUMP_SMALL) {
						if (app_data->jump_force <= 5) {
							if (app_data->jump_force == 0)
								app_data->jump_force = 1;

							app_data->jump_force = app_data->jump_force * -1;
							app_data->jump_state = JUMP_FALLING;
						}
					} else {
						if (app_data->jump_force <= 3) {
							if (app_data->jump_force == 0)
								app_data->jump_force = 1;

							app_data->jump_force = app_data->jump_force * -1;
							app_data->jump_state = JUMP_FALLING;
						}
					}
				} else {
					app_data->jump_shift += app_data->jump_force;
					app_data->jump_force = app_data->jump_force * 2;

					if (app_data->jump_shift <= 0) {
						app_data->jump_shift = 0;
						app_data->is_jump = false;
					}
				}

				app_data->current_sprite = SPRITE_DINO_JUMP;
			}

			if (!app_data->is_jump) {
				if (app_data->current_sprite == SPRITE_DINO_STEP_1)
					app_data->current_sprite = SPRITE_DINO_STEP_2;
				else
					app_data->current_sprite = SPRITE_DINO_STEP_1;
			}



			if (app_data->is_cactus_show) {
				if (app_data->cactus_preshow > 0)
					app_data->cactus_preshow--;
				else
					app_data->cactus_x -= app_data->speed;

				if (app_data->cactus_x < -24)
					app_data->is_cactus_show = false;
			}

			if ((1 + _rand() % 10) == 1 && !app_data->is_cactus_show && !app_data->is_bird_show) {
				app_data->is_cactus_show = true;
				app_data->cactus_preshow = 4;
				app_data->cactus_x = 176;

				if ((1 + _rand() % 2) == 1)
					app_data->cactus_type = CACTUS_SMALL;
				else
					app_data->cactus_type = CACTUS_BIG;
			}



			if (app_data->is_bird_show) {
				if (app_data->bird_preshow > 0)
					app_data->bird_preshow--;
				else
					app_data->bird_x -= app_data->speed;

				app_data->bird_frame++;
				if (app_data->bird_frame == 2) {
					app_data->bird_frame = 0;

					if (app_data->bird_sptite == SPRITE_BIRD_FLY_1)
						app_data->bird_sptite = SPRITE_BIRD_FLY_2;
					else
						app_data->bird_sptite = SPRITE_BIRD_FLY_1;
				}

				if (app_data->bird_x < -42)
					app_data->is_bird_show = false;
			}

			if (app_data->score > 500 && !app_data->is_bird_show && !app_data->is_cactus_show && (1 + _rand() % 15) == 1) {
				app_data->is_bird_show = true;
				app_data->bird_preshow = 4;
				app_data->bird_x = 134;
				app_data->bird_frame = 0;
				app_data->bird_sptite = SPRITE_BIRD_FLY_1;

				if ((1 + _rand() % 2) == 1)
					app_data->bird_y = 133;
				else
					app_data->bird_y = 90;
			}



			app_data->cloud_x -= app_data->speed / 6;

			if (app_data->cloud_x <= -66) {
				app_data->cloud_x = 180 + (1 + _rand() % 80);
				app_data->cloud_y = 10 + (1 + _rand() % 15);
			}

			app_data->ground_x -= app_data->speed;

			if (app_data->ground_x <= -166)
				app_data->ground_x = 0;

			app_data->score++;
			app_data->speed_counter++;

			if (app_data->speed_counter == 200) {
				app_data->speed_counter = 0;
				app_data->speed += 1;
			}

			if (app_data->map_type == MAP_TYPE_CLASSIC) {
				app_data->night_counter++;

				if (app_data->night_counter >= 480) {
					app_data->night_text_delay++;

					if (app_data->night_text_delay == 2) {
						app_data->night_text_delay = 0;
						app_data->night_text_show = !app_data->night_text_show;
					}
				}

				if (app_data->night_counter == 500) {
					app_data->is_night = !app_data->is_night;
					app_data->night_counter = 0;
					app_data->night_text_delay = 0;
					app_data->night_text_show = false;
				}
			}

			#ifdef DEBUG_COLLISION
				set_bg_color(COLOR_WHITE);
				set_fg_color(COLOR_BLACK);
				fill_screen_bg();
			#endif

			if (app_data->is_cactus_show) {
				/* Cactus colliders check */

				if (app_data->cactus_x <= 52 && app_data->cactus_x >= 12) {
					if (app_data->cactus_type == CACTUS_BIG) {
						for (int i = 0; i < 12; i++) {
							for (int j = (app_data->cactus_x - 12) / 2; j < 20; j++) {
#ifdef DEBUG_COLLISION
								set_fg_color(COLOR_RED);
								draw_horizontal_line(133 - app_data->jump_shift + dino_collider_bottom[j], 0, 80);
								set_fg_color(COLOR_BLUE);
								draw_horizontal_line(176 - cactus_big_collider[i], 0, 50);
#endif

								if (133 - app_data->jump_shift + dino_collider_bottom[j] > 176 - cactus_big_collider[i]) {
									handle_collision();
									break;
								}
							}

							if (app_data->is_damaged)
								break;
						}
					}

					if (app_data->cactus_type == CACTUS_SMALL) {
						for (int i = 0; i < 8; i++) {
							for (int j = (app_data->cactus_x - 12) / 2; j < 20; j++) {
#ifdef DEBUG_COLLISION
								set_fg_color(COLOR_RED);
								draw_horizontal_line(133 - app_data->jump_shift + dino_collider_bottom[j], 0, 80);
								set_fg_color(COLOR_BLUE);
								draw_horizontal_line(176 - cactus_small_collider[i], 0, 50);
#endif

								if (133 - app_data->jump_shift + dino_collider_bottom[j] > 176 - cactus_small_collider[i]) {
									handle_collision();
									break;
								}
							}

							if (app_data->is_damaged)
								break;
						}
					}
				}

				if (app_data->cactus_x <= 12 && app_data->cactus_x >= -24) {
					if (app_data->cactus_type == CACTUS_BIG) {
						for (int i = ((app_data->cactus_x - 12) / 2) * -1; i < 12; i++) {
							for (int j = 0; j < 20; j++) {
#ifdef DEBUG_COLLISION
								set_fg_color(COLOR_RED);
								draw_horizontal_line(133 - app_data->jump_shift + dino_collider_bottom[j], 0, 80);
								set_fg_color(COLOR_BLUE);
								draw_horizontal_line(176 - cactus_big_collider[i], 0, 50);
#endif

								if (133 - app_data->jump_shift + dino_collider_bottom[j] > 176 - cactus_big_collider[i]) {
									handle_collision();
									break;
								}
							}

							if (app_data->is_damaged)
								break;
						}
					}

					if (app_data->cactus_type == CACTUS_SMALL) {
						for (int i = ((app_data->cactus_x - 12) / 2) * -1; i < 8; i++) {
							for (int j = 0; j < 20; j++) {
#ifdef DEBUG_COLLISION
								set_fg_color(COLOR_RED);
								draw_horizontal_line(133 - app_data->jump_shift + dino_collider_bottom[j], 0, 80);
								set_fg_color(COLOR_BLUE);
								draw_horizontal_line(176 - cactus_small_collider[i], 0, 50);
#endif

								if (133 - app_data->jump_shift + dino_collider_bottom[j] > 176 - cactus_small_collider[i]) {
									handle_collision();
									break;
								}
							}

							if (app_data->is_damaged)
								break;
						}
					}
				}
			}

			if (app_data->is_bird_show) {
				/* Bird top colliders check */

				if (app_data->bird_x <= 52 && app_data->bird_x >= 12 && (133 - app_data->jump_shift + 21 <= app_data->bird_y + 18)) {
					if (app_data->bird_sptite == SPRITE_BIRD_FLY_1) {
						for (int i = 0; i < 21; i++) {
							for (int j = (app_data->bird_x - 12) / 2; j < 20; j++) {
#ifdef DEBUG_COLLISION
								set_fg_color(COLOR_RED);
								draw_horizontal_line(133 - app_data->jump_shift + dino_collider_bottom[j], 0, 80);
								set_fg_color(COLOR_BLUE);
								draw_horizontal_line((app_data->bird_y + 36) - bird_fly_1_collider_top[i], 0, 70);
#endif

								if (133 - app_data->jump_shift + dino_collider_bottom[j] > (app_data->bird_y + 36) - bird_fly_1_collider_top[i]) {
									handle_collision();
									break;
								}
							}

							if (app_data->is_damaged)
								break;
						}
					}

					if (app_data->bird_sptite == SPRITE_BIRD_FLY_2) {
						for (int i = 0; i < 21; i++) {
							for (int j = (app_data->bird_x - 12) / 2; j < 20; j++) {
#ifdef DEBUG_COLLISION
								set_fg_color(COLOR_RED);
								draw_horizontal_line(133 - app_data->jump_shift + dino_collider_bottom[j], 0, 80);
								set_fg_color(COLOR_BLUE);
								draw_horizontal_line((app_data->bird_y + 36) - bird_fly_2_collider_top[i], 0, 70);
#endif

								if (133 - app_data->jump_shift + dino_collider_bottom[j] > (app_data->bird_y + 36) - bird_fly_2_collider_top[i]) {
									handle_collision();
									break;
								}
							}

							if (app_data->is_damaged)
								break;
						}
					}
				}

				if (app_data->bird_x <= 12 && app_data->bird_x >= -24 && (133 - app_data->jump_shift + 21 <= app_data->bird_y + 18)) {
					if (app_data->bird_sptite == SPRITE_BIRD_FLY_1) {
						for (int i = ((app_data->bird_x - 12) / 2) * -1; i < 21; i++) {
							for (int j = 0; j < 20; j++) {
#ifdef DEBUG_COLLISION
								set_fg_color(COLOR_RED);
								draw_horizontal_line(133 - app_data->jump_shift + dino_collider_bottom[j], 0, 80);
								set_fg_color(COLOR_BLUE);
								draw_horizontal_line((app_data->bird_y + 36) - bird_fly_1_collider_top[i], 0, 70);
#endif

								if (133 - app_data->jump_shift + dino_collider_bottom[j] > (app_data->bird_y + 36) - bird_fly_1_collider_top[i]) {
									handle_collision();
									break;
								}
							}

							if (app_data->is_damaged)
								break;
						}
					}

					if (app_data->bird_sptite == SPRITE_BIRD_FLY_2) {
						for (int i = ((app_data->bird_x - 12) / 2) * -1; i < 21; i++) {
							for (int j = 0; j < 20; j++) {
#ifdef DEBUG_COLLISION
								set_fg_color(COLOR_RED);
								draw_horizontal_line(133 - app_data->jump_shift + dino_collider_bottom[j], 0, 80);
								set_fg_color(COLOR_BLUE);
								draw_horizontal_line((app_data->bird_y + 36) - bird_fly_2_collider_top[i], 0, 70);
#endif

								if (133 - app_data->jump_shift + dino_collider_bottom[j] > (app_data->bird_y + 36) - bird_fly_2_collider_top[i]) {
									handle_collision();
									break;
								}
							}

							if (app_data->is_damaged)
								break;
						}
					}
				}

				/* Bird bottom colliders check */

				if (app_data->bird_x <= 52 && app_data->bird_x >= 12 && (133 - app_data->jump_shift + 21 >= app_data->bird_y + 18)) {
					if (app_data->bird_sptite == SPRITE_BIRD_FLY_1) {
						for (int i = 0; i < 21; i++) {
							for (int j = (app_data->bird_x - 12) / 2; j < 20; j++) {
#ifdef DEBUG_COLLISION
								set_fg_color(COLOR_PURPLE);
								draw_horizontal_line(176 - app_data->jump_shift - dino_collider_top[j], 0, 80);
								set_fg_color(COLOR_AQUA);
								draw_horizontal_line(app_data->bird_y + bird_fly_1_collider_bottom[i], 0, 70);
#endif

								if (176 - app_data->jump_shift - dino_collider_top[j] < app_data->bird_y + bird_fly_1_collider_bottom[i]) {
									handle_collision();
									break;
								}
							}

							if (app_data->is_damaged)
								break;
						}
					}

					if (app_data->bird_sptite == SPRITE_BIRD_FLY_2) {
						for (int i = 0; i < 21; i++) {
							for (int j = (app_data->bird_x - 12) / 2; j < 20; j++) {
#ifdef DEBUG_COLLISION
								set_fg_color(COLOR_PURPLE);
								draw_horizontal_line(176 - app_data->jump_shift - dino_collider_top[j], 0, 80);
								set_fg_color(COLOR_AQUA);
								draw_horizontal_line(app_data->bird_y + bird_fly_2_collider_bottom[i], 0, 70);
#endif

								if (176 - app_data->jump_shift - dino_collider_top[j] < app_data->bird_y + bird_fly_2_collider_bottom[i]) {
									handle_collision();
									break;
								}
							}

							if (app_data->is_damaged)
								break;
						}
					}
				}

				if (app_data->bird_x <= 12 && app_data->bird_x >= -24 && (133 - app_data->jump_shift + 21 >= app_data->bird_y + 18)) {
					if (app_data->bird_sptite == SPRITE_BIRD_FLY_1) {
						for (int i = ((app_data->bird_x - 12) / 2) * -1; i < 21; i++) {
							for (int j = 0; j < 20; j++) {
#ifdef DEBUG_COLLISION
								set_fg_color(COLOR_PURPLE);
								draw_horizontal_line(176 - app_data->jump_shift - dino_collider_top[j], 0, 80);
								set_fg_color(COLOR_AQUA);
								draw_horizontal_line(app_data->bird_y + bird_fly_1_collider_bottom[i], 0, 70);
#endif

								if (176 - app_data->jump_shift - dino_collider_top[j] < app_data->bird_y + bird_fly_1_collider_bottom[i]) {
									handle_collision();
									break;
								}
							}

							if (app_data->is_damaged)
								break;
						}
					}

					if (app_data->bird_sptite == SPRITE_BIRD_FLY_2) {
						for (int i = ((app_data->bird_x - 12) / 2) * -1; i < 21; i++) {
							for (int j = 0; j < 20; j++) {
#ifdef DEBUG_COLLISION
								set_fg_color(COLOR_PURPLE);
								draw_horizontal_line(176 - app_data->jump_shift - dino_collider_top[j], 0, 80);
								set_fg_color(COLOR_AQUA);
								draw_horizontal_line(app_data->bird_y + bird_fly_2_collider_bottom[i], 0, 70);
#endif

								if (176 - app_data->jump_shift - dino_collider_top[j] < app_data->bird_y + bird_fly_2_collider_bottom[i]) {
									handle_collision();
									break;
								}
							}

							if (app_data->is_damaged)
								break;
						}
					}
				}
			}

			draw_game_screen();
			break;

		case SCREEN_PAUSE:
			app_data->pause_counter++;

			if (app_data->pause_counter == 5) {
				app_data->pause_counter = 0;
				app_data->pause_frame++;

				if (app_data->pause_frame > 3)
					app_data->pause_frame = 0;

				draw_pause_screen();
			}
			break;

		case SCREEN_GAME_OVER:
			draw_game_over_screen();
			break;
	}

	set_update_period(1, UPDATE_SCREEN_SPEED);
}

void touch_handler(void* param) {
	#ifdef BipEmulator
		struct gesture_* gest = (gesture_*)param;
	#else
		struct gesture_* gest = param;
	#endif

	switch (gest->gesture) {
		case GESTURE_CLICK:
			if (app_data->current_screen == SCREEN_GAME) {
				if (!app_data->is_jump) {
					app_data->is_jump = true;
					app_data->jump_state = JUMP_SMALL;
					app_data->jump_shift = 0;
					app_data->jump_force = 30;
				}
			} else if (app_data->current_screen == SCREEN_START) {
				if (app_data->menu_item == MENU_ITEM_PLAY) {
					start_game();
				} else {
					app_data->current_screen = SCREEN_SETTINGS;
					draw_settings_screen();
				}
			} else if (app_data->current_screen == SCREEN_SETTINGS) {
				if ((gest->touch_pos_y > 20) && (gest->touch_pos_y < 50)) {
					app_data->vibration = !app_data->vibration;

					data_p->vibration = app_data->vibration;
					ElfWriteSettings(app_data->proc->index_listed, data_p, 0, sizeof(data));

					if (app_data->vibration)
						vibrate(1, 40, 300);

					draw_settings_screen();
				}

				if ((gest->touch_pos_y > 50) && (gest->touch_pos_y < 80)) {
					app_data->map_type++;

					if (app_data->map_type > 2)
						app_data->map_type = 0;

					data_p->map_type = app_data->map_type;
					ElfWriteSettings(app_data->proc->index_listed, data_p, 0, sizeof(data));

					if (app_data->vibration)
						vibrate(1, 40, 300);

					draw_settings_screen();
				}
			} else if (app_data->current_screen == SCREEN_PAUSE) {
				app_data->current_screen = SCREEN_GAME;
			} else if (app_data->current_screen == SCREEN_GAME_OVER) {
				if (
					(gest->touch_pos_x > 61) && (gest->touch_pos_x < 115) &&
					(gest->touch_pos_y > 63) && (gest->touch_pos_y < 113)
				)
					start_game();
			}
			break;

		case GESTURE_SWIPE_RIGHT:

			break;

		case GESTURE_SWIPE_LEFT:

			break;

		case GESTURE_SWIPE_UP:
			if (app_data->current_screen == SCREEN_START) {
				app_data->menu_item = MENU_ITEM_PLAY;
				draw_start_screen();
			}

			if (app_data->current_screen == SCREEN_GAME)
				if (!app_data->is_jump) {
					app_data->is_jump = true;
					app_data->jump_state = JUMP_BIG;
					app_data->jump_shift = 0;
					app_data->jump_force = 38;
				}
			break;

		case GESTURE_SWIPE_DOWN:
			if (app_data->current_screen == SCREEN_START) {
				app_data->menu_item = MENU_ITEM_SETTINGS;
				draw_start_screen();
			}

			if (app_data->current_screen == SCREEN_GAME)
				if (app_data->is_jump) {
					app_data->jump_state = JUMP_FALLING;
					app_data->jump_force = -20;
					app_data->jump_shift = 30;
				}
			break;
	}
}

void start_game() {
	app_data->current_screen = SCREEN_GAME;

	app_data->score = 0;
	app_data->is_record = false;

	app_data->speed = 18;
	app_data->speed_counter = 0;

	app_data->is_night = app_data->map_type == MAP_TYPE_NIGHT ? true : false;
	app_data->night_counter = 0;
	app_data->night_text_delay = 0;
	app_data->night_text_show = false;

	app_data->current_sprite = SPRITE_DINO_STEP_1;

	app_data->is_jump = false;
	app_data->jump_state = JUMP_SMALL;
	app_data->jump_shift = 0;
	app_data->is_damaged = false;

	app_data->is_cactus_show = false;
	app_data->cactus_preshow = 0;

	app_data->is_bird_show = false;
	app_data->bird_preshow = 0;

	app_data->cloud_x = 20 + (1 + _rand() % 240);
	app_data->cloud_y = 10 + (1 + _rand() % 15);

	app_data->ground_x = 0;

	set_bg_color(COLOR_WHITE);
	fill_screen_bg();
	repaint_screen_lines(1, 176);

	if (app_data->vibration)
		vibrate(1, 40, 70);
	set_update_period(1, UPDATE_SCREEN_SPEED);
}

void handle_collision() {
	if (!app_data->is_damaged) {
		app_data->is_damaged = true;
		app_data->current_screen = SCREEN_GAME_OVER;

		if (data_p->score < app_data->score) {
			data_p->score = app_data->score;
			app_data->is_record = true;
			ElfWriteSettings(app_data->proc->index_listed, data_p, 0, sizeof(data));
		}

		if (app_data->vibration)
			vibrate(1, 40, 300);
	}
}

void draw_start_screen() {
	char score[20];

	set_bg_color(COLOR_WHITE);
	set_fg_color(COLOR_BLACK);
	fill_screen_bg();

	show_elf_res_by_id(app_data->proc->index_listed, 10, 0, 0);

	_sprintf(score, "Hi-score: %.5d", app_data->hi_score);
	text_out_center(score, 88, 24);

	show_elf_res_by_id(app_data->proc->index_listed, 0, 39, 68);
	show_elf_res_by_id(app_data->proc->index_listed, 4, 114, 65);

	text_out_center("Play", 88, 123);
	text_out_center("Settings", 88, 143);

	if (app_data->menu_item == MENU_ITEM_PLAY)
		text_out_center(">", 60, 123);
	else
		text_out_center(">", 42, 143);

	repaint_screen_lines(1, 176);
}

void draw_game_screen() {
	char score[10];

	#ifndef DEBUG_COLLISION
		set_bg_color(COLOR_WHITE);
		set_fg_color(COLOR_BLACK);
		fill_screen_bg();
	#endif

	if (app_data->is_night) {
		set_bg_color(COLOR_BLACK);
		set_fg_color(COLOR_WHITE);
		fill_screen_bg();
	} else {
		set_bg_color(COLOR_WHITE);
		set_fg_color(COLOR_BLACK);
		fill_screen_bg();
	}

	show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_GROUND : SPRITE_GROUND, app_data->ground_x, 166);
	show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_GROUND : SPRITE_GROUND, app_data->ground_x + 176, 166);

	switch (app_data->current_sprite) {
		case SPRITE_DINO_STEP_1:
			show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_DINO_STEP_1 : SPRITE_DINO_STEP_1, 12, 133);
			break;

		case SPRITE_DINO_STEP_2:
			show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_DINO_STEP_2 : SPRITE_DINO_STEP_2, 12, 133);
			break;

		case SPRITE_DINO_JUMP:
			show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_DINO_JUMP : SPRITE_DINO_JUMP, 12, 133 - app_data->jump_shift);
			break;
	}

	if ((app_data->cactus_preshow % 2) == 1)
		switch (app_data->cactus_type) {
			case CACTUS_BIG:
				show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_CACTUS_BIG_PRESHOW : SPRITE_CACTUS_BIG_PRESHOW, 152, 130);
				break;

			case CACTUS_SMALL:
				show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_CACTUS_SMALL_PRESHOW : SPRITE_CACTUS_SMALL_PRESHOW, 158, 143);
				break;
		}

	if (app_data->is_cactus_show && app_data->cactus_preshow == 0)
		switch (app_data->cactus_type) {
			case CACTUS_BIG:
				show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_CACTUS_BIG : SPRITE_CACTUS_BIG, app_data->cactus_x, 130);
				break;

			case CACTUS_SMALL:
				show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_CACTUS_SMALL : SPRITE_CACTUS_SMALL, app_data->cactus_x, 143);
				break;
		}

	if ((app_data->bird_preshow % 2) == 1)
		show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_BIRD_PRESHOW : SPRITE_BIRD_PRESHOW, 134, app_data->bird_y);

	if (app_data->is_bird_show && app_data->bird_preshow == 0)
		switch (app_data->bird_sptite) {
			case SPRITE_BIRD_FLY_1:
				show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_BIRD_FLY_1 : SPRITE_BIRD_FLY_1, app_data->bird_x, app_data->bird_y);
				break;
			case SPRITE_BIRD_FLY_2:
				show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_BIRD_FLY_2 : SPRITE_BIRD_FLY_2, app_data->bird_x, app_data->bird_y);
				break;
		}

	#ifdef BipEmulator
		//repaint_screen_lines(73, 176);
	#endif

	
	_sprintf(score, "HI %.5d", data_p->score);
	text_out(score, 8, 8);
	_sprintf(score, "%.5d", app_data->score);
	text_out(score, 124, 8);

	if (app_data->night_text_show) 
		text_out_center(app_data->is_night ? "A DAY HAS COMING" : "A NIGHT HAS COMING", 88, 30);

	show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_CLOUD : SPRITE_CLOUD, app_data->cloud_x, app_data->cloud_y);

	#ifdef BipEmulator
		//repaint_screen_lines(8, 18);
		//repaint_screen_lines(8, 38);

		repaint_screen_lines(1, 176);
	#endif
}

void draw_pause_screen() {
	set_bg_color(COLOR_WHITE);
	set_fg_color(COLOR_BLACK);
	fill_screen_bg();

	show_elf_res_by_id(app_data->proc->index_listed, 11, 61, 20);

	if (app_data->pause_frame >= 1)
		show_elf_res_by_id(app_data->proc->index_listed, 12, 110, 59);
	if (app_data->pause_frame >= 2)
		show_elf_res_by_id(app_data->proc->index_listed, 12, 115, 57);
	if (app_data->pause_frame == 3)
		show_elf_res_by_id(app_data->proc->index_listed, 12, 120, 55);

	show_elf_res_by_id(app_data->proc->index_listed, 13, 68, 68);

	text_out_center("touch to continue", 88, 126);
	text_out_center("press again to exit", 88, 144);

	repaint_screen_lines(1, 176);
}

void draw_game_over_screen() {
	char score[10];

	if (app_data->is_night) {
		set_bg_color(COLOR_BLACK);
		set_fg_color(COLOR_WHITE);
		fill_screen_bg();
	} else {
		set_bg_color(COLOR_WHITE);
		set_fg_color(COLOR_BLACK);
		fill_screen_bg();
	}

	if (app_data->is_record) 
		text_out_center("New record!", 88, 25);
	else 
		text_out_center("Game over!", 88, 25);

	_sprintf(score, "Your score: %d", app_data->score);
	text_out_center(score, 88, 43);

	show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_GROUND : SPRITE_GROUND, app_data->ground_x, 166);
	show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_GROUND : SPRITE_GROUND, app_data->ground_x + 176, 166);

	show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_RESTART_BUTTON : SPRITE_RESTART_BUTTON, 71, 73);
	show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_DINO_DAMAGED : SPRITE_DINO_DAMAGED, 12, 133 - app_data->jump_shift);

	if (app_data->is_cactus_show)
		switch (app_data->cactus_type) {
			case CACTUS_BIG:
				show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_CACTUS_BIG : SPRITE_CACTUS_BIG, app_data->cactus_x, 130);
				break;

			case CACTUS_SMALL:
				show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_CACTUS_SMALL : SPRITE_CACTUS_SMALL, app_data->cactus_x, 143);
				break;
		}

	if (app_data->is_bird_show)
		switch (app_data->bird_sptite) {
			case SPRITE_BIRD_FLY_1:
				show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_BIRD_FLY_1 : SPRITE_BIRD_FLY_1, app_data->bird_x, app_data->bird_y);
				break;
			case SPRITE_BIRD_FLY_2:
				show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_BIRD_FLY_2 : SPRITE_BIRD_FLY_2, app_data->bird_x, app_data->bird_y);
				break;
		}

	show_elf_res_by_id(app_data->proc->index_listed, app_data->is_night ? SPRITE_NIGHT_CLOUD : SPRITE_CLOUD, app_data->cloud_x, app_data->cloud_y);

	repaint_screen_lines(1, 176);
}

void draw_settings_screen() {
	set_bg_color(COLOR_WHITE);
	set_fg_color(COLOR_BLACK);
	fill_screen_bg();

	show_elf_res_by_id(app_data->proc->index_listed, 10, 0, 0);

	if (app_data->vibration)
		text_out_center("Vibration: [On]", 88, 35);
	else
		text_out_center("Vibration: [Off]", 88, 35);

	switch (app_data->map_type) {
		case MAP_TYPE_CLASSIC:
			text_out_center("Mode: [Classic]", 88, 65);
			break;
		case MAP_TYPE_DAY:
			text_out_center("Mode: [Day]", 88, 65);
			break;
		case MAP_TYPE_NIGHT:
			text_out_center("Mode: [Night]", 88, 65);
			break;
	}

	text_out_center("TRex 1.1", 88, 120);
	text_out_center("By Yuukari", 88, 140);

	repaint_screen_lines(1, 176);
}