#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <iostream>

void renderFunction();

void RenderFunction() {
    std::cerr << "[INFO] Running loop." << std::endl;
    glClear(GL_COLOR_BUFFER_BIT);

    glRotated(10, 0, 0, 1);
    glColor3f(0.0, 0.0, 1.0);

    glBegin(GL_POLYGON);
    glVertex3f(-0.5, -0.5, 0);
    glVertex3f(0.5, -0.5, 0);
    glVertex3f(0.5, 0.5, 0);
    glVertex3f(-0.5, 0.5, 0);
    glEnd();

    glFlush();
}

int main(int argc, char** argv) {
    std::cerr << "[INFO] Starting OpenGL main loop." << std::endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Window 1");
    glutDisplayFunc(&RenderFunction);
    glutMainLoop();
    std::cerr << "[INFO] Exit OpenGL main loop." << std::endl;
    return 0;
}
