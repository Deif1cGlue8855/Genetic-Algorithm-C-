#include "GA.hpp"

//Creates the character set the GA may use
std::vector<std::string> createBounds(){
    std::vector<std::string> tempList;
    for(int i = 36; i < 127; i++){
        //Loops through the numbers 36 -> 127
        //and converts the numbers ti their ASCII
        //Characters which are store in an array
        char chr = char(i);
        tempList.push_back(std::string(1,chr)); }
    tempList.push_back(" ");
    return tempList;
}
//Fitness function for this example
std::vector<float> fitnessFunction(GA& ga, std::string target){
    std::vector<DNA> allDNA = ga.getDNAList();
    std::string goal = target;
    std::vector<float> allFitness;
    /*
    Loops through each letter in the current guess it's looking at and compares it to 
    the letter in the same place in the goal string, eventually resulting with a percentage 
    on how accurate the guess was
    */
    for(auto& DNA : allDNA){
        float correct = 0;
        for(int i = 0; i < goal.length(); i++){
            if(DNA.getValueAt(i) == std::string(1,goal[i])){correct++;}
        }
        float percent = correct / goal.length();
        if(percent == 1){ga.gotCorrect();}
        allFitness.push_back(percent);
    }
    return allFitness;
}
//Location to be saved at
std::string fileName = "wordGuessSave.txt";
//Target string
std::string target = "If you can read this, you can read";

int main(){
    //Initialises RNG so multiple GA's can be used
    initRNG();

    //Gets all the characters the GA will be able to use
    std::vector<std::string> charBounds = createBounds();
    //Initialises the GA
    GA ga(100,target.size(),0.03, 0.4,fileName, charBounds, 1);
    //Creates the first generation
    ga.makeFirstGen();
    int i = 0;
    //Loops the learning forever
    while(true){
        //Learning process functions
        ga.setFitness(fitnessFunction(ga, target));
        ga.sortFitness();
        ga.crossover(10, 20, 3);
        ga.applyMutation();
        ga.changeMutation();
        i++;
        //Only show the graphics every 1 generation 
        if(i % 1 == 0){
            ga.barGraph(10);
            //ga.lineGraph();
            ga.showInfo(target.size());
            ga.saveGen();
            ga.printBuffer();
            //ga.pause();

            if(ga.hasGotten()){
                ga.pause();
            }
        }
    }
    return 0;
}
