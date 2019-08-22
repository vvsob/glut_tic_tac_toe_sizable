#define FIGURE_X 2
#define FIGURE_O 1

#include <cstdlib>
#include "glut.h"
#include <cmath>

#include <Windows.h>
#include <string>

unsigned int tableSize = 5;
unsigned int figToWin = 4;

int win_x_size = 700;
int win_y_size = 700;

int border_indent = 50;

int frame_step = (win_x_size - border_indent * 2) / tableSize;

short int move = 0;
bool gameOver = 0;
bool firstPlayer = 1;


short int **table = new short int*[tableSize];

bool computerPlayer = 0;

short int **cellPriority = new short int*[tableSize];

/*short int cellPriority[3][3] = {2, 1, 2,
								  1, 3, 1,
								  2, 1, 2};*/

bool **linePoints = new bool*[tableSize];

void init() {
	for (unsigned int i = 0; i < tableSize; i++)
		table[i] = new short int[tableSize];

	for (unsigned int i = 0; i < tableSize; i++)
		cellPriority[i] = new short int[tableSize];

	for (unsigned int i = 0; i < tableSize; i++)
		linePoints[i] = new bool[tableSize];

	for (unsigned int i = 0; i < tableSize; i++)
		for (unsigned int j = 0; j < tableSize; j++) {
			table[i][j] = 0;
			linePoints[i][j] = 0;
		}

	for (unsigned int i = 0; i < tableSize; i++) //set cell priorities
		for (unsigned int j = 0; j < tableSize; j++) {
			if (i == 0 || j == 0 || i == tableSize - 1 || j == tableSize - 1) {
				cellPriority[i][j] = 1;
				if (i == j || j == tableSize - i - 1)
					cellPriority[i][j] = 2;
			} else
				cellPriority[i][j] = 3;
		}
}

void changeSize(int w, int h) {
	// предупредим деление на ноль
	// если окно сильно перетянуто будет
	win_x_size = w;
	win_y_size = h;

	if (h == 0)
		h = 1;
	float ratio = 1.0 * w / h;

	// используем матрицу проекции
	glMatrixMode(GL_PROJECTION);

	// Reset матрицы
	glLoadIdentity();

	// определяем окно просмотра
	glViewport(0, 0, w, h);

	// установить корректную перспективу.
	glOrtho(0.0f, win_x_size, win_y_size, 0.0f, 0.0f, 1.0f);

	// вернуться к модели
	glMatrixMode(GL_MODELVIEW);
}

void drawX(int x, int y) {
	int fig_indent = 10;
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(x - frame_step / 2 + fig_indent, y - frame_step / 2 + fig_indent);
	glVertex2f(x + frame_step / 2 - fig_indent, y + frame_step / 2 - fig_indent);
	glVertex2f(x - frame_step / 2 + fig_indent, y + frame_step / 2 - fig_indent);
	glVertex2f(x + frame_step / 2 - fig_indent, y - frame_step / 2 + fig_indent);
	glEnd();
	glColor3f(1, 1, 1);
}

void drawO(int x, int y) {
	glColor3f(0, 0, 1);
	#define PI 3.14159265
	//Количество отрезков, на которое мы подразделяем
	//гладкую границу круга
	#define EDGES 30
	int radius = frame_step / 2 - 10;
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < EDGES; i++)
		glVertex2f(radius * cos((2 * PI*i) / EDGES) + x, radius * sin((2 * PI*i) / EDGES) + y);
	glEnd();
	glColor3f(1, 1, 1);
}

void drawLine() {
	bool temp = 0;
	glColor3f(0, 1, 0);
	glLineWidth(3);
	glBegin(GL_LINES);
	for (int i = 0; i < tableSize; i++)
		for (int j = 0; j < tableSize; j++) {
			if (linePoints[i][j]) {
				glVertex2f(j * frame_step + border_indent + frame_step / 2,
					i * frame_step + border_indent + frame_step / 2);
			}
		}
	glEnd();
	glColor3f(1, 1, 1);
	glLineWidth(1);

}


short int winCheck() {
	unsigned int count = 0;

	for (unsigned int i = 0; i < tableSize; i++)
		for (unsigned int j = 0; j < tableSize; j++) {
			//horizontal
			for (unsigned int k = i; k < tableSize; k++)
				if (table[i][j] == table[k][j])
					count++;
				else
					break;
			if (count >= figToWin)
				return table[i][j];
			count = 0;

			//vertical
			for (unsigned int k = j; k < tableSize; k++)
				if (table[i][j] == table[i][k])
					count++;
				else
					break;
			if (count >= figToWin)
				return table[i][j];
			count = 0;

			//diagonal main
			for (unsigned int k = 0; i + k < tableSize && j + k < tableSize; k++)
				if (table[i][j] == table[i + k][j + k])
					count++;
				else
					break;
			if (count >= figToWin)
				return table[i][j];
			count = 0;

			//diagonal side
			for (unsigned int k = 0; i + k < tableSize && j - k < tableSize; k++)
				if (table[i][j] == table[i + k][j - k])
					count++;
				else
					break;
			if (count >= figToWin)
				return table[i][j];
			count = 0;
		}

	return 0;
}

void makeLinePoints() {
	for (int i = 0; i < 3; i++) {
		if (table[0][i] == table[1][i] && table[1][i] == table[2][i] && table[0][i] > 0) {
			linePoints[0][i] = 1;
			linePoints[2][i] = 1;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (table[i][0] == table[i][1] && table[i][1] == table[i][2] && table[i][0] > 0) {
			linePoints[i][0] = 1;
			linePoints[i][2] = 1;
		}
	}

	if (table[0][0] == table[1][1] && table[1][1] == table[2][2] && table[0][0] > 0) {
		linePoints[0][0] = 1;
		linePoints[2][2] = 1;
	}
	if (table[0][2] == table[1][1] && table[1][1] == table[2][0] && table[0][2] > 0) {
		linePoints[0][2] = 1;
		linePoints[2][0] = 1;
	}
}

void restart() {
	for (unsigned int i = 0; i < tableSize; i++)
		for (unsigned int j = 0; j < tableSize; j++) {
			table[j][i] = 0;
			linePoints[j][i] = 0;
		}
	gameOver = 0;
	move = 0;

	for (unsigned int i = 0; i < tableSize; i++) //reset cell priorities
		for (unsigned int j = 0; j < tableSize; j++) {
			if (i - j == 1 || j - i == 1)
				cellPriority[i][j] = 1;
			else
				cellPriority[i][j] = 2;
			if (i == 1 && j == 1)
				cellPriority[i][j] = 3;
		}
}

void computerSetPriorities() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (table[i][j] == 0) {
				table[i][j] = FIGURE_O; //Win move check
				if (winCheck() > 0)
					cellPriority[i][j] = 6;

				table[i][j] = FIGURE_X; //Enemy win move check
				if (winCheck() > 0)
					cellPriority[i][j] = 5;

				table[i][j] = 0;
			}
		}
	}
}

void computerMove() { 
	computerSetPriorities();

	short int moveX, moveY, max = 0;
	for (unsigned int i = 0; i < tableSize; i++)
		for (unsigned int j = 0; j < tableSize; j++)
			if (cellPriority[i][j] > max && table[i][j] == 0) {
				moveX = i;
				moveY = j;
				max = cellPriority[i][j];
			}
	table[moveX][moveY] = FIGURE_O; //computer is always O

	computerSetPriorities();
}

void clickCheck(int ind_x, int ind_y) {
	if (table[ind_y][ind_x] < 1 ) {
		if (firstPlayer)
			table[ind_y][ind_x] = (move + 1) % 2 + 1;
		else
			table[ind_y][ind_x] = (move) % 2 + 1;
		move++;
	}
}

void drawFigs() {
	glLineWidth(5);
	for (unsigned int i = 0; i < tableSize; i++)
		for (unsigned int j = 0; j < tableSize; j++) {
			if (table[i][j] == FIGURE_X)
				drawX(j * frame_step + border_indent + frame_step / 2,
					i * frame_step + border_indent + frame_step / 2);
			if (table[i][j] == FIGURE_O)
				drawO(j * frame_step + border_indent + frame_step / 2,
					i * frame_step + border_indent + frame_step / 2);
		}
	glLineWidth(1);
}



void RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT);
	if (!gameOver) {
		glBegin(GL_LINE_LOOP); //frame
		glVertex2f(border_indent, border_indent);
		glVertex2f(win_x_size - border_indent, border_indent);
		glVertex2f(win_x_size - border_indent, win_y_size - border_indent);
		glVertex2f(border_indent, win_y_size - border_indent);
		glEnd();

		glBegin(GL_LINES);
		for (unsigned int i = 1; i < tableSize + 1; i++) {
			glVertex2f(border_indent + frame_step * i, border_indent);
			glVertex2f(border_indent + frame_step * i, win_y_size - border_indent);
		}

		for (unsigned int i = 1; i < tableSize + 1; i++) {
			glVertex2f(border_indent, border_indent + frame_step * i);
			glVertex2f(win_x_size - border_indent, border_indent + frame_step * i);
		}
		glEnd();

		drawFigs();

		drawLine();
	} else {
		char drawMsg[] = "DRAW\0";
		char winMsg[] = "WON\0";


		char* c = new char[10];

		if (winCheck() == FIGURE_X && !computerPlayer)
			sprintf_s(c, 10, "X's %s", winMsg);
		if (winCheck() == FIGURE_O && !computerPlayer)
			sprintf_s(c, 10, "O's %s", winMsg);
		if (winCheck() == FIGURE_X && computerPlayer)
			sprintf_s(c, 10, "YOU %s", winMsg);
		if (winCheck() == FIGURE_O && computerPlayer)
			sprintf_s(c, 10, "BOT %s", winMsg);

		int x1 = win_x_size / 2 - glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, '0') * 5;
		if (move == 9 && winCheck() == 0) {
			c = drawMsg;
			x1 += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, '0') * 2;
		}

		
		for (; *c != '\0'; c++) {
			glRasterPos2f(x1, 30);
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
			x1 += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, *c) + 5;
		}

	}

	glutSwapBuffers();
}

void MousePosition(int x, int y) {

}

void MouseClick(int button, int state, int width, int height) {
	if (state == GLUT_DOWN)
		return;

	int ind_x = (width - border_indent) / frame_step;
	int ind_y = (height - border_indent) / frame_step;
	if (!gameOver &&
		width < win_x_size - border_indent && //right
		width > border_indent && //left
		height < win_y_size - border_indent && // down
		height > border_indent // up
		)
		clickCheck(ind_x, ind_y);
}

void processMoveMenu(int option) {
	if (move > 0) {
		if (MessageBox(NULL, "Game already has started. Restart game?", "Game started", MB_ICONWARNING | MB_YESNO) == IDYES)
			restart();
		else
			return;
	}
	switch (option) {
		case 1:
			firstPlayer = FIGURE_X - 1;
			break;
		case 2:
			firstPlayer = FIGURE_O - 1;
			break;
	}
}

void processSecondPlayerMenu(int option) {
	if (move > 0) {
		if (MessageBox(NULL, "Game already has started. Restart game?", "Game started", MB_ICONWARNING | MB_YESNO) == IDYES)
			restart();
		else
			return;
	}
	switch (option) {
	case 1:
		computerPlayer = 0;
		break;
	case 2:
		computerPlayer = 1;
		break;
	}
}

void processMainMenu(int option) {
	restart();
}

void update(int value) {

	RenderScene();
	if ((!gameOver && winCheck() > 0) || (move == tableSize * tableSize)) {
		makeLinePoints();
		RenderScene();
		gameOver = 1;
		Sleep(1500);

		RenderScene();

		if (MessageBox(NULL, "Do you want to continue?", "Continue?", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			restart();
		else
			exit(0);
	}
	
	if (computerPlayer && (bool(move % 2) == firstPlayer) && !gameOver) {
		computerMove();
		move++;
	}

	glutTimerFunc(16, update, 1);
}

int main(int argc, char** argv) {
	init();

	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(win_x_size, win_y_size);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Tic-Tac-Toe");

	glutDisplayFunc(RenderScene);
	glutReshapeFunc(changeSize);

	glutTimerFunc(16, update, 1); //timer 16ms, about 60 fps

	glutMotionFunc(MousePosition);
	glutMouseFunc(MouseClick);

	int moveMenu;
	int secondPlayerMenu;
	int mainMenu;


	moveMenu = glutCreateMenu(processMoveMenu);
	glutAddMenuEntry("X", 1);
	glutAddMenuEntry("O", 2);

	secondPlayerMenu = glutCreateMenu(processSecondPlayerMenu);
	glutAddMenuEntry("Play with human", 1);
	glutAddMenuEntry("Play with Computer", 2);

	mainMenu = glutCreateMenu(processMainMenu);
	glutAddSubMenu("Who moves first", moveMenu);
	glutAddSubMenu("Second Player", secondPlayerMenu);
	glutAddMenuEntry("Restart", 1);

	glutAttachMenu(GLUT_RIGHT_BUTTON);


	glutMainLoop();
}