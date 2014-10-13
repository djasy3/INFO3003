// devoirinfo3003.cpp�: d�finit le point d'entr�e pour l'application console.
//
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

string ignoreComments(string);
vector<string> split(string);

int main()
{
	string opcodes[] = {"clear", "load", "add", "sub", "and", "xor", "asr", "dec", "inc", "if", "then", "go to" };
	string registres[]={"A", "M", "L", "Q" };
	string motReserve="%Bus";
	string operateurs[] = {"!", "<"};
	string label = "$";
	//
	string ligne = "voici la ligne avec un commentaire;assembleur";
	//size_t pos = ligne.find_first_of(';');
	string newligne = ignoreComments(ligne);
	//
	cout << ligne << endl;
	cout << newligne << endl;

	return 0;
}
//fonction qui retourne la ligne sans commentaire
string ignoreComments(string ligne)
{
	return ligne.substr(0, ligne.find_first_of(';'));
}
//fonction qui retourne une vecteur des mots coup�s
vector<string> split(string ligne)
{
	char *mots;
	char *context = NULL;
	char* char_line = (char*)ligne.c_str(); // Non-const cast required.
	mots = strtok_s(char_line, " ", &context);
	vector<string> tableMot;

	while(mots != NULL)
	{
		tableMot.push_back(mots);
		mots = strtok_s(NULL, " ", &context);
	}

	return tableMot;
}