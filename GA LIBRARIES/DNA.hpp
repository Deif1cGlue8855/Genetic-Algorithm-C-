#ifndef DNA_HPP
#define DNA_HPP

#include <vector>
#include <iostream>
#include <string>

class DNA{

    private: 
        //All of the attributes for the class
        std::vector<std::string> genes;
        int DNAlength;       
    public:
        //Instantiation code
        DNA();
        //Any other methods
        int getLength();
        void addGene(std::string c);
        void rewriteDNA(DNA newDNA);
        std::string getValueAt(int point);
        void setValueAt(std::string value, int point);
        void clearDNA();
        std::vector<std::string> getGenes();
};

//WRITING METHOD FUNCTION

inline DNA::DNA(){
    this->DNAlength = 0;
}

//Setter to add individual genes
inline void DNA::addGene(std::string c){
    this->genes.push_back(c);
    this->DNAlength ++;
}
//Getter to get the length of the DNA 
inline int DNA::getLength(){
    return this->DNAlength;
}
//Rewrites the whole DNA list
inline void DNA::rewriteDNA(DNA newLst){
    this->genes = newLst.getGenes();
    this->DNAlength = newLst.getLength();
}
//Gets the value from a specific position / gene in the DNA strand
inline std::string DNA::getValueAt(int point){
    if(point < 0 || point >= this->DNAlength){
        return "ERROR";
    }
    return this->genes[point];
}
inline void DNA::setValueAt(std::string value, int point){
    if(point < 0 || point >= this->DNAlength){
        std::cout << "ERROR";
        exit(1);
    }
    else{
        this->genes[point] = value;
    }
}
//Clears the DNA 
inline void DNA::clearDNA(){ 
    this->genes.clear();
    this->DNAlength = 0;
}

inline std::vector<std::string> DNA::getGenes(){
    return this->genes;
}
#endif
