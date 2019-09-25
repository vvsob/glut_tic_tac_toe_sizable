#define FIGURE_X 2
#define FIGURE_O 1

#define VECT_HOR 0
#define VECT_VERT 1
#define VECT_MAIN_DIAG 2
#define VECT_SIDE_DIAG 3
#define VECT_REV_HOR 4
#define VECT_REV_VERT 5
#define VECT_REV_MAIN_DIAG 6
#define VECT_REV_SIDE_DIAG 7

#include <cstdlib>
#include <cmath>

#include <Windows.h>
#include <string>

#include <iostream>
#include "glut.h"
unsigned int tableSize = 7;
unsigned int figToWin = 5;

int win_x_size = 700;
int win_y_size = 700;

int border_indent = 50;

int frame_step = (win_x_size - border_indent * 2) / tableSize;

short int move = 0;
bool gameOver = 0;
bool firstPlayer = 1;


short int **table = new short int*[tableSize];

bool compPlay = 0;

short int **cellPr = new short int*[tableSize];
short int **cellPrBon = new short int*[tableSize];

/*short int cellPr[3][3] = {2, 1, 2,
								  1, 3, 1,
								  2, 1, 2};*/

bool **linePnt = new bool*[tableSize];

struct point {
	unsigned int i;
	unsigned int j;
};

void init() {
	win_x_size -= (win_x_size - border_indent * 2) % tableSize;
	win_y_size = win_x_size;

	for (unsigned int i = 0; i < tableSize; i++)
		table[i] = new short int[tableSize];

	for (unsigned int i = 0; i < tableSize; i++)
		cellPr[i] = new short int[tableSize];

	for (unsigned int i = 0; i < tableSize; i++)
		cellPrBon[i] = new short int[tableSize];

	for (unsigned int i = 0; i < tableSize; i++)
		linePnt[i] = new bool[tableSize];

	for (unsigned int i = 0; i < tableSize; i++)
		for (unsigned int j = 0; j < tableSize; j++) {
			table[i][j] = 0;
			linePnt[i][j] = 0;
			cellPrBon[i][j] = 0;
		}

	for (unsigned int i = 0; i < tableSize; i++) //set cell priorities
		for (unsigned int j = 0; j < tableSize; j++) {
			if (i == 0 || j == 0 || i == tableSize - 1 || j == tableSize - 1) {
				cellPr[i][j] = 1;
				if (i == j || j == tableSize - i - 1)
					cellPr[i][j] = 2;
			} else
				cellPr[i][j] = 3;
		}
}

void logBotPriority() {
	for (unsigned int i = 0; i < tableSize; i++) {
		for (unsigned int j = 0; j < tableSize; j++)
			std::cout << cellPrBon[i][j] << ' ';
		std::cout << '\n';
	}
	std::cout << '\n';
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
	for (unsigned int i = 0; i < tableSize; i++) {
		for (unsigned int j = 0; j < tableSize; j++) {
			if (linePnt[i][j]) {
				glVertex2f((j * frame_step) + border_indent + (frame_step / 2), (i * frame_step) + border_indent + (frame_step / 2));
			}
			
		}
	}
	glEnd();
	glColor3f(1, 1, 1);
	glLineWidth(1);

}

point corrPnt(point pnt) {
	if (pnt.i < 0)
		pnt.i = 0;

	if (pnt.j < 0)
		pnt.j = 0;
	
	if (pnt.i >= tableSize)
		pnt.i = tableSize - 1;

	if (pnt.j >= tableSize - 1)
		pnt.j = tableSize - 1;
	return pnt;
}

point incrVectInd(int direction, point pnt) {
	switch (direction)
	{
	case VECT_HOR:
		pnt.j++;
		return pnt;
	case VECT_VERT:
		pnt.i++;
		return pnt;
	case VECT_MAIN_DIAG:
		pnt.i++;
		pnt.j++;
		return pnt;
	case VECT_SIDE_DIAG:
		pnt.i--;
		pnt.j++;
		return pnt;
	case VECT_REV_HOR:
		pnt.j--;
		return pnt;
	case VECT_REV_VERT:
		pnt.i--;
		return pnt;
	case VECT_REV_MAIN_DIAG:
		pnt.i--;
		pnt.j--;
		return pnt;
	case VECT_REV_SIDE_DIAG:
		pnt.i++;
		pnt.j--;
		return pnt;
	}
	return pnt;
}

point incrVectInd(int direction, int i, int j) {
	point pnt;
	pnt.i = i;
	pnt.j = j;
	switch (direction)
	{
	case VECT_HOR:
		pnt.j++;
		return pnt;
	case VECT_VERT:
		pnt.i++;
		return pnt;
	case VECT_MAIN_DIAG:
		pnt.i++;
		pnt.j++;
		return pnt;
	case VECT_SIDE_DIAG:
		pnt.i--;
		pnt.j++;
		return pnt;
	case VECT_REV_HOR:
		pnt.j--;
		return pnt;
	case VECT_REV_VERT:
		pnt.i--;
		return pnt;
	case VECT_REV_MAIN_DIAG:
		pnt.i--;
		pnt.j--;
		return pnt;
	case VECT_REV_SIDE_DIAG:
		pnt.i++;
		pnt.j--;
		return pnt;
	}
	return pnt;
}

template<typename T>
void directions(T *returnArray, short int i, short int j) {
	unsigned int count = 0;
	if (table[i][j] == 0)
	{
		for (unsigned int k = 0; k < 4; k++)
			returnArray[k] = 0;
		return;
	}
	//horizontal
	for (unsigned int k = j; k < tableSize; k++)
		if (table[i][j] == table[i][k])
			count++;
		else
			break;
	returnArray[VECT_HOR] = count;
	count = 0;

	//VERT
	for (unsigned int k = i; k < tableSize; k++)
		if (table[i][j] == table[k][j])
			count++;
		else
			break;
	returnArray[VECT_VERT] = count;
	count = 0;

	//DIAG main
	for (unsigned int k = 0; i + k < tableSize && j + k < tableSize; k++)
		if (table[i][j] == table[i + k][j + k])
			count++;
		else
			break;
	returnArray[VECT_MAIN_DIAG] = count;
	count = 0;

	//DIAG side
	for (unsigned int k = 0; i - k < tableSize && j + k < tableSize; k++)
		if (table[i][j] == table[i - k][j + k])
			count++;
		else
			break;
	returnArray[VECT_SIDE_DIAG] = count;
}

short int winCheck() {
	unsigned int count = 0;

	for (unsigned int i = 0; i < tableSize; i++)
		for (unsigned int j = 0; j < tableSize; j++) {
			if (table[i][j] == 0)
				continue;
			unsigned int* winDirections = new unsigned int[4];
			directions(winDirections, i, j);
			for (int t = 0; t < 4; t++)
				if (winDirections[t] >= figToWin)
					return table[i][j];
		}

	return 0;
}

void makelinePnt() {
	unsigned int* pntDir = new unsigned int[4];

	for (unsigned int i = 0; i < tableSize; i++)
		for (unsigned int j = 0; j < tableSize; j++) {
			directions(pntDir, i, j);
			//for debug
			for (unsigned int k = 0; k < 4; k++) {
				std::cout << pntDir[k] << ' ';
			}
			std::cout << '\n';
			//HOR
			if (pntDir[VECT_HOR] >= figToWin) {
				linePnt[i][j] = 1;
				linePnt[i][j + pntDir[VECT_HOR] - 1] = 1;
				return;
			}
			//VERT
			if (pntDir[VECT_VERT] >= figToWin) {
				linePnt[i][j] = 1;
				linePnt[i + pntDir[VECT_VERT] - 1][j] = 1;
				return;
			}
			//main DIAG
			if (pntDir[VECT_MAIN_DIAG] >= figToWin) {
				linePnt[i][j] = 1;
				linePnt[i + pntDir[VECT_MAIN_DIAG] - 1][j + pntDir[VECT_MAIN_DIAG] - 1] = 1;
				return;
			}
			//side DIAG
			if (pntDir[VECT_SIDE_DIAG] >= figToWin) {
				linePnt[i][j] = 1;
				linePnt[i - pntDir[VECT_SIDE_DIAG] + 1][j + pntDir[VECT_SIDE_DIAG] - 1] = 1;
				return;
			}
		}


}

void restart() {
	for (unsigned int i = 0; i < tableSize; i++)
		for (unsigned int j = 0; j < tableSize; j++) {
			table[j][i] = 0;
			linePnt[j][i] = 0;
			cellPrBon[i][j] = 0;
		}
	gameOver = 0;
	move = 0;

	for (unsigned int i = 0; i < tableSize; i++) //reset cell priorities
		for (unsigned int j = 0; j < tableSize; j++) {
			if (i - j == 1 || j - i == 1)
				cellPr[i][j] = 1;
			else
				cellPr[i][j] = 2;
			if (i == 1 && j == 1)
				cellPr[i][j] = 3;
		}
}

void computerSetPriorities() {
	int* pntDir = new int[4];
	for (unsigned int i = 0; i < tableSize; i++) {
		for (unsigned int j = 0; j < tableSize; j++) {
			if (table[i][j] == 0)
				continue;
			point A, B;
			A.i = i;
			A.j = j;
			B = A;
			directions(pntDir, i, j);
			for (int k = 0; k < 4; k++) 
			{
				A.i = i;
				A.j = j;
				B = A;
				A = incrVectInd(k + 4, A);
				for (int t = 0; t < pntDir[k]; t++)
					B = incrVectInd(k, B);
				A = corrPnt(A);
				B = corrPnt(B);
				if (pntDir[k] > cellPrBon[A.i][A.j])
					cellPrBon[A.i][A.j] = pntDir[k];
				if (pntDir[k] > cellPrBon[B.i][B.j])
					cellPrBon[B.i][B.j] = pntDir[k];
				std::cout << "A[" << A.i << "][" << A.j << "] = " << cellPrBon[A.i][A.j] << '\n';
				std::cout << "B[" << B.i << "][" << B.j << "] = " << cellPrBon[B.i][B.j] << '\n';
			}

		}
	}
}

void computerMove() { 
	computerSetPriorities();

	short int moveX = 0, moveY = 0, max = 0;
	for (unsigned int i = 0; i < tableSize; i++)
		for (unsigned int j = 0; j < tableSize; j++)
			if (cellPr[i][j] + cellPrBon[i][j] > max && table[i][j] == 0) {
				moveX = i;
				moveY = j;
				max = cellPr[i][j] + cellPrBon[i][j];
			}
	table[moveX][moveY] = FIGURE_O; //computer is always O

	//computerSetPriorities();

	logBotPriority();
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
		for (int i = 0; i < 10;i++)
			c[i] = '1';
		if (winCheck() == FIGURE_X && !compPlay)
			sprintf_s(c, 10, "X's %s", winMsg);
		else if (winCheck() == FIGURE_O && !compPlay)
			sprintf_s(c, 10, "O's %s", winMsg);
		else if (winCheck() == FIGURE_X && compPlay)
			sprintf_s(c, 10, "YOU %s", winMsg);
		else //if (winCheck() == FIGURE_O && compPlay)
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
		compPlay = 0;
		break;
	case 2:
		compPlay = 1;
		break;
	}
}

void processMainMenu(int option) {
	restart();
}

void update(int value) {

	RenderScene();
	if ((!gameOver && winCheck() > 0) || (move == tableSize * tableSize)) {
		makelinePnt();
		//for debug
		for (unsigned int i = 0; i < tableSize; i++) {
			for (unsigned int j = 0; j < tableSize; j++) {
				std::cout << linePnt[i][j] << ' ';
			}
			std::cout << '\n';
		}
		RenderScene();
		gameOver = 1;
		Sleep(1500);

		RenderScene();

		if (MessageBox(NULL, "Do you want to continue?", "Continue?", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			restart();
		else
			exit(0);
	}
	
	if (compPlay && (bool(move % 2) == firstPlayer) && !gameOver) {
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