#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include "tinyxml2.h"
#include "player.h"
#include "material.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define KEYFRAMES_WALK 12
#define KEYFRAMES_PUNCH 26
#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500
#define INICIAL_CAM_XY_ANGLE 40
using namespace tinyxml2;

//Malhas a serem desenhadas
mesh mesh_walk[KEYFRAMES_WALK];
mesh mesh_punch[KEYFRAMES_PUNCH];

GLfloat altura_padrao = 10;
GLfloat altura_cilindros = 10;

//Key status
int keyStatus[256];

GLdouble incKeyIdle = 1;

bool night_mode = true;
int pontual_light = 0;

//Controles de câmera
int lookatToggle = 3;
int zoom = 50;
double camXYAngle=INICIAL_CAM_XY_ANGLE;
double camXZAngle=45;
int lastX = 0;
int lastY = 0;
int buttonDown=0;

// Mostra ou não os círculos imaginários
int debug = 0;

// Arena dimensions
GLfloat Width = 0;
GLfloat Height = 0;

// Ponto inicial do click para socar
GLfloat pCliqueX = 0;

// Componentes do mundo virtual sendo modelado
Player player1;             //Player1 - Jogador Real
AutomaticPlayer player2;    //Player2 - Jogador Virtual

// Texturas
GLuint texturePlayers[7];
GLuint texturePlayer1;
GLuint texturePlayer2;
GLuint textureSolo;
GLuint textureParede;
GLuint textureArena;
GLuint textureTeto;
GLuint texturePorta;

// Placar
bool fim = false;
static char str1[1000];
static char str2[1000];
void * font = GLUT_BITMAP_9_BY_15;

void RasterChars(GLfloat x, GLfloat y, GLfloat z, const char * text, double r, double g, double b)
{
    //Push to recover original attributes
    glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

        //Draw text in the x, y, z position
        glColor3f(r,g,b);
        glRasterPos3f(x, y, z);
        const char* tmpStr;
        tmpStr = text;
        while( *tmpStr ){
            glutBitmapCharacter(font, *tmpStr);
            tmpStr++;
        }

    glPopAttrib();
}

void PrintText(GLfloat x, GLfloat y, const char * text, double r, double g, double b)
{
    //Draw text considering a 2D space (disable all 3d features)
    glMatrixMode(GL_PROJECTION);
    //Push to recover original PROJECTION MATRIX
    glPushMatrix();
        glLoadIdentity ();
        glOrtho(0, 1, 0, 1, -1, 1);
        RasterChars(x, y, 0, text, r, g, b);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void ImprimePlacar(GLfloat x, GLfloat y, double r, double g, double b)
{
    //Cria a string a ser impressa
    sprintf(str1, "Score Player 1: %d", player1.getScore() );
    sprintf(str2, "Score Player 2: %d", player2.getScore() );

    PrintText(x, y+0.04, str1, r, g, b);
    PrintText(x, y, str2, r, g, b);

}

void ImprimeVencedor(int vencedor, GLfloat x, GLfloat y)
{
    double r = 0;
    double g = 0;
    double b = 0;
    if (vencedor == 1) {
        g = 1;
    } else {
        r = 1;
    }

    //Cria a string a ser impressa
    sprintf(str1, "PLAYER %d VENCEU!!!", vencedor );
    sprintf(str2, "Pressione '0' para reiniciar o jogo.");

    PrintText(x, y, str1, r, g, b);
    PrintText(x, y-0.04, str2, r, g, b);
}

int existeVencedor()
{
    if (player1.getScore() == 10) {
        return 1;
    } else if (player2.getScore() == 10) {
        return 2;
    }
    return 0;
}

void desenhaCilindro(GLfloat x, GLfloat y, GLfloat z, GLfloat height, GLfloat radius, GLfloat size, int angle)
{
    GLUquadricObj *cylindro = gluNewQuadric();
    GLdouble base = radius;
    GLdouble top = radius;
    GLint slices = 10;
    GLint stacks = 10;

    glPushMatrix();
    glTranslatef(x, y, z);

    // Cilindro do canto
    glRotatef(-90, 1, 0, 0);
    gluCylinder(cylindro, base, top, height, slices, stacks);
    glPopMatrix();

    glPushMatrix();

    // Topo do cilindro
    glTranslatef(x, height, z);
    int sides = 15;
    glColor3f(1, 0, 0);
    glPointSize(2.0f);
    glBegin(GL_POLYGON);
    for (int a = 0; a < 360; a += 360 / sides)
    {
        double heading = a * MY_PI / 180;
        glVertex3f(cos(heading) * top, 0, sin(heading) * top);
        glNormal3f(0, 1, 0);
    }
    glEnd();

    glPopMatrix();

    // Tiras laterias
    base *= 0.2;
    top *= 0.2;

    Material red_plastic(0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.7f, 0.6f, 0.6f, 0.25f);
    Material blue_plastic(0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.6f, 0.6f, 0.7f, 0.25f);

    red_plastic.ativar();
    y += (height/4)*0.75;
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angle, 0, 1, 0);
    gluCylinder(cylindro, base, top, size, slices, stacks);
    glPopMatrix();

    blue_plastic.ativar();
    y += (height/4);
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angle, 0, 1, 0);
    gluCylinder(cylindro, base, top, size, slices, stacks);
    glPopMatrix();

    red_plastic.ativar();
    y += (height/4);
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angle, 0, 1, 0);
    gluCylinder(cylindro, base, top, size, slices, stacks);
    glPopMatrix();

    blue_plastic.ativar();
    y += (height/4);
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angle, 0, 1, 0);
    gluCylinder(cylindro, base, top, size, slices, stacks);
    glPopMatrix();

}

void desenhaPlano(GLfloat textureS, GLuint texture)
{
    Material plano(1.0f, 1.0f, 1.0f, 0.2f, 0.2f, 0.2f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.78f);
    Material plano2(0.1f, 0.1f, 0.1f, 0.2f, 0.2f, 0.2f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.78f);

    if (!night_mode) {
        plano.ativar();
    } else {
        plano2.ativar();
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
    glNormal3f(0,1,0);
    glTexCoord2f(0,0);
    glVertex3f(-1, 0, -1);

    glNormal3f(0,1,0);
    glTexCoord2f(0,textureS);
    glVertex3f(-1, 0, +1);

    glNormal3f(0,1,0);
    glTexCoord2f(textureS,textureS);
    glVertex3f(+1, 0, +1);

    glNormal3f(0,1,0);
    glTexCoord2f(textureS,0);
    glVertex3f(+1, 0, -1);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void desenhaPlanoComplexo(GLfloat incr, GLfloat width, GLfloat height, GLfloat altura, GLuint texture)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    for (float i = -width-incr; i <= width ; i += incr) {
        glBegin(GL_QUADS);
        for (float j = -height-incr; j <= height; j += incr) {
            GLfloat mapeamento_x = ((i - (-width-incr)) / (width - (-width-incr)) * (1 - 0) + 0);
            GLfloat mapeamento_y = ((j - (-height-incr)) / (height - (-height-incr)) * (1 - 0) + 0);

            glNormal3f(0, 1, 0);
            glTexCoord2f(mapeamento_x,mapeamento_y);
            glVertex3f(i,altura,j);

            mapeamento_x = ((i+incr - (-width-incr)) / (width - (-width-incr)) * (1 - 0) + 0);

            glNormal3f(0, 1, 0);
            glTexCoord2f(mapeamento_x,mapeamento_y);
            glVertex3f(i+incr,altura,j);

            mapeamento_y = ((j+incr - (-height-incr)) / (height - (-height-incr)) * (1 - 0) + 0);

            glNormal3f(0, 1, 0);
            glTexCoord2f(mapeamento_x,mapeamento_y);
            glVertex3f(i+incr,altura,j+incr);

            mapeamento_x = ((i - (-width-incr)) / (width - (-width-incr)) * (1 - 0) + 0);

            glNormal3f(0, 1, 0);
            glTexCoord2f(mapeamento_x,mapeamento_y);
            glVertex3f(i,altura,j+incr);

        }
        glEnd();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void desenhaTerreno(GLfloat width, GLfloat height)
{
    float incr = 10.0;
    float y = 0.0;

    // Desenha linhas
//    glColor3f(1.0,1.0,1.0);
//    glBegin(GL_LINES);
//    for (float i = -L; i <= L ; i += incr*4) {
//        // Verticais
//        glNormal3f(0, 1, 0);
//        glVertex3f(i,y,-L);
//
//        glNormal3f(0, 1, 0);
//        glVertex3f(i,y,L);
//
//        // Horizontais
//        glNormal3f(0, 1, 0);
//        glVertex3f(-L,y,i);
//
//        glNormal3f(0, 1, 0);
//        glVertex3f(L,y,i);
//    }
//    glEnd();

    Material red_plastic(0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.7f, 0.6f, 0.6f, 0.25f);
    Material blue_plastic(0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.6f, 0.6f, 0.7f, 0.25f);
    Material white_plastic(0.1f, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f, 0.55f, 0.55f, 0.55f, 0.7f, 0.7f, 0.7f, 0.25f);
    Material material_padrao(0.5, 0.5, 0.5, 0.1, 0.1, 0.1, 1.0, 1.0, 1.0, 0.1, 0.1, 0.1, 0.1);
    Material material_padrao_night(0.1f, 0.1f, 0.1f, 0.2f, 0.2f, 0.2f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.78f);

    // Desenha piso
    if (night_mode) {
        material_padrao_night.ativar();
    } else {
        material_padrao.ativar();
    }

    GLfloat altura_ring = altura_padrao/4;

    // Desenha solo do ring
    desenhaPlanoComplexo(incr, width/2, height/2, y, textureArena);

    // Faz solo da arena
    desenhaPlanoComplexo(incr, width, height, -altura_ring, textureSolo);

    // Desenha fundo da arena
    // Direita
    glPushMatrix();
    glTranslatef(0, -altura_ring,0);
    glScalef(1, (altura_ring/2), (height/2)+incr);
    glTranslatef(-(width/2)-incr, 1, 0);
    glRotatef(90,0,1,0);
    glRotatef(-90,1,0,0);
    desenhaPlano(1, textureArena);
    glPopMatrix();

    // Esquerda
    glPushMatrix();
    glTranslatef(0, -altura_ring,0);
    glScalef(1, (altura_ring/2), (height/2)+incr);
    glTranslatef((width/2)+incr, 1, 0);
    glRotatef(-90,0,1,0);
    glRotatef(-90,1,0,0);
    desenhaPlano(1, textureArena);
    glPopMatrix();

    // Frente
    glPushMatrix();
    glTranslatef(0, -altura_ring,0);
    glScalef((width/2)+incr, (altura_ring/2), 1);
    glTranslatef(0, 1, (height/2)+incr);
    glRotatef(180,0,0,1);
    glRotatef(90,1,0,0);
    desenhaPlano(1, textureArena);
    glPopMatrix();

    // Costas
    glPushMatrix();
    glTranslatef(0, -altura_ring,0);
    glScalef((width/2)+incr, (altura_ring/2), 1);
    glTranslatef(0, 1, -(height/2)-incr);
    glRotatef(180,0,0,1);
    glRotatef(-90,1,0,0);
    desenhaPlano(1, textureArena);
    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, 0);

    //Desenha cilindros nos cantos
    red_plastic.ativar();
    desenhaCilindro((width/2), 0, (height/2), altura_cilindros, incr, height, 180);
    white_plastic.ativar();
    desenhaCilindro((width/2), 0, -(height/2), altura_cilindros, incr, width, 270);
    blue_plastic.ativar();
    desenhaCilindro(-(width/2), 0, -(height/2), altura_cilindros, incr, height, 0);
    white_plastic.ativar();
    desenhaCilindro(-(width/2), 0, (height/2), altura_cilindros, incr, width, 90);

//    width = 0.75*width;
//    height = 0.75*height;

    // Faz teto
    glPushMatrix();
    glScalef(width, 1, height);
    glTranslatef(0,3*altura_padrao,0);
    desenhaPlano(5, textureTeto);
    glPopMatrix();

    int textureS = 8;

    // Desenha paredes costas
    glPushMatrix();
    glTranslatef(0, -altura_ring,0);
    glScalef(width, width, 1);
    glTranslatef(0, 1,-height);
    glRotatef(180,0,0,1);
    glRotatef(-90,1,0,0);
    desenhaPlano(textureS, textureParede);
    glPopMatrix();

    // Desenha paredes frente
    glPushMatrix();
    glTranslatef(0, -altura_ring,0);
    glScalef(width, width, 1);
    glTranslatef(0, 1, height);
    glRotatef(180,0,0,1);
    glRotatef(90,1,0,0);
    desenhaPlano(textureS, textureParede);
    glPopMatrix();

    // Desenha paredes esquerda
    glPushMatrix();
    glTranslatef(0, -altura_ring,0);
    glScalef(1, height, height);
    glTranslatef(width, 1, 0);
    glRotatef(-90,0,1,0);
    glRotatef(-90,1,0,0);
    desenhaPlano(textureS, textureParede);
    glPopMatrix();

    // Desenha paredes direita
    glPushMatrix();
    glTranslatef(0, -altura_ring,0);
    glScalef(1, height, height);
    glTranslatef(-width, 1, 0);
    glRotatef(90,0,1,0);
    glRotatef(-90,1,0,0);
    desenhaPlano(textureS, textureParede);
    glPopMatrix();

    // Porta
    glPushMatrix();
    glTranslatef(0, -altura_ring,0);
    glScalef(1, 0.75*altura_padrao, 0.75*altura_padrao);
    glTranslatef(width*0.995, 1, 0);
    glRotatef(-90,0,1,0);
    glRotatef(-90,1,0,0);
    desenhaPlano(1, texturePorta);
    glPopMatrix();

}

void DesenhaMinimapa()
{
    GLfloat maior = Width >= Height ? Width : Height;

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    //Draw text considering a 2D space (disable all 3d features)
    glMatrixMode(GL_PROJECTION);
    //Push to recover original PROJECTION MATRIX
    glPushMatrix();
    glLoadIdentity ();
    glOrtho(0, 1, 0, 1, -1, 1);
    glColor3f(1.0, 1.0, 1.0);
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(1-0.25*(Width/maior),0.25*(Height/maior),1);
    glVertex3f(1,0.25*(Height/maior),1);
    glVertex3f(1,0,1);
    glVertex3f(1-0.25*(Width/maior),0,1);
    glEnd();

    glPushMatrix();
    GLfloat minimapa_x = ( -player1.getGx() - (-Width /2) )/( (Width /2) - (-Width /2) ) * (1 - (1-0.25*(Width/maior))) + (1-0.25*(Width/maior));
    GLfloat minimapa_y = ( player1.getGz() - (-Height/2) )/( (Height/2) - (-Height/2) ) * (0.25*(Height/maior) - 0) + 0;

    glTranslatef(minimapa_x,minimapa_y,0);
    int sides = 30;
    glColor3f(0, 1, 0);
    glPointSize(2.0f);
    glBegin(GL_POLYGON);
    for (int a = 0; a < 360; a += 360 / sides)
    {
        double heading = a * MY_PI / 180;
        glVertex3f(cos(heading) * (0.25*player1.getRadius()/maior), sin(heading) * (0.25*player1.getRadius()/maior), 1);
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    minimapa_x = ( -player2.getGx() - (-Width/2) )/( (Width/2) - (-Width/2) ) * (1 - (1-0.25*(Width/maior))) + (1-0.25*(Width/maior));
    minimapa_y = ( player2.getGz() - (-Height/2) )/( (Height/2) - (-Height/2) ) * (0.25*(Height/maior) - 0) + 0;

    glTranslatef(minimapa_x,minimapa_y,0);
    glColor3f(1, 0, 0);
    glPointSize(2.0f);
    glBegin(GL_POLYGON);
    for (int a = 0; a < 360; a += 360 / sides)
    {
        double heading = a * MY_PI / 180;
        glVertex3f(cos(heading) * (0.25*player2.getRadius()/maior), sin(heading) * (0.25*player2.getRadius()/maior), 1);
    }
    glEnd();
    glPopMatrix();

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
}

void reshape (int w, int h)
{
    //Ajusta o tamanho da tela com a janela de visualizacao
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        gluPerspective (75, (GLfloat)h/(GLfloat)w, 0.1, 2000);
    else
        gluPerspective (75, (GLfloat)w/(GLfloat)h, 0.1, 2000);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
}

//Funcao auxiliar para fazer o produto vetorial entre dois vetores a x b = out
void cross(float a[3], float b[3], float out[3])
{
    out[0] = a[1]*b[2] - a[2]*b[1];
    out[1] = a[2]*b[0] - a[0]*b[2];
    out[2] = a[0]*b[1] - a[1]*b[0];
}

//Funcao auxiliar para normalizar um vetor a/|a|
void normalize(float a[3])
{
    double norm = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
    a[0] /= norm;
    a[1] /= norm;
    a[2] /= norm;
}

void renderScene(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (!fim) {
        //Limpa a cor com azulado
        glClearColor (0.30, 0.30, 1.0, 0.0);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float r, g, b;
        if (night_mode) {
            r = 0.2;
            g = 0.2;
            b = 1.0;
        } else {
            r = 0.0;
            g = 0.0;
            b = 1.0;
        }
        ImprimePlacar(0.01, 0.02, r, g, b);

        DesenhaMinimapa();

        if (int vencedor = existeVencedor()) {
            ImprimeVencedor(vencedor, 0.01, 0.96);
            fim = true;
        }

        //Controla camera
        if (lookatToggle == 3) {
            // camera circular
            //Utiliza uma esfera de raio zoom para guiar a posicao da camera
            //baseada em dois angulos (do plano XZ e do plano XY)
            gluLookAt(  player1.getGx()+zoom*sin(camXZAngle*M_PI/180)*cos((camXYAngle*M_PI/180)),
                        zoom*                            sin((camXYAngle*M_PI/180)),
                        player1.getGz()+zoom*cos(camXZAngle*M_PI/180)*cos((camXYAngle*M_PI/180)),
                        player1.getGx(), 0, player1.getGz(),
                        0, 1, 0);

        } else if (lookatToggle == 1) {
            // camera primeira pessoa
            glTranslatef(player1.getMeshX(PONTO_OLHOS),
                         -player1.getMeshY(PONTO_OLHOS),
                         player1.getMeshZ(PONTO_OLHOS)
                         );

            glRotatef(-player1.getDirection(), 0.0, 1.0, 0.0);

            gluLookAt(player1.getGx(),
                      0,
                      player1.getGz(),
                      player1.getGx(),
                      0,
                      player1.getGz()+10,
                      0, 1, 0);


        } else if (lookatToggle == 4) {
            // Camera cima
            glRotatef(-player1.getDirection(), 0.0, 0.0, 1.0);

            gluLookAt(player1.getGx(),
                      2*altura_padrao,
                      player1.getGz(),
                      player1.getGx(), 0, player1.getGz(),
                      0, 0, 1);

        } else if (lookatToggle == 2) {
            //Câmera relogio esquedo
            glTranslatef(player1.getMeshX(PONTO_BRACO_ESQUERDO),
                         -player1.getMeshY(PONTO_BRACO_ESQUERDO),
                         player1.getMeshZ(PONTO_BRACO_ESQUERDO)
                         );

//            GLfloat center_x = player1.getGx()+player1.getMeshX(PONTO_BRACO_ESQUERDO_AUX);
//            GLfloat center_y = player1.getGy()+player1.getMeshY(PONTO_BRACO_ESQUERDO_AUX);
//            GLfloat center_z = player1.getGz()+player1.getMeshZ(PONTO_BRACO_ESQUERDO_AUX);

//            GLfloat center_x = player1.getMeshX(PONTO_BRACO_ESQUERDO_AUX);
//            GLfloat center_y = player1.getMeshY(PONTO_BRACO_ESQUERDO_AUX);
//            GLfloat center_z = player1.getMeshZ(PONTO_BRACO_ESQUERDO_AUX);

            GLfloat center[3];
            GLfloat side[3];
            GLfloat up[3];

            player1.getCenterPoint(PONTO_BRACO_ESQUERDO_AUX, center[0], center[1], center[2]);
            player1.getCenterPoint(15510, side[0], side[1], side[2]);

            cross(side,center,up);
            normalize(up);

            glRotatef(-player1.getDirection(), 0.0, 1.0, 0.0);

//            printf("(x,y,z) = (%.2f,%.2f,%.2f)\n", player1.getGx(), player1.getGz(), player1.getGz());
//            printf("center_x = %.2f\ncenter_y = %.2f\ncenter_z = %.2f\n\n", center_x, center_y, center_z);

            gluLookAt(player1.getGx(),
                      0,
                      player1.getGz(),
                      player1.getGx()+center[0],
                      center[1],
                      player1.getGz()+center[2],
                      up[0], up[1], up[2]);

        } else if (lookatToggle == 5) {
            // camera canto da arena
            gluLookAt((altura_padrao+zoom)*sin(camXZAngle*M_PI/180)*cos((camXYAngle*M_PI/180)),
                      (altura_padrao+zoom)*                            sin((camXYAngle*M_PI/180)),
                      (altura_padrao+zoom)*cos(camXZAngle*M_PI/180)*cos((camXYAngle*M_PI/180)),
                      0, 0, 0,
                      0, 1, 0);
        }

        glPushMatrix();
        if (lookatToggle == 3) {
            //Translada para o centro do soldado para facilitar a rotacao da camera
            glTranslatef(0, -altura_padrao/2, 0);
        }

        desenhaTerreno(Width, Height);

        player1.Desenha(texturePlayers[player1.getTexture()], debug);
        player2.Desenha(texturePlayers[player2.getTexture()], debug);

        glPopMatrix();

        //Define e desenha a fonte de luz
        if (night_mode) {
            GLfloat light_position_player1[] = {player1.getGx(), 3*altura_padrao, player1.getGz(), 1};
            GLfloat light_position_player2[] = {player2.getGx(), 3*altura_padrao, player2.getGz(), 1};
            GLfloat light_direction[] = {0, -1, 0};

            GLfloat luz_ambiente[]  = { 0.0, 0.0, 0.0, 1 } ;
            GLfloat luz_difusa[]    = { 1.0, 1.0, 1.0, 1 } ;
            GLfloat luz_especular[] = { 1.0, 1.0, 1.0, 1 } ;

            // Spot light player1
            glLightfv( GL_LIGHT0, GL_POSITION, light_position_player1 );
            glLightfv( GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
            glLightf ( GL_LIGHT0, GL_SPOT_EXPONENT , 0);
            glLightf ( GL_LIGHT0, GL_SPOT_CUTOFF, 15.0);

            glLightfv(GL_LIGHT0, GL_AMBIENT, luz_ambiente );
            glLightfv(GL_LIGHT0, GL_DIFFUSE, luz_difusa );
            glLightfv(GL_LIGHT0, GL_SPECULAR, luz_especular );

            glLightf (GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.5);
            glLightf (GL_LIGHT0, GL_LINEAR_ATTENUATION, 0) ;
            glLightf (GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0) ;

            if (debug != 0) {
                glDisable(GL_LIGHTING);
                glPointSize(50);
                glColor3f(1.0, 1.0, 0.0);
                glBegin(GL_POINTS);
                glVertex3f(light_position_player1[0], light_position_player1[1], light_position_player1[2]);
                glEnd();
            }

            // Spot light player2
            glLightfv( GL_LIGHT1, GL_POSITION, light_position_player2 );
            glLightfv( GL_LIGHT1, GL_SPOT_DIRECTION, light_direction);
            glLightf ( GL_LIGHT1, GL_SPOT_EXPONENT , 0);
            glLightf ( GL_LIGHT1, GL_SPOT_CUTOFF, 15.0);

            glLightfv(GL_LIGHT1, GL_AMBIENT, luz_ambiente );
            glLightfv(GL_LIGHT1, GL_DIFFUSE, luz_difusa );
            glLightfv(GL_LIGHT1, GL_SPECULAR, luz_especular );

            glLightf (GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.5);
            glLightf (GL_LIGHT1, GL_LINEAR_ATTENUATION, 0) ;
            glLightf (GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0) ;

            if (debug != 0) {
                glDisable(GL_LIGHTING);
                glPointSize(50);
                glColor3f(1.0, 1.0, 0.0);
                glBegin(GL_POINTS);
                glVertex3f(light_position_player2[0], light_position_player2[1], light_position_player2[2]);
                glEnd();
            }

            glEnable(GL_LIGHTING);

            glEnable(GL_LIGHT0);
            glEnable(GL_LIGHT1);

            glDisable(GL_LIGHT3);
            glDisable(GL_LIGHT4);
            glDisable(GL_LIGHT5);
            glDisable(GL_LIGHT6);

        } else {
            GLfloat altura;
            if (lookatToggle == 3) {
                altura = altura_cilindros;
            } else {
                altura = altura_cilindros+(altura_padrao/2);
            }

            GLfloat light_position1[] = {Width / 2, altura, Height / 2, 1};
            GLfloat light_position2[] = {-Width/2, altura, Height/2, 1};
            GLfloat light_position3[] = {-Width/2, altura, -Height/2, 1};
            GLfloat light_position4[] = {Width/2, altura, -Height/2, 1};

            GLfloat luz_ambiente[]  = { 0.0, 0.0, 0.0, 1 } ;
            GLfloat luz_difusa[]    = { 1.0, 1.0, 1.0, 1 } ;
            GLfloat luz_especular[] = { 1.0, 1.0, 1.0, 1 } ;

            glLightfv(GL_LIGHT3, GL_POSITION, light_position1 );
            glLightfv(GL_LIGHT3, GL_AMBIENT, luz_ambiente );
            glLightfv(GL_LIGHT3, GL_DIFFUSE, luz_difusa );
            glLightfv(GL_LIGHT3, GL_SPECULAR, luz_especular );

            glLightfv(GL_LIGHT4, GL_POSITION, light_position2 );
            glLightfv(GL_LIGHT4, GL_AMBIENT, luz_ambiente );
            glLightfv(GL_LIGHT4, GL_DIFFUSE, luz_difusa );
            glLightfv(GL_LIGHT4, GL_SPECULAR, luz_especular );

            glLightfv(GL_LIGHT5, GL_POSITION, light_position3 );
            glLightfv(GL_LIGHT5, GL_AMBIENT, luz_ambiente );
            glLightfv(GL_LIGHT5, GL_DIFFUSE, luz_difusa );
            glLightfv(GL_LIGHT5, GL_SPECULAR, luz_especular );

            glLightfv(GL_LIGHT6, GL_POSITION, light_position4 );
            glLightfv(GL_LIGHT6, GL_AMBIENT, luz_ambiente );
            glLightfv(GL_LIGHT6, GL_DIFFUSE, luz_difusa );
            glLightfv(GL_LIGHT6, GL_SPECULAR, luz_especular );

            if (debug != 0) {
                glDisable(GL_LIGHTING);
                glPointSize(15);
                glColor3f(1.0, 1.0, 0.0);
                glBegin(GL_POINTS);
                if (pontual_light == 0 || pontual_light == 1) {
                    glVertex3f(light_position1[0], light_position1[1], light_position1[2]);
                }
                if (pontual_light == 0 || pontual_light == 2) {
                    glVertex3f(light_position2[0], light_position2[1], light_position2[2]);
                }
                if (pontual_light == 0 || pontual_light == 3) {
                    glVertex3f(light_position3[0], light_position3[1], light_position3[2]);
                }
                if (pontual_light == 0 || pontual_light == 4) {
                    glVertex3f(light_position4[0], light_position4[1], light_position4[2]);
                }
                glEnd();
                glEnable(GL_LIGHTING);
            }

            glDisable(GL_LIGHT3);
            glDisable(GL_LIGHT4);
            glDisable(GL_LIGHT5);
            glDisable(GL_LIGHT6);

            if (pontual_light == 0 || pontual_light == 1) {
                glEnable(GL_LIGHT3);
            }
            if (pontual_light == 0 || pontual_light == 2) {
                glEnable(GL_LIGHT4);
            }
            if (pontual_light == 0 || pontual_light == 3) {
                glEnable(GL_LIGHT5);
            }
            if (pontual_light == 0 || pontual_light == 4) {
                glEnable(GL_LIGHT6);
            }

            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHT1);
        }

        glutSwapBuffers(); // Desenha the new frame of the game.
    }
}

void keyPress(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'a':
            keyStatus[(int)('a')] = 1; //Using keyStatus trick
            break;
        case 'A':
            keyStatus[(int)('A')] = 1; //Using keyStatus trick
            break;
        case 'd':
            keyStatus[(int)('d')] = 1; //Using keyStatus trick
            break;
        case 'D':
            keyStatus[(int)('D')] = 1; //Using keyStatus trick
            break;
        case 'w':
            keyStatus[(int)('w')] = 1; //Using keyStatus trick
            break;
        case 'W':
            keyStatus[(int)('W')] = 1; //Using keyStatus trick
            break;
        case 's':
            keyStatus[(int)('s')] = 1; //Using keyStatus trick
            break;
        case 'S':
            keyStatus[(int)('S')] = 1; //Using keyStatus trick
            break;
        case '+':
            zoom -= 5;
            break;
        case '-':
            zoom += 5;
            break;
        case '0':
            fim = false;
            player1.ReiniciaPlayer();
            player2.ReiniciaPlayer();

            player2.setMouseDistance(0);
            player2.setMouseDirection(1);

            // Coloca a câmera 3 atras do lutador
            camXZAngle = 180+player1.getDirection();
            camXYAngle = INICIAL_CAM_XY_ANGLE;
            lookatToggle = 3;

            printf("\n\nReiniciando o Jogo...\n\n");
            break;
        case ',':
            if (player2.getEnableMoviment()) {
                player2.setEnableMoviment(false);
                printf("Movimentação do Player 2 Desativada!\n");
            } else {
                player2.setEnableMoviment(true);
                printf("Movimentação do Player 2 Ativada!\n");
            }
            break;
        case '.':
            if (debug == 4) {
                debug = 0;
            } else {
                debug++;
            }
            break;
        case '1':
            lookatToggle = 1;
            break;
        case '2':
            lookatToggle = 2;
            break;
        case '3':
            lookatToggle = 3;
            // Coloca a câmera atras do lutador
            camXZAngle = 180+player1.getDirection();
            break;
        case '4':
            lookatToggle = 4;
            break;
        case '5':
            lookatToggle = 5;
            // Coloca a câmera no meio do ring
            camXZAngle = 180;
            break;
        case 'n':
            night_mode = !night_mode;
            printf("night_mode = %d\n", night_mode);
            break;
        case 'o':
            if (player1.getTexture() == 6) {
                player1.setTexture(0);
            } else {
                player1.setTexture(player1.getTexture() + 1);
            }
            break;
        case 'p':
            if (player2.getTexture() == 6) {
                player2.setTexture(0);
            } else {
                player2.setTexture(player2.getTexture() + 1);
            }
            break;
        case 'i':
            if (pontual_light == 4) {
                pontual_light = 0;
            } else {
                pontual_light++;
            }
            break;
        case 27 :
            exit(0);
    }
    glutPostRedisplay();
}

void keyup(unsigned char key, int x, int y)
{
    keyStatus[(int)(key)] = 0;
    glutPostRedisplay();
}

void ResetKeyStatus()
{
    int i;
    //Initialize keyStatus
    for(i = 0; i < 256; i++)
        keyStatus[i] = 0;
}

void mouse_motion(int x, int y){
    if (player1.getSocando() and !fim){
        if (keyStatus[(int)('a')] or keyStatus[(int)('w')] or keyStatus[(int)('s')] or keyStatus[(int)('d')]) {
            player1.Soca(0, WINDOW_WIDTH, player2, 1);
        } else {
            if (player1.Soca(pCliqueX - x, WINDOW_WIDTH, player2, 0)) {
                if (!player1.getAtingindo()) {
                    player1.setAtingindo(true);
                    player1.IncrementaScore();
                    printf("Player 2 atingido: %d\n", player1.getScore());
                }
            } else {
                player1.setAtingindo(false);
            }
        }
    }

    if (!buttonDown)
        return;

    camXZAngle -= x - lastX;
    if (camXZAngle > 360) {
        camXZAngle -= 360;
    } else if (camXZAngle < 0) {
        camXZAngle += 360;
    }
//    printf("camXZAngle = %.2f\n", camXZAngle);

    if ( camXYAngle + y - lastY <= INICIAL_CAM_XY_ANGLE+60 and camXYAngle + y - lastY >= INICIAL_CAM_XY_ANGLE-60) {
        camXYAngle += y - lastY;
//        printf("camXYAngle = %.2f\n", camXYAngle);
    }

    lastX = x;
    lastY = y;
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y){
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        player1.setSocando(true);
        pCliqueX = x;
    } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP){
        player1.Soca(0, WINDOW_WIDTH, player2, 1);
    }

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
        lastX = x;
        lastY = y;
        buttonDown = 1;
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
        buttonDown = 0;
    }

    mouse_motion(x, y);
}

void idle(void)
{
    static GLdouble previousTime = glutGet(GLUT_ELAPSED_TIME);
    GLdouble currentTime, timeDiference;
    //Pega o tempo que passou do inicio da aplicacao
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    // Calcula o tempo decorrido desde de a ultima frame.
    timeDiference = currentTime - previousTime;
    //Atualiza o tempo do ultimo frame ocorrido
    previousTime = currentTime;

    GLdouble inc = incKeyIdle*timeDiference;
    //Treat keyPress
    if(keyStatus[(int)('a')] or keyStatus[(int)('A')])
    {
        if (player1.getSocando()) {
            player1.Soca(0, WINDOW_WIDTH, player2, 1);
        }
        player1.ViraPlayer(inc*0.65);
//        printf("PLAYER DIRECTION = %.2f\n", player1.getDirection());
    }
    if(keyStatus[(int)('d')] or keyStatus[(int)('D')])
    {
        if (player1.getSocando()) {
            player1.Soca(0, WINDOW_WIDTH, player2, 1);
        }
        player1.ViraPlayer(-inc*0.65);
//        printf("PLAYER DIRECTION = %.2f\n", player1.getDirection());
    }
    if(keyStatus[(int)('w')] or keyStatus[(int)('W')])
    {
        // Se estiver socando, cancela o soco ao andar
        if (player1.getSocando()) {
            player1.Soca(0, WINDOW_WIDTH, player2, 1);
        }
        // Para a animação não ficar muito rápida, foi configurado um intervalo entre as animações
        if (player1.getWalkIntervalW() % WALK_INTERVAL == 0) {
            player1.MoveXZ(inc, Width, Height, player2);
        } else if (player1.getWalkIntervalW() > WALK_INTERVAL){
            player1.setWalkIntervalW(player1.getWalkIntervalW() - WALK_INTERVAL);
        }
        player1.setWalkIntervalW(player1.getWalkIntervalW() + 1);
//        printf("(x,y,z) = (%.2f,%.2f,%.2f)\n", player1.getGx(),player1.getGy(),player1.getGz());
    }
    if(keyStatus[(int)('s')] or keyStatus[(int)('S')])
    {
        // Se estiver socando, cancela o soco ao andar
        if (player1.getSocando()) {
            player1.Soca(0, WINDOW_WIDTH, player2, 1);
        }
        // Para a animação não ficar muito rápida, foi configurado um intervalo entre as animações
        if (player1.getWalkIntervalS() % WALK_INTERVAL == 0) {
            player1.MoveXZ(-inc, Width, Height, player2);
        } else if (player1.getWalkIntervalS() > WALK_INTERVAL){
            player1.setWalkIntervalS(player1.getWalkIntervalS() - WALK_INTERVAL);
        }
        player1.setWalkIntervalS(player1.getWalkIntervalS() + 1);
    }

    // Movimentação player2
    if (!fim and player2.getEnableMoviment()) {
        if (player2.getWalkIntervalS() % WALK_INTERVAL == 0) {
            player2.MovimentacaoAutomatica(inc, Width, Height, player1);
        } else if (player2.getWalkIntervalS() > WALK_INTERVAL){
            player2.setWalkIntervalS(player2.getWalkIntervalS() - WALK_INTERVAL);
        }
        player2.setWalkIntervalS(player2.getWalkIntervalS() + 1);
    }

    glutPostRedisplay();
}

void setArenaSize(char* svg_file, GLfloat &xInit, GLfloat &zInit)
{
    XMLDocument doc;
    doc.LoadFile(svg_file);

    if (doc.Error()) {
        printf("ERRO: Não foi possível acessar o arquivo indicado!\n");
        exit(-1);
    }

    XMLElement* atributo_rect = doc.FirstChildElement()->FirstChildElement("rect");
    if (atributo_rect == nullptr) {
        printf("ERRO: Não foi possível encontrar a tag 'rect' no arquivo SVG indicado!\n");
        exit(-1);
    }

    atributo_rect->QueryFloatAttribute( "x", &xInit );
    atributo_rect->QueryFloatAttribute( "y", &zInit );
    atributo_rect->QueryFloatAttribute( "width", &Width );
    atributo_rect->QueryFloatAttribute( "height", &Height );

//    incKeyIdle = 0.00025*(Width+Height)/2;
    incKeyIdle = 0.3;
}

void playersSize(char* svg_file, GLfloat &player1_x, GLfloat &player1_z, GLfloat &player1_r, GLfloat &player2_x, GLfloat &player2_z, GLfloat &player2_r)
{
    XMLDocument doc;
    doc.LoadFile(svg_file);

    const char* cor;
    XMLElement* atributo_circ1 = doc.FirstChildElement()->FirstChildElement( "circle" );
    if (atributo_circ1 == nullptr) {
        printf("ERRO: Não foi possível encontrar a primeira tag 'circle' no arquivo SVG indicado!\n");
        exit(-1);
    }
    atributo_circ1->QueryStringAttribute("fill", &cor);

    XMLElement* atributo_circ2 = atributo_circ1->NextSiblingElement( "circle" );
    if (atributo_circ2 == nullptr) {
        printf("ERRO: Não foi possível encontrar a segunda tag 'circle' no arquivo SVG indicado!\n");
        exit(-1);
    }

    if (strcmp(cor,"red") == 0){
        atributo_circ1->QueryFloatAttribute("cx", &player2_x);
        atributo_circ1->QueryFloatAttribute("cy", &player2_z);
        atributo_circ1->QueryFloatAttribute("r", &player2_r);

        atributo_circ2->QueryFloatAttribute("cx", &player1_x);
        atributo_circ2->QueryFloatAttribute("cy", &player1_z);
        atributo_circ2->QueryFloatAttribute("r", &player1_r);
    } else {
        atributo_circ1->QueryFloatAttribute("cx", &player1_x);
        atributo_circ1->QueryFloatAttribute("cy", &player1_z);
        atributo_circ1->QueryFloatAttribute("r", &player1_r);

        atributo_circ2->QueryFloatAttribute("cx", &player2_x);
        atributo_circ2->QueryFloatAttribute("cy", &player2_z);
        atributo_circ2->QueryFloatAttribute("r", &player2_r);
    }
}

void setPlayers(Player &p1, Player &p2, char* svg_file, GLfloat xInit, GLfloat zInit)
{
    GLfloat player1_x = 0;
    GLfloat player1_z = 0;
    GLfloat player1_r = 0;
    GLfloat player2_x = 0;
    GLfloat player2_z = 0;
    GLfloat player2_r = 0;

    playersSize(svg_file, player1_x, player1_z, player1_r, player2_x, player2_z, player2_r);

    player1_x = -((player1_x - xInit) - Width/2);
    player1_z = -((player1_z - zInit) - Height/2);

    player2_x = -((player2_x - xInit) - Width/2);
    player2_z = -((player2_z - zInit) - Height/2);

    p1.setGxInit(player1_x);
    p1.setGzInit(player1_z);

//    printf("zInit = %.2f\n", zInit);
//    printf("gx_init = %.2f\ngz_init = %.2f\n\n", ((player1_x - xInit) - Width/2), (-((player1_z - zInit) - Height/2)));

    p1.setRadius(player1_r);
    p1.setRadiusGlove(player1_r*PLAYER_SCALE*0.13);
    p1.setRadiusMax(player1_r);
    p1.setRadiusHead(player1_r*PLAYER_SCALE*0.2);
//    p1.setPaddleHeight(player1_r*1.75);
//    p1.setPaddleWidth(player1_r*0.12);
    p1.setDirectionInit(atan2(player2_x-player1_x, player2_z-player1_z)*180/MY_PI);
    p1.setRole(0);
    p1.setMeshWalk(mesh_walk);
    p1.setMeshPunch(mesh_punch);
    p1.setScale(player1_r*PLAYER_SCALE);


    p2.setGxInit(player2_x);
    p2.setGzInit(player2_z);
    p2.setRadius(player2_r);
    p2.setRadiusGlove(player2_r*PLAYER_SCALE*0.13);
    p2.setRadiusMax(player2_r);
    p2.setRadiusHead(player2_r*PLAYER_SCALE*0.2);
//    p2.setPaddleHeight(player2_r*1.75);
//    p2.setPaddleWidth(player2_r*0.12);
    p2.setDirectionInit(atan2(player1_x-player2_x,player1_z-player2_z)*180/MY_PI);
    p2.setRole(1);
    p2.setMeshWalk(mesh_walk);
    p2.setMeshPunch(mesh_punch);
    p2.setScale(player2_r*PLAYER_SCALE);

}

GLuint LoadTextureRAW( const char * filename, int channel )
{

    GLuint texture;

    // -------------------- STB_IMAGE --------------------
    int w, h, comp;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* image = stbi_load(filename, &w, &h, &comp, channel);

    if(image == nullptr)
        throw(std::string("Failed to load texture"));

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

//    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(comp == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    else if(comp == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);


    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(image);


    return texture;


    // -------------------- THIGO --------------------
//    Image* image = loadBMP(filename);
//
//    glGenTextures( 1, &texture );
//    glBindTexture( GL_TEXTURE_2D, texture );
//    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
////    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE );
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
//    glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
//                 0,                            //0 for now
//                 GL_RGB,                       //Format OpenGL uses for image
//                 image->width, image->height,  //Width and height
//                 0,                            //The border of the image
//                 GL_RGB, //GL_RGB, because pixels are stored in RGB format
//                 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
//            //as unsigned numbers
//                 image->pixels);               //The actual pixel data
//    delete image;
//
//    return texture;
}

void init(void)
{
    ResetKeyStatus();

    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

//    glDepthFunc(GL_LEQUAL);

    texturePlayers[0] = LoadTextureRAW( "./modelos 3d/lutador/textures/textura.png", STBI_rgb );
    texturePlayers[1] = LoadTextureRAW( "./modelos 3d/lutador/textures/textura01.png", STBI_rgb_alpha );
    texturePlayers[2] = LoadTextureRAW( "./modelos 3d/lutador/textures/textura02.png", STBI_rgb_alpha );
    texturePlayers[3] = LoadTextureRAW( "./modelos 3d/lutador/textures/textura03.png", STBI_rgb_alpha );
    texturePlayers[4] = LoadTextureRAW( "./modelos 3d/lutador/textures/textura04.png", STBI_rgb_alpha );
    texturePlayers[5] = LoadTextureRAW( "./modelos 3d/lutador/textures/textura05.png", STBI_rgb_alpha );
    texturePlayers[6] = LoadTextureRAW( "./modelos 3d/lutador/textures/textura06.png", STBI_rgb_alpha );

    player1.setTexture(0);
    player2.setTexture(6);

//    texturePlayer1 = LoadTextureRAW( "./modelos 3d/lutador/textures/textura.png", STBI_rgb );
//    texturePlayer2 = LoadTextureRAW( "./modelos 3d/lutador/textures/textura01.png", STBI_rgb_alpha );

    textureSolo = LoadTextureRAW( "./modelos 3d/outras texturas/solo.png", STBI_rgb );
    textureParede = LoadTextureRAW( "./modelos 3d/outras texturas/parede.png", STBI_rgb_alpha );
    textureArena = LoadTextureRAW( "./modelos 3d/outras texturas/arena.png", STBI_rgb_alpha );
    textureTeto = LoadTextureRAW( "./modelos 3d/outras texturas/teto.png", STBI_rgb_alpha );
    texturePorta = LoadTextureRAW( "./modelos 3d/outras texturas/porta.png", STBI_rgb_alpha );


    //Carrega as meshes dos arquivos
    //Metade walk
    mesh_walk[0].loadMesh("./modelos 3d/lutador/Short Step Forward/walking00/walking00.obj");
    mesh_walk[1].loadMesh("./modelos 3d/lutador/Short Step Forward/walking02/walking02.obj");
    mesh_walk[2].loadMesh("./modelos 3d/lutador/Short Step Forward/walking04/walking04.obj");
    mesh_walk[3].loadMesh("./modelos 3d/lutador/Short Step Forward/walking06/walking06.obj");
    mesh_walk[4].loadMesh("./modelos 3d/lutador/Short Step Forward/walking08/walking08.obj");
    mesh_walk[5].loadMesh("./modelos 3d/lutador/Short Step Forward/walking10/walking10.obj");
    mesh_walk[6].loadMesh("./modelos 3d/lutador/Short Step Forward/walking12/walking12.obj");
    mesh_walk[7].loadMesh("./modelos 3d/lutador/Short Step Forward/walking14/walking14.obj");
    mesh_walk[8].loadMesh("./modelos 3d/lutador/Short Step Forward/walking16/walking16.obj");
    mesh_walk[9].loadMesh("./modelos 3d/lutador/Short Step Forward/walking18/walking18.obj");
    mesh_walk[10].loadMesh("./modelos 3d/lutador/Short Step Forward/walking20/walking20.obj");
    mesh_walk[11].loadMesh("./modelos 3d/lutador/Short Step Forward/walking22/walking22.obj");

    // Todos punch left
    mesh_punch[0].loadMesh("./modelos 3d/lutador/Punching Left/punchleft00/punchleft00.obj");
    mesh_punch[1].loadMesh("./modelos 3d/lutador/Punching Left/punchleft01/punchleft01.obj");
    mesh_punch[2].loadMesh("./modelos 3d/lutador/Punching Left/punchleft02/punchleft02.obj");
    mesh_punch[3].loadMesh("./modelos 3d/lutador/Punching Left/punchleft03/punchleft03.obj");
    mesh_punch[4].loadMesh("./modelos 3d/lutador/Punching Left/punchleft04/punchleft04.obj");
    mesh_punch[5].loadMesh("./modelos 3d/lutador/Punching Left/punchleft05/punchleft05.obj");
    mesh_punch[6].loadMesh("./modelos 3d/lutador/Punching Left/punchleft06/punchleft06.obj");
    mesh_punch[7].loadMesh("./modelos 3d/lutador/Punching Left/punchleft07/punchleft07.obj");
    mesh_punch[8].loadMesh("./modelos 3d/lutador/Punching Left/punchleft08/punchleft08.obj");
    mesh_punch[9].loadMesh("./modelos 3d/lutador/Punching Left/punchleft09/punchleft09.obj");
    mesh_punch[10].loadMesh("./modelos 3d/lutador/Punching Left/punchleft10/punchleft10.obj");
    mesh_punch[11].loadMesh("./modelos 3d/lutador/Punching Left/punchleft11/punchleft11.obj");
    mesh_punch[12].loadMesh("./modelos 3d/lutador/Punching Left/punchleft12/punchleft12.obj");

    // Todos  punch right
    mesh_punch[13].loadMesh("./modelos 3d/lutador/Punching Right/punchright00/punchright00.obj");
    mesh_punch[14].loadMesh("./modelos 3d/lutador/Punching Right/punchright01/punchright01.obj");
    mesh_punch[15].loadMesh("./modelos 3d/lutador/Punching Right/punchright02/punchright02.obj");
    mesh_punch[16].loadMesh("./modelos 3d/lutador/Punching Right/punchright03/punchright03.obj");
    mesh_punch[17].loadMesh("./modelos 3d/lutador/Punching Right/punchright04/punchright04.obj");
    mesh_punch[18].loadMesh("./modelos 3d/lutador/Punching Right/punchright05/punchright05.obj");
    mesh_punch[19].loadMesh("./modelos 3d/lutador/Punching Right/punchright06/punchright06.obj");
    mesh_punch[20].loadMesh("./modelos 3d/lutador/Punching Right/punchright07/punchright07.obj");
    mesh_punch[21].loadMesh("./modelos 3d/lutador/Punching Right/punchright08/punchright08.obj");
    mesh_punch[22].loadMesh("./modelos 3d/lutador/Punching Right/punchright09/punchright09.obj");
    mesh_punch[23].loadMesh("./modelos 3d/lutador/Punching Right/punchright10/punchright10.obj");
    mesh_punch[24].loadMesh("./modelos 3d/lutador/Punching Right/punchright11/punchright11.obj");
    mesh_punch[25].loadMesh("./modelos 3d/lutador/Punching Right/punchright12/punchright12.obj");


    altura_padrao = player1.getMeshY(PONTO_TOPO);
    altura_cilindros = 0.75*player1.getMeshY(PONTO_TOPO);
    camXZAngle = 180+player1.getDirection();
    zoom = player1.getRadius()*5;
}


int main(int argc, char *argv[])
{
    // Define tamanho da arena pelo arquivo SVG
    GLfloat arenaX = 0;
    GLfloat arenaZ = 0;

    if (argc < 2) {
        printf("ERRO: Deve ser passado um arquivo SVG como argumento!\n");
        exit(-1);
    }

    setArenaSize(argv[1], arenaX, arenaZ);

    // Initialize openGL with Double buffer and RGB color without transparency.
    // Its interesting to try GLUT_SINGLE instead of GLUT_DOUBLE.
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create the window.
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(2000,30);
    glutCreateWindow("Trabalho 3D - Matheus Rizzi e Renan Bottacine");

    setPlayers(player1, player2, argv[1], arenaX, arenaZ);

    // Define callbacks.
    glutIdleFunc(idle);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyPress);
    glutKeyboardUpFunc(keyup);
//    glutIgnoreKeyRepeat(true);
    glutMotionFunc(mouse_motion);
    glutMouseFunc(mouse);

    init();

    glutMainLoop();

    return 0;
}