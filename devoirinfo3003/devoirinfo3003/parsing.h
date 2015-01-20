#ifndef PARSING_H
#define PARSING_H

#include <vector>
#include <cctype>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <bitset>
#include <sstream>
#include <algorithm>

using namespace std;

typedef struct ActivationBit ActivationBit;
struct ActivationBit
{
	vector<int> position;
	vector<int> bitActivE;
};
//cette structure va contenir notre table d'opcode
typedef struct TableOpcode TableOpcode;
struct TableOpcode
{
	string m_adresse;
	string m_opcode;
};
//
typedef struct VariablesBooleenes VariablesBooleenes;
struct VariablesBooleenes
{
	string nomVar;//nom de la variable booleennes
	string adrsSel;//adresse de la variable booleenes(mux)
	string adrsBr;//adresse du branchement
};

map<string, string>traitementInstructions(vector<string>);
string ignoreComments(string);	//méthode qui permet d'ignorer les commentaires dans le code
vector<string> split(string, char[]);	//méthode qui permet d'obtenir chaque mot pour faire le parsing!
vector<string> split(vector<string>, char []);
vector<string>motsEnMiniscules(vector<string>);
vector<string> extractionInstructions();//permet d'extraire les microinstructions
vector<string> getAdrsLabels(vector<string>);	//méthode qui retourne les adresses des labels rencontrés
string getAdrsLabel(map<string, string>, string);//va récupérer une adresse spécifique du label dans le tableau des labels
bool isLabel(string);			//méthode qui vérifie si un mot est un label
bool isOpcode(string);			//méthode qui vérifie si un mot est un opcode
bool isRegister(string);
bool isBranch(string);
bool isOpLogique(string);
bool isBus(string);
string intTobinL(int);		//fonction qui convertit les entiers en binaire sous forme des chaines de caractères
string intTobinVB(int);
int positionDeuxPoint(string);
int positionPointExclamation(string);
int getMax(vector<TableOpcode>); //fonction qui renvoi le plus grand élément d'un tableau
bool estIndependante(string);
string trim(string, char[]);
string trim(string);
vector<int> ConditionBranch(string);
map<string, string> MapJeuInstruction (vector<string>);
vector<TableOpcode> JeuxInstructions(vector<string>);
string signal_F(string);
string Bit_Active(string);
int positionEgal(string);
string couper1erMot(string);

#endif
