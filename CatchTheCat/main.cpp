#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_image.h>
#include <iostream>

static SDL_Window* window;
static SDL_Renderer* renderer;

// Текстуры
SDL_Texture* cat_texture;
SDL_Texture* cell_texture;
SDL_Texture* closed_cell_texture;
SDL_Texture* you_win_texture;
SDL_Texture* game_over_texture;
// Области для текстур
SDL_FRect cat_rect;
SDL_FRect cell_rect;
SDL_FRect end_game_rect;

// Длинна поля
float field_length;
// Размер клетки
float cell_size;
// Двумерный массив для хранения состояний клеток (открыта - true, закрыта - false)
bool** cells_condition_array;
// Двумерный массив для отслеживания кошки
bool** array_cat_tracking;
// Отслеживание координат мышки
float mouse_x;
float mouse_y;
// Массив для проверки возможности хода кошки
bool array_move_possibilities[6];
// Победа
bool you_win;
// Поражение
bool game_over;
// Выбор хода
int move;
// Предыдущий ход
int prev_move;

// Функция для инициализация переменных
void mapComplection() {
	// Очистка двумерных массивов
	if (cells_condition_array) {
		for (int i = 0; i < field_length; i++) delete[] cells_condition_array[i];
		delete[] cells_condition_array;
	}
	if (array_cat_tracking) {
		for (int i = 0; i < field_length; i++) delete[] array_cat_tracking[i];
		delete[] array_cat_tracking;
	}
	// Обнуление указателей для безопасности
	cells_condition_array = nullptr;
	array_cat_tracking = nullptr;

	field_length = 11;
	cell_size = 64;
	you_win = false;
	game_over = false;
	end_game_rect = { 0, 0, cell_size * field_length + cell_size / 2 + 20, cell_size * field_length - cell_size / 4 * field_length + 40 };
	move = SDL_rand(6) + 1;
	prev_move = move;

	cells_condition_array = new bool* [field_length];
	array_cat_tracking = new bool* [field_length];
	for (int i = 0; i < field_length; i++) {
		cells_condition_array[i] = new bool[field_length];
		array_cat_tracking[i] = new bool[field_length];
	}
	for (int i = 0; i < field_length; i++) {
		for (int j = 0; j < field_length; j++) {
			cells_condition_array[i][j] = true;
			array_cat_tracking[i][j] = false;
		}
	}
	array_cat_tracking[int(field_length/2)][int(field_length/2)] = true;
}

// Функция для проверки массива на возможность хода кошки (true - есть ходы, false - нет ходов)
bool checkArray() {
	for (int i = 0; i < 6; i++) {
		if (array_move_possibilities[i] == true) {
			return true;
		}
	}
	return false;
}

// Функция для очищения массива проверки на возможность хода кошки
void clearArray() {
	for (int i = 0; i < 6; i++) {
		array_move_possibilities[i] = true;
	}
}

// Функция для хода кошки
int catMove() {
	int cat_i = -1;
	int cat_j = -1;
	// Поиск координат кошки
	for (int i = 0; i < field_length; i++) {
		for (int j = 0; j < field_length; j++) {
			if (array_cat_tracking[i][j] == true) {
				// Запись координат кошки
				cat_i = i;
				cat_j = j;
				// Проверка за выход массива
				if (cat_i - 1 < 0 || cat_j - 1 < 0 || cat_i + 1 > field_length - 1 || cat_j + 1 > field_length - 1) return -1;
				// Проверка возможных ходов
				if (cells_condition_array[i + 1][j] == false) array_move_possibilities[1] = false;
				if (cells_condition_array[i - 1][j] == false) array_move_possibilities[4] = false;
				if (j % 2 == 0) {
					if (cells_condition_array[i][j - 1] == false) array_move_possibilities[0] = false;
					if (cells_condition_array[i][j + 1] == false) array_move_possibilities[2] = false;
					if (cells_condition_array[i - 1][j + 1] == false) array_move_possibilities[3] = false;
					if (cells_condition_array[i - 1][j - 1] == false) array_move_possibilities[5] = false;
				}
				else {
					if (cells_condition_array[i + 1][j - 1] == false) array_move_possibilities[0] = false;
					if (cells_condition_array[i + 1][j + 1] == false) array_move_possibilities[2] = false;
					if (cells_condition_array[i][j + 1] == false) array_move_possibilities[3] = false;
					if (cells_condition_array[i][j - 1] == false) array_move_possibilities[5] = false;
				}
				if (checkArray() == false) return 1;
			}
		}
	}
	// Алгоритм выбора случайного хода
	while (array_move_possibilities[move - 1] == false) {
		prev_move = move;
		move += (SDL_rand(2) * 2 - 1);
		if (move == 7) move = 1;
		if (move == 0) move = 6;
	}
	if (array_move_possibilities[prev_move - 1] == true) {
		move = prev_move;
	}
	// Алгоритм хода
	if (move == 1) {
		if (cat_j % 2 == 0) {
			array_cat_tracking[cat_i][cat_j] = false;
			array_cat_tracking[cat_i][cat_j - 1] = true;
		}
		else {
			array_cat_tracking[cat_i][cat_j] = false;
			array_cat_tracking[cat_i + 1][cat_j - 1] = true;
		}
	}
	else if (move == 2) {
		array_cat_tracking[cat_i][cat_j] = false;
		array_cat_tracking[cat_i + 1][cat_j] = true;
	}
	else if (move == 3) {
		if (cat_j % 2 == 0) {
			array_cat_tracking[cat_i][cat_j] = false;
			array_cat_tracking[cat_i][cat_j + 1] = true;
		}
		else {
			array_cat_tracking[cat_i][cat_j] = false;
			array_cat_tracking[cat_i + 1][cat_j + 1] = true;
		}
	}
	else if (move == 4) {
		if (cat_j % 2 == 0) {
			array_cat_tracking[cat_i][cat_j] = false;
			array_cat_tracking[cat_i - 1][cat_j + 1] = true;
		}
		else {
			array_cat_tracking[cat_i][cat_j] = false;
			array_cat_tracking[cat_i][cat_j + 1] = true;
		}
	}
	else if (move == 5) {
		array_cat_tracking[cat_i][cat_j] = false;
		array_cat_tracking[cat_i - 1][cat_j] = true;
	}
	else if (move == 6) {
		if (cat_j % 2 == 0) {
			array_cat_tracking[cat_i][cat_j] = false;
			array_cat_tracking[cat_i - 1][cat_j - 1] = true;
		}
		else {
			array_cat_tracking[cat_i][cat_j] = false;
			array_cat_tracking[cat_i][cat_j - 1] = true;
		}
	}
	return 0;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
	system("chcp 1251");
	SDL_srand(SDL_GetTicks());
	mapComplection();
	// Создание окна приложения
	SDL_CreateWindowAndRenderer("CatchTheCat", cell_size * field_length + cell_size / 2 + 20, cell_size * field_length - cell_size / 4 * field_length + 40, NULL, &window, &renderer);
	// Инициализация текстур
	cat_texture = IMG_LoadTexture(renderer, "cat.png");
	cell_texture = IMG_LoadTexture(renderer, "cell.png");
	closed_cell_texture = IMG_LoadTexture(renderer, "closed_cell.png");
	you_win_texture = IMG_LoadTexture(renderer, "you_win.png");
	game_over_texture = IMG_LoadTexture(renderer, "game_over.png");
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	switch (event->type) {
	// Обновление координат при движении мышки
	case SDL_EVENT_MOUSE_MOTION: {
		mouse_x = event->motion.x;
		mouse_y = event->motion.y;
		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_DOWN: {
		switch (event->button.button) {
		case SDL_BUTTON_LEFT: {
			// Перезапуск при завершении игры
			if (you_win == true) {
				mapComplection();
				return SDL_APP_CONTINUE;
			}
			if (game_over == true) {
				mapComplection();
				return SDL_APP_CONTINUE;
			}
			int closest_i = -1;
			int closest_j = -1;
			float closest_x = cell_size;
			float closest_y = cell_size;
			for (int i = 0; i < field_length; i++) {
				for (int j = 0; j < field_length; j++) {
					cell_rect = { cell_size * i + 10, cell_size * j - cell_size / 4 * j + 10, cell_size, cell_size };
					if (j % 2 == 1) cell_rect.x += cell_size / 2;
					// Поиск ближайшей нажатой клетки, так как тектуры накладываются друг на друга
					if (mouse_x >= cell_rect.x && mouse_x <= cell_rect.x + cell_rect.w &&
						mouse_y >= cell_rect.y && mouse_y <= cell_rect.y + cell_rect.h) {
						if (closest_x + closest_y > abs(int(cell_size / 2 - (cell_rect.x + cell_rect.w - mouse_x))) + abs(int(cell_size / 2 - (cell_rect.y + cell_rect.h - mouse_y)))) {
							closest_x = abs(int(cell_size / 2 - (cell_rect.x + cell_rect.w - mouse_x)));
							closest_y = abs(int(cell_size / 2 - (cell_rect.y + cell_rect.h - mouse_y)));
							closest_i = i;
							closest_j = j;
						}
					}
				}
			}
			// Обработка нажатой клетки
			if (closest_i != -1 && closest_j != -1) {
				if (array_cat_tracking[closest_i][closest_j] == false && cells_condition_array[closest_i][closest_j] == true) {
					cells_condition_array[closest_i][closest_j] = false;
					clearArray();
					int result = catMove();
					if (result == 1) you_win = true;
					else if (result == -1) game_over = true;
				}
			}
			break;
		}
		}
		break;
	}
	case SDL_EVENT_KEY_DOWN: {
		switch (event->key.scancode) {
		// Выход на esc
		case SDL_SCANCODE_ESCAPE:
			return SDL_APP_SUCCESS;
		// Перезапуск на enter
		case SDL_SCANCODE_RETURN:
			mapComplection();
		}
		break;
	}
	// Выход на крестик
	case SDL_EVENT_QUIT: {
		return SDL_APP_SUCCESS;
	}
	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
	// Заливка фона цветом
	SDL_SetRenderDrawColor(renderer, 20, 135, 15, 0);
	SDL_RenderClear(renderer);

	// Сборка карты
	for (int i = 0; i < field_length; i++) {
		for (int j = 0; j < field_length; j++) {
			// Запись координат клетки
			cell_rect = { cell_size * i + 10, cell_size * j - cell_size / 4 * j + 10, cell_size, cell_size };
			if (j % 2 == 1) cell_rect.x += cell_size / 2;
			// Прорисовка клетки по координатам
			if (cells_condition_array[i][j] == true) SDL_RenderTexture(renderer, cell_texture, NULL, &cell_rect);
			else SDL_RenderTexture(renderer, closed_cell_texture, NULL, &cell_rect);
			// Запись координат кошки
			if (array_cat_tracking[i][j] == true) {
				cat_rect = cell_rect;
				cat_rect.y -= cell_size / 4;
			}
		}
	}
	// Прорисовка кошки по координатам
	SDL_RenderTexture(renderer, cat_texture, NULL, &cat_rect);
	// Проверка на конец игры
	if (you_win == true) {
		SDL_RenderTexture(renderer, you_win_texture, NULL, &end_game_rect);
	}
	if (game_over == true) {
		SDL_RenderTexture(renderer, game_over_texture, NULL, &end_game_rect);
	}

	// Вывод получившейся картинки
	SDL_RenderPresent(renderer);
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	// Удаление текстур
	SDL_DestroyTexture(cat_texture);
	SDL_DestroyTexture(cell_texture);
	SDL_DestroyTexture(closed_cell_texture);
	SDL_DestroyTexture(you_win_texture);
	SDL_DestroyTexture(game_over_texture);
}