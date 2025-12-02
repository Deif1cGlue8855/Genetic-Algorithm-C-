#include <GA.hpp>
#include <raylib.h>
#include <chrono>

//To compile raylib
//!g++ main.cpp -o game -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

int map[10][10] = {
{1,1,1,1,1,1,1,1,1,1},
{1,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,0,0,1},
{1,1,1,1,1,1,1,1,1,1}};

//Inputs are U,D,L,R
std::vector<int> simulator(DNA testDNA, int start[2], int goal[2]){
    int testPos[2] = {start[0], start[1]};
    int moves = 0; for(int i = 0; i < testDNA.getLength(); i++){ std::string command = testDNA.getValueAt(i);

        if(command == "U" && map[testPos[1] - 1][testPos[0]] != 1){
            testPos[1] -= 1;
        }
        else if(command == "D" && map[testPos[1] + 1][testPos[0]] != 1){
            testPos[1] += 1;
        }
        else if(command == "L" && map[testPos[1]][testPos[0] - 1] != 1){
            testPos[0] -= 1;
        }
        else if(command == "R" && map[testPos[1]][testPos[0] + 1] != 1){
            testPos[0] += 1;
        }
        moves += 1;

        if(testPos == goal){break;}
    }

    std::vector<int> ret = {testPos[0], testPos[1], moves};
    //Returns X, Y, how many moves that took
    return ret;
}

DNA testInp;

int main(){
    InitWindow(1000,1000,"GA Path finder");
    testInp.addGene("U");
    testInp.addGene("L");
    testInp.addGene("L");
    testInp.addGene("L");

    int start[2] = {1,1};
    int goal[2] = {8,8};

    std::vector<int> result = simulator(testInp, start, goal);

    for(auto& x: result){std::cout << x << std::endl;}

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(GRAY);
        //Prints the map
        for(int y = 0; y < 10; y++){
            for(int x = 0; x < 10; x++){
                Color colour = GRAY;

                if(x == start[0] && y == start[1]){
                    colour = RED;
                }
                else if(x == goal[0] && y == goal[1]){
                    colour = GREEN;
                }
                else if(map[y][x] == 1){
                    colour = BLACK;
                }

                DrawRectangle(100 * x, 100 * y, 100, 100, colour);
            }
        }    


        EndDrawing();
    }
    CloseWindow();
    return 0;
}
