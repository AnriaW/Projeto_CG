#include <GL/glut.h>

static int year=0, day=0;

// Inicializa parâmetros de rendering
void init(void){
	glClearColor (0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW); 
    gluPerspective(60.0, 1.0, 1.0, 80.0);
    
}

// Função callback chamada para fazer o desenho
void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    
	glPushMatrix(); //Nosso Sol
		glColor3f (1.0, 1.0, 0.0);
        glRotatef ((GLfloat) year, 1.0, 0.0, 0.0);
        glRotatef ((GLfloat) day, 0.0, 0.0, 1.0);
        glutWireSphere(1.0, 20, 16);  
	glPopMatrix();
    
	glPushMatrix(); //planeta ciano/azul
		glColor3f (0.0, 0.7, 0.7);
        glRotatef ((GLfloat) year, 0.0, 1.0, 0.0);
        glTranslatef (2.0, 0.0, 0.0); 
        glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);
        glutWireSphere(0.2, 8, 8);

        glPushMatrix(); // Lua maior branca
            glColor3f(1.0, 1.0, 1.0); 
            glRotatef((GLfloat)year, 0.0, 1.0, 0.0);
            glTranslatef(0.5, 0.0, 0.0); 
            glutWireSphere(0.1, 10, 8); 
        glPopMatrix();

    
        glPushMatrix(); //Lua menor cinza
            glColor3f(0.7, 0.7, 0.7); 
            glRotatef(45, 0.0, 0.0, 1.0); 
            glRotatef((GLfloat) year, 0.0, 2.0, 0.0);
            glTranslatef(0.9, 0.0, 0.0);
            glutWireSphere(0.06, 8, 8); 
        glPopMatrix();
    glPopMatrix();  

    
	glPushMatrix(); //O planeta rosa
        glColor3f (1.0, 0.0, 1.0);
        glRotatef ((GLfloat) year, year, 1.0, 0.0);
        glTranslatef (0.0, 2.0, 0.0); 
        glRotatef ((GLfloat) day, 1.0, 0.0, 0.0);
        glutWireSphere(0.2, 10, 8);  
    glPopMatrix();
    
    glPushMatrix(); // O planeta verde
        glColor3f (0.0, 1.0, 0.0);
        glRotatef (-(GLfloat)year, 0.0, 1.0, 0.0);
        glTranslatef (3.0, 0.0, 0.0); 
        glRotatef ((GLfloat) day, 0.0, 1.0, 0.0);
        glutWireSphere(0.15, 8, 8);
    glPopMatrix();

    glutSwapBuffers();
}

void reshape (int w, int h){
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
	gluLookAt (0.0, 0.0, 6.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); //Reajuste pq o verde tava fora da janela
    
}

void keyboard (unsigned char key, int x, int y){
	switch (key) {
        case 'd':
            day = (day + 10) % 360;
            glutPostRedisplay();
            break;
        case 'D':
            day = (day - 10) % 360;
            glutPostRedisplay();
            break;
        case 'y':
            year = (year + 5) % 360;
            glutPostRedisplay();
            break;
        case 'Y':
            year = (year - 5) % 360;
            glutPostRedisplay();
            break;
        default:
            break;
	}
}

int main(int argc, char** argv){
	
    glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition (100, 100);
	glutCreateWindow("Planetario");
	
    init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
}
