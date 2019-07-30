#define FIGURE_X 2
#define FIGURE_O 1

#include <stdlib.h>
#include "glut.h"
#include <cmath>

//cursor position
int curs_x;
int curs_y;

int win_x_size = 700;
int win_y_size = 700;

int border_indent = 50;

int frame_step = (win_x_size - border_indent * 2) / 3;

bool move = 1;

short int table[3][3];

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

void clickCheck(int ind_x, int ind_y) {
	if (table[ind_x][ind_y] < 1) {
		table[ind_x][ind_y] = int(move) + 1;
		move = !move;
	}
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

void winCheck() {
	short int win = 0;

	for (int i = 0; i < 3; i++) {
		if (table[0][i] == table[1][i] && table[1][i] == table[2][i])
			win = table[0][i];
	}
	for (int i = 0; i < 3; i++) {
		if (table[i][0] == table[i][1] && table[i][1] == table[i][2])
			win = table[i][0];
	}

	if (table[0][0] == table[1][1] && table[1][1] == table[2][2])
		win = table[0][0];
	if (table[0][2] == table[1][1] && table[1][1] == table[2][0])
		win = table[0][2];
}

void RenderScene() {

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

	//glBegin(GL_POINTS);
	//glVertex2f(curs_x - 5, curs_y - 5);
	//glEnd();

	drawFigs();

	glutSwapBuffers();
}

void MousePosition(int x, int y) {

}

void MouseClick(int button, int state, int width, int height) {
	curs_x = width;
	curs_y = height;
	int ind_x = (curs_x - border_indent) / frame_step;
	int ind_y = (curs_y - border_indent) / frame_step;
	clickCheck(ind_x, ind_y);
}

int main(int argc, char** argv) {
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