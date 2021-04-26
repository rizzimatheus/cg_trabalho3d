#ifndef TRABALHO2D_PLAYER_H
#define TRABALHO2D_PLAYER_H

#endif //TRABALHO2D_PLAYER_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "objloader.h"
#include "material.h"
#define MY_PI 3.1415926535897932384626433832795
//#define BRACO_DESCOLAMENTO1 0.9     // Deslocamento da primeira haste do braço para dentro do corpo
//#define BRACO_DESCOLAMENTO2 0.97    // Deslocamento da segunda haste do braço para dentro da primeira haste
//#define GTHETAL1_MIN 50             // Ângulo mínimo/padrão do ombro do braço esquerdo
//#define GTHETAL1_MAX -80            // Ângulo máximo do ombro do braço esquerdo
//#define GTHETAL2_MIN -140           // Ângulo mínimo/padrão do cotovelo do braço esquerdo
//#define GTHETAL2_MAX -60            // Ângulo máximo do cotovelo do braço esquerdo
//#define GTHETAR1_MIN -50            // Ângulo mínimo/padrão do ombro do braço direito
//#define GTHETAR1_MAX 80             // Ângulo máximo do ombro do braço direito
//#define GTHETAR2_MIN 140            // Ângulo mínimo/padrão do cotovelo do braço direito
//#define GTHETAR2_MAX 60             // Ângulo máximo do cotovelo do braço direito
#define MESH_WALK_SIZE 11
#define MESH_PUNCH_SIZE 25
#define WALK_INTERVAL 2
#define PLAYER_SCALE 1.9
#define PONTO_TOPO 1288
#define PONTO_OLHOS 688
#define PONTO_BRACO_ESQUERDO 15501
#define PONTO_BRACO_ESQUERDO_AUX 18338 //18238
//#define PONTO_BRACO_ESQUEDO_AUX 688

#define PONTO_LUVA_ESQUERDA 19704
#define PONTO_LUVA_DIREITA 23249


class Player {
protected:
    GLfloat gX_init;        // Posição X inicial do player
    GLfloat gY_init;        // Posição Y inicial do player
    GLfloat gZ_init;        // Posição Z inicial do player
    GLfloat direction_init; // Direção inicial do player
    GLfloat gX;             // Posição X atual do player
    GLfloat gY;             // Posição Y atual do player
    GLfloat gZ;             // Posição Z atual do player
    GLfloat direction;      // Direção atual do player
//    GLfloat paddleHeight;   // Comprimento da parte que compõe o braço
//    GLfloat paddleWidth;    // Largura da parte que compõe o braço
//    GLfloat gThetaL1;       // Ângulo do ombro do braço esquerdo
//    GLfloat gThetaL2;       // Ângulo do cotovelo do braço esquerdo
//    GLfloat gThetaR1;       // Ângulo do ombro do braço direito
//    GLfloat gThetaR2;       // Ângulo do cotovelo do braço direito
    GLfloat radius;         // Raio do círculo que forma o corpo do player
    GLfloat radius_glove;   // Raio do círculo que forma a luva do player
    GLfloat radius_max;     // Raio do círculo imaginário ao redor do player
    GLfloat radius_head;    // Raio do círculo imaginário ao redor da cabeça do player
    bool socando;            // True: player no estado socando; False: player não está no estado socando
    bool socoDisponivel;     // True: soco disponível para pontuar; False: soco não está disponível para pontuar
    bool atingindo;          // True: luva do player atingindo o outro player
    int score;              // Pontuação do player
    int role;               // 0: Jogador controlado por pessoa; 1: Jogador controlado pelo computador
    mesh* mesh_walk;
    mesh* mesh_punch;
    int alpha_walk;
    int alpha_punch;
    int walk_interval_w;
    int walk_interval_s;
    float scale;
    int texture;

    enum LadoBraco {
        BRACO_ESQUEDO,
        BRACO_DIREITO
    };

    void DesenhaRect( GLfloat height, GLfloat width, GLfloat R, GLfloat G, GLfloat B);
    void DesenhaCirc( GLfloat radius, GLfloat R, GLfloat G, GLfloat B);
    void DesenhaNariz( GLfloat x, GLfloat y, GLfloat R, GLfloat G, GLfloat B);
    void DesenhaBraco( GLfloat x, GLfloat y, GLfloat paddleVector, GLfloat theta1, GLfloat theta2, GLfloat R, GLfloat G, GLfloat B);
    void DesenhaPlayer( GLfloat x, GLfloat z, GLfloat direction, int role, GLuint texture, int debug);
    void DesenhaCircImginario( GLint radius, GLfloat R, GLfloat G, GLfloat B);
    void MexeBracoEsquerdo(GLfloat dx, GLfloat width, Player &otherPlayer);
    void MexeBracoDireito(GLfloat dx, GLfloat width, Player &otherPlayer);
    void RetornaBracos();
    int VerificaSoco(Player &otherPlayer, LadoBraco ladoBraco);
    void GetLuva(LadoBraco ladoBraco, GLfloat &x, GLfloat &y, GLfloat &z);
    void GetCabeca(GLfloat &x, GLfloat &y, GLfloat &z);
    void RotatePoint(GLfloat x, GLfloat y, GLfloat angle, GLfloat &xOut, GLfloat &yOut);

    void DrawPlayerAxes();


public:
    Player(){
        gX_init = 0;
        gY_init = 0;
        gZ_init = 0;
        direction_init = 0;
        gX = 0;
        gY = 0;
        gZ = 0;
        direction = 0;
//        paddleHeight = 0;
//        paddleWidth = 0;
//        gThetaL1 = GTHETAL1_MIN;     //base: 50;    máx: -60
//        gThetaL2 = GTHETAL2_MIN;     //base: -140;  máx: -70
//        gThetaR1 = GTHETAR1_MIN;     //base: -50;   máx: 60
//        gThetaR2 = GTHETAR2_MIN;     //base: 140;   máx: 70
        radius = 0;
        radius_glove = 0;
        radius_max = 0;
        radius_head = 0;
        socando = false;
        socoDisponivel = true;
        atingindo = false;
        score = 0;
        role = 0;
        mesh_walk = NULL;
        mesh_punch = NULL;
        alpha_walk = 0;
        alpha_punch = 0;
        walk_interval_w = 0;
        walk_interval_s = 0;
        scale = 1;
        texture = 0;
    };

    void Desenha(GLuint texture, int debug){
        DesenhaPlayer(gX, gZ, direction, role, texture, debug);
    };
    void ViraPlayer(GLfloat inc);
    int Soca(GLfloat dx, GLfloat width, Player &otherPlayer, int reset);
    void MoveXZ(GLfloat dist, GLfloat arenaWidth, GLfloat arenaHeight, Player &otherPlayer);
    void IncrementaScore();
    void ReiniciaPlayer();
    GLfloat DistanciaOutroPlayer(Player &otherPlayer);
    GLfloat getMeshX(int vertice) const;
    GLfloat getMeshY(int vertice) const;
    GLfloat getMeshZ(int vertice) const;
    void getCenterPoint(int vertice, GLfloat &xOut, GLfloat &yOut, GLfloat &zOut);

    GLfloat getGx() const;

    GLfloat getGy() const;

    GLfloat getGz() const;

    GLfloat getRadiusMax() const;

    GLfloat getDirection() const;

    GLfloat getRadius() const;

    bool getSocando() const;

    bool getAtingindo() const;

    int getScore() const;

    GLfloat getAltura() const;

    void setGx(GLfloat gX);

    GLfloat getRadiusHead() const;

    void setRadiusHead(GLfloat radiusHead);

    void setGy(GLfloat gY);

    int getAlphaWalk() const;

    int getAlphaPunch() const;

    int getTexture() const;

    void setTexture(GLuint texture);

    void setGzInit(GLfloat gZInit);

    void setGz(GLfloat gZ);

    void setRadius(GLfloat radius);

    void setRadiusMax(GLfloat radiusMax);

    void setDirection(GLfloat direction);

    int getWalkIntervalW() const;

    void setWalkIntervalW(int walkIntervalW);

    int getWalkIntervalS() const;

    void setWalkIntervalS(int walkIntervalS);

    void setRadiusGlove(GLfloat radiusGlove);

    void setPaddleHeight(GLfloat paddleHeight);

    void setPaddleWidth(GLfloat paddleWidth);

    void setSocando(bool socando);

    void setAtingindo(bool atingindo);

    float getScale() const;

    void setScale(float scale);

    void setScore(int score);

    void setGxInit(GLfloat gXInit);

    void setGyInit(GLfloat gYInit);

    void setDirectionInit(GLfloat directionInit);

    void setRole(int role);

    void setMeshWalk(mesh *meshWalk);

    void setMeshPunch(mesh *meshPunch);
};

class AutomaticPlayer : public Player {
    GLfloat mouseDistance;
    int mouseDirection;
    bool enableMoviment;

public:
    AutomaticPlayer() {
        mouseDistance = 0;
        mouseDirection = 1;
        enableMoviment = false;
    };

    void MovimentacaoAutomatica(GLfloat dist, GLfloat width, GLfloat height, Player &otherPlayer);

    bool getEnableMoviment() const;

    void setMouseDistance(GLfloat mouseDistance);

    void setMouseDirection(int mouseDirection);

    void setEnableMoviment(bool enableMoviment);
};