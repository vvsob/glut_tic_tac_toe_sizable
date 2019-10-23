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
#include <random>
#include <ctime>
#include <iostream>
#include <fstream>
#include "glut.h"

unsigned int tableSize = 15;
unsigned int figToWin = 5;

int win_x_size = 800;
int win_y_size = 800;

int border_indent = 50;

int frame_step = (win_x_size - border_indent * 2) / tableSize;

short int move = 0;
bool gameOver = 0;
bool firstPlayer = 1;

short int difficulty = 2;

short int **t;

bool compPlay = 1;

//short int **cellPr = new short int*[tableSize];
short int ***cellPr;

bool **linePnt;

struct point {
	int i;
	int j;
};

void init() {
	t = new short int* [tableSize];
	cellPr = new short int** [2];
	linePnt = new bool* [tableSize];

	frame_step = (win_x_size - border_indent * 2) / tableSize;

	win_x_size -= (win_x_size - border_indent * 2) % tableSize;
	win_y_size = win_x_size;

	for (unsigned int i = 0; i < tableSize; i++)
		t[i] = new short int[tableSize];

	//for (unsigned int i = 0; i < tableSize; i++)
	//	cellPr[i] = new short int[tableSize];


	for (unsigned int i = 0; i < tableSize; i++)
		linePnt[i] = new bool[tableSize];

	for (unsigned int i = 0; i < tableSize; i++)
		for (unsigned int j = 0; j < tableSize; j++) {
			t[i][j] = 0;
			linePnt[i][j] = 0;
		}

	for (unsigned int k = 0; k < 2; k++)
		cellPr[k] = new short int*[tableSize];

	for (unsigned int k = 0; k < 2; k++)
		for (unsigned int i = 0; i < tableSize; i++)
			cellPr[k][i] = new short int[tableSize];

	for (unsigned int k = 0; k < 2; k++)
		for (unsigned int i = 0; i < tableSize; i++)
			for (unsigned int j = 0; j < tableSize; j++)
				cellPr[k][i][j] = 0;


	/*
	for (unsigned int i = 0; i < tableSize; i++) //set cell priorities
		for (unsigned int j = 0; j < tableSize; j++) {
			if (i == 0 || j == 0 || i == tableSize - 1 || j == tableSize - 1) {
				cellPr[i][j] = 1;
				if (i == j || j == tableSize - i - 1)
					cellPr[i][j] = 2;
			} else
				cellPr[i][j] = 3;
		}*/
}

void runSettings() {
	TCHAR buffer[MAX_PATH];
	GetCurrentDirectory(sizeof(buffer), buffer);
	std::string filepath = "\\Setting_tic_toc.exe";
	filepath = buffer + filepath;

	STARTUPINFO cif;
	ZeroMemory(&cif, sizeof(STARTUPINFO));
	PROCESS_INFORMATION pi;
	CreateProcess(filepath.c_str(), NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &cif, &pi);
	std::cout << "handle " << pi.hProcess << std::endl;
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	char buff[50];
	std::ifstream fin("buf.txt");
	if (!fin.is_open())
		std::cout << "Failed to open file!\n";
	else
	{
		fin >> buff;
		std::cout << "t size " << buff << std::endl;
		tableSize = atoi(buff);
		fin >> buff;
		std::cout << "Fig to win " << buff << std::endl;
		figToWin = atoi(buff);

		init();

		fin.close();
	}


}

void logBotPriority() {
	for (unsigned int k = 0; k < 2; k++) {
		if (k == 0)
			std::cout << "O's:\n";
		else
			std::cout << "X's\n";

		for (unsigned int i = 0; i < tableSize; i++) {
			for (unsigned int j = 0; j < tableSize; j++)
				std::cout << cellPr[k][i][j] << ' ';
			std::cout << '\n';
		}
		std::cout << '\n';
	}
}

void changeSize(int w, int h) {
	w = win_x_size;
	h = win_y_size;

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
	
	if (unsigned(pnt.i) >= tableSize)
		pnt.i = tableSize - 1;

	if (unsigned(pnt.j) >= tableSize - 1)
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
	if (t[i][j] == 0)
	{
		for (unsigned int k = 0; k < 4; k++)
			returnArray[k] = 0;
		return;
	}
	//horizontal
	for (unsigned int k = j; k < tableSize; k++)
		if (t[i][j] == t[i][k])
			count++;
		else
			break;
	returnArray[VECT_HOR] = count;
	count = 0;

	//VERT
	for (unsigned int k = i; k < tableSize; k++)
		if (t[i][j] == t[k][j])
			count++;
		else
			break;
	returnArray[VECT_VERT] = count;
	count = 0;

	//DIAG main
	for (unsigned int k = 0; i + k < tableSize && j + k < tableSize; k++)
		if (t[i][j] == t[i + k][j + k])
			count++;
		else
			break;
	returnArray[VECT_MAIN_DIAG] = count;
	count = 0;

	//DIAG side
	for (unsigned int k = 0; i - k < tableSize && j + k < tableSize; k++)
		if (t[i][j] == t[i - k][j + k])
			count++;
		else
			break;
	returnArray[VECT_SIDE_DIAG] = count;
}

template<typename T>
void directions(T* returnArray, point P) {
	unsigned int count = 0;
	if (t[P.i][P.j] == 0)
	{
		for (unsigned int k = 0; k < 4; k++)
			returnArray[k] = 0;
		return;
	}
	//horizontal
	for (unsigned int k = P.j; k < tableSize; k++)
		if (t[P.i][P.j] == t[P.i][k])
			count++;
		else
			break;
	returnArray[VECT_HOR] = count;
	count = 0;

	//VERT
	for (unsigned int k = P.i; k < tableSize; k++)
		if (t[P.i][P.j] == t[k][P.j])
			count++;
		else
			break;
	returnArray[VECT_VERT] = count;
	count = 0;

	//DIAG main
	for (unsigned int k = 0; P.i + k < tableSize && P.j + k < tableSize; k++)
		if (t[P.i][P.j] == t[P.i + k][P.j + k])
			count++;
		else
			break;
	returnArray[VECT_MAIN_DIAG] = count;
	count = 0;

	//DIAG side
	for (unsigned int k = 0; P.i - k < tableSize && P.j + k < tableSize; k++)
		if (t[P.i][P.j] == t[P.i - k][P.j + k])
			count++;
		else
			break;
	returnArray[VECT_SIDE_DIAG] = count;
}

short int winCheck() {
	unsigned int count = 0;

	for (unsigned int i = 0; i < tableSize; i++)
		for (unsigned int j = 0; j < tableSize; j++) {
			if (t[i][j] == 0)
				continue;
			unsigned int* winDirections = new unsigned int[4];
			directions(winDirections, i, j);
			for (int k = 0; k < 4; k++)
				if (winDirections[k] >= figToWin)
					return t[i][j];
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
			t[j][i] = 0;
			linePnt[j][i] = 0;
		}

	for (unsigned int k = 0; k < 2; k++)
		for (unsigned int i = 0; i < tableSize; i++)
			for (unsigned int j = 0; j < tableSize; j++)
				cellPr[k][i][j] = 0;

	gameOver = 0;
	move = 0;

	/*for (unsigned int i = 0; i < tableSize; i++) //reset cell priorities
		for (unsigned int j = 0; j < tableSize; j++) {
			if (i - j == 1 || j - i == 1)
				cellPr[i][j] = 1;
			else
				cellPr[i][j] = 2;
			if (i == 1 && j == 1)
				cellPr[i][j] = 3;
		}*/
}

/*---Computer set priorities steps---*/

//1st -- Add priority according to number of figures in a row
void PrAddFigInRow(point N,point A, point B, int k, int* pntDir) {
	
	cellPr[t[N.i][N.j] - 1][A.i][A.j] += 2 * pntDir[k] - 1;
	if (t[N.i][N.j] == FIGURE_O)
		cellPr[t[N.i][N.j] - 1][A.i][A.j]++;
	
	cellPr[t[N.i][N.j] - 1][B.i][B.j] += 2 * pntDir[k] - 1;
	if (t[N.i][N.j] == FIGURE_O)
		cellPr[t[N.i][N.j] - 1][B.i][B.j]++;
		
}

//2nd -- Add priority in empty interval between equal figures
bool AddPrInInterv(point N, int k){
	point A = corrPnt(incrVectInd(k, N));
	point B = corrPnt(incrVectInd(k, A));

	if (t[N.i][N.j] == t[B.i][B.j] && t[A.i][A.j] == 0) {
		cellPr[t[N.i][N.j] - 1][A.i][A.j] += 1;
		return 0;
	}
	return 1;
}

void computerSetPriorities() {

	for (unsigned int k = 0; k < 2; k++)
		for (unsigned int i = 0; i < tableSize; i++)
			for (unsigned int j = 0; j < tableSize; j++)
				cellPr[k][i][j] = 0;
	
	point N;

	int* pntDir = new int[4];
	for (N.i = 0; N.i < tableSize; N.i++) {
		for (N.j = 0; N.j < tableSize; N.j++) {
			if (t[N.i][N.j] == 0)
				continue;

			point A, B;
			A.i = N.i;
			A.j = N.j;
			B = A;
			directions(pntDir, N);

			bool allowIntervRewrite = 1;

			for (int k = 0; k < 4; k++) 
			{
				A.i = N.i;
				A.j = N.j;
				B = A;
				A = incrVectInd(k + 4, A);
				for (int t = 0; t < pntDir[k]; t++)
					B = incrVectInd(k, B);
				A = corrPnt(A);
				B = corrPnt(B);


				//set priorities
				if (t[A.i][A.j] != t[N.i][N.j] || (A.i == N.i && A.j == N.j))
					PrAddFigInRow(N, A, B, k, pntDir);

				if (allowIntervRewrite)
					allowIntervRewrite = AddPrInInterv(N, k);

				//hard bot
				/*if (difficulty == 3 /*&& t[B.i][B.j] == 0 && t[A.i][A.j] != t[N.i][N.j])
				{
					//border check
					if (tableSize != figToWin) {
						point pA = A;
						point pB = B;
						for (int q = pntDir[k]; q < figToWin - 1; q++) {
							pA = incrVectInd(k + 4, pA);
							pB = incrVectInd(k, pB);

							if (pA.i >= int(tableSize) ||
								pA.j >= int(tableSize) ||
								pA.i < 0 ||
								pA.j < 0) {
								cellPr[A.i][A.j] = 0;
							}
							else if (t[pA.i][pA.j] != 0 && t[pA.i][pA.j] != t[N.i][N.j])
								cellPr[A.i][A.j] = 0;

							if (pB.i >= int(tableSize) ||
								pB.j >= int(tableSize) ||
								pB.i < 0 ||
								pB.j < 0) {
								cellPr[B.i][B.j] = 0;
							}
							else if (t[pB.i][pB.j] != 0 && t[pB.i][pB.j] != t[N.i][N.j])
								cellPr[B.i][B.j] = 0;
						}
					}
					else if (move <= 1) {
						if ((A.i == 0 && A.j == 0) ||
							(A.i == 0 && A.j == tableSize - 1) ||
							(A.i == tableSize - 1 && A.j == 0) ||
							(A.i == tableSize - 1 && A.j == tableSize - 1))
							cellPr[A.i][A.j]++;
						if ((B.i == 0 && B.j == 0) ||
							(B.i == 0 && B.j == tableSize - 1) ||
							(B.i == tableSize - 1 && B.j == 0) ||
							(B.i == tableSize - 1 && B.j == tableSize - 1))
							cellPr[B.i][B.j]++;
					}
					
					point C = incrVectInd(k, B);
					C = corrPnt(C);
					if (t[C.i][C.j] == t[N.i][N.j]) {
						int *tempDir = new int[4];
						directions(tempDir, C);
						cellPr[B.i][B.j] += tempDir[k];
					}
				}*/

				//add random numbers if easy difficulty
				if (difficulty == 1) {
					cellPr[t[N.i][N.j] - 1][A.i][A.j] += round((rand() % 30 - 15.0) / 10.0);
					cellPr[t[N.i][N.j] - 1][B.i][B.j] += round((rand() % 30 - 15.0) / 10.0);
				}

				//set max priority if there is a chance to win
				if (pntDir[k] == figToWin - 1) {
					cellPr[t[N.i][N.j] - 1][A.i][A.j] += figToWin + t[N.i][N.j];
					cellPr[t[N.i][N.j] - 1][B.i][B.j] += figToWin + t[N.i][N.j];
				}

				//near border doesnt set max priority

				//log points
				//std::cout << "A[" << A.i << "][" << A.j << "] = " << cellPr[A.i][A.j] << '\n';
				//std::cout << "B[" << B.i << "][" << B.j << "] = " << cellPr[B.i][B.j] << '\n';
			}

		}
	}
}

void computerMove() { 
	if (move == 0 && tableSize != 3) {
		t[tableSize / 2][tableSize / 2] = FIGURE_O;
		return;
	}

	computerSetPriorities();

	short int moveX[2] = { 0, 0 }, moveY[2] = { 0, 0 }, max[2] = { -1, -1 };
	for (unsigned int k = 0; k < 2; k++)
		for (unsigned int i = 0; i < tableSize; i++)
			for (unsigned int j = 0; j < tableSize; j++)
				if (cellPr[k][i][j] > max[k] && t[i][j] == 0) {
					moveX[k] = i;
					moveY[k] = j;
					max[k] = cellPr[k][i][j];
				}
	if (max[0] < max[1])
		t[moveX[1]][moveY[1]] = FIGURE_O;
	else
		t[moveX[0]][moveY[0]] = FIGURE_O; //computer is always O

	//computerSetPriorities();

	logBotPriority();
}

void clickCheck(int ind_x, int ind_y) {
	if (t[ind_y][ind_x] < 1 ) {
		if (firstPlayer)
			t[ind_y][ind_x] = (move + 1) % 2 + 1;
		else
			t[ind_y][ind_x] = (move) % 2 + 1;
		move++;
	}
}

void drawFigs() {
	glLineWidth(5);
	for (unsigned int i = 0; i < tableSize; i++)
		for (unsigned int j = 0; j < tableSize; j++) {
			if (t[i][j] == FIGURE_X)
				drawX(j * frame_step + border_indent + frame_step / 2,
					i * frame_step + border_indent + frame_step / 2);
			if (t[i][j] == FIGURE_O)
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
		else if (winCheck() == FIGURE_O && compPlay)
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

bool restartWarning() {
	if (move > 0) {
		if (MessageBox(NULL, "Game already has started. Restart game?", "Game started", MB_ICONWARNING | MB_YESNO) == IDYES) {
			restart();
			return 1;
		}
		else
			return 0;
	}
	return 1;
}

void processMoveMenu(int option) {
	if (!restartWarning())
		return;
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
	if (!restartWarning())
		return;
	compPlay = 0;
}

void processDifficultySelector(int option) {
	if (!restartWarning())
		return;
	difficulty = option;
	compPlay = 1;
}

void processMainMenu(int option) {
	switch (option) {
	case 1:
		restart();
		break;
	case 2:
		if (!restartWarning())
			return;
		runSettings();
	}
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
		Sleep(5000);

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
	srand(time(0));

	runSettings();

	//init();

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
	int difficultySelector;
	int mainMenu;


	moveMenu = glutCreateMenu(processMoveMenu);
	glutAddMenuEntry("X", 1);
	glutAddMenuEntry("O", 2);

	difficultySelector = glutCreateMenu(processDifficultySelector);
	glutAddMenuEntry("Easy difficulty", 1);
	glutAddMenuEntry("Medium difficulty", 2);
	glutAddMenuEntry("Hard difficulty", 3);

	secondPlayerMenu = glutCreateMenu(processSecondPlayerMenu);
	glutAddMenuEntry("Play with human", 1);
	glutAddSubMenu("Play with Computer", difficultySelector);

	mainMenu = glutCreateMenu(processMainMenu);
	glutAddSubMenu("Who moves first", moveMenu);
	glutAddSubMenu("Second Player", secondPlayerMenu);
	glutAddMenuEntry("Restart", 1);
	glutAddMenuEntry("Run settings", 2);

	glutAttachMenu(GLUT_RIGHT_BUTTON);


	glutMainLoop();
}