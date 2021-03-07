#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <string.h>
#define CRUSH_CODE 0
#define MOUSE_SWAPPING 0
#define TEXT_SWAPPING 1
#define MAX_SCORE 100
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)


typedef int FLAG;
typedef int** MATRIX;
typedef struct repeating_candies_info {
	int begining_node;
	int ending_node;
	int line;
	struct repeating_candies_info* next;
}TOCRUSH;
typedef struct scroll_position {
	int vertical_pos;
	int horizontal_pos;
}SCROLL;
FLAG swapping_mode;
int HEIGHT, WIDTH, PIXELS_PER_SCROLL_VERT, PIXELS_PER_SCROLL_HORZ;
float dpi;
const char character_list[7] = { ' ','R','O','G','Y','P','B' };
static int score = 0;


MATRIX generate_matrix(int m, int n);
int fill_matrix(MATRIX initial, int m, int n);
void print_matrix(MATRIX initial, int m, int n);
TOCRUSH *find_verticle_chain(MATRIX initial, int m, int n);
TOCRUSH *find_horizontal_chain(MATRIX initial, int m, int n);
int crush(MATRIX initial, int m, int n, FLAG print);
int candy_drop_onestep(MATRIX initial, int m, int n);
void print_dropped_matrix(MATRIX initial, int m, int n);
void advanced_swapping(MATRIX initial, int m, int n);
POINT get_position(RECT rect, POINT mouse, SCROLL scroll_pos);
void mouse_swapping(MATRIX initial, int m, int n);
int highlight(int sign);
int colorize(int sign);
int click_colorize(int sign);
void click_highlight(MATRIX initial, int m, int n, POINT position);
int prechecking(MATRIX initial, int m, int n);
FLAG click_option(int is_gameover);
void swap(MATRIX initial, int m, int n);
void presetting(void);

MATRIX generate_matrix(int m, int n) {
	printf("Generating a %d x %d map, please wait...", m, n);
	MATRIX initial = (int**)calloc(m, sizeof(int));
	for (int i = 0; i < m; i++) {
		initial[i] = (int*)calloc(n, sizeof(int));
	}
	Sleep(1200);
	return initial;
}

int fill_matrix(MATRIX initial, int m, int n) {
	srand((unsigned)time(NULL));
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++) {
			initial[i][j] = rand() % 6 + 1;
		}
	return 1;
}

void print_matrix(MATRIX initial, int m, int n) {
	int i, j, k;
	system("cls");
	for (j = 0; j < n + 1; j++)
		printf("+---");
	printf("+\n|   ");
	for (j = 0; j < n; j++)
		printf("|%2d ", j);
	printf("|\n");
	for (j = 0; j < n + 1; j++)
		printf("+---");
	printf("+\n");
	for (i = 0; i < m ; i++) {
		printf("|%2d ", i);
		for (j = 0; j < n ; j++) {
			k = initial[i][j];
			if (k > 6 || k == 0)
				highlight(k);
			else
				colorize(k);
		}	
		printf("|\n");
		for (j = 0; j < n + 1 ; j++)
			printf("+---");
		printf("+\n");
	}
	printf("+------------+\n|SCORES: %4d|\n+------------+\n", score);
}

TOCRUSH *find_verticle_chain(MATRIX initial, int m, int n) {
	int i, j;
	TOCRUSH *p1, *p2;
	p1 = NULL;
	int count = 1;
	int temp_start = 0, temp_end;
	
	for (j = 0; j < n; j++) {
		for (i = 1; i < m; i++) {
			if (initial[i][j] == initial[temp_start][j]) {
				count++;
			}
			else {
				if (count >= 3) {
					temp_end = i - 1;
					if (p1 == NULL) {
						p1 = (TOCRUSH*)malloc(sizeof(TOCRUSH));
						p1->next = NULL;
						p1->begining_node = temp_start;
						p1->ending_node = temp_end;
						p1->line = j;
					}
					else {
						p2 = (TOCRUSH*)malloc(sizeof(TOCRUSH));
						p2->begining_node = temp_start;
						p2->ending_node = temp_end;
						p2->line = j;
						p2->next = p1;
						p1 = p2;
					}
				}
				temp_start = i;
				count = 1;
			}
		}
		if (count >= 3) {
			temp_end = i - 1;
			if (p1 == NULL) {
				p1 = (TOCRUSH*)malloc(sizeof(TOCRUSH));
				p1->next = NULL;
				p1->begining_node = temp_start;
				p1->ending_node = temp_end;
				p1->line = j;
			}
			else {
				p2 = (TOCRUSH*)malloc(sizeof(TOCRUSH));
				p2->begining_node = temp_start;
				p2->ending_node = temp_end;
				p2->line = j;
				p2->next = p1;
				p1 = p2;
			}
		}
		temp_start = 0;
		count = 1;
	}
	return p1;
}

TOCRUSH *find_horizontal_chain(MATRIX initial, int m, int n) {
	int i, j;
	TOCRUSH *p1, *p2;
	p1 = NULL;
	int count = 1;
	int temp_start = 0, temp_end;
	
	for (j = 0; j < m; j++) {
		for (i = 1; i < n; i++) {
			if (initial[j][i] == initial[j][temp_start]) {
				count++;
			}
			else {
				if (count >= 3) {
					temp_end = i - 1;
					if (p1 == NULL) {
						p1 = (TOCRUSH*)malloc(sizeof(TOCRUSH));
						p1->next = NULL;
						p1->begining_node = temp_start;
						p1->ending_node = temp_end;
						p1->line = j;
					}
					else {
						p2 = (TOCRUSH*)malloc(sizeof(TOCRUSH));
						p2->begining_node = temp_start;
						p2->ending_node = temp_end;
						p2->line = j;
						p2->next = p1;
						p1 = p2;
					}
				}
				temp_start = i;
				count = 1;
			}
		}
		if (count >= 3) {
			temp_end = i - 1;
			if (p1 == NULL) {
				p1 = (TOCRUSH*)malloc(sizeof(TOCRUSH));
				p1->next = NULL;
				p1->begining_node = temp_start;
				p1->ending_node = temp_end;
				p1->line = j;
			}
			else {
				p2 = (TOCRUSH*)malloc(sizeof(TOCRUSH));
				p2->begining_node = temp_start;
				p2->ending_node = temp_end;
				p2->line = j;
				p2->next = p1;
				p1 = p2;
			}
		}
		temp_start = 0;
		count = 1;
	}
	return p1;
}

int crush(MATRIX initial, int m, int n, FLAG print) {
	TOCRUSH *pVert = find_verticle_chain(initial, m, n);
	TOCRUSH *pHorz = find_horizontal_chain(initial, m, n);
	if (pVert == NULL && pHorz == NULL) {
		free(pVert);
		free(pHorz);
		return 0;
	}
	int i;
	TOCRUSH *tempV = pVert, *tempH = pHorz;
	int candy_crushed = 0;
	
	if (print) {
		while (pVert != NULL) {
			for (i = pVert->begining_node; i <= pVert->ending_node; i++) {
				initial[i][pVert->line] += 6;
			}
			pVert = pVert->next;
		}
		while (pHorz != NULL) {
			for (i = pHorz->begining_node; i <= pHorz->ending_node; i++) {
				if (initial[pHorz->line][i] <= 6)
					initial[pHorz->line][i] += 6;
			}
			pHorz = pHorz->next;
			system("cls");
			print_matrix(initial, m, n);
			Sleep(1000);
		}
	}
	pVert = tempV; pHorz = tempH;

	while (pVert != NULL) {
		for (i = pVert->begining_node; i <= pVert->ending_node; i++) {
			initial[i][pVert->line] = CRUSH_CODE;
			candy_crushed++;
		}
		pVert = pVert->next;
	}
	while (pHorz != NULL) {
		for (i = pHorz->begining_node; i <= pHorz->ending_node; i++) {
			if (initial[pHorz->line][i] != CRUSH_CODE)
				candy_crushed++;
			initial[pHorz->line][i] = CRUSH_CODE;
		}
		pHorz = pHorz->next;
	}
	while (tempV != NULL) {
		pVert = tempV;
		tempV = pVert->next;
		free(pVert);
	}
	while (tempH != NULL) {
		pHorz = tempH;
		tempH = pHorz->next;
		free(pHorz);
	}
	return candy_crushed;
}

int candy_drop_onestep(MATRIX initial, int m, int n) {
	int i, a, b;
	FLAG drop_over = 1;

	for (i = 0; i < n; i++) {
		for (a = m - 1; a >= 0; a--) {
			if (initial[a][i] == 0) {
				for (b = a; b > 0; b--) {
					initial[b][i] = initial[b - 1][i];
				}
				initial[0][i] = rand() % 6 + 1;
				drop_over = 0;
				break;
			}
		}
	}
	if (!drop_over) {
		system("cls");
		print_matrix(initial, m, n);
		Sleep(400);
	}
	return drop_over;
}

int candy_drop_onestep_no_print(MATRIX initial, int m, int n) {
	int i, a, b;
	FLAG drop_over = 1;

	for (i = 0; i < n; i++) {
		for (a = m - 1; a >= 0; a--) {
			if (initial[a][i] == 0) {
				for (b = a; b > 0; b--) {
					initial[b][i] = initial[b - 1][i];
				}
				initial[0][i] = rand() % 6 + 1;
				drop_over = 0;
				break;
			}
		}
	}
	return drop_over;
}

void print_dropped_matrix(MATRIX initial, int m, int n) {
	while (candy_drop_onestep(initial, m, n) != 1);
}

void advanced_swapping(MATRIX initial, int m, int n) {
	char order[100];
	int temp, x1, x2 = -1, y1, y2 = -1;
	while (x2 < 0 || y2 < 0) {
		printf("How do you want to do the swaping?\n");
		setbuf(stdin, NULL);
		gets(order);
		char *ret, *ret2, *ret3, location[6];
		if ((ret2 = strchr(order, '(')) == NULL) {
			printf("You should specify the candy's location in comma, like (i, j). Do it again.\n");
			continue;
		}
		strncpy(location, ret2, 5);
		location[5] = '\0';

		x1 = location[1] - 48;
		y1 = location[3] - 48;
		
		if ((ret3 = strchr(ret2 + 1, '(')) != NULL) {
			strncpy(location, ret3, 5);
			x2 = location[1] - 48;
			y2 = location[3] - 48;
			if (abs(x1 - x2) > 1 || abs(y1 - y2) > 1) {
				printf("Illegal swaping, please swap the adjacent candies\n");
				x2 = -1;
			}
		}
		else {
			char directions[4][6] = { "right","left", "above", "under" };
			int i;
			for (i = 0; i < 4; i++) {
				ret = strstr(order, directions[i]);
				if (ret != NULL)
					break;
			}
			switch (i) {
			case 0:
				x2 = x1 + 1;
				y2 = y1;
				break;
			case 1:
				x2 = x1 - 1;
				y2 = y1;
				break;
			case 2:
				x2 = x1;
				y2 = y1 - 1;
				break;
			case 3:
				x2 = x1;
				y2 = y1 + 1;
				break;
			case 4:
				x2 = -1;
				printf("the position discription is confusing, try again\n");
			}
		}
	}
	temp = initial[y1][x1];
	initial[y1][x1] = initial[y2][x2];
	initial[y2][x2] = temp;
	print_matrix(initial, m, n);
	Sleep(2000);
	if (find_verticle_chain(initial, m, n) == NULL && find_horizontal_chain(initial, m, n) == NULL) {
		temp = initial[y1][x1];
		initial[y1][x1] = initial[y2][x2];
		initial[y2][x2] = temp;
		print_matrix(initial, m, n);
		printf("\t+--------------------------------+\n\t|OOPS, IT DOSN'T WORK, SWAP AGAIN|\n\t+--------------------------------+\n");
		advanced_swapping(initial, m, n);
	}
}

POINT get_position(RECT rect, POINT mouse, SCROLL scroll_pos) {
	POINT result;
	long mx = mouse.x;
	long my = mouse.y;
	long wx = rect.left;
	long wy = rect.top;
	
	result.x = (mx - wx  - WIDTH - (float)WIDTH/8 + scroll_pos.horizontal_pos * PIXELS_PER_SCROLL_HORZ - GetSystemMetrics(SM_CXFRAME)) / WIDTH;
	result.y = (my - wy  - HEIGHT  - (float)HEIGHT/4 + scroll_pos.vertical_pos * PIXELS_PER_SCROLL_VERT - GetSystemMetrics(SM_CYCAPTION)) / HEIGHT;
	return result;

}

void mouse_swapping(MATRIX initial, int m, int n) {
	HWND hwnd;
	RECT rect;	
	POINT point;
	SCROLL scroll_pos;
	int temp, x1, x2, y1, y2;
	FLAG click = 0;
	FLAG swap_success = 0;
	
	while (!swap_success) {
		printf("Click right button of your mouse to choose the candy you wanna swap.");
		while (click < 2) {
			Sleep(100);

			while (KEY_DOWN(VK_RBUTTON)) {
				
				hwnd = GetForegroundWindow();
				
				GetWindowRect(hwnd, &rect);
				
				GetCursorPos(&point);
				
				scroll_pos.vertical_pos = GetScrollPos(hwnd, SB_VERT);
				scroll_pos.horizontal_pos = GetScrollPos(hwnd, SB_HORZ); 
				Sleep(10);
				
				POINT temp = get_position(rect, point, scroll_pos);
				if (temp.x <0 || temp.x > n - 1 || temp.y < 0 || temp.y > m - 1)
					break;
				if (click == 0) {
					x1 = temp.x;
					y1 = temp.y;
					click_highlight(initial, m, n, temp);		
				}
				else {
					x2 = temp.x;
					y2 = temp.y;
					if ((abs(x1 - x2) + abs(y1 - y2)) > 1) {
						printf("Illegal swaping, please swap the adjacent candies\n");
						click = 0;
						Sleep(200);
						break;
					}
				}
				click++;
				/*if (click == 1) {
					SetScrollPos(hwnd, SB_VERT, scroll_pos.vertical_pos, FALSE);
					SetScrollPos(hwnd, SB_HORZ, scroll_pos.horizontal_pos, FALSE);
				}*/
				Sleep(200);
			}
		}
		temp = initial[y1][x1];
		initial[y1][x1] = initial[y2][x2];
		initial[y2][x2] = temp;
		print_matrix(initial, m, n);
		if (find_verticle_chain(initial, m, n) == NULL && find_horizontal_chain(initial, m, n) == NULL) {
			temp = initial[y1][x1];
			initial[y1][x1] = initial[y2][x2];
			initial[y2][x2] = temp;
			Sleep(1000);
			print_matrix(initial, m, n);
			printf("\t+--------------------------------+\n\t|OOPS, IT DOSN'T WORK, SWAP AGAIN|\n\t+--------------------------------+\n");
			click = 0;
		}
		else
			swap_success = 1;
	}
}

int highlight(int sign){
	printf("|\033[47;31m%2c \033[0m", character_list[sign - 6]);
	return 0;
}

int colorize(int sign) {
	switch (sign) {
	case 1:
		printf("|\033[41;32m%2c \033[0m", character_list[1]);
		break;
	case 2:
		printf("|\033[40;37m%2c \033[0m", character_list[2]);
		break;
	case 3:
		printf("|\033[42;31m%2c \033[0m", character_list[3]);
		break;
	case 4:
		printf("|\033[43;34m%2c \033[0m", character_list[4]);
		break;
	case 5:
		printf("|\033[45;32m%2c \033[0m", character_list[5]);
		break;
	case 6:
		printf("|\033[44;33m%2c \033[0m", character_list[6]);
		break;
	}
	return 0;
}

int click_colorize(int sign) {
	switch (sign) {
	case 1:
		printf("|\033[41m \033[0m");
		printf("\033[7;41;32m%1c\033[0m", character_list[1]);
		printf("\033[41m \033[0m");
		break;
	case 2:
		printf("|\033[40m \033[0m");
		printf("\033[7;40;37m%1c\033[0m", character_list[2]);
		printf("\033[40m \033[0m");
		break;
	case 3:
		printf("|\033[42m \033[0m");
		printf("\033[7;42;31m%1c\033[0m", character_list[3]);
		printf("\033[42m \033[0m");
		break;
	case 4:
		printf("|\033[43m \033[0m");
		printf("\033[7;43;34m%1c\033[0m", character_list[4]);
		printf("\033[43m \033[0m");
		break;
	case 5:
		printf("|\033[45m \033[0m");
		printf("\033[7;45;32m%1c\033[0m", character_list[5]);
		printf("\033[45m \033[0m");
		break;
	case 6:
		printf("|\033[44m \033[0m");
		printf("\033[7;44;33m%1c\033[0m", character_list[6]);
		printf("\033[44m \033[0m");
		break;
	}
	return 0;
}

void click_highlight(MATRIX initial, int m, int n, POINT position) {
	int i, j, k;
	system("cls");
	for (j = 0; j < n + 1; j++)
		printf("+---");
	printf("+\n|   ");
	for (j = 0; j < n; j++)
		printf("|%2d ", j);
	printf("|\n");
	for (j = 0; j < n + 1; j++)
		printf("+---");
	printf("+\n");
	for (i = 0; i < m; i++) {
		printf("|%2d ", i);
		for (j = 0; j < n; j++) {
			k = initial[i][j];
			if (position.x == j && position.y == i)
				click_colorize(k);
			else
				colorize(k);
		}
		printf("|\n");
		for (j = 0; j < n + 1; j++)
			printf("+---");
		printf("+\n");
	}
	printf("+------------+\n|SCORES: %4d|\n+------------+\n", score);
}

int prechecking(MATRIX initial, int m, int n) {
	int i, j, temp;
	for (i = 0; i < m; ++i)
		for (j = 0; j < n - 1; ++j) {
			temp = initial[i][j];
			initial[i][j] = initial[i][j + 1];
			initial[i][j + 1] = temp;

			TOCRUSH *pVert1 = find_verticle_chain(initial, m, n);
			if (pVert1 != NULL) {
				temp = initial[i][j];
				initial[i][j] = initial[i][j + 1];
				initial[i][j + 1] = temp;
				return 1;
			}
			TOCRUSH *pHorz1 = find_horizontal_chain(initial, m, n);
			if (pHorz1 != NULL) {
				temp = initial[i][j];
				initial[i][j] = initial[i][j + 1];
				initial[i][j + 1] = temp;
				return 1;
			}
			temp = initial[i][j];
			initial[i][j] = initial[i][j + 1];
			initial[i][j + 1] = temp;
		}
	for (j = 0; j < n; ++j)
		for (i = 0; i < m - 1; ++i) {
			temp = initial[i][j];
			initial[i][j] = initial[i + 1][j];
			initial[i + 1][j] = temp;
		
			TOCRUSH *pVert2 = find_verticle_chain(initial, m, n);
			if (pVert2 != NULL) {
				temp = initial[i][j];
				initial[i][j] = initial[i + 1][j];
				initial[i + 1][j] = temp;
				return 1;
			}
			TOCRUSH *pHorz2 = find_horizontal_chain(initial, m, n);
			if (pHorz2 != NULL) {
				temp = initial[i][j];
				initial[i][j] = initial[i + 1][j];
				initial[i + 1][j] = temp;
				return 1;
			}
			temp = initial[i][j];
			initial[i][j] = initial[i + 1][j];
			initial[i + 1][j] = temp;
		}
	return 0;
}

FLAG click_option(int is_gameover) {
	
	HWND hwnd;
	RECT rect;
	POINT point;
	FLAG click = 0;
	FLAG option_chosen = 0;
	while (option_chosen == 0) {
		printf("Now you can choose:\n\t+--------------------------+\n\t\
|1.to start new game       |\n\t+--------------------------+\n\t\
|2.to continue current game|\n\t+--------------------------+\n\t\
|3.to quit game            |\n\t+--------------------------+\n");
		if (swapping_mode == MOUSE_SWAPPING) {
			while (!option_chosen) {
				while (click < 1) {
					while (KEY_DOWN(VK_RBUTTON)) {
						hwnd = GetForegroundWindow();
						GetWindowRect(hwnd, &rect);

						GetCursorPos(&point);

						SCROLL scroll_pos = {
							GetScrollPos(hwnd, SB_VERT),
							GetScrollPos(hwnd, SB_HORZ)
						};

						POINT temp = get_position(rect, point, scroll_pos);

						if (temp.x < 1 || temp.x > 6 || temp.y < 0 || temp.y > 2)
							break;

						click++;
						option_chosen = temp.y + 1;

					}
					Sleep(100);
				}
			}
		}
		else {
			printf("Please enter the option number:\n");
			scanf("%d", &option_chosen);
			getchar();
		}
		system("cls");
		if (is_gameover && option_chosen == 2) {
			printf("All candies was stuck, please start new game or quit.\n");
			option_chosen = 0;
			click = 0;
			Sleep(1000);
			system("cls");
		}
	}
	return option_chosen;
}

void swap(MATRIX initial, int m, int n) {
	switch (swapping_mode) {
	case MOUSE_SWAPPING:
		mouse_swapping(initial, m, n);
		break;

	case TEXT_SWAPPING:
		advanced_swapping(initial, m, n);
		break;
	}
}
 
void presetting(void)
{
	HANDLE hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_FONT_INFOEX font_info;
	font_info.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	SetConsoleCP(437);
	SetConsoleOutputCP(437);
	GetCurrentConsoleFontEx(hConOut, 0, &font_info);
	COORD text = GetConsoleFontSize(hConOut, font_info.nFont);
	PIXELS_PER_SCROLL_VERT = text.Y; HEIGHT = 2 * text.Y;
	PIXELS_PER_SCROLL_HORZ = text.X; WIDTH = 4 * text.X;
	
	
	Sleep(10);
	/*wchar_t* font = L"Courier New";
	wcscpy(font_info.FaceName, font);
	SetCurrentConsoleFontEx(hConOut, 0, &font_info);
	GetCurrentConsoleFontEx(hConOut, 0, &font_info);
	printf("%ls\n", font_info.FaceName);
	system("pause");*/
	COORD coord = {
		 9001,9001
	};
	
	SetConsoleScreenBufferSize(hConOut, coord);
	printf("%d %d", text.X, text.Y);
	system("pause");
	HDC hdcScreen = GetDC(NULL);
	float dpi_default = (float)GetDeviceCaps(hdcScreen, DESKTOPHORZRES) / GetDeviceCaps(hdcScreen, HORZRES);
	float dpi_personalize = (float)GetDeviceCaps(hdcScreen, LOGPIXELSX) / 0.96 / 100;
	if (dpi_default == 1.0)
		dpi = dpi_personalize;
	else
		dpi = dpi_default;

	int precised_100dpi = (int)(dpi * 100);
	if (precised_100dpi != 100 && precised_100dpi != 125 && precised_100dpi != 150 && precised_100dpi != 175) {
		printf("*******\nTIPS:\n\tYour screen zooming ratio (SZR) is %d%%, which is not recommended.\n ", precised_100dpi);
		printf("\tIf you want to play this game with your mouse, please\n\treset your SZR to 100%%/125%%/150%%/175%%\n");
		printf("\tElse you can also play this through keyboard typing:)\n\n*******\n");
		swapping_mode = TEXT_SWAPPING;
		system("pause");
	}
	else {
		printf("*******\nTIPS:\n\tYou can play this game with your mouse.\n ");
		printf("\tIf you want to play this game with your keyboard by typing something, please\n\t");
		printf("type 1 (Press enter to ignore this):\n");
		if (getchar() == 1)
			swapping_mode = TEXT_SWAPPING;
		else
			swapping_mode = MOUSE_SWAPPING;
	}
	system("cls");
}

int main() {
	int m, n;
	int turn_score = 0;
	int score_mod = 0;
	MATRIX initial;
	FLAG play_again = 2;
	FLAG gameover = 0;
	FLAG matrix_exist = 1;
	
	presetting();

	while (1) {
		printf("Enter the size of the gaming board by \"m n\", \nwhere m is the HEIGHT and n is the WIDTH\n");
		scanf("%d %d", &m, &n);
		getchar();
		if (m < 3 && n < 3)
			printf("*******\nSORRY!\n\tThe map is too small to play.\n\tPlease make at least one side has the length longer than 3.\n\tEnter again.\n\n*******\n");
		else
			system("cls");
			break;
	}

	initial = generate_matrix(m, n);
	do {
		fill_matrix(initial, m, n);
	}while (prechecking(initial, m, n) == 0);
	
	while ((turn_score = crush(initial, m, n, 0)) != 0) {
		while (candy_drop_onestep_no_print(initial, m, n) != 1);
	}

	Sleep(500);
	system("cls");
	printf("Map generated, enjoy your game:)");
	matrix_exist = 1;

	while (play_again != 3) {
		if (play_again == 1 && !matrix_exist) {
			score = 0;
			printf("enter the size of the gaming board by \"m n\", where m is the HEIGHT and n is the WIDTH\n");
			setbuf(stdin, NULL);
			scanf("%d %d", &m, &n);
			printf("Map generating, please wait...");
			getchar();

			initial = generate_matrix(m, n);
			do {
				fill_matrix(initial, m, n);
			} while (prechecking(initial, m, n) == 0);

			system("cls");
			printf("Map generated, enjoy your game:)");
			Sleep(2000);
			print_matrix(initial, m, n);
			Sleep(1500);
			do {
				while ((turn_score = crush(initial, m, n, 1)) != 0) {
					score += turn_score;
					print_matrix(initial, m, n);
					Sleep(1000);
					print_dropped_matrix(initial, m, n);
					Sleep(400);
					if ((score - MAX_SCORE * score_mod) >= MAX_SCORE) {
						break;
					}
				}
				if ((score - MAX_SCORE * score_mod) >= MAX_SCORE) {
					score_mod += (score - MAX_SCORE * score_mod) % MAX_SCORE;
					break;
				}
				if (prechecking(initial, m, n) == 0) {
					printf("All candies got stuck. GAMEOVER:(");
					gameover = 1;
					Sleep(7000);
					break;
				}
				swap(initial, m, n);
				Sleep(1500);
			} while (1);
			system("cls");
			if (!gameover)
				printf("Congratulations! You've reached 100 scores.\n");
			system("cls");
			if ((play_again = click_option(gameover)) == 1) {
				for (int i = 0; i < n; ++i)
					free(initial[i]);
				free(initial);
				matrix_exist = 0;
				score_mod = 0;
			}
		}
		else {
			system("cls");
			printf("Ready? We'll go:)");
			Sleep(2000);
			print_matrix(initial, m, n);
			Sleep(1500);
			do {
				while ((turn_score = crush(initial, m, n, 1)) != 0) {
					score += turn_score;
					print_matrix(initial, m, n);
					Sleep(1000);
					print_dropped_matrix(initial, m, n);
					Sleep(400);
					if ((score - MAX_SCORE * score_mod) >= MAX_SCORE) {
						break;
					}
				}
				if ((score - MAX_SCORE * score_mod) >= MAX_SCORE) {
					score_mod+= (score - MAX_SCORE * score_mod)/ MAX_SCORE;
					break;
				}
				if (prechecking(initial, m, n) == 0) {
					printf("All candies got stuck. GAMEOVER:(");
					gameover = 1;
					Sleep(7000);
					break;
				}
				swap(initial, m, n);
				Sleep(1500);
			} while (1);
			system("cls");
			if (!gameover)
				printf("*******CONGRATUATIONS!\n\tYou've reached %d00 scores.\n\n*******\n", score_mod);
			Sleep(4000);
			system("cls");
			setbuf(stdin, NULL);
			if ((play_again = click_option(gameover)) == 1) {
				for (int i = 0; i < m; ++i)
					free(initial[i]);
				free(initial);
				matrix_exist = 0;
				score_mod = 0;
			}
		}
	}
	system("cls");
	printf("\t+----+\n\t|bye~|\n\t+----+");
	system("pause");
	return 0;
}
