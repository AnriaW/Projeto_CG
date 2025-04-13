#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Tamanho da janela
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Dimensões do campo
#define FIELD_WIDTH 700
#define FIELD_HEIGHT 500

// Dimensões da bola
#define BALL_RADIUS 10
#define BALL_SPEED 5

// Dimensões do gol
#define GOAL_WIDTH 100
#define GOAL_HEIGHT 50

// Variáveis globais
float ball_x = WINDOW_WIDTH / 2;
float ball_y = WINDOW_HEIGHT / 2;
int score_left = 0;
int score_right = 0;
bool key_states[256];

// Estrutura para representar um jogador
typedef struct {
    float x, y;
    int team; // 0 = time da esquerda, 1 = time da direita
} Player;

// Array de jogadores (5 para cada time)
#define NUM_PLAYERS_PER_TEAM 5
Player players[NUM_PLAYERS_PER_TEAM * 2];

// Implementação do algoritmo de Bresenham para linhas
void bresenhamLine(int x1, int y1, int x2, int y2) {
    glBegin(GL_POINTS);
    
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (x1 != x2 || y1 != y2) {
        glVertex2i(x1, y1);
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
    glVertex2i(x2, y2); // Desenha o último ponto
    
    glEnd();
}

// Implementação do algoritmo de Bresenham para círculos
void bresenhamCircle(int xc, int yc, int r) {
    glBegin(GL_POINTS);
    
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    
    while (y >= x) {
        // Desenhar os oito pontos simétricos
        glVertex2i(xc + x, yc + y);
        glVertex2i(xc - x, yc + y);
        glVertex2i(xc + x, yc - y);
        glVertex2i(xc - x, yc - y);
        glVertex2i(xc + y, yc + x);
        glVertex2i(xc - y, yc + x);
        glVertex2i(xc + y, yc - x);
        glVertex2i(xc - y, yc - x);
        
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
    
    glEnd();
}

// Função para inicializar os jogadores
void initializePlayers() {
    // Time da esquerda
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
        players[i].team = 0;
        players[i].x = WINDOW_WIDTH / 4 + (rand() % 100 - 50);
        players[i].y = WINDOW_HEIGHT / 2 + (i - NUM_PLAYERS_PER_TEAM / 2) * 80;
    }
    
    // Time da direita
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
        players[i + NUM_PLAYERS_PER_TEAM].team = 1;
        players[i + NUM_PLAYERS_PER_TEAM].x = 3 * WINDOW_WIDTH / 4 + (rand() % 100 - 50);
        players[i + NUM_PLAYERS_PER_TEAM].y = WINDOW_HEIGHT / 2 + (i - NUM_PLAYERS_PER_TEAM / 2) * 80;
    }
}

// Função para desenhar um jogador
void drawPlayer(Player player) {
    glPushMatrix();
    glTranslatef(player.x, player.y, 0);
    
    // Desenha o corpo do jogador
    if (player.team == 0) {
        glColor3f(1.0, 0.0, 0.0); // Time vermelho
    } else {
        glColor3f(0.0, 0.0, 1.0); // Time azul
    }
    
    // Cabeça
    bresenhamCircle(0, 15, 10);
    
    // Corpo
    bresenhamLine(0, 5, 0, -20);
    
    // Braços
    bresenhamLine(0, 0, -15, -5);
    bresenhamLine(0, 0, 15, -5);
    
    // Pernas
    bresenhamLine(0, -20, -10, -40);
    bresenhamLine(0, -20, 10, -40);
    
    glPopMatrix();
}

// Função para desenhar o campo
void drawField() {
    int field_x = (WINDOW_WIDTH - FIELD_WIDTH) / 2;
    int field_y = (WINDOW_HEIGHT - FIELD_HEIGHT) / 2;
    
    // Desenha a grama (fundo verde)
    glColor3f(0.0, 0.6, 0.0);
    glBegin(GL_QUADS);
    glVertex2i(field_x, field_y);
    glVertex2i(field_x + FIELD_WIDTH, field_y);
    glVertex2i(field_x + FIELD_WIDTH, field_y + FIELD_HEIGHT);
    glVertex2i(field_x, field_y + FIELD_HEIGHT);
    glEnd();
    
    // Desenha as linhas do campo (brancas)
    glColor3f(1.0, 1.0, 1.0);
    glLineWidth(2.0);
    
    // Linha de contorno
    bresenhamLine(field_x, field_y, field_x + FIELD_WIDTH, field_y);
    bresenhamLine(field_x + FIELD_WIDTH, field_y, field_x + FIELD_WIDTH, field_y + FIELD_HEIGHT);
    bresenhamLine(field_x + FIELD_WIDTH, field_y + FIELD_HEIGHT, field_x, field_y + FIELD_HEIGHT);
    bresenhamLine(field_x, field_y + FIELD_HEIGHT, field_x, field_y);
    
    // Linha do meio-campo
    bresenhamLine(WINDOW_WIDTH / 2, field_y, WINDOW_WIDTH / 2, field_y + FIELD_HEIGHT);
    
    // Círculo central
    bresenhamCircle(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 50);
    
    // Áreas dos gols
    
    // Área do gol esquerdo
    bresenhamLine(field_x, field_y + FIELD_HEIGHT / 2 - GOAL_HEIGHT, 
                 field_x + GOAL_WIDTH, field_y + FIELD_HEIGHT / 2 - GOAL_HEIGHT);
    bresenhamLine(field_x + GOAL_WIDTH, field_y + FIELD_HEIGHT / 2 - GOAL_HEIGHT, 
                 field_x + GOAL_WIDTH, field_y + FIELD_HEIGHT / 2 + GOAL_HEIGHT);
    bresenhamLine(field_x + GOAL_WIDTH, field_y + FIELD_HEIGHT / 2 + GOAL_HEIGHT, 
                 field_x, field_y + FIELD_HEIGHT / 2 + GOAL_HEIGHT);
    
    // Área do gol direito
    bresenhamLine(field_x + FIELD_WIDTH, field_y + FIELD_HEIGHT / 2 - GOAL_HEIGHT, 
                 field_x + FIELD_WIDTH - GOAL_WIDTH, field_y + FIELD_HEIGHT / 2 - GOAL_HEIGHT);
    bresenhamLine(field_x + FIELD_WIDTH - GOAL_WIDTH, field_y + FIELD_HEIGHT / 2 - GOAL_HEIGHT, 
                 field_x + FIELD_WIDTH - GOAL_WIDTH, field_y + FIELD_HEIGHT / 2 + GOAL_HEIGHT);
    bresenhamLine(field_x + FIELD_WIDTH - GOAL_WIDTH, field_y + FIELD_HEIGHT / 2 + GOAL_HEIGHT, 
                 field_x + FIELD_WIDTH, field_y + FIELD_HEIGHT / 2 + GOAL_HEIGHT);
                 
    // Pequena área do gol esquerdo
    int small_area_width = GOAL_WIDTH / 2;
    int small_area_height = GOAL_HEIGHT / 2;
    bresenhamLine(field_x, field_y + FIELD_HEIGHT / 2 - small_area_height, 
                 field_x + small_area_width, field_y + FIELD_HEIGHT / 2 - small_area_height);
    bresenhamLine(field_x + small_area_width, field_y + FIELD_HEIGHT / 2 - small_area_height, 
                 field_x + small_area_width, field_y + FIELD_HEIGHT / 2 + small_area_height);
    bresenhamLine(field_x + small_area_width, field_y + FIELD_HEIGHT / 2 + small_area_height, 
                 field_x, field_y + FIELD_HEIGHT / 2 + small_area_height);
                 
    // Pequena área do gol direito
    bresenhamLine(field_x + FIELD_WIDTH, field_y + FIELD_HEIGHT / 2 - small_area_height, 
                 field_x + FIELD_WIDTH - small_area_width, field_y + FIELD_HEIGHT / 2 - small_area_height);
    bresenhamLine(field_x + FIELD_WIDTH - small_area_width, field_y + FIELD_HEIGHT / 2 - small_area_height, 
                 field_x + FIELD_WIDTH - small_area_width, field_y + FIELD_HEIGHT / 2 + small_area_height);
    bresenhamLine(field_x + FIELD_WIDTH - small_area_width, field_y + FIELD_HEIGHT / 2 + small_area_height, 
                 field_x + FIELD_WIDTH, field_y + FIELD_HEIGHT / 2 + small_area_height);
    
    // Canto superior esquerdo
    bresenhamCircle(field_x, field_y, 10);
    
    // Canto superior direito
    bresenhamCircle(field_x + FIELD_WIDTH, field_y, 10);
    
    // Canto inferior esquerdo
    bresenhamCircle(field_x, field_y + FIELD_HEIGHT, 10);
    
    // Canto inferior direito
    bresenhamCircle(field_x + FIELD_WIDTH, field_y + FIELD_HEIGHT, 10);
}

// Função para desenhar a bola
void drawBall() {
    glColor3f(1.0, 1.0, 1.0);
    bresenhamCircle(ball_x, ball_y, BALL_RADIUS);
    
    // Detalhes da bola (pentágonos pretos)
    glColor3f(0.0, 0.0, 0.0);
    for (int i = 0; i < 5; i++) {
        float angle = i * 2.0f * M_PI / 5.0f;
        float x = ball_x + (BALL_RADIUS - 3) * cos(angle);
        float y = ball_y + (BALL_RADIUS - 3) * sin(angle);
        bresenhamCircle(x, y, 3);
    }
}

// Função para desenhar o placar
void drawScore() {
    char score_text[50];
    sprintf(score_text, "%d x %d", score_left, score_right);
    
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2i(WINDOW_WIDTH / 2 - 20, 20);
    
    for (int i = 0; i < strlen(score_text); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, score_text[i]);
    }
}

// Função para verificar gols
void checkGoal() {
    int field_x = (WINDOW_WIDTH - FIELD_WIDTH) / 2;
    int field_y = (WINDOW_HEIGHT - FIELD_HEIGHT) / 2;
    
    // Verifica gol no lado direito
    if (ball_x > field_x + FIELD_WIDTH && 
        ball_y > field_y + FIELD_HEIGHT / 2 - GOAL_HEIGHT && 
        ball_y < field_y + FIELD_HEIGHT / 2 + GOAL_HEIGHT) {
        score_left++;
        // Reinicia a bola no centro
        ball_x = WINDOW_WIDTH / 2;
        ball_y = WINDOW_HEIGHT / 2;
    }
    
    // Verifica gol no lado esquerdo
    if (ball_x < field_x && 
        ball_y > field_y + FIELD_HEIGHT / 2 - GOAL_HEIGHT && 
        ball_y < field_y + FIELD_HEIGHT / 2 + GOAL_HEIGHT) {
        score_right++;
        // Reinicia a bola no centro
        ball_x = WINDOW_WIDTH / 2;
        ball_y = WINDOW_HEIGHT / 2;
    }
}

// Função para atualizar o jogo
void update() {
    int field_x = (WINDOW_WIDTH - FIELD_WIDTH) / 2;
    int field_y = (WINDOW_HEIGHT - FIELD_HEIGHT) / 2;
    
    // Movimento da bola baseado nas teclas pressionadas
    if (key_states['w'] || key_states['W']) {
        ball_y -= BALL_SPEED;
    }
    if (key_states['s'] || key_states['S']) {
        ball_y += BALL_SPEED;
    }
    if (key_states['a'] || key_states['A']) {
        ball_x -= BALL_SPEED;
    }
    if (key_states['d'] || key_states['D']) {
        ball_x += BALL_SPEED;
    }
    
    // Verificação de colisão com as bordas do campo, exceto na área dos gols
    if ((ball_x < field_x && (ball_y < field_y + FIELD_HEIGHT / 2 - GOAL_HEIGHT || 
                              ball_y > field_y + FIELD_HEIGHT / 2 + GOAL_HEIGHT)) || 
        ball_x < field_x - BALL_RADIUS) {
        ball_x = field_x + BALL_RADIUS;
    }
    
    if ((ball_x > field_x + FIELD_WIDTH && (ball_y < field_y + FIELD_HEIGHT / 2 - GOAL_HEIGHT || 
                                           ball_y > field_y + FIELD_HEIGHT / 2 + GOAL_HEIGHT)) || 
        ball_x > field_x + FIELD_WIDTH + BALL_RADIUS) {
        ball_x = field_x + FIELD_WIDTH - BALL_RADIUS;
    }
    
    if (ball_y < field_y) {
        ball_y = field_y + BALL_RADIUS;
    }
    
    if (ball_y > field_y + FIELD_HEIGHT) {
        ball_y = field_y + FIELD_HEIGHT - BALL_RADIUS;
    }
    
    // Verifica se houve gol
    checkGoal();
}

// Funções de callback para o GLUT
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Desenha o campo
    drawField();
    
    // Desenha o placar
    drawScore();
    
    // Desenha os jogadores
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM * 2; i++) {
        drawPlayer(players[i]);
    }
    
    // Desenha a bola
    drawBall();
    
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int value) {
    update();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // Aproximadamente 60 FPS
}

void keyboard(unsigned char key, int x, int y) {
    key_states[key] = true;
}

void keyboardUp(unsigned char key, int x, int y) {
    key_states[key] = false;
}

// Função principal
int main(int argc, char** argv) {
    // Inicialização do GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Campo de Futebol - OpenGL com Bresenham");
    
    // Configuração inicial
    glClearColor(0.0, 0.3, 0.0, 1.0);
    
    // Inicializa os estados do teclado
    memset(key_states, 0, sizeof(key_states));
    
    // Inicializa os jogadores
    initializePlayers();
    
    // Registra callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    
    // Inicia o loop principal
    glutMainLoop();
    
    return 0;
}