#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>
#include <GL/glut.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include<windows.h>
#include <cstdlib>
#include <iostream>
#include <cmath>
using namespace std;

/*Known Bugs
1. input should happen instantly and not wait for the next key press, otherwise not a big deal
*/
/*Quality of Life
Textures
Simple/Dumb AI

*/

//----Structure Zone----
typedef struct {
    bool corrected;
    bool occupied;
    bool cursor;
    int height;
    int pieceTeam;
    int positionValue;
    bool target;
    bool source;
}board;


typedef struct {
    int team;
    int pieceCount;
}player;

//----End of ConSTRUCTion Zone----


//----Function Decalarations----

//----Game Logic----
void DrawTextBoard();
void BoardHeightGenerator();
void BoardCorrection();
void CaseZeroCorrection(int, int, bool, bool, bool, bool);
void CaseOneCorrection(int, int, bool, bool, bool, bool);
void CaseTwoCorrection(int, int, bool, bool, bool, bool);
void CaseThreeCorrection(int, int, bool, bool, bool, bool);
void CaseFourCorrection(int, int, bool, bool, bool, bool);
void IntPiecePlacement();
void TeamDecider();
void setValue();

double MoveRules(int, int, int, double);
bool AttackRules(int);
void MovePiece(int, int, int, int);
void specialAttack(int, int);
void pieceSelectOld();
void pieceSelectNew();

char getAns(char[]);

//----Graphics----
void display();
int initGL(GLvoid);
int DrawGLScene(GLvoid);
void drawSquare();
void drawCursorSquare();
void drawBoard();
void drawGround();
void drawBlackPiece(int ,int);
void drawWhitePiece(int, int);
void drawCursor();
void processKeyboardOld(unsigned char, int, int);
void processKeyboardNew(unsigned char, int, int);
void processKeyboardAttack(unsigned char, int, int);
void processSpecialKeys(int, int, int);
void reshape(GLsizei, GLsizei);
void timer(int);

//----AI----
void dumbAI();
void AIInitCursorPos();
void pieceSelectOldAI();
void pieceSelectNewAI();
void moveCursorAI();
void processAttackAI();
void cursorMoveAI(int);
void getTargetPiece();
void getSourcePiece();
int getPlayerPosition();
int getComputerPosition();
void specialAttackAI(int, int);


//----Core----
void initializeGame();
void graphics(int, char**);

//----End of Function Declarations----


//----Global Variable----

//----Game Logic----
board boardTile[7][7];
player playerTeam;
player enemyTeam;
bool attackPos;
bool moveRules;
bool skipTurn;
int cursorPosY = 1;
int cursorPosX = 1;
int oldX, oldY, newX, newY;
int turnTimer = 1;
int movPts = 5;

//----Graphics----
int refreshMills = 15;                                               //refresh interval in millis
float angle = 0.0;
float camLookAtX = 0.0f, camLookAtY = 0.0, camLookAtZ = 0.0f;       //set origin point we rotate around
float camPosX = 0.0f, camPosY = 10.0, camPosZ = -12.0f;             //XYZ pos of camera

#define PI 3.14159265

//----AI----
int targetX, targetY;
int sourceX, sourceY;
int dirAI;
bool AI;



//----End of Global Variables----
///////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    
    initializeGame(); 
    graphics(argc, argv);

    return 0;
}


void initializeGame() {
    srand(time(NULL)); //random seed

    printf("Generating Board\n");
    BoardHeightGenerator();
    printf("Placing Pieces\n");
    IntPiecePlacement();
    printf("Correcting Board\n");
    BoardCorrection();
    printf("Deciding Teams\n");
    TeamDecider();
    printf("Setting Values\n");
    setValue();
    printf("Setting Cursor\n");
    boardTile[cursorPosY][cursorPosX].cursor = true;
    printf("Singleplayer or Two Players? 1 OR 2\n");
    char ch = _getch();
    ch = tolower(ch);
    switch (ch)
    {
    case '1':
        printf("Singleplayer mode\n");
        AI = true;
        break;

    case '2':
        printf("Two Player mode\n");
        AI = false;
        break;
    }

    printf("HERE WE GOOOOOOOOOO\n");
}

void graphics(int argc, char** argv) {

    glutInit(&argc, argv); //init GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //enable double-buffered mode
    glutInitWindowSize(640, 480);     //set window's init height and width
    glutInitWindowPosition(50, 50);   //Window's Position - Top Left
    glutCreateWindow("Senior Project");          //Window is created with title
    glutDisplayFunc(display);         //Register callback handler for window re-paint event
    glutReshapeFunc(reshape);         //register callback handler for window re-size event
    glutKeyboardFunc(processKeyboardOld);
    glutSpecialFunc(processSpecialKeys);
    initGL();                         //our own OpenGl init  
    glutTimerFunc(0, timer, 0);       //First timer call immediately
    glutMainLoop();                   //infin event-processing loop

}

//Graphic Initialization~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int initGL(GLvoid)
{
    glShadeModel(GL_SMOOTH);              //Enable smooth shading
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black & opaque background
    glClearDepth(1.0f);                   //Set background depth to farthest
    glEnable(GL_DEPTH_TEST);              //Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);               //Set the type of depth-test
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); //Nice perspective corrections

    return TRUE;                    //Initialization went ok
}

void display() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                   //reset model-view matrix
    gluPerspective(40.0, 1.0, 1.0, 10000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    DrawGLScene();

    glutSwapBuffers();
}

int DrawGLScene(GLvoid) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color and depth buffers
    glLoadIdentity();

   
    gluLookAt(camPosX, camPosY, camPosZ,
        camLookAtX, camLookAtY, camLookAtZ,
        0.0f, 1.0f, 0.0f);
    
    drawGround();
    drawBoard();
   //glutSwapBuffers();

    return TRUE;
}

void drawGround() {
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, 0.0f, -100.0f);
    glVertex3f(-100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, -100.0f);
    glEnd();
}

void drawBoard(){
    for (int y = -3; y != 4; y++) {
        for (int x = -3; x != 4; x++) {
            int height = boardTile[y+3][x+3].height + 1;
            glPushMatrix();
            glTranslatef(y * 1.1, 0, x * 1.1);
            glScalef(1.0, height, 1.0);
            if (!boardTile[y+3][x+3].cursor)
            {
                drawSquare();
            }
            else if (boardTile[y+3][x+3].cursor) {
                drawCursorSquare();
            }
            glPopMatrix();
            glPushMatrix();
            glTranslatef(y * 1.1, 0.0, x * 1.1);
            if (boardTile[y + 3][x + 3].pieceTeam == 1 && boardTile[y+3][x+3].occupied)
            {
                drawBlackPiece(y, x);
            }
            else if (boardTile[y + 3][x + 3].pieceTeam == 0 && boardTile[y + 3][x + 3].occupied)
            {
                drawWhitePiece(y, x);
            }
            glPopMatrix();

            glPushMatrix();
            glTranslatef(y * 1.1, 0.0, x * 1.1);
            if (boardTile[y + 3][x + 3].cursor)
            {
                drawCursor();
            }
            glPopMatrix();
        }
    }
}

void drawSquare() {
    glBegin(GL_QUADS);                  //Begin drawing the color cube with 6 quads
        //Top face (y = 1.0f)
        //Define vertices in counter-clockwise (CCW) order with normal pointing out
    
    glColor3f(0.0f, 0.5f, 0.5f); //Green
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);

    //Bottom face (y = -1.0f)
    glColor3f(1.0f, 0.5f, 0.0f); //Orange
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    // Front face  (z = 1.0f)
    glColor3f(1.0f, 0.0f, 0.0f);     // Red
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);

    // Back face (z = -1.0f)
    glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    // Left face (x = -1.0f)
    glColor3f(0.0f, 0.0f, 1.0f);     // Blue
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    // Right face (x = 1.0f)
    glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    glEnd();  // End of drawing color-cube
}

void drawCursorSquare() {
    glBegin(GL_QUADS);                  //Begin drawing the color cube with 6 quads
        //Top face (y = 1.0f)
        //Define vertices in counter-clockwise (CCW) order with normal pointing out

    glColor3f(1.0f, 0.5f, 1.5f); //Green
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);

    //Bottom face (y = -1.0f)
    glColor3f(1.0f, 0.5f, 0.0f); //Orange
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    // Front face  (z = 1.0f)
    glColor3f(1.0f, 0.0f, 0.0f);     // Red
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);

    // Back face (z = -1.0f)
    glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    // Left face (x = -1.0f)
    glColor3f(0.0f, 0.0f, 1.0f);     // Blue
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    // Right face (x = 1.0f)
    glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    glEnd();  // End of drawing color-cube
}

void drawBlackPiece(int heightY, int heightX) {
    double radius= .3;
    double pieceHeight = .2;
    float x = 0.0;
    float y = 0.0;
    float angle = 0.0;
    float stepsize = 0.1;

    glColor3f(0.0, 0.0, 0.0);
    if (boardTile[heightY + 3][heightX + 3].height == 0) { glTranslatef(0.0, 0.8, 0.0); }
    else if (boardTile[heightY + 3][heightX + 3].height == 1) { glTranslatef(0.0, 1.2, 0.0); }
    else if (boardTile[heightY + 3][heightX + 3].height == 2) { glTranslatef(0.0, 1.8, 0.0); }
    else if (boardTile[heightY + 3][heightX + 3].height == 3) { glTranslatef(0.0, 2.2, 0.0); }
    else if (boardTile[heightY + 3][heightX + 3].height == 4) { glTranslatef(0.0, 2.8, 0.0); }
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glBegin(GL_QUAD_STRIP);
    angle = 0.0;
    while (angle < 2 * PI) {
        x = radius * cos(angle);
        y = radius * sin(angle);
        glVertex3f(x, y, pieceHeight);
        glVertex3f(x, y, 0.0);
        angle = angle + stepsize;
    }
    glVertex3f(radius, 0.0, pieceHeight);
    glVertex3f(radius, 0.0, 0.0);
    glEnd();

    glColor3f(0.0, 1.0, 0.0);
    glTranslatef(0.0, 0.0, -0.2);
    glBegin(GL_POLYGON);
    angle = 0.0;
    while (angle < 2 * PI) {
        x = radius * cos(angle);
        y = radius * sin(angle);
        glVertex3f(x, y, pieceHeight);
        angle = angle + stepsize;
    }
    glVertex3f(radius, 0.0, pieceHeight);
    glEnd();

}

void drawWhitePiece(int heightY, int heightX) {
    double radius = .3;
    double height = .2;
    float x = 0.0;
    float y = 0.0;
    float angle = 0.0;
    float stepsize = 0.1;

    glColor3f(0.8, 0.8, 0.8);
    if (boardTile[heightY + 3][heightX + 3].height == 0) { glTranslatef(0.0, 0.8, 0.0); }
    else if(boardTile[heightY+3][heightX+3].height == 1) { glTranslatef(0.0, 1.2, 0.0); }
    else if (boardTile[heightY+3][heightX+3].height == 2) { glTranslatef(0.0, 1.8, 0.0); }
    else if (boardTile[heightY+3][heightX+3].height == 3) { glTranslatef(0.0, 2.2, 0.0); }
    else if (boardTile[heightY+3][heightX+3].height == 4) { glTranslatef(0.0, 2.8, 0.0); }
    
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glBegin(GL_QUAD_STRIP);
    angle = 0.0;
    while (angle < 2 * PI) {
        x = radius * cos(angle);
        y = radius * sin(angle);
        glVertex3f(x, y, height);
        glVertex3f(x, y, 0.0);
        angle = angle + stepsize;
    }
    glVertex3f(radius, 0.0, height);
    glVertex3f(radius, 0.0, 0.0);
    glEnd();

    glColor3f(1.0, 0.0, 0.0);
    glTranslatef(0.0, 0.0, -0.2);
    glBegin(GL_POLYGON);
    angle = 0.0;
    while (angle < 2 * PI) {
        x = radius * cos(angle);
        y = radius * sin(angle);
        glVertex3f(x, y, height);
        angle = angle + stepsize;
    }
    glVertex3f(radius, 0.0, height);
    glEnd();
}

void drawCursor() {
    
    double base = .2;
    double height = .75;
    int slices = 50;
    int stacks = 50;

    double cursorHeight = 0.0;

    glColor3f(0.2, 0.0, 0.0);
    glTranslatef(0.0, 4.0, 0.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glutSolidCone(base, height, slices, stacks);
   
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//Main Functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void processKeyboardOld(unsigned char key, int xx, int yy) {

    if (AI) {
        if (turnTimer == 30 || playerTeam.pieceCount == 0 || enemyTeam.pieceCount == 0) {
            printf("GameOver\n");
        }
        if (turnTimer % 2 == 0) {
            printf("Enemy AI Turn - Turn: %d\n", turnTimer);
            dumbAI();
        }
        else if (turnTimer % 2 == 1) {
            printf("Player Turn - Turn: %d\n", turnTimer);
        }
    }

    else if (!AI) {
        if (turnTimer == 30 || playerTeam.pieceCount == 0 || enemyTeam.pieceCount == 0) {
            printf("GameOver\n");
        }
        if (turnTimer % 2 == 0) {
            printf("Enemy Turn - Turn: %d\n", turnTimer);
        }
        else if (turnTimer % 2 == 1) {
            printf("Player Turn - Turn: %d\n", turnTimer);
        }
    }
    

    movPts = 5;
    skipTurn = false;
    //printf("processKeyboardOld\n");

    if (turnTimer % 2 == 1 && AI || !AI) {
        switch (key)
        {
        case'x':
            if (boardTile[cursorPosY][cursorPosX].occupied == false) { printf("Tile is empty\n"); }
            else {
                pieceSelectOld();
            }

            break;
        case 'd':
            if (cursorPosY - 1 >= 0) {
                boardTile[cursorPosY][cursorPosX].cursor = false;
                cursorPosY = cursorPosY - 1;
                boardTile[cursorPosY][cursorPosX].cursor = true;
            }
            else {
                printf("Out of bounds\n");
            }
            break;
        case 's':
            if (cursorPosX - 1 >= 0) {
                boardTile[cursorPosY][cursorPosX].cursor = false;
                cursorPosX = cursorPosX - 1;
                boardTile[cursorPosY][cursorPosX].cursor = true;
            }
            else {
                printf("Out of bounds \n");
            }
            break;
        case 'a':
            if (cursorPosY + 1 <= 6) {
                boardTile[cursorPosY][cursorPosX].cursor = false;
                cursorPosY = cursorPosY + 1;
                boardTile[cursorPosY][cursorPosX].cursor = true;

            }
            else {
                printf("Out of bounds \n");
            }
            break;
        case 'w':
            if (cursorPosX + 1 <= 6) {
                boardTile[cursorPosY][cursorPosX].cursor = false;
                cursorPosX = cursorPosX + 1;
                boardTile[cursorPosY][cursorPosX].cursor = true;

            }
            else {
                printf("Out of bounds \n");
            }
            break;

        case'q':
            printf("Turn skipped\n");
            turnTimer += 1;
            break;
        }
    }
}

void pieceSelectOld() {
    //printf("pieceSelectOld\n");
    if (turnTimer % 2 == 0 && boardTile[cursorPosY][cursorPosX].pieceTeam == 1) { 
        oldX = cursorPosX; oldY = cursorPosY;
        printf("Piece selected\n");
        glutKeyboardFunc(processKeyboardNew);
    }
    else if (turnTimer % 2 == 1 && boardTile[cursorPosY][cursorPosX].pieceTeam == 0) {
        oldX = cursorPosX; oldY = cursorPosY;
        printf("Piece selected\n");
        glutKeyboardFunc(processKeyboardNew);
    }
}

void processKeyboardNew(unsigned char key, int xx, int yy) {
    //printf("processKeyboardNew\n");
    char ch = 0;
    int dir = 0;

    switch (key)
    {
    case'x':
        pieceSelectNew();
        break;

    case 'd': //North
        if (cursorPosY - 1 >= 0) {
            dir = 1;
            movPts = MoveRules(cursorPosX, cursorPosY, dir, movPts);
            if (moveRules) {
                boardTile[cursorPosY][cursorPosX].cursor = false;
                cursorPosY = cursorPosY - 1;
                boardTile[cursorPosY][cursorPosX].cursor = true;
            }
        }
        else {
            printf("Out of bounds %d, %d\n", cursorPosX, cursorPosY);
        }
        break;

    case 's': //West
        if (cursorPosX - 1 >= 0) {
            dir = 2;
            movPts = MoveRules(cursorPosX, cursorPosY, dir, movPts);
            if (moveRules) {
                boardTile[cursorPosY][cursorPosX].cursor = false;
                cursorPosX = cursorPosX - 1;
                boardTile[cursorPosY][cursorPosX].cursor = true;
                //  DrawTextBoard();
            }
        }
        else {
            printf("Out of bounds %d, %d\n", cursorPosX, cursorPosY);
        }
        break;

    case 'a': //South
        if (cursorPosY + 1 <= 6) {
            dir = 3;
            movPts = MoveRules(cursorPosX, cursorPosY, dir, movPts);
            if (moveRules) {
                boardTile[cursorPosY][cursorPosX].cursor = false;
                cursorPosY = cursorPosY + 1;
                boardTile[cursorPosY][cursorPosX].cursor = true;
                //  DrawTextBoard();
            }

        }
        else {
            printf("Out of bounds %d, %d\n", cursorPosX, cursorPosY);
        }
        break;

    case 'w': //East
        if (cursorPosX + 1 <= 6) {
            dir = 4;
            movPts = MoveRules(cursorPosX, cursorPosY, dir, movPts);
            if (moveRules) {
                boardTile[cursorPosY][cursorPosX].cursor = false;
                cursorPosX = cursorPosX + 1;
                boardTile[cursorPosY][cursorPosX].cursor = true;
                // DrawTextBoard();
            }
        }
        else {
            printf("Out of bounds %d, %d\n", cursorPosX, cursorPosY);
        }
        break;

    case 'q':
        printf("Piece at %d, %d was unselected\n", oldX, oldY);
        glutKeyboardFunc(processKeyboardOld);
        break;
    }
    // } while (ch != 'q' && !pieceMoved && (movPts > 0 || movPts < 1));
}

void pieceSelectNew() {
    //printf("pieceSelectNew\n");
    char ch = 0;

    if (boardTile[oldY][oldX].positionValue == boardTile[cursorPosY][cursorPosX].positionValue) {
        printf("Same position? Are you sure? Y/N\n");
        ch = _getch();
        ch = tolower(ch);
        if (ch == 'y') {
            newX = cursorPosX; newY = cursorPosY;
            MovePiece(oldY, oldX, newY, newX);
            glutKeyboardFunc(processKeyboardAttack);
        }
        else if (ch == 'n') { glutKeyboardFunc(processKeyboardNew); }
    }
    else if (boardTile[cursorPosY][cursorPosX].occupied == false)
    {
        printf("Position selected\n");
        newX = cursorPosX; newY = cursorPosY;
        MovePiece(oldY, oldX, newY, newX);
        glutKeyboardFunc(processKeyboardAttack);    
    }
    else if (boardTile[cursorPosY][cursorPosX].occupied == true && !attackPos && boardTile[cursorPosY][cursorPosX].pieceTeam != boardTile[oldY][oldX].pieceTeam) {
        specialAttack(oldY, oldX);
    }

    else if (boardTile[cursorPosY][cursorPosX].occupied == true) {
        printf("New tile is occupied\n");
    }
}

void processKeyboardAttack(unsigned char key, int xx, int yy) {
   //printf("processKeyboardAttack\n");
    char ch = 0;
    int dir;

    if (!boardTile[cursorPosY - 1][cursorPosX].occupied &&
        !boardTile[cursorPosY + 1][cursorPosX].occupied &&
        !boardTile[cursorPosY][cursorPosX + 1].occupied &&
        !boardTile[cursorPosY][cursorPosX - 1].occupied ||
        attackPos == false) //if no adjacent pieces or if I fell or climbed to attack
    {
        turnTimer += 1;
        glutKeyboardFunc(processKeyboardOld);
    }
    else {
        printf("Attack or Skip\n");
    }
    switch (key) {
    case 'd':
        dir = 1;
        if (boardTile[cursorPosY - 1][cursorPosX].occupied) {
            if (AttackRules(dir))
            {
                if (boardTile[cursorPosY - 1][cursorPosX].pieceTeam == playerTeam.team) {
                    playerTeam.pieceCount -= 1;
                    boardTile[cursorPosY - 1][cursorPosX].occupied = false;
                }
                else if (boardTile[cursorPosY - 1][cursorPosX].pieceTeam == enemyTeam.team) {
                    enemyTeam.pieceCount -= 1;
                    boardTile[cursorPosY - 1][cursorPosX].occupied = false;
                }
                turnTimer += 1;
                glutKeyboardFunc(processKeyboardOld);
            }
        }
       
        break;
    case'a':
        dir = 2;
        if (AttackRules(dir))
        {
            if (boardTile[cursorPosY + 1][cursorPosX].pieceTeam == playerTeam.team) {
                playerTeam.pieceCount -= 1;
                boardTile[cursorPosY + 1][cursorPosX].occupied = false;
            }
            else if (boardTile[cursorPosY + 1][cursorPosX].pieceTeam == enemyTeam.team) {
                enemyTeam.pieceCount -= 1;
                boardTile[cursorPosY + 1][cursorPosX].occupied = false;
            }
            turnTimer += 1;
            glutKeyboardFunc(processKeyboardOld);
        }
        break;
    case'w':
        dir = 3;
        if (AttackRules(dir))
        {
            if (boardTile[cursorPosY][cursorPosX + 1].pieceTeam == playerTeam.team) {
                playerTeam.pieceCount -= 1;
                boardTile[cursorPosY][cursorPosX + 1].occupied = false;
            }
            else if (boardTile[cursorPosY][cursorPosX + 1].pieceTeam == enemyTeam.team) {
                enemyTeam.pieceCount -= 1;
                boardTile[cursorPosY][cursorPosX + 1].occupied = false;
            }
            turnTimer += 1;
            glutKeyboardFunc(processKeyboardOld);
        }
        break;
    case's':
        dir = 4;
        if (AttackRules(dir))
        {
            if (boardTile[cursorPosY][cursorPosX - 1].pieceTeam == playerTeam.team) {
                playerTeam.pieceCount -= 1;
                boardTile[cursorPosY][cursorPosX - 1].occupied = false;
            }
            else if (boardTile[cursorPosY][cursorPosX - 1].pieceTeam == enemyTeam.team) {
                enemyTeam.pieceCount -= 1;
                boardTile[cursorPosY][cursorPosX - 1].occupied = false;
            }
            turnTimer += 1;
            glutKeyboardFunc(processKeyboardOld);
        }
        break;
    case'q':
        printf("Skip");
        turnTimer += 1;
        glutKeyboardFunc(processKeyboardOld);
        break;
    }
}

void processSpecialKeys(int key, int xx, int yy) {

    double Px = camPosX;
    double Py = camPosY;
    double Pz = camPosZ;
    double theta = 0.0;


    switch (key) {
    case GLUT_KEY_RIGHT:
        theta += 0.7 * (PI / 180);
        camPosX = (cos(theta) * Px) + (0 * Py) + (sin(theta) * Pz);
        camPosY = (0 * Px) + (1 * Py) + (0 * Pz);
        camPosZ = (-sin(theta) * Px) + (0 * Py) + (cos(theta) * Pz);

        break;
    case GLUT_KEY_LEFT:
        theta -= 0.7 * (PI / 180);
        camPosX = (cos(theta) * Px) + (0 * Py) + (sin(theta) * Pz);
        camPosY = (0 * Px) + (1 * Py) + (0 * Pz);
        camPosZ = (-sin(theta) * Px) + (0 * Py) + (cos(theta) * Pz);
        break;
    case 'w':
        camPosX = 0;
        camPosY = 0;
        camPosZ = 0;
        break;
    }
}

void MovePiece(int oldY, int oldX, int newY, int newX) {
    if (boardTile[oldY][oldX].occupied) { //this is to double-check a piece is there instead of just setting new space
        if (boardTile[oldY][oldX].pieceTeam == 1) {
            boardTile[newY][newX].pieceTeam = 1;
        }
        else if (boardTile[oldY][oldX].pieceTeam == 0) {
            boardTile[newY][newX].pieceTeam = 0;
        }
        boardTile[oldY][oldX].occupied = false;
        boardTile[newY][newX].occupied = true;
    }
    else {
       
    }
}

double MoveRules(int cursorPosX, int cursorPosY, int dir, double movPts) {
    moveRules = false;
    attackPos = true;
    board D = boardTile[cursorPosY - 1][cursorPosX]; //North
    board A = boardTile[cursorPosY + 1][cursorPosX]; //South
    board W = boardTile[cursorPosY][cursorPosX + 1]; //East
    board S = boardTile[cursorPosY][cursorPosX - 1]; //West
    board curTile = boardTile[cursorPosY][cursorPosX];

    //printf("CursorPosX = %d, CursorPosY = %d,", cursorPosX, cursorPosY);

    if (dir == 1) {
        if (D.height - curTile.height == 0 && movPts >= 1) { //if height is the same level
           // printf("Same height\n");
            movPts -= 1.0;
            moveRules = true;
        }
        else if (D.height - curTile.height == 1 && movPts >= 1.5) //if height is one above prev
        {
            //printf("Height one above\n");
            movPts -= 1.75;
            moveRules = true;
        }
        else if (D.height - curTile.height == -1 && movPts >= 1.25) { //if height is one below prev
            //printf("Height one below\n");
            movPts -= 1.5;
            moveRules = true;
        }
        else if (D.height - curTile.height == -3 && movPts >= 2) {
            //  printf("Falling\n");
              //the case for falling
            movPts = 0;
            moveRules = true;
            attackPos = false;
        }
        else if (D.height - curTile.height == 2 && movPts >= 2) {
            // printf("Climbing\n");
             //the case for climbing
            movPts = 0;
            moveRules = true;
            attackPos = false;
        }
        else {
            printf("Tile is not accessible from this one\n");
            moveRules = false;
        }
        printf("MovPts: %.2f\n", movPts);
    }

    else if (dir == 2)
    {
        if (S.height - curTile.height == 0 && movPts >= 1) { //if height is the same level
          //  printf("Same height\n");
            //if board tile was travelled subtract movPts
            movPts -= 1.0;
            moveRules = true;
        }
        else if (S.height - curTile.height == 1 && movPts >= 1.5) //if height is one above prev
        {
            //  printf("Height one above\n");

            movPts -= 1.5;
            moveRules = true;
        }
        else if (S.height - curTile.height == -1 && movPts >= 1.25) { //if height is one below prev
         //   printf("Height one below\n");
            movPts -= 1.25;
            moveRules = true;
        }
        else if (S.height - curTile.height == -3 && movPts >= 2) {
            //  printf("Falling\n");
              //the case for falling
            movPts = 0;
            attackPos = false;
            moveRules = true;
        }
        else if (S.height - curTile.height == 2 && movPts >= 2) {
            //  printf("Climbing\n");
              //the case for climbing
            movPts = 0;
            attackPos = false;
            moveRules = true;
        }
        else {
            printf("Tile is not accessible from this one\n");
            moveRules = false;
        }
        printf("MovPts: %.2f\n", movPts);
    }

    else if (dir == 3)
    {
        if (A.height - curTile.height == 0 && movPts >= 1) { //if height is the same level
          //  printf("Same height\n");
            //if board tile was travelled subtract movPts
            movPts -= 1.0;
            moveRules = true;
        }
        else if (A.height - curTile.height == 1 && movPts >= 1.5) //if height is one above prev
        {
            //  printf("Height one above\n");
            movPts -= 1.5;
            moveRules = true;
        }
        else if (A.height - curTile.height == -1 && movPts >= 1.25) { //if height is one below prev
           // printf("Height one below\n");
            movPts -= 1.25;
            moveRules = true;
        }
        else if (A.height - curTile.height == -3  && movPts >= 2) {
            //  printf("Falling\n");
              //the case for falling
            movPts = 0;
            attackPos = false;
            moveRules = true;
        }
        else if (A.height - curTile.height == 2 && movPts >= 2) {
            //  printf("Climbing\n");
              //the case for climbing
            movPts = 0;
            attackPos = false;
            moveRules = true;
        }
        else {
            printf("Tile is not accessible from this one\n");
            moveRules = false;
        }
        printf("MovPts: %.2f\n", movPts);
    }

    else if (dir == 4)
    {
        if (W.height - curTile.height == 0 && movPts >= 1) { //if height is the same level
          //   printf("Same height\n");
             //if board tile was travelled subtract movPts
            movPts -= 1.0;
            moveRules = true;

        }
        else if (W.height - curTile.height == 1 && movPts >= 1.5) //if height is one above prev
        {
            //  printf("Height one above\n");
            movPts -= 1.5;
            moveRules = true;
        }
        else if (W.height - curTile.height == -1 && movPts >= 1.25) { //if height is one below prev
          //  printf("Height one below\n");
            movPts -= 1.25;
            moveRules = true;
        }
        else if (W.height - curTile.height == -3 && movPts >= 2) {
            //  printf("Falling\n");
              //the case for falling
            movPts = 0;
            attackPos = false;
            moveRules = true;
        }
        else if (W.height - curTile.height == 2 && movPts >= 2) {
            //  printf("Climbing\n");
              //the case for climbing
            movPts = 0;
            attackPos = false;
            moveRules = true;
        }
        else {
            printf("Tile is not accessible from this one\n");
            moveRules = false;
        }
        printf("MovPts: %.2f\n", movPts);
    }

    return movPts;
}

bool AttackRules(int dir) {  //avoid attacking the same team while falling or climbing
    bool canAttack = false;
    board D = boardTile[cursorPosY - 1][cursorPosX]; //North
    board A = boardTile[cursorPosY + 1][cursorPosX]; //South
    board W = boardTile[cursorPosY][cursorPosX + 1]; //East
    board S = boardTile[cursorPosY][cursorPosX - 1]; //West


    if (D.occupied && D.pieceTeam != boardTile[cursorPosY][cursorPosX].pieceTeam && dir == 1) //North
    {
        if (boardTile[cursorPosY][cursorPosX].height - D.height == 0 || boardTile[cursorPosY][cursorPosX].height - D.height == 1) {
            canAttack = true;
        }
    }
    else if (A.occupied && A.pieceTeam != boardTile[cursorPosY][cursorPosX].pieceTeam && dir == 2) //South
    {
        if (boardTile[cursorPosY][cursorPosX].height - A.height == 0 || boardTile[cursorPosY][cursorPosX].height - A.height == 1) {
            canAttack = true;
        }
    }
    else if (W.occupied && W.pieceTeam != boardTile[cursorPosY][cursorPosX].pieceTeam && dir == 3) //East
    {
        if (boardTile[cursorPosY][cursorPosX].height - W.height == 0 || boardTile[cursorPosY][cursorPosX].height - W.height == 1) {
            canAttack = true;
        }
    }
    else if (S.occupied && S.pieceTeam != boardTile[cursorPosY][cursorPosX].pieceTeam && dir == 4) //West
    {
        if (boardTile[cursorPosY][cursorPosX].height - S.height == 0 || boardTile[cursorPosY][cursorPosX].height - S.height == 1) {
            canAttack = true;
        }
    }

    return canAttack;
}

void specialAttack(int oldY, int oldX) { //used for falling and climbing special attack condition
    char ch = 0;

    printf("This will attack the tile you are landing on? Are you sure? Y / N\n");
    ch = _getch();
    ch = tolower(ch);
    switch (ch) {
    case'y':
        if (boardTile[cursorPosY][cursorPosX].pieceTeam == playerTeam.team) {
            playerTeam.pieceCount -= 1;
            //printf("Cursor X: %d  Cursor Y: %d", cursorPosX, cursorPosY);
        }
        else { enemyTeam.pieceCount -= 1; }
        boardTile[cursorPosY][cursorPosX].occupied = false;
        turnTimer += 1;
        MovePiece(oldY, oldX, cursorPosY, cursorPosX);
        glutKeyboardFunc(processKeyboardOld);
        break;
    case'n':

        break;
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



//Initialize Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//board correction still needs to be tweaked slightly
//board generation until board height complies with rules
void BoardCorrection() {
    bool North, South, East, West;
    int i, j, currentPlace;
    for (i = 0; i != 7; i++) { //
        North = 1;
        South = 1;
        if (i == 0) { North = 0; }
        else if (i == 6) { South = 0; } //check where we are on the board. Set a position to false to prevent checking the direction
        for (j = 0; j != 7; j++) {
            East = 1;
            West = 1;
            currentPlace = boardTile[i][j].height;
            // printf("I'm at position %d, %d - ",i,  j);
            if (j == 0) { West = 0; }
            else if (j == 6) { East = 0; }
            switch (currentPlace) {
            case 0: //if N,S,E,or W is >=2. 0 and 1 are legal heights. Sim to C4C
                CaseZeroCorrection(i, j, North, South, East, West);
                // printf("Case 0\n");
                break;

            case 1: //if N,S,E,or W is >=3. 0,1,2 are legal heights. Sim to C3C
                CaseOneCorrection(i, j, North, South, East, West);
                //printf("Case 1\n");
                break;

            case 2: //if N,S,E,or W is 0 or 4. 1,2,3 are legal heights. Unique
                CaseTwoCorrection(i, j, North, South, East, West);
                //printf("Case 2\n");
                break;

            case 3: //if N,S,E,or W is <=1. 2,3,4 are legal heights. Sim to C1C
                CaseThreeCorrection(i, j, North, South, East, West);
                // printf("Case 3\n");
                break;

            case 4: //if N,S,E,or W is <= 2. 3 and 4 are legal heights. Sim to C0C
                CaseFourCorrection(i, j, North, South, East, West);
                // printf("Case 4\n");
                break;
            }
        }
    }
}

void CaseZeroCorrection(int i, int j, bool North, bool South, bool East, bool West) {
    if (!North && !West) { //top left corner
        //printf("In !N&!W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 7 - 1) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height >= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2;
                loop = 1;
            }
            count++;
        }
    }
    else if (!North && !East) { //top left corner
       // printf("In !N&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height >= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !West) { //bottom left
       // printf("In !S&!W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if (boardTile[i - 1][j].height >= 2 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !East) { //bottom right
        //printf("In !S&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if (boardTile[i - 1][j].height >= 2 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2;
                loop = 1;
            }
            count++;
        }
    }

    else if (!North) { //top row, excluding corners
       // printf("In !N");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i + 1][j].height >= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 2 && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2;
                loop = 1;
            }
            count++;
        }
    }

    else if (!South) { //bottom row, excluding corners
    //printf("In !S");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i - 1][j].height >= 2 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 2 && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2;
                loop = 1;
            }
            count++;
        }
    }
    else if (!East) { //right most column, excluding corners
    //printf("In !E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i + 1][j].height >= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i - 1][j].height >= 2 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2;
                loop = 1;
            }
            count++;
        }
    }

    else if (!West) {
        //printf("In !W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i + 1][j].height >= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i - 1][j].height >= 2 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2;
                loop = 1;
            }
            count++;
        }
    }
    else {
        //printf("In Mid");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 4;

            if (boardTile[i + 1][j].height >= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i - 1][j].height >= 2 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 2 && !boardTile[i][j - 1].corrected && random == 3) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2;
                loop = 1;
            }
            count++;
        }
    }

}
void CaseOneCorrection(int i, int j, bool North, bool South, bool East, bool West) {
    if (!North && !West) { //top left corner
       // printf("In !N&!W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height >= 3 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3;
                loop = 1;
            }
            count++;
        }
    }
    else if (!North && !East) { //top left corner
        //printf("In !N&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height >= 3 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 3 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !West) { //bottom left
        //printf("In !S&!W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if (boardTile[i - 1][j].height >= 3 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 3 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !East) { //bottom right
       // printf("In !S&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if (boardTile[i - 1][j].height >= 3 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 3 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3;
                loop = 1;
            }
            count++;
        }
    }

    else if (!North) { //top row, excluding corners
       // printf("In !N");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i + 1][j].height >= 3 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 3 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 3 && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3;
                loop = 1;
            }
            count++;
        }
    }

    else if (!South) { //bottom row, excluding corners
    //printf("In !S");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i - 1][j].height >= 3 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 3 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 3 && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3;
                loop = 1;
            }
            count++;
        }
    }
    else if (!East) { //right most column, excluding corners
    //printf("In !E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i + 1][j].height >= 3 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 3 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3;
                loop = 1;
            }
            if (boardTile[i - 1][j].height >= 3 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3;
                loop = 1;
            }
            count++;
        }
    }

    else if (!West) {
        //printf("In !W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i + 1][j].height >= 3 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 3 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3;
                loop = 1;
            }
            if (boardTile[i - 1][j].height >= 3 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3;
                loop = 1;
            }
            count++;
        }
    }

    else {
        //printf("In Mid");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 4;

            if (boardTile[i + 1][j].height >= 3 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 3 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3;
                loop = 1;
            }
            if (boardTile[i - 1][j].height >= 3 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 3 && !boardTile[i][j - 1].corrected && random == 3) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3;
                loop = 1;
            }
            count++;
        }
    }
}
void CaseTwoCorrection(int i, int j, bool North, bool South, bool East, bool West) {
    if (!North && !West) { //top left corner
       // printf("In !N&!W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if ((boardTile[i + 1][j].height == 0 || boardTile[i + 1][j].height == 4) && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3 + 1;
                loop = 1;
            }
            if ((boardTile[i][j + 1].height == 0 || boardTile[i][j + 1].height == 4) && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3 + 1;
                loop = 1;
            }
            count++;
        }
    }
    else if (!North && !East) { //top left corner
        //printf("In !N&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if ((boardTile[i + 1][j].height == 0 || boardTile[i + 1][j].height == 4) && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3 + 1;
                loop = 1;
            }
            if ((boardTile[i][j - 1].height == 0 || boardTile[i][j - 1].height == 4) && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3 + 1;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !West) { //bottom left
        //printf("In !S&!W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if ((boardTile[i - 1][j].height == 0 || boardTile[i - 1][j].height == 4) && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3 + 1;
                loop = 1;
            }
            if ((boardTile[i][j + 1].height == 0 || boardTile[i][j + 1].height == 4) && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3 + 1;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !East) { //bottom right
       // printf("In !S&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if ((boardTile[i - 1][j].height == 0 || boardTile[i - 1][j].height == 4) && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3 + 1;
                loop = 1;
            }
            if ((boardTile[i][j - 1].height == 0 || boardTile[i][j - 1].height == 4) && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3 + 1;
                loop = 1;
            }
            count++;
        }
    }

    else if (!North) { //top row, excluding corners
       // printf("In !N");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if ((boardTile[i + 1][j].height == 0 || boardTile[i + 1][j].height == 4) && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3 + 1;
                loop = 1;
            }
            if ((boardTile[i][j - 1].height == 0 || boardTile[i][j - 1].height == 4) && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3 + 1;
                loop = 1;
            }
            if ((boardTile[i][j + 1].height == 0 || boardTile[i][j + 1].height == 4) && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3 + 1;
                loop = 1;
            }
            count++;
        }
    }

    else if (!South) { //bottom row, excluding corners
    //printf("In !S");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if ((boardTile[i - 1][j].height == 0 || boardTile[i - 1][j].height == 4) && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3 + 1;
                loop = 1;
            }
            if ((boardTile[i][j - 1].height == 0 || boardTile[i][j - 1].height == 4) && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3 + 1;
                loop = 1;
            }
            if ((boardTile[i][j + 1].height == 0 || boardTile[i][j + 1].height == 4) && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3 + 1;
                loop = 1;
            }
            count++;
        }
    }
    else if (!East) { //right most column, excluding corners
    //printf("In !E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if ((boardTile[i + 1][j].height == 0 || boardTile[i + 1][j].height == 4) && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3 + 1;
                loop = 1;
            }
            if ((boardTile[i][j - 1].height == 0 || boardTile[i][j - 1].height == 4) && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3 + 1;
                loop = 1;
            }
            if ((boardTile[i - 1][j].height == 0 || boardTile[i - 1][j].height == 4) && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3 + 1;
                loop = 1;
            }
            count++;
        }
    }

    else if (!West) {
        //printf("In !W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if ((boardTile[i + 1][j].height == 0 || boardTile[i + 1][j].height == 4) && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3 + 1;
                loop = 1;
            }
            if ((boardTile[i][j + 1].height == 0 || boardTile[i][j + 1].height == 4) && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3 + 1;
                loop = 1;
            }
            if ((boardTile[i - 1][j].height == 0 || boardTile[i - 1][j].height == 4) && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3 + 1;
                loop = 1;
            }
            count++;
        }
    }

    else {
        //printf("In Mid");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 4;

            if ((boardTile[i + 1][j].height == 0 || boardTile[i + 1][j].height == 4) && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3 + 1;
                loop = 1;
            }
            if ((boardTile[i][j + 1].height == 0 || boardTile[i][j + 1].height == 4) && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3 + 1;
                loop = 1;
            }
            if ((boardTile[i - 1][j].height == 0 || boardTile[i - 1][j].height == 4) && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3 + 1;
                loop = 1;
            }
            if ((boardTile[i][j - 1].height == 0 || boardTile[i][j - 1].height == 4) && !boardTile[i][j - 1].corrected && random == 3) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3 + 1;
                loop = 1;
            }
            count++;
        }
    }
}
void CaseThreeCorrection(int i, int j, bool North, bool South, bool East, bool West) {
    if (!North && !West) { //top left corner
       // printf("In !N&!W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height <= 1 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3 + 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 1 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3 + 2;
                loop = 1;
            }
            count++;
        }
    }
    else if (!North && !East) { //top left corner
       // printf("In !N&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height <= 1 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3 + 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 1 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3 + 2;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !West) { //bottom left
       // printf("In !S&!W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if (boardTile[i - 1][j].height <= 1 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3 + 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 1 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3 + 2;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !East) { //bottom right
       // printf("In !S&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if (boardTile[i - 1][j].height <= 1 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3 + 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 1 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3 + 2;
                loop = 1;
            }
            count++;
        }
    }

    else if (!North) { //top row, excluding corners
       // printf("In !N");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i + 1][j].height <= 1 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3 + 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 1 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3 + 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 1 && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3 + 2;
                loop = 1;
            }
            count++;
        }
    }

    else if (!South) { //bottom row, excluding corners
    //printf("In !S");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i - 1][j].height <= 1 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3 + 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 1 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3 + 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 1 && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3 + 2;
                loop = 1;
            }
            count++;
        }
    }
    else if (!East) { //right most column, excluding corners
    //printf("In !E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i + 1][j].height <= 1 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3 + 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 1 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3 + 2;
                loop = 1;
            }
            if (boardTile[i - 1][j].height <= 1 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3 + 2;
                loop = 1;
            }
            count++;
        }
    }

    else if (!West) {
        //printf("In !W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i + 1][j].height <= 1 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3 + 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 1 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3 + 2;
                loop = 1;
            }
            if (boardTile[i - 1][j].height <= 1 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3 + 2;
                loop = 1;
            }
            count++;
        }
    }

    else {
        //printf("In Mid");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 4;

            if (boardTile[i + 1][j].height <= 1 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 3 + 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 1 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 3 + 2;
                loop = 1;
            }
            if (boardTile[i - 1][j].height <= 1 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 3 + 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 1 && !boardTile[i][j - 1].corrected && random == 3) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 3 + 2;
                loop = 1;
            }
            count++;
        }
    }
}
void CaseFourCorrection(int i, int j, bool North, bool South, bool East, bool West) {
    if (!North && !West) { //top left corner
       // printf("In !N&!W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 7) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height <= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 3;
                loop = 1;
            }
            count++;
        }
    }
    else if (!North && !East) { //top left corner
       // printf("In !N&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height <= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 3;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !West) { //bottom left
       // printf("In !S&!W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if (boardTile[i - 1][j].height <= 2 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 3;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !East) { //bottom right
       // printf("In !S&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 2;

            if (boardTile[i - 1][j].height <= 2 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 3;
                loop = 1;
            }
            count++;
        }
    }

    else if (!North) { //top row, excluding corners

       // printf("In !N");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i + 1][j].height <= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 2 && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 3;
                loop = 1;
            }
            count++;
        }
    }

    else if (!South) { //bottom row, excluding corners
    //printf("In !S");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i - 1][j].height <= 2 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 2 && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 3;
                loop = 1;
            }
            count++;
        }
    }
    else if (!East) { //right most column, excluding corners
    //printf("In !E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i + 1][j].height <= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 3;
                loop = 1;
            }
            if (boardTile[i - 1][j].height <= 2 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 3;
                loop = 1;
            }
            count++;
        }
    }

    else if (!West) {
        //printf("In !W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 3;

            if (boardTile[i + 1][j].height <= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 3;
                loop = 1;
            }
            if (boardTile[i - 1][j].height <= 2 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 3;
                loop = 1;
            }
            count++;
        }
    }

    else {
        //printf("In Mid");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 6) {
            int random = rand() % 4;

            if (boardTile[i + 1][j].height <= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 3;
                loop = 1;
            }
            if (boardTile[i - 1][j].height <= 2 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 2 && !boardTile[i][j - 1].corrected && random == 3) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 3;
                loop = 1;
            }
            count++;
        }
    }
}

void TeamDecider() {
    playerTeam.team = 0;
    enemyTeam.team = 1;
}

void BoardHeightGenerator() {
    for (int y = 0; y != 7; y++) { //y part of matrix
        for (int x = 0; x != 7; x++) { //x part of matrix
            boardTile[y][x].height = rand() % 5;
            boardTile[y][x].occupied = 0;
            boardTile[y][x].corrected = 0;
            // printf("%d ", boardTile[y][x].height);
        }
        printf("\n");
    }
}

void IntPiecePlacement() { //Player is 0, Enemy is 1
    boardTile[1][1].occupied = true;
    boardTile[4][4].occupied = true;
    boardTile[1][4].occupied = true;
    boardTile[4][1].occupied = true;

    boardTile[1][1].pieceTeam = 0;
    boardTile[1][4].pieceTeam = 0;

    boardTile[4][4].pieceTeam = 1;
    boardTile[4][1].pieceTeam = 1;

    playerTeam.pieceCount = 2;
    enemyTeam.pieceCount = 2;
}

void DrawTextBoard() {
    char line[] = "//////////////";
    char spaceLineLeft[] = "//    ";
    char spaceLineRight[] = "     //";

    if (turnTimer % 2 == 1)
    {
        printf("Player Turn\n");
    }
    else {
        printf("Enemy Turn\n");
    }


    for (int y = 0; y <= 6; y++) {
        for (int x = 0; x <= 6; x++) {
            printf("%s", line);
            if (x == 6) { printf("\n"); }
        }
        int j = 0;
        while (j < 3) {
            for (int x = 0; x != 7; x++) {
                int heightIcon = boardTile[y][x].height;

                if (j == 0) { printf("%s%d%s", spaceLineLeft, heightIcon, spaceLineRight); } //HeightLine
                if (j == 1 && boardTile[y][x].occupied && boardTile[y][x].pieceTeam == 1) { printf("%sE%s", spaceLineLeft, spaceLineRight); } //OccupiedLine w/ Cursor & Player
                else if (j == 1 && boardTile[y][x].occupied && boardTile[y][x].pieceTeam == 0) { printf("%sP%s", spaceLineLeft, spaceLineRight); } //OccupiedLine w/ Cursor & Enemy
                else if (j == 1 && !boardTile[y][x].occupied) { printf("%s-%s", spaceLineLeft, spaceLineRight); } //unoccupied
                if (j == 2 && boardTile[y][x].cursor == true) { printf("%s^%s", spaceLineLeft, spaceLineRight); }//Cursor
                else if (j == 2 && boardTile[y][x].cursor == false) { printf("%s-%s", spaceLineLeft, spaceLineRight); }

                if (x == 6) { printf("\n"); }

            }
            j++;
        }
    }
    for (int x = 0; x <= 6; x++) {
        printf("%s", line);
    }
    printf("\n");
} //Test Class for console

void setValue() {
    int count = 0;
    for (int y = 0; y != 5; y++) { //y part of matrix
        for (int x = 0; x != 5; x++) { //x part of matrix
            boardTile[y][x].positionValue = count;
            count++;
        }
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//Support Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

char getAns(char mesg[]) {
    char ans;
    printf("%s", mesg);
    ans = _getch();
    return tolower(ans);
}

void reshape(GLsizei width, GLsizei height) { //GLsizei for non-neg int

    /*Handler for window re-size event. Called back when the window first appears and
 whenever the window is re-sized with its new width and height*/

    if (height == 0) { height = 1; } //prevents divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    //set viewport to cover new window
    glViewport(0, 0, width, height);

    //Set aspect ration of the clipping volume to match the viewport
    glMatrixMode(GL_PROJECTION); //operate on projection matrix
    glLoadIdentity();            //reset

    //Enable perspective projection fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
    /* gluLookAt(0.0f, 5.5f, 15.0f, //eye
         0.0f, 0.0f, 0.0f,  //center
         0.0f, 1.0f, 0.0f); //up*/

}

void timer(int value) {
    glutPostRedisplay();                    //post re-paint request to activate display()
    glutTimerFunc(refreshMills, timer, 0);  //next timer call milliseconds later
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//AI Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void dumbAI() {
    getSourcePiece();
    getTargetPiece();
    AIInitCursorPos();
    pieceSelectOldAI();
    moveCursorAI();
    pieceSelectNewAI();
    processAttackAI();
}

void moveCursorAI() 
{
    board source = boardTile[cursorPosY][cursorPosX];
    int y;
    int x;
    int dir, count = 0;
    

        for (y = 0; y != 7; y++) {
            for (x = 0; x != 7; x++) {
                if (boardTile[y][x].positionValue == source.positionValue) {
                    sourceX = x;
                    sourceY = y;
                }
                if (boardTile[y][x].target) {
                    targetX = x;
                    targetY = y;
                }
            }
        }

        printf("Target y,x is: %d, %d\n", targetY, targetX);

            if (sourceX == targetX) {
                printf("test1\n");
                if (sourceY > targetY) { //D
                    if (cursorPosX - 1 >= 0) {
                        dir = 1;
                        movPts = MoveRules(cursorPosX, cursorPosY, dir, movPts);
                        if (moveRules) {
                            cursorMoveAI(dir);
                        }
                    }
                }
                else if (sourceY < targetY) { //A
                    printf("test2\n");
                    if (cursorPosY + 1 <= 6) {
                        dir = 3;
                        movPts = MoveRules(cursorPosX, cursorPosY, dir, movPts);
                        if (moveRules) {
                            cursorMoveAI(dir);
                        }
                    }
                }
            }

            else if (sourceY == targetY) {
                printf("test3\n");
                if (sourceX > targetX) { //S
                    if (cursorPosX - 1 >= 0) {
                        dir = 2;
                        movPts = MoveRules(cursorPosX, cursorPosY, dir, movPts);
                        if (moveRules) {
                            cursorMoveAI(dir);
                        }
                    }
                }

                else if (sourceX < targetX) { //W
                    printf("test4\n");
                    if (cursorPosX + 1 <= 6) {
                        dir = 4;
                        movPts = MoveRules(cursorPosX, cursorPosY, dir, movPts);
                        if (moveRules) {
                            cursorMoveAI(dir);
                        }
                    }
                }
            }

            else if (sourceY != targetY && sourceX != targetX) {
                printf("test5\n");
                int dir = rand() % 4 + 1;
                movPts = MoveRules(cursorPosX, cursorPosY, dir, movPts);
                if (moveRules) {
                    cursorMoveAI(dir);
                }
            }
            count++;
}

void pieceSelectOldAI() {
    //printf("pieceSelectOld\n");
    if (turnTimer % 2 == 0 && boardTile[cursorPosY][cursorPosX].pieceTeam == 1) {
        oldX = cursorPosX; oldY = cursorPosY;
        printf("Piece selected at %d, %d:\n", cursorPosY, cursorPosX);
    }
}

void pieceSelectNewAI() {
    //printf("pieceSelectNew\n");
     newX = cursorPosX; newY = cursorPosY;   
     MovePiece(oldY, oldX, newY, newX);
}

void processAttackAI() {
    //printf("processKeyboardAttack\n");
    int dir;

    if (!boardTile[cursorPosY - 1][cursorPosX].occupied &&
        !boardTile[cursorPosY + 1][cursorPosX].occupied &&
        !boardTile[cursorPosY][cursorPosX + 1].occupied &&
        !boardTile[cursorPosY][cursorPosX - 1].occupied ||
        attackPos == false) //if no adjacent pieces or if I fell or climbed to attack
    {
        turnTimer += 1;

    }

    if (boardTile[cursorPosY - 1][cursorPosX].occupied) { //D
        dir = 1;
        if (AttackRules(dir))
        {
            if (boardTile[cursorPosY - 1][cursorPosX].pieceTeam == playerTeam.team) {
                playerTeam.pieceCount -= 1;
                boardTile[cursorPosY - 1][cursorPosX].occupied = false;
            }
            turnTimer += 1;

        }
    }


    else if (boardTile[cursorPosY][cursorPosX - 1].occupied) { //S
        dir = 2;
        if (AttackRules(dir)) {
            if (boardTile[cursorPosY][cursorPosX - 1].pieceTeam == playerTeam.team) {
                playerTeam.pieceCount -= 1;
                boardTile[cursorPosY][cursorPosX - 1].occupied = false;
            }
            turnTimer += 1;
        }
    }

    else if (boardTile[cursorPosY + 1][cursorPosX].occupied) {//A
        dir = 3;
        if (AttackRules(dir))
        {
            if (boardTile[cursorPosY + 1][cursorPosX].pieceTeam == playerTeam.team) {
                playerTeam.pieceCount -= 1;
                boardTile[cursorPosY + 1][cursorPosX].occupied = false;
            }
            turnTimer += 1;

        }
    }



    else if (boardTile[cursorPosY][cursorPosX + 1].occupied) {//W
        dir = 4;
        if (AttackRules(dir))
        {
            if (boardTile[cursorPosY][cursorPosX + 1].pieceTeam == playerTeam.team) {
                playerTeam.pieceCount -= 1;
                boardTile[cursorPosY][cursorPosX + 1].occupied = false;
            }
            turnTimer += 1;

        }


    }

    else {
        turnTimer += 1;
    }

}

void specialAttackAI(int oldY, int oldX) { //used for falling and climbing special attack condition
    char ch = 0;

    
        if (boardTile[cursorPosY][cursorPosX].pieceTeam == playerTeam.team) {
            playerTeam.pieceCount -= 1;
            //printf("Cursor X: %d  Cursor Y: %d", cursorPosX, cursorPosY);
        }
        else { enemyTeam.pieceCount -= 1; }
        boardTile[cursorPosY][cursorPosX].occupied = false;
        turnTimer += 1;
        MovePiece(oldY, oldX, cursorPosY, cursorPosX);
}

void cursorMoveAI(int dir) {

    switch (dir) {
    case 1:
        if (cursorPosX - 1 >= 0) {
            boardTile[cursorPosY][cursorPosX].cursor = false;
            cursorPosY = cursorPosY - 1;
            boardTile[cursorPosY][cursorPosX].cursor = true;
        }
        break;

    case 2:
        if (cursorPosX - 1 >= 0) {
            boardTile[cursorPosY][cursorPosX].cursor = false;
            cursorPosX = cursorPosX - 1;
            boardTile[cursorPosY][cursorPosX].cursor = true;
        }
        break;

    case 3:
        if (cursorPosY + 1 <= 6) {
            boardTile[cursorPosY][cursorPosX].cursor = false;
            cursorPosY = cursorPosY + 1;
            boardTile[cursorPosY][cursorPosX].cursor = true;
        }
        break;

    case 4:
        if (cursorPosX + 1 <= 6) {
            boardTile[cursorPosY][cursorPosX].cursor = false;
            cursorPosX = cursorPosX + 1;
            boardTile[cursorPosY][cursorPosX].cursor = true;
        }
        break;
    }
}


void getSourcePiece() {
    int source = getComputerPosition();
    int y = 0;
    int x = 0;
    //set previous source pos to false
    //printf("In getSourcePiece\n");
    for (y = 0; y != 7; y++) {
        for (x = 0; x != 7; x++) {
            if (boardTile[y][x].source) {
                boardTile[y][x].source = false;
            }
        }
    }

    //get new target
    for (y = 0; y != 7; y++) {
        for (x = 0; x != 7; x++) {
            if (boardTile[y][x].positionValue == source) {
                boardTile[y][x].source = true;
                //printf("Source piece is %d, %d", y, x);
            }
        }
    }
}
int getComputerPosition() {
    int piece1;
    int piece2;
    int random = rand() % 2;

    if (random == 0) {
        for (int y = 0; y != 7; y++) {
            for (int x = 0; x != 7; x++) {
                if (boardTile[y][x].occupied && boardTile[y][x].pieceTeam == 1) {
                    piece1 = boardTile[y][x].positionValue;
                    return piece1;
                }

            }
        }
    }

    else if (random == 1) {
        for (int y = 6; y != -1; y--) {
            for (int x = 6; x != -1; x--) {
                if (boardTile[y][x].occupied && boardTile[y][x].pieceTeam == 1) {
                    piece2 = boardTile[y][x].positionValue;
                    return piece2;
                }
            }
        }
    }
}

void getTargetPiece() {

    int target = getPlayerPosition();
    int y = 0;
    int x = 0;
    //set previous target pos to false
    for (y = 0; y != 7; y++) {
        for (x = 0; x != 7; x++) {
            if (boardTile[y][x].target) {
                boardTile[y][x].target = false;
            }
        }
    }
    //get new target
    for (y = 0; y != 7; y++) {
        for (x = 0; x != 7; x++) {
            if (boardTile[y][x].positionValue == target) {
                boardTile[y][x].target = true;
            }
        }
    }
}
int getPlayerPosition() {
    int piece1;
    int piece2;
    int random = rand() % 2;

    if (random == 0) {
        for (int y = 0; y != 7; y++) {
            for (int x = 0; x != 7; x++) {
                if (boardTile[y][x].occupied && boardTile[y][x].pieceTeam == 0) {
                    piece1 = boardTile[y][x].positionValue;
                    return piece1;
                }

            }
        }
    }

    else if (random == 1) {
        for (int y = 6; y != -1; y--) {
            for (int x = 6; x != -1; x--) {
                if (boardTile[y][x].occupied && boardTile[y][x].pieceTeam == 0) {
                    piece2 = boardTile[y][x].positionValue;
                    return piece2;
                }
            }
        }
    }
}

void AIInitCursorPos() {

    for (int y = 0; y != 7; y++) {
        for (int x = 0; x != 7; x++) {
            if (boardTile[y][x].source) {
                boardTile[cursorPosY][cursorPosX].cursor = false;
                cursorPosY = y; cursorPosX = x;
                boardTile[cursorPosY][cursorPosX].cursor = true;
            }
        }
    }
   
}

