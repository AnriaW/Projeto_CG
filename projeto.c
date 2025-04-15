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
#define GOAL_WIDTH 30
#define GOAL_HEIGHT 100

// Variáveis globais
float ball_x = WINDOW_WIDTH / 2;
float ball_y = WINDOW_HEIGHT / 2;
int score_left = 0;
int score_right = 0;
bool special_keys[4] = {false, false, false, false}; // UP, DOWN, LEFT, RIGHT

// Estrutura para representar um jogador
typedef struct {
    float x, y;
    int team; // 0 = time da esquerda, 1 = time da direita
    int radius; // Raio para detecção de colisão
} Player;

// Estrutura para representar um goleiro
typedef struct {
    float x, y;
    int team; // 0 = time da esquerda, 1 = time da direita
    int radius; // Raio para detecção de colisão
    float speed; // Velocidade de movimento
    float direction; // Direção do movimento (1 para baixo, -1 para cima)
} Goalkeeper;

// Array de jogadores (5 para cada time)
#define NUM_PLAYERS_PER_TEAM 5
Player players[NUM_PLAYERS_PER_TEAM * 2];

// Goleiros (um para cada time)
Goalkeeper goalkeepers[2];

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

// Função para inicializar os jogadores e goleiros
void initializePlayers() {
    int field_x = (WINDOW_WIDTH - FIELD_WIDTH) / 2;
    int field_y = (WINDOW_HEIGHT - FIELD_HEIGHT) / 2;
    
    // Time da esquerda
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
        players[i].team = 0;
        players[i].x = WINDOW_WIDTH / 4 + (rand() % 100 - 50);
        players[i].y = WINDOW_HEIGHT / 2 + (i - NUM_PLAYERS_PER_TEAM / 2) * 80;
        players[i].radius = 20; // Raio para colisão
    }
    
    // Time da direita
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
        players[i + NUM_PLAYERS_PER_TEAM].team = 1;
        players[i + NUM_PLAYERS_PER_TEAM].x = 3 * WINDOW_WIDTH / 4 + (rand() % 100 - 50);
        players[i + NUM_PLAYERS_PER_TEAM].y = WINDOW_HEIGHT / 2 + (i - NUM_PLAYERS_PER_TEAM / 2) * 80;
        players[i + NUM_PLAYERS_PER_TEAM].radius = 20; // Raio para colisão
    }
    
    // Inicializa os goleiros
    // Goleiro do time esquerdo
    goalkeepers[0].team = 0;
    goalkeepers[0].x = field_x - GOAL_WIDTH/2; // Posiciona na linha do gol
    goalkeepers[0].y = field_y + FIELD_HEIGHT/2; // Centro do gol
    goalkeepers[0].radius = 25; // Raio para colisão (maior que jogadores normais)
    goalkeepers[0].speed = 3.0f; // Velocidade de movimento
    goalkeepers[0].direction = 1; // Começa movendo para baixo
    
    // Goleiro do time direito
    goalkeepers[1].team = 1;
    goalkeepers[1].x = field_x + FIELD_WIDTH + GOAL_WIDTH/2; // Posiciona na linha do gol
    goalkeepers[1].y = field_y + FIELD_HEIGHT/2; // Centro do gol
    goalkeepers[1].radius = 25; // Raio para colisão
    goalkeepers[1].speed = 3.0f; // Velocidade de movimento
    goalkeepers[1].direction = -1; // Começa movendo para cima
}

// Função para desenhar um jogador (agora orientado corretamente)
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
    bresenhamCircle(0, -15, 10);
    
    // Corpo
    bresenhamLine(0, -5, 0, 20);
    
    // Braços
    bresenhamLine(0, 0, -15, 5);
    bresenhamLine(0, 0, 15, 5);
    
    // Pernas
    bresenhamLine(0, 20, -10, 40);
    bresenhamLine(0, 20, 10, 40);
    
    glPopMatrix();
}

// Função para desenhar um goleiro
void drawGoalkeeper(Goalkeeper goalkeeper) {
    glPushMatrix();
    glTranslatef(goalkeeper.x, goalkeeper.y, 0);
    
    // Desenha o corpo do goleiro
    if (goalkeeper.team == 0) {
        glColor3f(1.0, 0.5, 0.0); // Laranja para goleiro do time vermelho
    } else {
        glColor3f(0.0, 1.0, 1.0); // Ciano para goleiro do time azul
    }
    
    // Cabeça
    bresenhamCircle(0, -15, 12); // Cabeça um pouco maior
    
    // Corpo
    bresenhamLine(0, -5, 0, 25); // Corpo um pouco maior
    
    // Braços estendidos (postura de goleiro)
    if (goalkeeper.team == 0) {
        // Goleiro da esquerda, braços apontando para direita
        bresenhamLine(0, 0, 20, -5);
        bresenhamLine(0, 0, 20, 10);
    } else {
        // Goleiro da direita, braços apontando para esquerda
        bresenhamLine(0, 0, -20, -5);
        bresenhamLine(0, 0, -20, 10);
    }
    
    // Pernas
    bresenhamLine(0, 25, -10, 45);
    bresenhamLine(0, 25, 10, 45);
    
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

    // Grande área do gol esquerdo
    int large_area_width = 100;
    int large_area_height = 200;
    bresenhamLine(field_x, field_y + (FIELD_HEIGHT - large_area_height)/2, 
                 field_x + large_area_width, field_y + (FIELD_HEIGHT - large_area_height)/2);
    bresenhamLine(field_x + large_area_width, field_y + (FIELD_HEIGHT - large_area_height)/2, 
                 field_x + large_area_width, field_y + (FIELD_HEIGHT + large_area_height)/2);
    bresenhamLine(field_x + large_area_width, field_y + (FIELD_HEIGHT + large_area_height)/2, 
                 field_x, field_y + (FIELD_HEIGHT + large_area_height)/2);
    
    // Grande área do gol direito
    bresenhamLine(field_x + FIELD_WIDTH, field_y + (FIELD_HEIGHT - large_area_height)/2, 
                 field_x + FIELD_WIDTH - large_area_width, field_y + (FIELD_HEIGHT - large_area_height)/2);
    bresenhamLine(field_x + FIELD_WIDTH - large_area_width, field_y + (FIELD_HEIGHT - large_area_height)/2, 
                 field_x + FIELD_WIDTH - large_area_width, field_y + (FIELD_HEIGHT + large_area_height)/2);
    bresenhamLine(field_x + FIELD_WIDTH - large_area_width, field_y + (FIELD_HEIGHT + large_area_height)/2, 
                 field_x + FIELD_WIDTH, field_y + (FIELD_HEIGHT + large_area_height)/2);
                 
    // Pequena área do gol esquerdo
    int small_area_width = 50;
    int small_area_height = 120;
    bresenhamLine(field_x, field_y + (FIELD_HEIGHT - small_area_height)/2, 
                 field_x + small_area_width, field_y + (FIELD_HEIGHT - small_area_height)/2);
    bresenhamLine(field_x + small_area_width, field_y + (FIELD_HEIGHT - small_area_height)/2, 
                 field_x + small_area_width, field_y + (FIELD_HEIGHT + small_area_height)/2);
    bresenhamLine(field_x + small_area_width, field_y + (FIELD_HEIGHT + small_area_height)/2, 
                 field_x, field_y + (FIELD_HEIGHT + small_area_height)/2);
                 
    // Pequena área do gol direito
    bresenhamLine(field_x + FIELD_WIDTH, field_y + (FIELD_HEIGHT - small_area_height)/2, 
                 field_x + FIELD_WIDTH - small_area_width, field_y + (FIELD_HEIGHT - small_area_height)/2);
    bresenhamLine(field_x + FIELD_WIDTH - small_area_width, field_y + (FIELD_HEIGHT - small_area_height)/2, 
                 field_x + FIELD_WIDTH - small_area_width, field_y + (FIELD_HEIGHT + small_area_height)/2);
    bresenhamLine(field_x + FIELD_WIDTH - small_area_width, field_y + (FIELD_HEIGHT + small_area_height)/2, 
                 field_x + FIELD_WIDTH, field_y + (FIELD_HEIGHT + small_area_height)/2);

    // Gol esquerdo (fora do campo e centralizado)
    bresenhamLine(field_x, field_y + FIELD_HEIGHT/2 - GOAL_HEIGHT/2, 
                 field_x - GOAL_WIDTH, field_y + FIELD_HEIGHT/2 - GOAL_HEIGHT/2);
    bresenhamLine(field_x - GOAL_WIDTH, field_y + FIELD_HEIGHT/2 - GOAL_HEIGHT/2, 
                 field_x - GOAL_WIDTH, field_y + FIELD_HEIGHT/2 + GOAL_HEIGHT/2);
    bresenhamLine(field_x - GOAL_WIDTH, field_y + FIELD_HEIGHT/2 + GOAL_HEIGHT/2, 
                 field_x, field_y + FIELD_HEIGHT/2 + GOAL_HEIGHT/2);

    // Gol direito (fora do campo e centralizado)
    bresenhamLine(field_x + FIELD_WIDTH, field_y + FIELD_HEIGHT/2 - GOAL_HEIGHT/2, 
                 field_x + FIELD_WIDTH + GOAL_WIDTH, field_y + FIELD_HEIGHT/2 - GOAL_HEIGHT/2);
    bresenhamLine(field_x + FIELD_WIDTH + GOAL_WIDTH, field_y + FIELD_HEIGHT/2 - GOAL_HEIGHT/2, 
                 field_x + FIELD_WIDTH + GOAL_WIDTH, field_y + FIELD_HEIGHT/2 + GOAL_HEIGHT/2);
    bresenhamLine(field_x + FIELD_WIDTH + GOAL_WIDTH, field_y + FIELD_HEIGHT/2 + GOAL_HEIGHT/2, 
                 field_x + FIELD_WIDTH, field_y + FIELD_HEIGHT/2 + GOAL_HEIGHT/2);
    
    // Cantos do campo
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

// Função para verificar colisão entre a bola e um jogador ou goleiro
bool checkPlayerCollision(float x, float y, float radius, float new_ball_x, float new_ball_y) {
    // Calcula a distância entre o centro da bola e o centro do jogador/goleiro
    float dx = new_ball_x - x;
    float dy = new_ball_y - y;
    float distance = sqrt(dx * dx + dy * dy);
    
    // Verifica se a distância é menor que a soma dos raios (colisão)
    return distance < (BALL_RADIUS + radius);
}

// Função para calcular a nova direção da bola após colisão com um jogador ou goleiro
void resolveCollision(float x, float y, float *dx, float *dy) {
    // Vetor da bola para o jogador/goleiro
    float vx = x - ball_x;
    float vy = y - ball_y;
    
    // Normaliza o vetor
    float length = sqrt(vx * vx + vy * vy);
    vx /= length;
    vy /= length;
    
    // Inverte a direção da bola e aplica um pequeno impulso
    *dx = -vx * BALL_SPEED * 1.5;
    *dy = -vy * BALL_SPEED * 1.5;
}

// Função para verificar gols
void checkGoal() {
    int field_x = (WINDOW_WIDTH - FIELD_WIDTH) / 2;
    int field_y = (WINDOW_HEIGHT - FIELD_HEIGHT) / 2;
    
    // Verifica gol no lado direito
    if (ball_x > field_x + FIELD_WIDTH && 
        ball_y > field_y + FIELD_HEIGHT / 2 - GOAL_HEIGHT/2 && 
        ball_y < field_y + FIELD_HEIGHT / 2 + GOAL_HEIGHT/2 &&
        ball_x < field_x + FIELD_WIDTH + GOAL_WIDTH) {
        score_left++;
        // Reinicia a bola no centro
        ball_x = WINDOW_WIDTH / 2;
        ball_y = WINDOW_HEIGHT / 2;
    }
    
    // Verifica gol no lado esquerdo
    if (ball_x < field_x && 
        ball_y > field_y + FIELD_HEIGHT / 2 - GOAL_HEIGHT/2 && 
        ball_y < field_y + FIELD_HEIGHT / 2 + GOAL_HEIGHT/2 &&
        ball_x > field_x - GOAL_WIDTH) {
        score_right++;
        // Reinicia a bola no centro
        ball_x = WINDOW_WIDTH / 2;
        ball_y = WINDOW_HEIGHT / 2;
    }
}

// Função para atualizar os goleiros
void updateGoalkeepers() {
    int field_x = (WINDOW_WIDTH - FIELD_WIDTH) / 2;
    int field_y = (WINDOW_HEIGHT - FIELD_HEIGHT) / 2;
    
    // Limites de movimento para os goleiros
    float goal_top = field_y + FIELD_HEIGHT/2 - GOAL_HEIGHT/2 + 15; // Margem para não ficar colado na trave
    float goal_bottom = field_y + FIELD_HEIGHT/2 + GOAL_HEIGHT/2 - 15;
    
    for (int i = 0; i < 2; i++) {
        // Comportamento base: movimento para cima e para baixo
        goalkeepers[i].y += goalkeepers[i].direction * goalkeepers[i].speed;
        
        // Verifica se o goleiro chegou nos limites do gol
        if (goalkeepers[i].y <= goal_top) {
            goalkeepers[i].y = goal_top;
            goalkeepers[i].direction = 1; // Muda direção para baixo
        } else if (goalkeepers[i].y >= goal_bottom) {
            goalkeepers[i].y = goal_bottom;
            goalkeepers[i].direction = -1; // Muda direção para cima
        }
        
        // Comportamento de defesa: segue a bola quando estiver próxima
        float ball_distance;
        if (i == 0) { // Goleiro da esquerda
            ball_distance = fabs(ball_x - field_x);
            if (ball_distance < 150) {
                // Ajusta a velocidade do goleiro com base na proximidade da bola
                float target_y = ball_y;
                
                // Limita a posição para dentro do gol
                if (target_y < goal_top) target_y = goal_top;
                if (target_y > goal_bottom) target_y = goal_bottom;
                
                // Move o goleiro em direção à altura da bola
                if (goalkeepers[i].y < target_y - 5) {
                    goalkeepers[i].y += goalkeepers[i].speed * 1.5;
                } else if (goalkeepers[i].y > target_y + 5) {
                    goalkeepers[i].y -= goalkeepers[i].speed * 1.5;
                }
            }
        } else { // Goleiro da direita
            ball_distance = fabs(ball_x - (field_x + FIELD_WIDTH));
            if (ball_distance < 150) {
                // Ajusta a velocidade do goleiro com base na proximidade da bola
                float target_y = ball_y;
                
                // Limita a posição para dentro do gol
                if (target_y < goal_top) target_y = goal_top;
                if (target_y > goal_bottom) target_y = goal_bottom;
                
                // Move o goleiro em direção à altura da bola
                if (goalkeepers[i].y < target_y - 5) {
                    goalkeepers[i].y += goalkeepers[i].speed * 1.5;
                } else if (goalkeepers[i].y > target_y + 5) {
                    goalkeepers[i].y -= goalkeepers[i].speed * 1.5;
                }
            }
        }
    }
}

// Função para atualizar o jogo
void update() {
    int field_x = (WINDOW_WIDTH - FIELD_WIDTH) / 2;
    int field_y = (WINDOW_HEIGHT - FIELD_HEIGHT) / 2;
    
    // Atualiza os goleiros
    updateGoalkeepers();
    
    // Movimento da bola baseado nas teclas pressionadas
    float dx = 0, dy = 0;
    
    if (special_keys[0]) { // UP
        dy = -BALL_SPEED;
    }
    if (special_keys[1]) { // DOWN
        dy = BALL_SPEED;
    }
    if (special_keys[2]) { // LEFT
        dx = -BALL_SPEED;
    }
    if (special_keys[3]) { // RIGHT
        dx = BALL_SPEED;
    }
    
    // Novas coordenadas da bola
    float new_ball_x = ball_x + dx;
    float new_ball_y = ball_y + dy;
    
    // Verifica colisão com jogadores
    bool collision = false;
    
    // Verifica colisão com jogadores de campo
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM * 2; i++) {
        if (checkPlayerCollision(players[i].x, players[i].y, players[i].radius, new_ball_x, new_ball_y)) {
            collision = true;
            resolveCollision(players[i].x, players[i].y, &dx, &dy);
            new_ball_x = ball_x + dx;
            new_ball_y = ball_y + dy;
            break;
        }
    }
    
    // Verifica colisão com goleiros
    if (!collision) {
        for (int i = 0; i < 2; i++) {
            if (checkPlayerCollision(goalkeepers[i].x, goalkeepers[i].y, goalkeepers[i].radius, new_ball_x, new_ball_y)) {
                collision = true;
                resolveCollision(goalkeepers[i].x, goalkeepers[i].y, &dx, &dy);
                // Goleiros dão um impulso maior na bola
                dx *= 3;
                dy *= 3;
                new_ball_x = ball_x + dx;
                new_ball_y = ball_y + dy;
                break;
            }
        }
    }
    
    // Atualiza a posição da bola se não houver colisão
    if (!collision) {
        ball_x = new_ball_x;
        ball_y = new_ball_y;
    } else {
        // Se houver colisão, aplica o impulso calculado
        ball_x += dx;
        ball_y += dy;
    }
    
    // Verificação de colisão com as bordas do campo, exceto na área dos gols
    if ((ball_x < field_x && (ball_y < field_y + FIELD_HEIGHT / 2 - GOAL_HEIGHT/2 || 
                              ball_y > field_y + FIELD_HEIGHT / 2 + GOAL_HEIGHT/2)) || 
        ball_x < field_x - GOAL_WIDTH - BALL_RADIUS) {
        ball_x = field_x + BALL_RADIUS;
    }
    
    if ((ball_x > field_x + FIELD_WIDTH && (ball_y < field_y + FIELD_HEIGHT / 2 - GOAL_HEIGHT/2 || 
                                           ball_y > field_y + FIELD_HEIGHT / 2 + GOAL_HEIGHT/2)) || 
        ball_x > field_x + FIELD_WIDTH + GOAL_WIDTH + BALL_RADIUS) {
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
    
    // Desenha os goleiros
    for (int i = 0; i < 2; i++) {
        drawGoalkeeper(goalkeepers[i]);
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

void specialKeyDown(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            special_keys[0] = true;
            break;
        case GLUT_KEY_DOWN:
            special_keys[1] = true;
            break;
        case GLUT_KEY_LEFT:
            special_keys[2] = true;
            break;
        case GLUT_KEY_RIGHT:
            special_keys[3] = true;
            break;
    }
}

void specialKeyUp(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            special_keys[0] = false;
            break;
        case GLUT_KEY_DOWN:
            special_keys[1] = false;
            break;
        case GLUT_KEY_LEFT:
            special_keys[2] = false;
            break;
        case GLUT_KEY_RIGHT:
            special_keys[3] = false;
            break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Campo de Futebol - OpenGL com Bresenham");
    
    // Configuração inicial
    glClearColor(0.0, 0.3, 0.0, 1.0);
    
    // Inicializa os estados das teclas especiais
    memset(special_keys, 0, sizeof(special_keys));
    
    // Inicializa os jogadores
    initializePlayers();

    // Registra callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutSpecialFunc(specialKeyDown);
    glutSpecialUpFunc(specialKeyUp);
    
    // Inicia o loop principal
    glutMainLoop();
    
    return 0;
}