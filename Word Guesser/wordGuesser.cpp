#include "GA.hpp"
/*
 

How to make the libraries universal
===================================
(Arch)
sudo cp #####.hpp /usr/local/include/

(Windows)
place .hpp files into 
C:\Program Files\mingw64\include\


*/

//Creates the character set the GA may use
std::vector<std::string> createBounds(){
    std::vector<std::string> tempList;
    for(int i = 36; i < 127; i++){
        char chr = char(i);
        tempList.push_back(std::string(1,chr));
    }
    tempList.push_back(" ");
    return tempList;
}

//Fitness function for this example
std::vector<float> fitnessFunction(std::vector<DNA> allDNA, std::string target){
    std::string goal = target;
    std::vector<float> allFitness;

    for(auto& DNA : allDNA){
        float correct = 0;
        for(int i = 0; i < goal.length(); i++){
            if(DNA.getValueAt(i) == std::string(1,goal[i])){correct++;}
        }
        float percent = correct / goal.length();
        allFitness.push_back(percent);
    }
    return allFitness;
}

std::string fileName = "wordGuessSave.txt";
std::string target = "dkafgow g8ooq3g ro4qg2igr 892y3pq 34rqg ' '    arq k 9 23";

int main(){
    //initialise the RNG so multiple GA's can be used
    initRNG();
    std::vector<std::string> charBounds = createBounds();
    //Instantiating the GA class
    GA ga(100,target.size(),0.03,fileName, charBounds, 1);
    ga.makeFirstGen();
    //ga.loadGen();
    int i = 0;
    while(true){
        ga.setFitness(fitnessFunction(ga.getDNAList(), target));
        ga.sortFitness();
        ga.crossover(4, 20);
        ga.applyMutation();
        //ga.changeMutation(0.01, 0.01, 0.1);
        i++;
        if(i % 1 == 0){
            ga.initScreen();
            ga.drawBars(10);
            ga.showInfo(target.size());
            ga.saveGen();
            ga.printBuffer();
            if(ga.hasGotten()){
                ga.pause();
            }
        }
    }
    return 0;
}

