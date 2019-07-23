#define FIGURE_X 1
#define FIGURE_O 2

#include "glut.h"

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

void clickCheck() {

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

	glBegin(GL_POINTS);
	glVertex2f(curs_x - 5, curs_y - 5);
	glEnd();

	glutSwapBuffers();
}

void MousePosition(int x, int y) {
	curs_x = x;
	curs_y = y;
}

void MouseClick(int button, int state, int width, int height) {

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