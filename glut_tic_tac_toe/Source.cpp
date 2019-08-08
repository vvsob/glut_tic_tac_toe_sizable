#define FIGURE_X 2
#define FIGURE_O 1

#include <cstdlib>
#include "glut.h"
#include <cmath>

#include <Windows.h>
#include <string>

//cursor position
int curs_x;
int curs_y;

int win_x_size = 700;
int win_y_size = 700;

int border_indent = 50;

int frame_step = (win_x_size - border_indent * 2) / 3;

bool move = 1;
bool gameOver = 0;

short int table[3][3];

bool linePoints[3][3];

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
	glBegin(GL_LINES); //frame
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
	glLineWidth(5);
	glBegin(GL_LINES);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			if (linePoints[j][i]) {
				glVertex2f(j * frame_step + border_indent + frame_step / 2,
					i * frame_step + border_indent + frame_step / 2);
				if (temp == 1)
				{
					//glEnd();
					//Sleep(3000);
					gameOver = 1;
				}
				temp = 1;
			}
		}
	glEnd();
	glColor3f(1, 1, 1);
	glLineWidth(1);

}


short int winCheck() {

	for (int i = 0; i < 3; i++) {
		if (table[0][i] == table[1][i] && table[1][i] == table[2][i] && table[0][i] > 0) {
			linePoints[0][i] = 1;
			linePoints[2][i] = 1;
			return table[0][i];
		}
	}
	for (int i = 0; i < 3; i++) {
		if (table[i][0] == table[i][1] && table[i][1] == table[i][2] && table[i][0] > 0) {
			linePoints[i][0] = 1;
			linePoints[i][2] = 1;
			return table[i][0];
		}
	}

	if (table[0][0] == table[1][1] && table[1][1] == table[2][2] && table[0][0] > 0) {
		linePoints[0][0] = 1;
		linePoints[2][2] = 1;
		return table[0][0];
	}
	if (table[0][2] == table[1][1] && table[1][1] == table[2][0] && table[0][2] > 0) {
		linePoints[0][2] = 1;
		linePoints[2][0] = 1;
		return table[0][2];
	}

	return 0;
}

void clickCheck(int ind_x, int ind_y) {
	if (table[ind_x][ind_y] < 1 ) {
		table[ind_x][ind_y] = int(move) + 1;
		move = !move;
	}
	winCheck();
}

void yesNoClickCheck(int x, int y) {

}

void drawFigs() {
	glLineWidth(5);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			if (table[j][i] == FIGURE_X)
				drawX(j * frame_step + border_indent + frame_step / 2,
					i * frame_step + border_indent + frame_step / 2);
			if (table[j][i] == FIGURE_O)
				drawO(j * frame_step + border_indent + frame_step / 2,
					i * frame_step + border_indent + frame_step / 2);
		}
	glLineWidth(1);
}



void RenderScene() {
	if (!gameOver) {
		glBegin(GL_LINE_LOOP); //frame
		glVertex2f(border_indent, border_indent);
		glVertex2f(win_x_size - border_indent, border_indent);
		glVertex2f(win_x_size - border_indent, win_y_size - border_indent);
		glVertex2f(border_indent, win_y_size - border_indent);
		glEnd();

		glBegin(GL_LINES);
		glVertex2f(border_indent + frame_step, border_indent);
		glVertex2f(border_indent + frame_step, win_y_size - border_indent);
		glVertex2f(border_indent + frame_step * 2, border_indent);
		glVertex2f(border_indent + frame_step * 2, win_y_size - border_indent);

		glVertex2f(border_indent, border_indent + frame_step);
		glVertex2f(win_x_size - border_indent, border_indent + frame_step);
		glVertex2f(border_indent, border_indent + frame_step * 2);
		glVertex2f(win_x_size - border_indent, border_indent + frame_step * 2);
		glEnd();

		drawFigs();

		drawLine();
		if (gameOver) { 
			glutSwapBuffers();
			Sleep(1500); 
		}
	} else {
		glClear(GL_COLOR_BUFFER_BIT);
		//glBegin(GL_POINTS);
		//glVertex2f(curs_x - 5, curs_y - 5);
		//glEnd();
		int x1 = win_x_size / 6 - glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, '0') * 1.5 + win_x_size / 3 * 1;
		//char* scr = new char[4];
		//memset(scr, '\0', 3);
		char msg[] = "WIN\0";
		//char *msg = new char[50];
		//strcpy_s(msg)
		for (char* c = msg; *c != '\0'; c++) {
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
	curs_x = width;
	curs_y = height;
	int ind_x = (curs_x - border_indent) / frame_step;
	int ind_y = (curs_y - border_indent) / frame_step;
	if (!gameOver)
		clickCheck(ind_x, ind_y);
	else
		yesNoClickCheck(curs_x, curs_y);
}

int main(int argc, char** argv) {
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			linePoints[i][j] = 0;
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(win_x_size, win_y_size);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Tic-Tac-Toe");

	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
	glutReshapeFunc(changeSize);

	glutMotionFunc(MousePosition);
	glutMouseFunc(MouseClick);

	glutMainLoop();
}