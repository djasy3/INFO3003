#ifndef PARSING_H
#define PARSING_H

#include <vector>
#include <string>
#include <iostream>
#include <map>

using namespace std;

//cette structure va contenir notre table d'opcode
typedef struct TableOpcode TableOpcode;
struct TableOpcode
{
	string m_adresse;
	string m_opcode;
};

typedef struct Instruction Instruction;
struct Instruction
{
	string m_micro;
	string m_signal;
};

vector<Instruction>traitementInstructions(vector<string>);

string ignoreComments(string);	//méthode qui permet d'ignorer les commentaires dans le code
vector<string>split(string, char[]);	//méthode qui permet d'obtenir chaque mot pour faire le parsing!
vector<string>motsEnMiniscules(vector<string>);
vector<string> getAdrsLabels(vector<string>);	//méthode qui retourne les adresses des labels rencontrés
string getAdrsLabel(map<string, string>, string);//va récupérer une adresse spécifique du label dans le tableau des labels
bool isLabel(string);			//méthode qui vérifie si un mot est un label
bool isOpcode(string);			//méthode qui vérifie si un mot est un opcode
bool isRegister(string);
bool isBranch(string);
bool isBus(string);
string intTobin(int);		//fonction qui convertit les entiers en binaire sous forme des chaines de caractères
int positionDeuxPoint(string);
int positionPointExclamation(string);
bool estIndependante(string);

#endif