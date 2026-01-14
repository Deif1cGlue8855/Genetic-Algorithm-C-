#ifndef GA_HPP
#define GA_HPP

/*
TO MAKE IT GLOBAL:

sudo cp GA.hpp /usr/local/include/

How to use UTF-8 on powershell:
$OutputEncoding = [Console]::OutputEncoding = [Text.UTF8Encoding]::UTF8

TO DO LIST:
- Reduce mutation rate functions (differernt ways)
- More graph types
- Add some way of the number of DNA selected for Crossover gets reduced
- Change the 'has gotten' function so it can be changed by the user 
- Change Documentation for crossover rate
- Add all the getters as a version in word thingy

AMENDED:
- Crossover function redone 
- Crossover rate used now
- Added elitism

*/

//Tests if the OS is windoes
#if defined(_WIN32) || defined(_WIN64)
//uses windows library
    #include <windows.h>
#else
//Else use mac / linux libraries
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include <vector>
#include <iostream>
#include <string>
#include "DNA.hpp"
#include <cmath>
#include <csignal>
#include <utility>
#include <sstream>
#include <fstream>
#include <filesystem>

struct RGB{
    int r,g,b;
};

class GA{

    private: 
        //All of the attributes for the class
        float mutationRate;
        float crossoverRate;
        float baseMutationRate;
        int simCount = 0;

        int numOfDNA;
        int DNALength;

        std::string saveLocation;

        std::vector<std::string> allPosVals;

        std::string bestGuess;
    
        //GA DNA management
        std::vector<DNA> DNAList;
        std::vector<float> fitnesses;
        std::vector<float> averageFitness;
        int generation = 1;

        //ALl for printing on screen
        std::ostringstream buffer;
        int screenSize[2] = {0,0};
        int barAmount = 0;
        int barLength = 0;
        bool correctGuess = false;
        int correctGuessGen = -1;

    
        //Preset colours for graphics
        RGB white = {255,255,255};
        RGB gruvGreen = {142,192,124};
        RGB gruvRed = {204,36,29};
        RGB gruvCream = { 235, 219,178};
        RGB gruvBlue = {69,113,136};
        RGB gruvYellow = {215,153,33};
        //Character sets for different themes
        int charSet = 0;
        std::string charSets[4][8] = {
        {"-","|","+","+","+","+","#","."},
        {"═","║","╔","╗","╚","╝","█","▒"},
        {"─","│","╭","╮","╰","╯","●","○"},
        {"─","│","┌","┐","└","┘","■","□"}
        };

        //PRIVATE METHODS
        
        //Gene manipulationn
        std::string genRandGene();
        void setDNA(std::vector<DNA> importList);

        //Quick sort methods
        void swapIndexes(int iOne, int iTwo);
        void quickSort(int lp, int hp);
        int part(int lp, int hp);

        //Saving methods
        bool checkSaveLocation(bool createNew);
        
        //ANSI print methods
        void printAt(std::string text, int x, int y, const RGB& colour);
        void getTerminalDim();
        static void cleanUp(int n);
        static void sendError(std::string error);
        
        //Data visualisation methods
        void bar(int w, int l,int full, std::string message, const RGB& colour);

    public:
        //METHODS THAT THE USER WILL BE ABLE TO USE

        //Instantiation method 
        GA(int numOfDNA, int DNALength, float mutationRate, float crossoverRate, std::string saveLocation,std::vector<std::string> allPosVals, int charSet);
        //Any other methods
        void showInfo(int width);

        int getNumOfDNA();
        int getDNALength();
        float getMutationRate();
        std::string getSaveLocation();
        int getCorrectGuessGen();
        std::string getBestGuess();
        int getGenNumber();
        float getBestFitness();
        float getCrossoverRate();
        std::vector<float> getFitnesses();
        
        //Learning methods
        void sortFitness();

        void setFitness(std::vector<float> newFitness);
        //Slicing methods
        void crossover(int numOfSections, int topGenes, int elites);
        //Applies the mutation to the new generation
        void applyMutation();
        void changeMutation();
    
        //Modifiying the main DNA list
        void makeFirstGen();
        std::vector<DNA> getDNAList();

        bool hasGotten();
        
        //Saving methods
        void saveGen();
        void loadGen();

        //Data visualisation methods
        void barGraph(int numPrevAverage);
        void lineGraph();
        void initScreen();
        void clearScreen();
        void printBuffer();
        void pause();
        
     
};

//METHOD WRITING

inline GA::GA(int numOfDNA, int DNALength, float mutationRate, float crossoverRate, std::string saveLocation,std::vector<std::string> allPosVals, int charSet){
    this->charSet = charSet;
    if(mutationRate <= 0 || mutationRate >= 1){sendError("Mutation rate outside of possible range");}
    this->mutationRate = mutationRate;
    this->baseMutationRate = mutationRate;
    this->numOfDNA = numOfDNA;
    this->DNALength = DNALength; 
    this->saveLocation = saveLocation;
    this->allPosVals = allPosVals;
    this->crossoverRate = crossoverRate;
    //std::signal(SIGINT, cleanUp);
}

inline void GA::sendError(std::string message){
    //exits the 
    std::cout << "\033[?1049l";
    std::cout.flush();

    std::cout << "\x1b[?25h";
    std::cout << "\u001b[31m[ERROR] " + message + "\u001b[0m" << std::endl;
    exit(1);
}

//Used for debugging, it shows all data on the GA 
inline void GA::showInfo(int w){

    int width = w;
    int height = 20;

    if(width < 20){width = 20;}

    int line = 1;
    int accross = 1;

    std::pair<std::string, RGB> nLine = {"", white};

    //Prints the boarder
    for(int i = 1; i < height; i++){
        printAt(this->charSets[this->charSet][1], this->screenSize[1] - width, i, gruvCream);
    }
    for(int i = width; i >= 0; i--){
        if(i == width){
            printAt(this->charSets[this->charSet][4], this->screenSize[1] - i, height, gruvCream);
        }
        else{
            printAt(this->charSets[this->charSet][0], this->screenSize[1] - i, height, gruvCream);
        }
    }
    //Covers background
    for(int h = 1; h < height; h++){
        for(int w = width - 1; w >= 0; w--){
            printAt(" ", this->screenSize[1] - w, h, white);
        }
    }
    std::string bGuess = bestGuess;
    if(bGuess.length() > width){bGuess = "Too long";}
    std::vector<std::pair<std::string, RGB>> statsToPrint = {
    //Number of DNA
    {"No. DNA: ", gruvCream}, nLine, {std::to_string(this->numOfDNA), gruvRed}, nLine,
    //DNA length
    {"DNA length: ", gruvCream}, nLine, {std::to_string(this->DNALength), gruvRed}, nLine,
    //Current Generation
    {"Current generation: ", gruvCream}, nLine, {std::to_string(this->generation), gruvBlue}, nLine,
    //Mutation rate stats
    {"Original M.rate: ", gruvCream}, nLine, {std::to_string(this->baseMutationRate), gruvGreen}, nLine,
    {"Current M.rate: ", gruvCream}, nLine, {std::to_string(this->baseMutationRate), gruvGreen}, nLine,
    //Terminal size
    {"Terminal size: ", gruvCream}, nLine, {"[", gruvCream}, {std::to_string(this->screenSize[0]), gruvRed}, {",", gruvCream}, {std::to_string(this->screenSize[1]), gruvRed},{"]", gruvCream}, nLine,
    //Shows best fitness
    {"Gen best fitness: ", gruvCream}, nLine, {std::to_string(this->fitnesses[0]), gruvYellow}, nLine,
    //Shows the generation it got it at
    {"Got at gen: ", gruvCream}, nLine, {std::to_string(this->correctGuessGen), gruvBlue}, nLine,
    //Shows best guess
    {"Best guess: ", gruvCream}, nLine, {bGuess, gruvYellow}, nLine
    };
    for(int i = 0; i < statsToPrint.size(); i++){
        if(statsToPrint[i].first != ""){
            printAt(statsToPrint[i].first, this->screenSize[1] - width + accross , line, statsToPrint[i].second);
            accross += statsToPrint[i].first.length();
        }
        else{
            line++;
            accross = 1;
        }
    }
}

//Getter methods
inline int GA::getDNALength(){return this->DNALength;}
inline int GA::getNumOfDNA(){return this->numOfDNA;}
inline float GA::getMutationRate(){return this->mutationRate;}
inline std::string GA::getSaveLocation(){return this->saveLocation;}
inline int GA::getCorrectGuessGen(){return this->correctGuessGen;}
inline std::string GA::getBestGuess(){return this->bestGuess;}
inline int GA::getGenNumber(){return this->generation;}
inline float GA::getBestFitness(){return this->fitnesses[0];}
inline float GA::getCrossoverRate(){return this->crossoverRate;}
inline std::vector<float> GA::getFitnesses(){return this->fitnesses;}
//EXISTED BEFORE
inline std::vector<DNA> GA::getDNAList(){return this->DNAList;}

//Quick sort methods
inline void GA::sortFitness(){
    //Quick sort to sort the fitnesses and their corrisponding DNA strands
    quickSort(0, this->numOfDNA - 1);
    DNA tList = this->DNAList[0];
    std::string tempStr;
    for(int i = 0; i < tList.getLength(); i++){
        tempStr += tList.getValueAt(i);
    }
    bestGuess = tempStr;
    /*
    if(this->fitnesses[0] == 1 && !this->correctGuess){
        this->correctGuess = true; 
        this->correctGuessGen = this->generation;
    }
    */
}

inline void GA::quickSort(int lp, int hp){
    if(lp < hp){
        int partition = part(lp,hp);

        quickSort(lp, partition - 1);
        quickSort(partition + 1, hp);
    }
}

inline int GA::part(int lp, int hp){
    float pivot = this->fitnesses[hp];

    int finSwap = lp - 1;

    for(int i = lp; i < hp; i++){
        if(this->fitnesses[i] > pivot && finSwap != i){
            finSwap++;
            swapIndexes(finSwap, i);
        }
    }

    swapIndexes(finSwap + 1, hp);
    return finSwap + 1;
}

inline void GA::swapIndexes(int iOne, int iTwo){
    //Swaps the vaues in the the fitnesses
    float tempFit = this->fitnesses[iOne];
    this->fitnesses[iOne] = this->fitnesses[iTwo];
    this->fitnesses[iTwo] = tempFit;
    
    //Swaps the DNA strands around
    DNA tempDNA = this->DNAList[iOne];
    this->DNAList[iOne] = this->DNAList[iTwo];
    this->DNAList[iTwo] = tempDNA;
}

//Sets the fitnesses to a list
inline void GA::setFitness(std::vector<float> newFitness){
    if(newFitness.size() != this->numOfDNA){sendError("Number of fitnesses don't align with number of DNA");}
    this->fitnesses = newFitness;
    float total = 0;
    //for the average fitness
    for(auto& n : this->fitnesses){total += n;}
    this->averageFitness.push_back(total/this->numOfDNA);
}
//Slice current genes
inline void GA::crossover(int numOfSections, int topGenes, int elites){
    if(topGenes > this->numOfDNA){sendError("Crossover too many genes selected");}
    std::vector<DNA> newGen;
    DNA tempDNA;
    int cutPoint = this->DNALength/numOfSections;
    //For loop for whole new DNA strand
    for(int i = 0; i < elites; i++){
        newGen.push_back(this->DNAList[i]);
    }
    for(int i = 0; i < this->numOfDNA - elites; i++){
        //Selecting the differetn sections 
        int p1 = (rand() % topGenes);
        int p2 = (rand() % topGenes);
        for(int j = 0; j < this->DNALength; j+= cutPoint){
            //Adds the GENES in the individual sections
            for(int k = 0; k < cutPoint; k++){
                if(j + k < this->DNALength){
                    //ADD CROSSOVER RATE 
                    double boundry = static_cast<double>(rand()) / RAND_MAX;
                    if(boundry <= this->crossoverRate){
                        tempDNA.addGene(this->DNAList[p2].getValueAt(j+k));
                    }
                    else{
                        tempDNA.addGene(this->DNAList[p1].getValueAt(j+k));
                    }
                }
            }
        }
        //Finished making a DNA strand
        newGen.push_back(tempDNA);
        tempDNA.clearDNA();
    }
    this->DNAList = newGen;
    this->generation ++;
}

//Applies mutation to current generation (hopefully once it's been set to the new generation)
inline void GA::applyMutation(){
    for(auto& DNAstrands :this->DNAList){
        for(int i = 0; i < DNAstrands.getLength(); i++){
            double boundry = static_cast<double>(rand()) / RAND_MAX;
            if(boundry < this->mutationRate){
                DNAstrands.setValueAt(genRandGene(), i);
            }
        }
    }
}

//STILL NEEDS WORKING ON
inline void GA::changeMutation(){
    this->mutationRate = this->baseMutationRate * std::exp(-0.001 * this->generation);
}

//Modifying the gene list
inline void GA::setDNA(std::vector<DNA> tempLst){
    //Setter to change the whole DNA list in the GA class
    this->DNAList = tempLst; }

inline std::string GA::genRandGene(){
    //Generates a DNA with random genes from the inputted list
    int randNum = rand() % this->allPosVals.size();
    return this->allPosVals[randNum];
}

inline void GA::makeFirstGen(){
    //Will create a first generation based on the different possible inputs it's been given
    for(int i = 0; i < this->numOfDNA; i++){
        DNA tempDNA;
        for(int j = 0; j < this->DNALength; j++){
            //Uses RNG to create the first generation, hence no bias from the user
            tempDNA.addGene(genRandGene());
        }
        DNAList.push_back(tempDNA);
    }
}


//Saving and reading from text file
inline bool GA::checkSaveLocation(bool createNew){
    
    //MODIFY SO IT WORKS LIKE A POPUP WINDOW//

    std::filesystem::path filePath = this->saveLocation;
    //Checks to see if a file exitsts in the given location 
    if(std::filesystem::exists(filePath)){
        //If the file exists, it prints that it exists and returns true
        //std::cout << "File path found!" << std::endl;
        return true;
    }
    else{
        if(createNew){
            //If the file doesn't exist, it will ask you if you want to make a file with the name given in the project directory
            //this only works if the file is being searched for while SAVING the current generation
            while(true){
                std::cout << "FILE PATH NOT FOUND\nWould you like to creat one called " << this->saveLocation << " in project directory? [y/n]" << std::endl;
                char ans;
                std::cin >> ans;
            
                if(ans == 'y'){
                    //Will create a file with the given name in file directory
                    std::ofstream saveFile(this->saveLocation);
                    saveFile.close();
                    std::cout << "File created" << std::endl;
                    //Returns true
                    return true; 
                }
                else if(ans == 'n'){
                    //Will not create the file and return flase
                    return false;
                }
                else{
                    //Loops unitl a viable answer has been given
                    std::cout << "Please answer with lowercase [y/n]" << std::endl;
                }
            }
        }
        else{
            //Only during the READING file command
            //Will return with a message that the file hasn't been found and will quit the program
            sendError("Save file can't be found");
            return false;
        }
    }

}

inline void GA::saveGen(){
    //The current generation will now be written to a text file with certain characters put in so it can be read later on
    if(checkSaveLocation(true)){
        std::ofstream saveFile(this->saveLocation);
        for(auto& x : this->DNAList){
            for(int i = 0; i < this->DNALength; i++){
                // Pipes (|) indicate a space between genes
                saveFile << "|" << x.getValueAt(i);
            }
            //Hastags (#) represent the end of a DNA strand
            saveFile << "|#";
        }
        std::string genStr = std::to_string(this->generation);
        for(auto& x : genStr){
            saveFile << "|" << x;
        }
        saveFile << "|_";
    }
    else{
        //Will quit the program if a file doesn't exist
        exit(1);
    }
}

inline void GA::loadGen(){
    if(checkSaveLocation(false)){
        //Reads the whole text file (if it exists) and saves it to a variable
        std::string fileLine;
        std::ifstream saveFile(this->saveLocation);
        while(std::getline(saveFile, fileLine)){
            fileLine = fileLine;
        }
        //Need to add so it can tell if the saved DNA will fit into the current one
        //Same DNA length and Number of DNA
        DNA tempDNA;
        saveFile.close();
        std::string tempStr;
        for(auto& x : fileLine){
            //Splits the string by the pipes to find individual genes
            if(x == '|'){
                if(tempStr != ""){
                    tempDNA.addGene(tempStr);
                    tempStr = "";
                }
            }
            //Splits the string by the hashtags to work out the end of a DNA strand
            else if(x == '#'){
                this->DNAList.push_back(tempDNA);
                tempDNA.clearDNA();
                tempStr = "";
            }
            else if(x == '_'){
                std::string genStr = "";
                for(int i = 0; i < tempDNA.getLength(); i++){
                    genStr += tempDNA.getValueAt(i);
                }
                this->generation = std::stoi(genStr); 
                tempDNA.clearDNA();
            }
            else{
                tempStr += x;
            }
        }
    }
    else{
        //Will quite the program if the file isn't found
        exit(1);
    }
}

//All for drawing the Graphs
inline void GA::initScreen(){
    getTerminalDim();
    //Enters a no scroll buffer
    std::cout << "\033[?1049h";
    buffer << "\x1b[?25l";
}

inline void GA::getTerminalDim(){
    int row, col;
#if defined(_WIN32) || defined(_WIN64)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    col = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    row = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    //std::cout << w.ws_row << " " << w.ws_col << std::endl;
    row = w.ws_row;
    col = w.ws_col;
#endif
    this->screenSize[0] = row;
    this->screenSize[1] = col;
    this->barAmount = (row-7) / 5;
    this->barLength = col;
}

inline void GA::printAt(std::string text, int x, int y, const RGB& colour){
    //Add the ability to print in colour
    std::string line =
        //location to print at
        "\x1b[" + std::to_string(y) + ";" + std::to_string(x) + "H" +
        //Colour definition
        "\x1b[38;2;" + std::to_string(colour.r) + ";"+ std::to_string(colour.g) + ";"+ std::to_string(colour.b) + "m" +
        //Actual text
        text + 
        //Text format
        "\x1b[0m";
    buffer << line;
}

inline void GA::clearScreen(){
    buffer << "\033[H";
}

inline void GA::printBuffer(){
    std::cout << buffer.str() << std::flush;
    buffer.str("");
    buffer.clear();

}

inline void GA::cleanUp(int n){
    //exits the 
    std::cout << "\033[?1049l";
    std::cout.flush();

    std::cout << "\x1b[?25h";
    exit(1);
}

inline void GA::bar(int h, int y, int fullBar, std::string message, const RGB& colour){
//GEN
//BARS
//empty space
    //Draws the gen info
    printAt(message, 1, y, gruvCream);
    y++;
    //Draws top bar line
    std::string line = this->charSets[this->charSet][2];
    for(int i = 0; i < this->barLength - 2; i++){ line += this->charSets[this->charSet][0];}
    line += this->charSets[this->charSet][3];
    printAt(line, 1, y, gruvCream);
    y++;
    //Draws bar that shows the Data
    for(int i = 0; i < h; i++){
        line = this->charSets[this->charSet][1];
        printAt(line, 1, y, gruvCream);
        line = "";
        std::string full = "";
        std::string empty = "";
        for(int i = 0; i < this->barLength - 2; i++){
            if(i <= fullBar){line += this->charSets[this->charSet][6];}
            else{ line += this->charSets[this->charSet][7];}
        }
        printAt(line, 2, y, colour);
        line = this->charSets[this->charSet][1];
        printAt(line, this->barLength, y, gruvCream);
        y++;
    }
    //Draws bottom line
    line = this->charSets[this->charSet][4];
    for(int i = 0; i < this->barLength - 2; i++){ line += this->charSets[this->charSet][0];}
    line += this->charSets[this->charSet][5];
    printAt(line, 1, y, gruvCream);
    y++;
    //Draws empty line
    line = "";
    for(int i = 0; i < this->barLength; i++){ line += " ";}
    printAt(line,1,y,white);
}

inline void GA::barGraph(int numPrevAverage){
    initScreen();
    //Add colour to the bars to make them look more interesting
    //WILL NEED UTF-8 AS CHARCTER SET
    if(this->screenSize[1] > 4){
        if(averageFitness.size() >= this->barAmount){
            for(int barNum = 0; barNum < this->barAmount; barNum++){
                //DRAWS THE SMALL INDIVIDUAL BARS
                int reverse = (this->generation - (this->barAmount - barNum));
                float ave = averageFitness[averageFitness.size() - 1 - barNum];
                int fitnessBar = (this->barLength - 4) * averageFitness[this->averageFitness.size() - 1 - barNum];

                std::string genInfo = "Generation:" + std::to_string(reverse) + " Average:" + std::to_string(ave);
                std::string space = "";
                for(int i = genInfo.size(); i < this->barLength; i++){space += " ";}
                genInfo += space;

                bar(1, (barNum * 5)+1, fitnessBar, genInfo, gruvGreen);
            }
            
        }
        //Big Average bar
        if(this->screenSize[1] >= 7 && numPrevAverage <= this->averageFitness.size()){
            float ave = 0;
            for(int i = 0; i < numPrevAverage; i++){
                ave += this->averageFitness[this->averageFitness.size() - i - 1];
            }
            ave /= numPrevAverage;
            int aveInt = ave * this->barLength;

            //Only print this when you have space needs to be programmed in 
            std::string message = "Average fitness of the last " + std::to_string(numPrevAverage) + " generations: " + std::to_string(ave);
            std::string space ="";
            for(int i = message.size(); i < this->barLength;i++){
                space+=" ";
            }
            message += space;
            bar(3, (barAmount * 5) + 1, aveInt, message, gruvRed);  
        }
    }
}


inline void GA::lineGraph(){
    initScreen();

    //Clears screen
    for(int y = 1; y < this->screenSize[0] + 1; y++){
        for(int x = 1; x < this->screenSize[1] + 1; x++){
            printAt(" ", x, y, white);
        }
    }

    //For line graph
    int lineMid = 0;
    float lineUpperVal = 0;
    float lineLowerVal = 0;

    int numOfGens = this->screenSize[1] / 5;
    
    if(numOfGens < averageFitness.size()){
        for(int i = averageFitness.size() - numOfGens - 1; i < averageFitness.size(); i++){
            //Finds if it needs to change the maximum or the minimum value
            if(averageFitness[i] < lineLowerVal){
                lineLowerVal = averageFitness[i];
            } 

            if(averageFitness[i] > lineUpperVal){
                lineUpperVal = averageFitness[i];
            }
        }
    }

    //Some variables requred to figure out where the horizontal line goes
    float range = lineUpperVal - lineLowerVal;
    float dist = range / this->screenSize[0];

    //Finds the closes match to where 0 line should be
    float closestY = 1000000000;
    for(int i = 0; i < this->screenSize[0] + 1; i++){
        if(std::abs(lineUpperVal - (dist * i)) < closestY){
            closestY = std::abs(lineUpperVal - (dist * i));
            lineMid = i;
        }
    }

    //Draws vertical bar
    for(int y = 1; y < this->screenSize[0] + 1; y++){
        printAt(this->charSets[this->charSet][1], 1, y, gruvCream);
    }

    //Draws horizontal bar
    for(int x = 2; x < this->screenSize[1] + 1; x++){
        printAt(this->charSets[this->charSet][0], x, lineMid, gruvCream);
    }
    
    //Draws all previous generations
    if(numOfGens < averageFitness.size()){
        for(int i = 0; i < numOfGens; i++){

            float closestVal = 1000000000;
            int line = 0;

            for(int j = 0; j < this->screenSize[0] + 1; j++){
                if(std::abs(averageFitness[(averageFitness.size() - numOfGens) + i] - (lineUpperVal - (dist * j))) < closestVal){
                    closestVal = std::abs(averageFitness[(averageFitness.size() - numOfGens) + i] - (lineUpperVal - (dist * j)));
                    line = j;
                }
            }

            printAt("X", 2 + (5 * i) , line, gruvRed); 
        }
    }
}

inline void GA::pause(){
    std::cin.get();
}

inline bool GA::hasGotten(){
    return this->correctGuess;
}
//Sets up the RNG for the GA (and allows multiple GA's to run with their own RNG
inline void initRNG(){
    srand(time(0)); 
}

#endif
