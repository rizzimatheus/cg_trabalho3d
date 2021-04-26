#include "player.h"
#include <cmath>
#include <cstdio>

void Player::DrawPlayerAxes()
{
    double size = scale*mesh_walk[0].vertsPos[PONTO_TOPO].y;

    GLfloat mat_ambient_r[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient_g[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat mat_ambient_b[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
                 no_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_mat);

    //x axis
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_r);
    glColor3fv(mat_ambient_r);
    glScalef(size, size*0.01, size*0.01);
    glTranslatef(0.5, 0, 0); // put in one end
    glutSolidCube(1.0);
    glPopMatrix();

    //y axis
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_g);
    glColor3fv(mat_ambient_g);
    glRotatef(90,0,0,1);
    glScalef(size, size*0.01, size*0.01);
    glTranslatef(0.5, 0, 0); // put in one end
    glutSolidCube(1.0);
    glPopMatrix();

    //z axis
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_b);
    glColor3fv(mat_ambient_b);
    glRotatef(-90,0,1,0);
    glScalef(size, size*0.01, size*0.01);
    glTranslatef(0.5, 0, 0); // put in one end
    glutSolidCube(1.0);
    glPopMatrix();

}

//void Player::DesenhaRect(GLfloat height, GLfloat width, GLfloat R, GLfloat G, GLfloat B)
//{
//    float w_half = width/2;
//
//    glBegin(GL_POLYGON);
//    glColor3f(R, G, B);
//    glVertex3f(-w_half, 0, 0);
//    glVertex3f(w_half,0, 0);
//    glVertex3f(w_half, height, 0);
//    glVertex3f(-w_half, height, 0);
//    glEnd();
//}
//
//void Player::DesenhaBraco(GLfloat x, GLfloat y, GLfloat paddleVector, GLfloat theta1, GLfloat theta2, GLfloat R, GLfloat G, GLfloat B)
//{
//    glPushMatrix();
//
//    // Primeira haste
//    glTranslatef(x, y, 0);
//    glRotatef(theta1, 0, 0, 1);
//    DesenhaRect(paddleVector, paddleWidth, R, G, B);
//    // Segunda haste
//    glTranslatef(0, paddleVector*BRACO_DESCOLAMENTO2, 0);
//    glRotatef(theta2, 0, 0, 1);
//    DesenhaRect(paddleVector, paddleWidth, R, G, B);
//    // Luva
//    glTranslatef(0, paddleVector, 0);
//    glutSolidSphere(radius_glove, 20, 10);
////    DesenhaCirc(radius_glove,R*0.5,G*0.5,B*0.5);
////    DesenhaCirc(radius_glove*0.9, R, G, B);
//
//    glPopMatrix();
//}
//
//void Player::DesenhaCirc(GLfloat radius, GLfloat R, GLfloat G, GLfloat B)
//{
//    int sides = 30;
//    glColor3f(R, G, B);
//    glPointSize(2.0f);
//    glBegin(GL_POLYGON);
//    for (int a = 0; a < 360; a += 360 / sides)
//    {
//        double heading = a * MY_PI / 180;
//        glVertex3f(cos(heading) * radius, sin(heading) * radius, 0);
//    }
//    glEnd();
//}
//
//void Player::DesenhaCircImginario(GLint radius, GLfloat R, GLfloat G, GLfloat B)
//{
//    int sides = 36;
//    glColor3f(R, G, B);
//    glPointSize(2.0f);
//    glBegin(GL_POINTS);
//    for (int a = 0; a < 360; a += 360 / sides)
//    {
//        double heading = a * MY_PI / 180;
//        glVertex3f(cos(heading) * radius, sin(heading) * radius, 0);
//    }
//    glEnd();
//}
//
//// x e y do player
//void Player::DesenhaNariz(GLfloat x, GLfloat y, GLfloat R, GLfloat G, GLfloat B)
//{
//    glPushMatrix();
//
//    glTranslatef(x, y, 0);
//    glutSolidSphere(radius*0.2, 20, 10);
////    DesenhaCirc(radius*0.25, R*0.5, G*0.5, B*0.5);
////    DesenhaCirc(radius*0.2, R, G, B);
//
//    glPopMatrix();
//}

void Player::DesenhaPlayer(GLfloat x, GLfloat z, GLfloat direction,  int role, GLuint texture, int debug)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    if (debug != 0 and role == 0) {
        DrawPlayerAxes();
    }

    glTranslatef(x, 0, z);
    glRotatef(direction, 0.0, 1.0, 0.0);

    glPushMatrix();
    glScalef(scale, scale, scale);

    glColor3f(0,1,0);

    if (socando) {
        mesh_punch[alpha_punch].draw(texture);
        alpha_walk = 0;
    } else {
        mesh_walk[alpha_walk].draw(texture);
        alpha_punch = 0;
    }

    glPopMatrix();

    Material verde(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f);
    Material vermelho(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f);

    if (role) {
        vermelho.ativar();
    } else {
        verde.ativar();
    }

    if (debug == 2) {
        glPushMatrix();
        // cilindro
        GLUquadricObj *cylindro = gluNewQuadric();
        glRotatef(-90, 1, 0, 0);
        gluCylinder(cylindro, radius, radius, getMeshY(PONTO_TOPO), 20, 20);
        glPopMatrix();
    } else if (debug == 3) {
        //esfera cabeça
        glPushMatrix();
        glTranslatef(getMeshX(PONTO_TOPO),
                     getMeshY(PONTO_TOPO)*.9,
                     getMeshZ(PONTO_TOPO)
        );
        glutSolidSphere(radius_head, 20, 20);
        glPopMatrix();

        //esfera luva esqueda
        glPushMatrix();
        glTranslatef(getMeshX(PONTO_LUVA_ESQUERDA),
                     getMeshY(PONTO_LUVA_ESQUERDA),
                     getMeshZ(PONTO_LUVA_ESQUERDA)*.97
        );
        glutSolidSphere(radius_glove, 20, 20);
        glPopMatrix();

        //esfera luva direita
        glPushMatrix();
        glTranslatef(getMeshX(PONTO_LUVA_DIREITA),
                     getMeshY(PONTO_LUVA_DIREITA),
                     getMeshZ(PONTO_LUVA_DIREITA)*.97
        );
        glutSolidSphere(radius_glove, 20, 20);
        glPopMatrix();
    } else if (debug == 4) {
        //esfera cabeça
        glPushMatrix();
        glTranslatef(getMeshX(PONTO_TOPO),
                     getMeshY(PONTO_TOPO)*.9,
                     getMeshZ(PONTO_TOPO)
                     );
        glutWireSphere(radius_head, 20, 20);
        glPopMatrix();

        //esfera luva esqueda
        glPushMatrix();
        glTranslatef(getMeshX(PONTO_LUVA_ESQUERDA),
                     getMeshY(PONTO_LUVA_ESQUERDA),
                     getMeshZ(PONTO_LUVA_ESQUERDA)*.97
        );
        glutWireSphere(radius_glove, 20, 20);
        glPopMatrix();

        //esfera luva direita
        glPushMatrix();
        glTranslatef(getMeshX(PONTO_LUVA_DIREITA),
                     getMeshY(PONTO_LUVA_DIREITA),
                     getMeshZ(PONTO_LUVA_DIREITA)*.97
        );
        glutWireSphere(radius_glove, 20, 20);
        glPopMatrix();
    }

    if (debug != 0) {
        // Desenha círculo (bolacha) no pé do lutador
        glPushMatrix();
        glTranslatef(0, 0.5, 0);
        int sides = 30;
        glColor3f(1, 0, 0);
        glPointSize(2.0f);
        glBegin(GL_POLYGON);
        for (int a = 0; a < 360; a += 360 / sides) {
            double heading = a * MY_PI / 180;
            glVertex3f(cos(heading) * radius, 0, sin(heading) * radius);
            glNormal3f(0, 1, 0);
        }
        glEnd();
        glPopMatrix();

        DrawPlayerAxes();
    }

    glPopMatrix();
}

void Player::ViraPlayer(GLfloat inc)
{
    direction += inc;
    if (direction > 360) {
        direction -= 360;
    } else if (direction < 0) {
        direction += 360;
    }
}

void Player::MoveXZ(GLfloat dist, GLfloat arenaWidth, GLfloat arenaHeight, Player &otherPlayer)
{
    if (dist > 0) {
        if (alpha_walk == MESH_WALK_SIZE) {
            alpha_walk = 0;
        } else {
            alpha_walk++;
        }
    } else if (dist < 0) {
        if (alpha_walk == 0) {
            alpha_walk = MESH_WALK_SIZE;
        } else {
            alpha_walk--;
        }
    }

    GLfloat new_gx = gX + sin(direction*MY_PI/180)*dist; // sin(angle) = CO/H => CO = sin(angle)*H
    GLfloat new_gz = gZ + cos(direction*MY_PI/180)*dist; // cos(angle) = CA/H => CA = cos(angle)*H

    // Verifica colisão entre players
    GLfloat new_distance = sqrt(pow(new_gx - otherPlayer.getGx(),2)+pow(new_gz - otherPlayer.getGz(),2));
    if (new_distance >= (getRadius() + otherPlayer.getRadiusMax()) and new_distance >= (otherPlayer.getRadius() + getRadiusMax())) {
        // Verifica colisão com bordas laterais da arena
        if ((new_gx - radius) >= -arenaWidth/2 and (new_gx + radius) <= arenaWidth/2) {
            gX = new_gx;
        }
        // Verifica colisão com bordas superior e inferior da arena
        if ((new_gz - radius) >= -arenaHeight/2 and (new_gz + radius) <= arenaHeight/2) {
            gZ = new_gz;
        }
    }

//    printf("Posição Player1 = (%.2f, %.2f)\n", gX, gZ);
}

int Player::Soca(GLfloat dx, GLfloat width, Player &otherPlayer, int reset)
{
    // Manipula a animação do soco
    GLfloat interval = width/(2*12);
    if (abs(dx) > 0 && abs(dx) < interval){
        alpha_punch = 1;
    } else if (abs(dx) >= interval && abs(dx) < 2*interval) {
        alpha_punch = 2;
    } else if (abs(dx) >= 2*interval && abs(dx) < 3*interval) {
        alpha_punch = 3;
    } else if (abs(dx) >= 3*interval && abs(dx) < 4*interval) {
        alpha_punch = 4;
    } else if (abs(dx) >= 4*interval && abs(dx) < 5*interval) {
        alpha_punch = 5;
    } else if (abs(dx) >= 5*interval && abs(dx) < 6*interval) {
        alpha_punch = 6;
    } else if (abs(dx) >= 6*interval && abs(dx) < 7*interval) {
        alpha_punch = 7;
    } else if (abs(dx) >= 7*interval && abs(dx) < 8*interval) {
        alpha_punch = 8;
    } else if (abs(dx) >= 8*interval && abs(dx) < 9*interval) {
        alpha_punch = 9;
    } else if (abs(dx) >= 9*interval && abs(dx) < 10*interval) {
        alpha_punch = 10;
    } else if (abs(dx) >= 10*interval && abs(dx) < 11*interval) {
        alpha_punch = 11;
    } else if (abs(dx) >= 11*interval) {
        alpha_punch = 12;
    } else {
        alpha_punch = 0;
    }
    if (dx < 0) {
        alpha_punch += 13;
    }


    // Faz com que o player tenha que voltar com o braço após fazer ponto para pontuar novamente
    if ( (alpha_punch < 6 or alpha_punch > 19) or (role == 1) ) {
        socoDisponivel = true;
    }
    if (reset) {
//        RetornaBracos();
        setAtingindo(false);
        setSocando(false); // Precisa disso? Tem que soltar o mouse e pressionar novamente para socar
        socoDisponivel = true;
    } else if (dx > 0) {
//        MexeBracoEsquerdo(dx, width, otherPlayer);
        return VerificaSoco(otherPlayer, BRACO_ESQUEDO);
    } else if (dx < 0) {
//        MexeBracoDireito(dx, width, otherPlayer);
        return VerificaSoco(otherPlayer, BRACO_DIREITO);
    }
    return 0;
}

//void Player::MexeBracoEsquerdo(GLfloat dx, GLfloat width, Player &otherPlayer)
//{
//    width = width/2;
//    if (dx <= width) {
//        gThetaL1 = (dx - 0) / (width - 0) * (GTHETAL1_MAX - GTHETAL1_MIN) + GTHETAL1_MIN;
//        gThetaL2 = (dx - 0) / (width - 0) * (GTHETAL2_MAX - GTHETAL2_MIN) + GTHETAL2_MIN;
//    }
//}

//void Player::MexeBracoDireito(GLfloat dx, GLfloat width, Player &otherPlayer)
//{
//    dx = -dx;
//    width = width/2;
//    if (dx <= width) {
//        gThetaR1 = (dx - 0) / (width - 0) * (GTHETAR1_MAX - GTHETAR1_MIN) + GTHETAR1_MIN;
//        gThetaR2 = (dx - 0) / (width - 0) * (GTHETAR2_MAX - GTHETAR2_MIN) + GTHETAR2_MIN;
//    }
//}

//void Player::RetornaBracos()
//{
//    gThetaL1 = GTHETAL1_MIN;
//    gThetaL2 = GTHETAL2_MIN;
//    gThetaR1 = GTHETAR1_MIN;
//    gThetaR2 = GTHETAR2_MIN;
//}

int Player::VerificaSoco(Player &otherPlayer, LadoBraco ladoBraco)
{
    GLfloat x = 0;
    GLfloat y = 0;
    GLfloat z = 0;
    GetLuva(ladoBraco, x, y, z);

    GLfloat other_player_x = 0;
    GLfloat other_player_y = 0;
    GLfloat other_player_z = 0;
    otherPlayer.GetCabeca(other_player_x, other_player_y, other_player_z);

//    printf("PLAYER1 LUVA: (x,y,z) (%.2f,%.2f,%.2f)\n", x,y,z);
//    printf("PLAYER2 CABEÇA: (x,y,z) (%.2f,%.2f,%.2f)\n\n", other_player_x,other_player_y,other_player_z);

    GLfloat distCabecaLuva = sqrt(pow(x - other_player_x,2)+pow(y - other_player_y,2)+pow(z - other_player_z,2));

    if (socoDisponivel and distCabecaLuva <= (otherPlayer.getRadiusHead() + radius_glove)) {
        socoDisponivel = false;
        return 1;
    }

    return 0;
}

void Player::GetLuva(LadoBraco ladoBraco, GLfloat &x, GLfloat &y, GLfloat &z)
{
    if (ladoBraco == BRACO_DIREITO) { // Braço direito
        x = gX+getMeshX(PONTO_LUVA_DIREITA);
        y = gY+getMeshY(PONTO_LUVA_DIREITA);
        z = gZ+getMeshZ(PONTO_LUVA_DIREITA);
    } else { // Braço esquerdo
        x = gX+getMeshX(PONTO_LUVA_ESQUERDA);
        y = gY+getMeshY(PONTO_LUVA_ESQUERDA);
        z = gZ+getMeshZ(PONTO_LUVA_ESQUERDA);
    }
    RotatePoint(gX, gZ, (direction)*(MY_PI/180), x, z);
}

void Player::GetCabeca(GLfloat &x, GLfloat &y, GLfloat &z)
{
    x = gX+getMeshX(PONTO_TOPO);
    y = gY+getMeshY(PONTO_TOPO)*0.9;
    z = gZ+getMeshZ(PONTO_TOPO);

    RotatePoint(gX, gZ, (direction)*(MY_PI/180), x, z);
}

//Funcao auxiliar de rotacao
void Player::RotatePoint(GLfloat x, GLfloat z, GLfloat angle, GLfloat &xOut, GLfloat &zOut)
{
    // Translada o ponto de volta para a origem
    xOut -= x;
    zOut -= z;

    // Rotaciona o ponto
    GLfloat pz = zOut*cos(angle) - xOut*sin(angle);
    GLfloat px = zOut*sin(angle) + xOut*cos(angle);


    // Translada o ponto de volta
    xOut = px + x;
    zOut = pz + z;
}

void Player::getCenterPoint(int vertice, GLfloat &xOut, GLfloat &yOut, GLfloat &zOut)
{
    xOut = gX+getMeshX(vertice);
    yOut = gY+getMeshY(vertice);
    zOut = gZ+getMeshZ(vertice);

    RotatePoint(gX, gZ, (direction)*(MY_PI/180), xOut, zOut);
}

void Player::IncrementaScore()
{
    Player::score++;
}

void Player::ReiniciaPlayer()
{
    gX = gX_init;
    gY = gY_init;
    gZ = gZ_init;
    score = 0;
    direction = direction_init;
    socando = false;
    atingindo = false;
}

void AutomaticPlayer::MovimentacaoAutomatica(GLfloat dist, GLfloat width, GLfloat height, Player &otherPlayer)
{
    // Distância para começar a socar
    GLfloat distanciaMaximaSoco = (getRadiusMax() + otherPlayer.getRadius()) * 1.1;

    // Direção para o outro player
    GLfloat dx = otherPlayer.getGx()-gX;
    GLfloat dz = otherPlayer.getGz()-gZ;
    GLfloat anguloObjetivo = atan2(dx,dz)*180/MY_PI;

    // Deixa o ângulo positivo, se tiver negativo
    if (anguloObjetivo < 0) {
        anguloObjetivo += 360;
    }

    // Verifica se a distância entre os players é suficientemente pequena para socar e seta 'socando' em 1
    // Depois que começa a socar não para até terminar a animação
    // ALTERAR O AND PELO OR
    if (getSocando() or
        (DistanciaOutroPlayer(otherPlayer) < distanciaMaximaSoco and
        abs(getDirection() - anguloObjetivo) < 10)) {

        setSocando(true);

        // Incrementa/Decrementa a distância simulada do clique e posição do mouse
        // Alterna entre Incremento/Decremento quando chega no máximo da distância
        if (mouseDirection > 0 and mouseDistance <= 500 / 2) {
            //mouseDistance += ViewingWidth*0.05;
            mouseDistance += dist*4;
//            printf("MOUSE DISTANCE = %f; dist*10 = %f\n", mouseDistance, dist*10);
        } else if (mouseDirection < 0 and mouseDistance >= -500 / 2) {
            //mouseDistance -= ViewingWidth*0.05;
            mouseDistance -= dist*4;
//            printf("MOUSE DISTANCE = %f; dist*10 = %f\n", mouseDistance, dist*10);
        } else {
            mouseDirection *= -1;
        }

        // Se tiver atingido o outro player com o soco
        if (Soca(mouseDistance, 500, otherPlayer, 0)) {
            if (!getAtingindo()) {
                setAtingindo(true);
                IncrementaScore();
                printf("Player 1 atingido: %d\n", getScore());
            }
        } else {
            setAtingindo(false);
        }
        // Libera o player para andar, se necessário, após fazer toda a animação do soco
        if (abs(mouseDistance) <= 30) {
            setSocando(false);
        }
    } else {
        mouseDistance = 0;
        Soca(mouseDistance, 500, otherPlayer, 1);

        // Evita ficar mudando o ângulo constantemente
        if (abs(anguloObjetivo - direction) > dist/2) {
            // Verifica se o ângulo atual deve ser incrementado ou decrementado para atingir o ângulo objtivo
            // Se o ãngulo objetivo for maior que o ângulo atual, então incrementa o ângulo atual
            // Se o ãngulo objetivo for menor que o ângulo atual, então decrementa o ângulo atual
            // Mas antes verifica se fazer a operação inversa não é mais rápido
            // Exemplo: Ângulo Objetivo = 5°; Ângulo atual = 350°.
            // Como o ângulo objetivo < ângulo atual, deveria decrementar, mas incrementar chega no ângulo objetivo mais rápido
            if ((360 - direction) + anguloObjetivo < abs(direction - anguloObjetivo)) {
                ViraPlayer(dist);
            } else if ((360 - anguloObjetivo) + direction < abs(direction - anguloObjetivo)) {
                ViraPlayer(-dist);
            } else if (anguloObjetivo > direction) {
                ViraPlayer(dist);
            } else if (anguloObjetivo < direction) {
                ViraPlayer(-dist);
            }
        }
        MoveXZ(dist, width, height, otherPlayer);
    }
}

GLfloat Player::DistanciaOutroPlayer(Player &otherPlayer)
{
    return sqrt(pow(gX - otherPlayer.getGx(), 2) + pow(gZ - otherPlayer.getGz(), 2));
}

bool AutomaticPlayer::getEnableMoviment() const {
    return enableMoviment;
}

void AutomaticPlayer::setEnableMoviment(bool enableMoviment) {
    AutomaticPlayer::enableMoviment = enableMoviment;
}

void AutomaticPlayer::setMouseDistance(GLfloat mouseDistance) {
    AutomaticPlayer::mouseDistance = mouseDistance;
}

void AutomaticPlayer::setMouseDirection(int mouseDirection) {
    AutomaticPlayer::mouseDirection = mouseDirection;
}

GLfloat Player::getDirection() const {
    return direction;
}

GLfloat Player::getGx() const {
    return gX;
}

GLfloat Player::getGy() const {
    return gY;
}

void Player::setRadiusMax(GLfloat radiusMax) {
    radius_max = radiusMax;
}

GLfloat Player::getRadiusMax() const {
    return radius_max;
}

GLfloat Player::getRadius() const {
    return radius;
}

void Player::setRadiusGlove(GLfloat radiusGlove) {
    radius_glove = radiusGlove;
}

//void Player::setPaddleHeight(GLfloat paddleHeight) {
//    Player::paddleHeight = paddleHeight;
//}

//void Player::setPaddleWidth(GLfloat paddleWidth) {
//    Player::paddleWidth = paddleWidth;
//}

void Player::setSocando(bool socando) {
    Player::socando = socando;
}

void Player::setAtingindo(bool atingindo) {
    Player::atingindo = atingindo;
}

bool Player::getSocando() const {
    return socando;
}

bool Player::getAtingindo() const {
    return atingindo;
}

int Player::getScore() const {
    return score;
}

void Player::setScore(int score) {
    Player::score = score;
}

void Player::setGxInit(GLfloat gXInit) {
    gX_init = gXInit;
    gX = gXInit;
}

void Player::setGyInit(GLfloat gYInit) {
    gY_init = gYInit;
    gY = gYInit;
}

void Player::setDirectionInit(GLfloat directionInit) {
    direction_init = directionInit;
    direction = directionInit;
}

void Player::setRole(int role) {
    Player::role = role;
}

void Player::setGx(GLfloat gX) {
    Player::gX = gX;
}

void Player::setGy(GLfloat gY) {
    Player::gY = gY;
}

void Player::setRadius(GLfloat radius) {
    Player::radius = radius;
}

void Player::setDirection(GLfloat direction) {
    Player::direction = direction;
}

void Player::setGzInit(GLfloat gZInit) {
    gZ_init = gZInit;
    gZ = gZInit;
}

void Player::setGz(GLfloat gZ) {
    Player::gZ = gZ;
}

GLfloat Player::getGz() const {
    return gZ;
}

void Player::setMeshWalk(mesh *meshWalk) {
    mesh_walk = meshWalk;
}

void Player::setMeshPunch(mesh *meshPunch) {
    mesh_punch = meshPunch;
}

int Player::getWalkIntervalW() const {
    return walk_interval_w;
}

void Player::setWalkIntervalW(int walkIntervalW) {
    walk_interval_w = walkIntervalW;
}

int Player::getWalkIntervalS() const {
    return walk_interval_s;
}

void Player::setWalkIntervalS(int walkIntervalS) {
    walk_interval_s = walkIntervalS;
}

int Player::getAlphaWalk() const {
    return alpha_walk;
}

int Player::getAlphaPunch() const {
    return alpha_punch;
}

float Player::getScale() const {
    return scale;
}

void Player::setScale(float scale) {
    Player::scale = scale;
}

GLfloat Player::getMeshX(int vertice) const {
    if (socando) {
        return scale*mesh_punch[alpha_punch].vertsPos[vertice].x;
    } else {
        return scale*mesh_walk[alpha_walk].vertsPos[vertice].x;
    }
}

GLfloat Player::getMeshY(int vertice) const {
    if (socando) {
        return scale*mesh_punch[alpha_punch].vertsPos[vertice].y;
    } else {
        return scale*mesh_walk[alpha_walk].vertsPos[vertice].y;
    }
}

GLfloat Player::getMeshZ(int vertice) const {
    if (socando) {
        return scale*mesh_punch[alpha_punch].vertsPos[vertice].z;
    } else {
        return scale*mesh_walk[alpha_walk].vertsPos[vertice].z;
    }
}

GLfloat Player::getRadiusHead() const {
    return radius_head;
}

void Player::setRadiusHead(GLfloat radiusHead) {
    radius_head = radiusHead;
}

void Player::setTexture(GLuint texture) {
    Player::texture = texture;
}

int Player::getTexture() const {
    return texture;
}

