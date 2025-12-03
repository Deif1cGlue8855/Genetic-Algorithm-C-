#include <GA.hpp>
#include <cmath>
#include <raylib.h>
#include <chrono>
#include <unistd.h>

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

struct point{
 int x, y;
};

//Inputs are U,D,L,R
std::vector<point> simulator(DNA testDNA, int start[2], int goal[2]){
    point testPos = {start[0], start[1]};
    int moves = 0; 
    bool gotIt = false;
    
    std::vector<point> Path = {};

    for(int i = 0; i < testDNA.getLength(); i++){ 

        std::string command = testDNA.getValueAt(i);

        if(command == "U" && map[testPos.y - 1][testPos.x] != 1){
            testPos.y -= 1;
        }
        else if(command == "D" && map[testPos.y + 1][testPos.x] != 1){
            testPos.y += 1;
        }
        else if(command == "L" && map[testPos.y][testPos.x - 1] != 1){
            testPos.x -= 1;
        }
        else if(command == "R" && map[testPos.y][testPos.x + 1] != 1){
            testPos.x += 1;
        }
        if(!gotIt){
            moves += 1;
        }


        Path.push_back(testPos);

        if(testPos.x == goal[0] && testPos.y == goal[1]){gotIt = true;}
    }
    //Returns X, Y, how many moves that took
    return Path;
}

std::vector<float> fitnessFunction(std::vector<std::vector<point>> poses, int goal[2], int boardArea){
    float dX, dY;
    std::vector<float> allFitnesses;

    for(int i = 0; i < poses.size(); i++){

        dX = poses[i][poses[i].size() - 1].x - goal[0];
        dY = poses[i][poses[i].size() - 1].y - goal[1];

        float dist = std::sqrt(pow(dX,2) + std::pow(dY,2));

        float fitness = (boardArea - (dist * (1.0 / poses[i].size())));

        allFitnesses.push_back(fitness);
    }

    return allFitnesses;

}

int main(){
    InitWindow(1000,1000,"GA Path finder");

    int start[2] = {1,1};
    int goal[2] = {8,8};

    //Sets the inputs to be only U, D, L and R
    std::vector<std::string> charSet = {"U", "D", "L", "R"};
    //Instantiating the GA
    GA ga(300,40, 0.03, "PathSave.txt", charSet, 1);
    //Create the first gen 
    ga.makeFirstGen();

    int it = 0;
    std::vector<std::vector<point>> paths;

    while(!WindowShouldClose()){
        
        if(it == 11){
            it = 0;
        }

        if(it == 0){
            paths.clear();
            for(DNA x : ga.getDNAList()){
                paths.push_back(simulator(x, start, goal));
            }

            ga.setFitness(fitnessFunction(paths, goal, 100));
            ga.sortFitness();
            ga.crossover(3, 20);
            ga.applyMutation();
            ga.saveGen();
            
        }
        else{

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
            
            for(auto& x : paths){
                DrawRectangle(x[it-1].x * 100, x[it-1].y * 100, 100, 100, YELLOW);
            }
            //INFO / DEBUG PANEL

            DrawText(ga.getBestGuess().c_str(),0,0,20, WHITE);
            DrawText(std::to_string(ga.getCorrectGuessGen()).c_str(),0,20,20, WHITE);


            EndDrawing();
        }

        it++;
        ga.showInfo(10);
    }
    CloseWindow();
    return 0;
}
