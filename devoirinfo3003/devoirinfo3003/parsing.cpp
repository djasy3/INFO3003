#include "parsing.h"

string tabOpcodes[] = {"clear", "load", "add", "sub", "and", "xor", "asr", "dec", "inc" };
string tabOpcodesBranchement[] = {"goto","if", "then" };
string tabRegistres[]={"a", "m", "l", "q", "A", "M", "L", "Q" };
string bus="%Bus";
string operateurs[] = {"!", "<"};
char label = '$';

int compteur(0), tailleOpcodes(9), tailleBranchement(3), tailleReg(8);//la variable compteur comptera juste le nombre de opcode sur la même ligne
vector<string> tabInstructions;
vector <string> adresseLabels;//va contenir les adresses des labels
//fonction qui retourne la ligne sans commentaire
string ignoreComments(string ligne)
{
	return ligne.substr(0, ligne.find_first_of(';'));
}
string intTobin(int nbre)
{
	string resultat;
	int rslt = nbre;
	while(nbre >0)
	{
		resultat += (nbre%2 == 0) ? "0" : "1";
		/*
		if(rslt%2 != 0)
		{
			resultat += (nbre%2 == 0) ? "0" : "1";
			//nbre /=2;
		}
		else
		{
			resultat += (nbre%2 == 0) ? "1" : "0";
			//nbre /=2;
		}*/
		nbre /=2;
	}
	return resultat;
}
//fonction qui retourne une vecteur des mots coupés
vector<string> split(string ligne, char  delims[]	= " ,\t")
{
	char *mots;
	char *context = NULL;
	char* char_line = (char*)ligne.c_str(); // Non-const cast required.
	mots = strtok_s(char_line, delims, &context);

	vector<string> tableMot;

	while(mots != NULL)
	{
		tableMot.push_back(mots);
		mots = strtok_s(NULL, delims, &context);
	}
	return tableMot;
}
//fonction qui retourne les caractères en miniscules de chaque mots
vector<string> motsEnMiniscules(vector<string> mot)
{
	vector<string> motsTemp;
	for(vector<string>::iterator it = mot.begin(); it != mot.end(); ++it)
	{
		string motTemp = *it;
		string nouveauMot;
		for(unsigned int i(0); i < motTemp.size(); ++i)
			nouveauMot += tolower(motTemp[i]);
		//on remet dans le vecteur les mots en miniscules
		motsTemp.push_back(nouveauMot); 
	}
	return motsTemp;
}
//methode prenant comme paramètre un vecteur
vector<string> getAdrsLabels(vector<string> mots)
{
	string motTemp = mots[0];//on copie tout le mot dans une variable string pour comparer que le premier caractère
	if(isLabel(motTemp)) 
		adresseLabels.push_back(motTemp);//on insère le label dans un vecteur des labels
	return adresseLabels;
}
//méthode prenant comme paramètre un string
bool isLabel(string mot) { return mot[0] == label; }
//méthode pour vérifier si le mot est  un opcode
bool isOpcode(string mot)
{
	compteur = 0;
	for(int i(0); i < tailleOpcodes; i++ )
	{
		if(mot == tabOpcodes[i])
			compteur++;
	}
	return compteur > 0;
}
//
bool isRegister(string mot)
{
	compteur = 0;
	for(int i(0); i < tailleReg; i++ )
	{
		if(mot == tabRegistres[i])
			compteur++;
	}
	return compteur > 0;//retourne true s'il trouve registre
}
//fonction qui cherche les instruction indépendantes
int positionDeuxPoint(string ligne)
{
	size_t position= ligne.find(":");
	return position; // return la position des deux points
}
//on ne sait pas controller si il ne trouve pas le point d'interrogation quelle valeur il va envoyer
int positionPointExclamation(string ligne)
{
	size_t position= ligne.find("!");
	return position; // return la position de la ! 
}
//renvoi true s'il trouve le !
bool estIndependante(string ligne)
{
	compteur = 0;
	//on cherche si il y'a plusieurs instruction sur la ligne
	for(unsigned int i(0); i < ligne.size(); ++i)
	{
		if(ligne[i] == '!')
			compteur++;
	}
	return compteur > 0;//on retourne vrai si la ligne a plusieurs microinstructions
}
//si l'opcode n'est pas un branchement
bool isBranch(string ligne)
{
	compteur = 0;
	for(int i(0); i < tailleBranchement; ++i)
	{
		if(ligne == tabOpcodesBranchement[i])
			compteur++;
	}
	return compteur > 0;
}
//
bool isBus(string ligne)
{
	return ligne == bus;
}
string getAdrsLabel(map<string , string > adrsLabels, string label)
{
	string adresseDuLabel;

	for(map<string, string>::iterator it = adrsLabels.begin(); it != adrsLabels.end(); ++it)
	{
		//si le label correspond à celui de la table des labels, on renvoi son adresse
		if(it->first == label )
		{
			adresseDuLabel = it->second;
			break;
		}
	}
	return adresseDuLabel;//on retourne l'adresse
}
//
vector<Instruction>traitementInstructions(vector<string> lignes)
{
	//on coupe les lignes
	vector<string> temp;
	vector<Instruction> tempInstr;
	Instruction structInstr;

	for(vector<string>::iterator it = lignes.begin(); it != lignes.end(); ++it)
	{
		temp = split(*it, ":");
		for(int i(0); i < temp.size(); ++i)
			tabInstructions.push_back(temp.at(i));
	}
	//on parcourt le tableau des instructions
	
	for(int j(0); j < tabInstructions.size(); ++j)
	{
		if(j%2==0)
			structInstr.m_signal = tabInstructions.at(j);
		if(j%2!=0)
			structInstr.m_micro = tabInstructions.at(j);
		tempInstr.push_back(structInstr);
	} 
	return tempInstr;
}