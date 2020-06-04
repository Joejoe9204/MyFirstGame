#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <math.h>
#include <stdbool.h>

//----Structure Zone----
typedef struct{
    bool corrected;
    bool occupied;
    bool cursor;
    bool travelled;
    int height;
}board;

typedef struct {
    bool turn;
    bool team;
}Player;

typedef struct{
    bool turn;
    bool team;
}Enemy;

typedef struct{
    int movPts;
    bool selected;
    bool attacked;
    bool Pteam;
    bool ETeam;
}piece;


//----End of ConSTRUCTion Zone----

//----Function Declaration----
void DrawBoard();
void BoardHeightGenerator();
void BoardCorrection();
void CaseZeroCorrection(int, int, bool, bool, bool, bool);
void CaseOneCorrection(int, int, bool, bool, bool, bool);
void CaseTwoCorrection(int, int, bool, bool, bool, bool);
void CaseThreeCorrection(int, int, bool, bool, bool, bool);
void CaseFourCorrection(int, int, bool, bool, bool, bool);
bool MoveRules(int ,int, int, double);
void IntPiecePlacement();
void pieceSelectOld();
void pieceSelectNew(int, int);
void TeamDecider();
//void Piece();
//void Player();
//void Enemy();
//void AI();
bool MovePiece(int, int, int ,int);
char getAns(char[]);

//----End of Function Declarations----

//Global Variables
board boardTile[5][5];
bool pieceMoved;
bool tipsOn;
int cursorPosY = 1;
int cursorPosX = 1;


//----End of Global Variables----

int main() {
    srand(time(NULL)); //random seed
    
        BoardHeightGenerator();
        IntPiecePlacement();
        BoardCorrection();
        TeamDecider();

        if (getAns("Tips on? Y/N...\n") == 'y') { tipsOn = true; }
        else { tipsOn = false; }

        boardTile[cursorPosY][cursorPosX].cursor = true;
        

        for (int turnTimer = 1; turnTimer <= 15; turnTimer++) {
            DrawBoard();
            pieceSelectOld();
            if (pieceMoved) {
                printf("\nNew Board\n");
                pieceMoved = false;
            }
            printf("Turn: %d\n", turnTimer);
        }
    
    return 0;
}

void TeamDecider(){
    int team = rand()%1; //Black is 0, White is 1
    Player playerTeam;
    Enemy enemyTeam;
    if(team == 0){
        playerTeam.team = 0;
        enemyTeam.team = 1;
    } else {
        playerTeam.team = 1;
        enemyTeam.team = 0;
    }
}


void IntPiecePlacement() { //Black is 0, white is 1
    boardTile[1][1].occupied = 1;
    boardTile[4][4].occupied = 1;

    for(int y = 0; y!=5; y++) { //y part of matrix
        for(int x = 0; x != 5; x++) { //x part of matrix
            if (boardTile[y][x].occupied) {
                printf("y:%d, x:%d, True\n", y ,x);
            } else {
                printf("y:%d, x:%d, False\n", y, x);
            }
        }
    }
}


void DrawBoard() {
    char line[] = "//////////////";
    char spaceLineLeft[] = "//    ";
    char spaceLineRight[] = "     //";

    for(int y = 0; y!=5; y++) {
        for(int x = 0; x!=5; x++) {
            printf("%s", line);
            if(x == 4) { printf("\n");}
        }
        int j = 0;
        while(j < 3){
            for (int x = 0; x != 5; x++) {
                int heightIcon = boardTile[y][x].height;
                
                if(j==0) { printf("%s%d%s", spaceLineLeft, heightIcon, spaceLineRight);} //HeightLine
                if(j==1 && boardTile[y][x].occupied ) { printf("%s1%s", spaceLineLeft, spaceLineRight); } //OccupiedLine w/ Cursor
                else if(j==1 && !boardTile[y][x].occupied) { printf("%s0%s", spaceLineLeft, spaceLineRight); }
                if (j == 2 && boardTile[y][x].cursor == true) { printf("%s^%s", spaceLineLeft, spaceLineRight); }//Cursor
                else if(j == 2 && boardTile[y][x].cursor == false) { printf("%s-%s", spaceLineLeft, spaceLineRight); }
               
                
                if (x == 4) { printf("\n"); }

            }
            j++;
        }
    }
    for (int x = 0; x !=5; x++) {
        printf("%s", line);
    }
    printf("\n");
}

//fills matrix with random values to then be corrected later
void BoardHeightGenerator() {
    for(int y = 0; y!=5; y++) { //y part of matrix
        for(int x = 0; x!=5; x++) { //x part of matrix
            boardTile[y][x].height = rand() % 5;
            boardTile[y][x].occupied = 0;
            boardTile[y][x].corrected = 0;
            printf("%d ", boardTile[y][x].height);
        }
        printf("\n");
    }
}

bool MovePiece(int oldY, int oldX, int newY, int newX) {
    if (boardTile[oldY][oldX].occupied) { //this is to double-check a piece is there instead of just setting new space
        boardTile[oldY][oldX].occupied = false;
        boardTile[newY][newX].occupied = true;
        pieceMoved = true;
        return pieceMoved;
    }
    else {
        return pieceMoved;
    }
}


//board correction still needs to be tweaked slightly
//board generation until board height complies with rules
void BoardCorrection() {
    bool North, South, East, West;
    int i, j, currentPlace;
    for(i = 0; i!=5; i++){ //
        North = 1;
        South = 1;
        if (i == 0) { North = 0; } else if (i == 4) { South = 0; } //check where we are on the board. Set a position to false to prevent checking the direction
        for(j = 0; j!=5; j++){
            East = 1;
            West = 1;
            currentPlace = boardTile[i][j].height;
           // printf("I'm at position %d, %d - ",i,  j);
            if (j == 0) { West = 0; } else if (j == 4) { East = 0; }
            switch(currentPlace) {
                case 0: //if N,S,E,or W is >=2. 0 and 1 are legal heights. Sim to C4C
                    CaseZeroCorrection(i, j, North, South, East, West);
                    printf("Case 0\n");
                    break;

                case 1: //if N,S,E,or W is >=3. 0,1,2 are legal heights. Sim to C3C
                    CaseOneCorrection(i, j, North, South, East, West);
                    printf("Case 1\n");
                    break;

                case 2: //if N,S,E,or W is 0 or 4. 1,2,3 are legal heights. Unique
                    CaseTwoCorrection(i, j, North, South, East, West);
                    printf("Case 2\n");
                    break;

                case 3: //if N,S,E,or W is <=1. 2,3,4 are legal heights. Sim to C1C
                    CaseThreeCorrection(i, j, North, South, East, West);
                    printf("Case 3\n");
                    break;

                case 4: //if N,S,E,or W is <= 2. 3 and 4 are legal heights. Sim to C0C
                    CaseFourCorrection(i, j, North, South, East, West);
                    printf("Case 4\n");
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
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i + 1][j].height >= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 1;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 1;
                loop = 1;
            }
            count++;
        }
    }
    else if (!North && !East) { //top left corner
       // printf("In !N&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i + 1][j].height >= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 1;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 1;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !West) { //bottom left
       // printf("In !S&!W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i - 1][j].height >= 2 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 1;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 1;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !East) { //bottom right
        //printf("In !S&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i - 1][j].height >= 2 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 1;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 1;
                loop = 1;
            }
            count++;
        }
    }

    else if (!North) { //top row, excluding corners
       // printf("In !N");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height >= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 1;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 1;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 2 && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 1;
                loop = 1;
            }
            count++;
        }
    }

    else if (!South) { //bottom row, excluding corners
    //printf("In !S");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i - 1][j].height >= 2 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 1;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 1;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 2 && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 1;
                loop = 1;
            }
            count++;
        }
    }
    else if (!East) { //right most column, excluding corners
    //printf("In !E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height >= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 1;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 1;
                loop = 1;
            }
            if (boardTile[i - 1][j].height >= 2 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 1;
                loop = 1;
            }
            count++;
        }
    }

    else if (!West) {
    //printf("In !W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height >= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 1;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 1;
                loop = 1;
            }
            if (boardTile[i - 1][j].height >= 2 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 1;
                loop = 1;
            }
            count++;
        }
    }
    else {
    //printf("In Mid");
    int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height >= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 1;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 1;
                loop = 1;
            }
            if (boardTile[i - 1][j].height >= 2 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 1;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 1;
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
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i + 1][j].height >= 3 && !boardTile[i + 1][j].corrected && random == 0) {
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
        //printf("In !N&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i + 1][j].height >= 3 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 3 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !West) { //bottom left
        //printf("In !S&!W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i - 1][j].height >= 3 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 3 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !East) { //bottom right
       // printf("In !S&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i - 1][j].height >= 3 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 3 && !boardTile[i][j - 1].corrected && random == 1) {
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
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height >= 3 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 3 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 3 && !boardTile[i][j + 1].corrected && random == 2) {
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
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i - 1][j].height >= 3 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 3 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 3 && !boardTile[i][j + 1].corrected && random == 2) {
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
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height >= 3 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 3 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i - 1][j].height >= 3 && !boardTile[i - 1][j].corrected && random == 2) {
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
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height >= 3 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 3 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i - 1][j].height >= 3 && !boardTile[i - 1][j].corrected && random == 2) {
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
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height >= 3 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height >= 3 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i - 1][j].height >= 3 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height >= 3 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2;
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
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if ((boardTile[i + 1][j].height == 0 || boardTile[i + 1][j].height == 4) && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 1;
                loop = 1;
            }
            if ((boardTile[i][j + 1].height == 0 || boardTile[i][j + 1].height == 4) && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 1;
                loop = 1;
            }
            count++;
        }
    }
    else if (!North && !East) { //top left corner
        //printf("In !N&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if ((boardTile[i + 1][j].height == 0 || boardTile[i + 1][j].height == 4) && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 1;
                loop = 1;
            }
            if ((boardTile[i][j - 1].height == 0 || boardTile[i][j - 1].height == 4) && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 1;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !West) { //bottom left
        //printf("In !S&!W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if ((boardTile[i - 1][j].height == 0 || boardTile[i - 1][j].height == 4) && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 1;
                loop = 1;
            }
            if ((boardTile[i][j + 1].height == 0 || boardTile[i][j + 1].height == 4) && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 1;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !East) { //bottom right
       // printf("In !S&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if ((boardTile[i - 1][j].height == 0 || boardTile[i - 1][j].height == 4) && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 1;
                loop = 1;
            }
            if ((boardTile[i][j - 1].height == 0 || boardTile[i][j - 1].height == 4) && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 1;
                loop = 1;
            }
            count++;
        }
    }

    else if (!North) { //top row, excluding corners
       // printf("In !N");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if ((boardTile[i + 1][j].height == 0 || boardTile[i + 1][j].height == 4) && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 1;
                loop = 1;
            }
            if ((boardTile[i][j - 1].height == 0 || boardTile[i][j - 1].height == 4) && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 1;
                loop = 1;
            }
            if ((boardTile[i][j + 1].height == 0 || boardTile[i][j + 1].height == 4) && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 1;
                loop = 1;
            }
            count++;
        }
    }

    else if (!South) { //bottom row, excluding corners
    //printf("In !S");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if ((boardTile[i - 1][j].height == 0 || boardTile[i - 1][j].height == 4) && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 1;
                loop = 1;
            }
            if ((boardTile[i][j - 1].height == 0 || boardTile[i][j - 1].height == 4) && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 1;
                loop = 1;
            }
            if ((boardTile[i][j + 1].height == 0 || boardTile[i][j + 1].height == 4) && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 1;
                loop = 1;
            }
            count++;
        }
    }
    else if (!East) { //right most column, excluding corners
    //printf("In !E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if ((boardTile[i + 1][j].height == 0 || boardTile[i + 1][j].height == 4) && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 1;
                loop = 1;
            }
            if ((boardTile[i][j - 1].height == 0 || boardTile[i][j - 1].height == 4) && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 1;
                loop = 1;
            }
            if ((boardTile[i - 1][j].height == 0 || boardTile[i - 1][j].height == 4) && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 1;
                loop = 1;
            }
            count++;
        }
    }

    else if (!West) {
    //printf("In !W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if ((boardTile[i + 1][j].height == 0 || boardTile[i + 1][j].height == 4) && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 1;
                loop = 1;
            }
            if ((boardTile[i][j + 1].height == 0 || boardTile[i][j + 1].height == 4) && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 1;
                loop = 1;
            }
            if ((boardTile[i - 1][j].height == 0 || boardTile[i - 1][j].height == 4) && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 1;
                loop = 1;
            }
            count++;
        }
    }

    else {
    //printf("In Mid");
    int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if ((boardTile[i + 1][j].height == 0 || boardTile[i + 1][j].height == 4) && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 1;
                loop = 1;
            }
            if ((boardTile[i][j + 1].height == 0 || boardTile[i][j + 1].height == 4) && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 1;
                loop = 1;
            }
            if ((boardTile[i - 1][j].height == 0 || boardTile[i - 1][j].height == 4) && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 1;
                loop = 1;
            }
            if ((boardTile[i][j - 1].height == 0 || boardTile[i][j - 1].height == 4) && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 1;
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
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i + 1][j].height <= 1 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 1 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 2;
                loop = 1;
            }
            count++;
        }
    }
    else if (!North && !East) { //top left corner
       // printf("In !N&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i + 1][j].height <= 1 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 1 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 2;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !West) { //bottom left
       // printf("In !S&!W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i - 1][j].height <= 1 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 1 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 2;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !East) { //bottom right
       // printf("In !S&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i - 1][j].height <= 1 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 1 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 2;
                loop = 1;
            }
            count++;
        }
    }

    else if (!North) { //top row, excluding corners
       // printf("In !N");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height <= 1 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 1 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 1 && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 2;
                loop = 1;
            }
            count++;
        }
    }

    else if (!South) { //bottom row, excluding corners
    //printf("In !S");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i - 1][j].height <= 1 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 1 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 1 && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 2;
                loop = 1;
            }
            count++;
        }
    }
    else if (!East) { //right most column, excluding corners
    //printf("In !E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height <= 1 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 1 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 2;
                loop = 1;
            }
            if (boardTile[i - 1][j].height <= 1 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 2;
                loop = 1;
            }
            count++;
        }
    }

    else if (!West) {
    //printf("In !W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height <= 1 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 1 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 2;
                loop = 1;
            }
            if (boardTile[i - 1][j].height <= 1 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 2;
                loop = 1;
            }
            count++;
        }
    }

    else {
    //printf("In Mid");
    int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height <= 1 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 2 + 2;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 1 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 2 + 2;
                loop = 1;
            }
            if (boardTile[i - 1][j].height <= 1 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 2 + 2;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 1 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 2 + 2;
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
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i + 1][j].height <= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 1 + 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 1 + 3;
                loop = 1;
            }
            count++;
        }
    }
    else if (!North && !East) { //top left corner
       // printf("In !N&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i + 1][j].height <= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 1 + 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 1 + 3;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !West) { //bottom left
       // printf("In !S&!W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i - 1][j].height <= 2 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 1 + 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 1 + 3;
                loop = 1;
            }
            count++;
        }
    }
    else if (!South && !East) { //bottom right
       // printf("In !S&!E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 1;

            if (boardTile[i - 1][j].height <= 2 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 1 + 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 1 + 3;
                loop = 1;
            }
            count++;
        }
    }

    else if (!North) { //top row, excluding corners

       // printf("In !N");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height <= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 1 + 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 1 + 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 2 && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 1 + 3;
                loop = 1;
            }
            count++;
        }
    }

    else if (!South) { //bottom row, excluding corners
    //printf("In !S");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i - 1][j].height <= 2 && !boardTile[i - 1][j].corrected && random == 0) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 1 + 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 1 + 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 2 && !boardTile[i][j + 1].corrected && random == 2) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 1 + 3;
                loop = 1;
            }
            count++;
        }
    }
    else if (!East) { //right most column, excluding corners
    //printf("In !E");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height <= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 1 + 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 1 + 3;
                loop = 1;
            }
            if (boardTile[i - 1][j].height <= 2 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 1 + 3;
                loop = 1;
            }
            count++;
        }
    }

    else if (!West) {
    //printf("In !W");
        int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height <= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 1 + 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 1 + 3;
                loop = 1;
            }
            if (boardTile[i - 1][j].height <= 2 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 1 + 3;
                loop = 1;
            }
            count++;
        }
    }

    else {
    //printf("In Mid");
    int count = 0;
        int loop = 0;
        while (loop == 0 && count <= 4) {
            int random = rand() % 2;

            if (boardTile[i + 1][j].height <= 2 && !boardTile[i + 1][j].corrected && random == 0) {
                boardTile[i + 1][j].corrected = 1;
                boardTile[i + 1][j].height = rand() % 1 + 3;
                loop = 1;
            }
            if (boardTile[i][j + 1].height <= 2 && !boardTile[i][j + 1].corrected && random == 1) {
                boardTile[i][j + 1].corrected = 1;
                boardTile[i][j + 1].height = rand() % 1 + 3;
                loop = 1;
            }
            if (boardTile[i - 1][j].height <= 2 && !boardTile[i - 1][j].corrected && random == 2) {
                boardTile[i - 1][j].corrected = 1;
                boardTile[i - 1][j].height = rand() % 1 + 3;
                loop = 1;
            }
            if (boardTile[i][j - 1].height <= 2 && !boardTile[i][j - 1].corrected && random == 1) {
                boardTile[i][j - 1].corrected = 1;
                boardTile[i][j - 1].height = rand() % 1 + 3;
                loop = 1;
            }
            count++;
        }
    }
}


char getAns(char mesg[]) {
    char ans;
    printf("%s", mesg);
    ans = getch();
    return tolower(ans);
}


void pieceSelectOld() {
    char ch = 0;
    int newX, newY; 
    int count = 0;
    int oldX, oldY;
    int exitCode;
    bool moveSelected = false;
    

    printf(" Press Q to skip\n");
    do
    {
        ch = getch();
        ch = tolower(ch);
        

        switch (ch)
        {
        case'x':
            if (boardTile[cursorPosY][cursorPosX].occupied == false) {
                printf("Tile is empty\n");
            }
            else {
                oldX = cursorPosX; oldY = cursorPosY;
                pieceSelectNew(oldX, oldY);
            }
            break;
        case 'w':
            if (cursorPosY - 1 >= 0) {
                boardTile[cursorPosY][cursorPosX].cursor = false;
                cursorPosY = cursorPosY - 1;
                boardTile[cursorPosY][cursorPosX].cursor = true;
                DrawBoard();
            }
            else {
                printf("Out of bounds %d, %d\n", cursorPosY, cursorPosY);
            }
            break;
        case 'a':
            if (cursorPosX - 1 >= 0) {
                boardTile[cursorPosY][cursorPosX].cursor = false;
                cursorPosX = cursorPosX - 1;
                boardTile[cursorPosY][cursorPosX].cursor = true;
                DrawBoard();
            }
            else {
                printf("Out of bounds %d, %d\n", cursorPosX, cursorPosY);
            }
            break;
        case 's':
            if (cursorPosY + 1 <= 4) {
                boardTile[cursorPosY][cursorPosX].cursor = false;
                cursorPosY = cursorPosY + 1;
                boardTile[cursorPosY][cursorPosX].cursor = true;
                DrawBoard();
            }
            else {
                printf("Out of bounds %d, %d\n", cursorPosX, cursorPosY);
            }
            break;
        case 'd':
            if (cursorPosX + 1 <= 4) {
                boardTile[cursorPosY][cursorPosX].cursor = false;
                cursorPosX = cursorPosX + 1;
                boardTile[cursorPosY][cursorPosX].cursor = true;
                DrawBoard();
            }
            else {
                printf("Out of bounds %d, %d\n", cursorPosX, cursorPosY);
            }
            break;

        case'q':
            printf("Turn skipped");
            break;
        }
    } while (ch != 'q' && !pieceMoved);
}

void pieceSelectNew(int oldX, int oldY) {
    int newX, newY, dir;
    bool x, moveSelected = false;
    char ch = 0;
    int count = 0;
    double *movPts = 5;
   
    printf(" Press Q to unselect\n");
    do
    {
        ch = getch();
        ch = tolower(ch);

        switch (ch)
        {
        case'x':
            if (boardTile[cursorPosY][cursorPosX].occupied == false)
            {
                newX = cursorPosX; newY = cursorPosY;
                if (count == 0) { //error checking to prevent multiple moves
                    MovePiece(oldY, oldX, newY, newX);
                    count++;
                }
            }
            else {
                printf("New tile is occupied\n");
            }
            break;
        case 'w':
            if (cursorPosY - 1 >= 0) {  
                dir = 1;
                if (MoveRules(cursorPosX, cursorPosY, dir, &movPts)) {
                    boardTile[cursorPosY][cursorPosX].cursor = false;
                    cursorPosY = cursorPosY - 1;
                    boardTile[cursorPosY][cursorPosX].cursor = true;
                    DrawBoard();
                }
            }
            else {
                printf("Out of bounds %d, %d\n", cursorPosX, cursorPosY);
            }
            break;
        case 'a':
            if (cursorPosX - 1 >= 0) {
                dir = 2;
                MoveRules(cursorPosX, cursorPosY, dir);
            }
            else {
                printf("Out of bounds %d, %d\n", cursorPosX, cursorPosY);
            }
            break;
        case 's':
            if (cursorPosY + 1 <= 4) {
                dir = 3;
                MoveRules(cursorPosX, cursorPosY, dir);
            }
            else {
                printf("Out of bounds %d, %d\n", cursorPosX, cursorPosY);
            }
            break;
        case 'd':
            if (cursorPosX + 1 <= 4) {
                dir = 4;
                MoveRules(cursorPosX, cursorPosY, dir);
            }
            else {
                printf("Out of bounds %d, %d\n", cursorPosX, cursorPosY);
            }
            break;

        case 'q':
            printf("Piece at %d, %d was unselected\n", oldX, oldY);
            break;
        }
    } while (ch != 'q' && !pieceMoved && (movPts > 0 || movPts < 1));

}



bool MoveRules(int cursorPosX, int cursorPosY, int dir, double *movPts) {
    bool x = false;

    printf("CursorPosX = %d, CursorPosY = %d,", cursorPosX, cursorPosY);

    if (dir == 1) {
        if (boardTile[cursorPosY - 1][cursorPosX].height - boardTile[cursorPosY][cursorPosX].height == 0 && *movPts >= 1) { //if height is the same level
            printf("Same height\n");
            *movPts--;
            x = true;
        }
        else if (boardTile[cursorPosY - 1][cursorPosX].height - boardTile[cursorPosY][cursorPosX].height == 1 && *movPts >= 1.5) //if height is one above prev
        {
            printf("Height one above\n");
            *movPts -= 1.5;
            x = true;
        }
        else if (boardTile[cursorPosY - 1][cursorPosX].height - boardTile[cursorPosY][cursorPosX].height == -1 && *movPts >= 1.25) { //if height is one below prev
            printf("Height one below\n");
            *movPts -= 1.25;
            x = true;
        }
        else if ((boardTile[cursorPosY - 1][cursorPosX].height - boardTile[cursorPosY][cursorPosX].height == -3 || boardTile[cursorPosY - 1][cursorPosX].height - boardTile[cursorPosY][cursorPosX].height == -2) && *movPts >= 2) {
            printf("Falling\n");
            //the case for falling
            *movPts = 0;
            x = true;
        }
        else if (boardTile[cursorPosY - 1][cursorPosX].height - boardTile[cursorPosY][cursorPosX].height == 2 && *movPts >= 2) {
            printf("Climbing\n");
            //the case for climbing
            *movPts = 0;
            x = true;
        }
        else {
            printf("Tile is not accessible from this one\n");
            x = false;
        }
        printf("MovPts: %.2f\n", *movPts);
    }

    else if (dir == 2)
    {
        if (boardTile[cursorPosY][cursorPosX - 1].height - boardTile[cursorPosY][cursorPosX].height == 0 && *movPts >= 1) { //if height is the same level
            printf("Same height\n");
            //if board tile was travelled subtract movPts
            *movPts--;
            x = true;
        }
        else if (boardTile[cursorPosY][cursorPosX - 1].height - boardTile[cursorPosY][cursorPosX].height == 1 && *movPts >= 1.5) //if height is one above prev
        {
            printf("Height one above\n");

            *movPts -= 1.5;
            x = true;
        }
        else if (boardTile[cursorPosY][cursorPosX - 1].height - boardTile[cursorPosY][cursorPosX].height == -1 && *movPts >= 1.25) { //if height is one below prev
            printf("Height one below\n");
            *movPts -= 1.25;
            x = true;
        }
        else if ((boardTile[cursorPosY][cursorPosX - 1].height - boardTile[cursorPosY][cursorPosX].height == -3 || boardTile[cursorPosY][cursorPosX - 1].height - boardTile[cursorPosY][cursorPosX].height == -2) && *movPts >= 2) {
            printf("Falling\n");
            //the case for falling
            *movPts = 0;
            x = true;
        }
        else if (boardTile[cursorPosY][cursorPosX - 1].height - boardTile[cursorPosY][cursorPosX].height == 2 && *movPts >= 2) {
            printf("Climbing\n");
            //the case for climbing
            *movPts = 0;
            x = true;
        }
        else {
            printf("Tile is not accessible from this one\n");
            x = false;
        }
        printf("MovPts: %.2f\n", *movPts);
    }
    
    else if (dir == 3)
    {
        if (boardTile[cursorPosY + 1][cursorPosX].height - boardTile[cursorPosY][cursorPosX].height == 0 && *movPts >= 1) { //if height is the same level
            printf("Same height\n");
            //if board tile was travelled subtract movPts
            *movPts--;
            x = true;
        }
        else if (boardTile[cursorPosY + 1][cursorPosX].height - boardTile[cursorPosY][cursorPosX].height == 1 && *movPts >= 1.5) //if height is one above prev
        {
            printf("Height one above\n");
            *movPts -= 1.5;
            x = true;
        }
        else if (boardTile[cursorPosY + 1][cursorPosX].height - boardTile[cursorPosY][cursorPosX].height == -1 && *movPts >= 1.25) { //if height is one below prev
            printf("Height one below\n");
            *movPts -= 1.25;
            x = true;
        }
        else if ((boardTile[cursorPosY + 1][cursorPosX].height - boardTile[cursorPosY][cursorPosX].height == -3 || boardTile[cursorPosY - 1][cursorPosX].height - boardTile[cursorPosY][cursorPosX].height == -2) && *movPts >= 2) {
            printf("Falling\n");
            //the case for falling
            *movPts = 0;
            x = true;
        }
        else if (boardTile[cursorPosY + 1][cursorPosX].height - boardTile[cursorPosY][cursorPosX].height == 2 && *movPts >= 2) {
            printf("Climbing\n");
            //the case for climbing
            *movPts = 0;
            x = true;
        }
        else {
            printf("Tile is not accessible from this one\n");
            x = false;
        }
        printf("MovPts: %.2f\n", *movPts);
    }

    else if (dir == 4)
    {
       if (boardTile[cursorPosY][cursorPosX + 1].height - boardTile[cursorPosY][cursorPosX].height == 0 && *movPts >= 1) { //if height is the same level
            printf("Same height\n");
            //if board tile was travelled subtract movPts
            *movPts--;
            x = true;
            
        }
        else if (boardTile[cursorPosY][cursorPosX + 1].height - boardTile[cursorPosY][cursorPosX].height == 1 && *movPts >= 1.5) //if height is one above prev
        {
            printf("Height one above\n");
            *movPts -= 1.5;
            x = true;
        }
        else if (boardTile[cursorPosY][cursorPosX + 1].height - boardTile[cursorPosY][cursorPosX].height == -1 && *movPts >= 1.25) { //if height is one below prev
            printf("Height one below\n");
            *movPts -= 1.25;
            x = true;
        }
        else if ((boardTile[cursorPosY][cursorPosX + 1].height - boardTile[cursorPosY][cursorPosX].height == -3 || boardTile[cursorPosY][cursorPosX - 1].height - boardTile[cursorPosY][cursorPosX].height == -2) && *movPts >= 2) {
            printf("Falling\n");
            //the case for falling
            *movPts = 0;
            x = true;
        }
        else if (boardTile[cursorPosY][cursorPosX + 1].height - boardTile[cursorPosY][cursorPosX].height == 2 && *movPts >= 2) {
            printf("Climbing\n");
            //the case for climbing
            *movPts = 0;
            x = true;
        }
        else {
            printf("Tile is not accessible from this one\n");
            x = false;
        }
        printf("MovPts: %.2f\n", *movPts);
    }

    return x;
}


