// devoirinfo3003.cpp : définit le point d'entrée pour l'application console.
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include <map>
#include "parsing.h"

using namespace std;

int main()
{
	string ligne = " ";//va contenir les lignes du fichier parsing
	string ligne2 = " ";//va contenir les lignes du fichier des jeux d'instructions
	string selection = "000";//variables de selection
	string branchement= "00000";//variables de branchement
	string signaux = "";//variables des signaux
	vector<string> adressesLabels;
	vector<string> instructions;
	vector<string> jeuxInstr;
	TableOpcode opcode;
	int positionVarBool(1), compteurLigne(0);
	VariablesBooleenes varBool;
	vector<VariablesBooleenes> tabVarBool;

/******************************* section 1: récupération des fichiers **************************************************/
	ifstream finParsing("parsing.txt", ios::in);
	if(!finParsing)
	{
		cout << "impossible d'ouvrir le fichier source" << endl;
		return(EXIT_FAILURE);
	}
	//lecture de tout le fichier et ensuite on va stocker toute les lignes dans un vecteur instr...
	while(getline(finParsing, ligne))
	{
		ligne = ignoreComments(ligne);					//on commence par ignorer les commentaires
		if(!ligne.empty())								//on vérifie si la ligne n'est pas vide
			instructions.push_back(trim(ligne, " \t"));//on réarrange les mot en éliminant des tabulations et
	}
	if(instructions.empty())
	{
		cerr << "Votre fichier est vide ! " << endl;
		return(EXIT_FAILURE);
	}
	finParsing.close();
	//on va chercher les jeux d'instructions
	ifstream finInstrSet("jeu_Ins.txt", ios::in);
	if(!finInstrSet)
	{
		cout << "impossible d'ouvrir le fichier des jeux d'instructions" << endl;
		return(EXIT_FAILURE);
	}
	//on parcourt le fichier qui contient les jeux d'instructions et on enregistre chaque ligne dans un vecteur
	while(getline(finInstrSet, ligne2))
	{
		ligne2 = ignoreComments(ligne2);
		if(!ligne2.empty())
			jeuxInstr.push_back(trim(ligne2, " \t"));
	}
	// si le fichier est vide!
	if(jeuxInstr.empty())
	{
		cerr << "Votre fichier est vide ! " << endl;
		return(EXIT_FAILURE);
	}
	finInstrSet.close();
	/*for(unsigned int j(0); j < instructions.size(); j++)
		cout << instructions.at(j) << endl;*/
/*******************************  fin section 1: récupération des fichiers **************************************************/

/*******************************  section 2 : traitement du fichier parsing **************************************************/
	vector <string> ext;
	//on va chercher les labels et enregistrer leur adresses
	for(vector<string>::iterator it = instructions.begin(); it != instructions.end(); ++it)
	{
		vector<string> temporaire;
		compteurLigne++;
		if(!it->empty())
		{
			if(estIndependante(*it))
			{
				//on vérifie si les instructions indépendantes utilisent les mêmes registres ou pas
				vector<string> temp = split(*it, " ");
				string regTemp;//va sauvegarder un registre temporairement pour vérifier s'il est semblable aux autres registres sur la même ligne
				for(unsigned int i(0); i < temp.size(); ++i)
				{
					if(isRegister(temp.at(i)))
					{
						regTemp = temp.at(i);
						for(unsigned j(i+1); j < temp.size(); ++j)
						{
							if(regTemp == temp.at(j))//on renvoi une erreur
							{
								cerr << "Un meme registre ne peut pas etre reutiliser pour les instructions independantes" <<endl;
								cerr << "Erreur: " << *it << " ligne: " << compteurLigne << endl;
								return(EXIT_FAILURE);
							}
						}
					}
				}
				//si tout est correcte, on sauvegarde les instructions
				temporaire = split(*it, "!");
				for(unsigned int i(0); i < temporaire.size(); ++i)
					ext.push_back(trim(temporaire.at(i)," \t"));
			}
			else 
				ext.push_back(trim(*it, " \t"));
		}
	}
	//on réédite le vecteurs des instructions avec les nouvelles adresses
	instructions = ext;
	//on crée une map pour enregistrer les adresses des labels ainsi que leurs noms
	map<string, string> tableAdressesLabels;
	//on parcourt notre vecteurs des labels
	for(unsigned int i(0); i < instructions.size(); ++i)
	{
		if(isLabel(instructions.at(i)))
			tableAdressesLabels[couper1erMot(instructions[i])] = intTobinL(i+1);
	}
	//
	vector<TableOpcode> tableAdrsOpcodeLabels;
	compteurLigne -= ext.size() ;
	compteurLigne ++;//+1 parce qu'on compte les lignes à partir de 1
	for(vector< string >::iterator it = instructions.begin(); it != instructions.end(); ++it)
	{
		bool estLa = false;
		compteurLigne++;
		//ensuite on sépare le label de l'instruction s'il y'en a
		vector<string> temp = split(*it, " ");//on sépare le label du opcode puisque les deux premiers ont été pris ensembles
		// vérifie si le début de la ligne commence par un label
		if(isLabel(temp.at(0)))
		{
			if(temp.size() > 1)
			{
				*it = "";
				for(unsigned int i(1); i < temp.size(); ++i)
					it->append(temp.at(i)).append(" ");
				//on réarrange la ligne
				*it = trim(*it, " ");
				//string str = *it;
				if(isBranch(temp.at(1)))
				{
					if(temp.size() == 5 && temp.at(1) == "if")
					{
						if(temp.at(3) == "then" && temp.at(1) == "if")
						{
							if(isLabel(temp.at(4)))
							{
								varBool.nomVar = temp.at(2);
								varBool.adrsSel = intTobinVB(positionVarBool);
							
								//on cherchera l'adresse du label dans la map
								for(map<string, string>::iterator itr = tableAdressesLabels.begin(); itr != tableAdressesLabels.end(); ++itr)
								{
									if(itr->first == temp.at(4))
									{
										varBool.adrsBr = itr->second;
										estLa = true;
									}
								}
								if(!estLa)
								{
									cerr << "Adresse introuvable" << *it <<" a la ligne: " << compteurLigne << endl;
									return(EXIT_FAILURE);
								}
								//
								tabVarBool.push_back(varBool);
								positionVarBool++;//on incrémente la variable booleene
								*it = temp.at(2);
							}
							else
							{
								cerr << "Erreur, label introuvable apres le then: "<< *it;
								cerr << " a la ligne: " << compteurLigne << endl;
								return(EXIT_FAILURE);
							}
						}
						else
						{
							cerr << "Erreur de syntaxe: "<< *it ;
							cerr << " a la ligne: " << compteurLigne << endl;
							return(EXIT_FAILURE);
						}
					}//goto
					else if(temp.size() == 3 && temp.at(1) == "goto")
					{
						//on cherchera l'adresse du label dans la map
						for(map<string, string>::iterator itr = tableAdressesLabels.begin(); itr != tableAdressesLabels.end(); ++itr)
						{
							if(itr->first == temp.at(2))
							{
								*it = temp.at(2);
								estLa = true;
							}
						}
						if(!estLa)
						{
							cerr << "Adresse introuvable a la ligne: " << *it << endl;
							cerr << " ligne: " << compteurLigne << endl;
							return(EXIT_FAILURE);
						}
					}
					else
					{
						cerr << "Erreur de syntaxe dans l'instruction: " << *it << endl;//instruction ou adresse manquante
						cerr << " a la ligne: " << compteurLigne << endl;
						return(EXIT_FAILURE);
					}
				}
				else
				{
					//si ce n'est pas un branchement et que ce n'est pas un opcode
					if(!isOpcode(*it))
					{
						cerr << "Instruction incorrecte: " << *it;//instruction ou adresse manquante
						cerr << " a la ligne: " << compteurLigne << endl;
						return(EXIT_FAILURE);
					}
				}
			}
		}//si le début de la ligne n'est pas un label
		else
		{
			if(isBranch(temp.at(0)))
			{
				if(temp.size() == 4 && temp.at(0) == "if")
				{
					if(temp.at(2) == "then" && temp.at(0) == "if" )
					{
						if(isLabel(temp.at(3)))
						{
							//on ajoute les variables booleenes et leurs adresses
							varBool.nomVar = temp.at(1);
							varBool.adrsSel = intTobinVB(positionVarBool);
							//on cherchera l'adresse du label dans la map
							for(map<string, string>::iterator itr = tableAdressesLabels.begin(); itr != tableAdressesLabels.end(); ++itr)
							{
								if(itr->first == temp.at(3))
								{
									varBool.adrsBr = itr->second;
									estLa = true;
								}
							}
							if(!estLa)
							{
								cerr << "Adresse introuvable" << *it <<" a la ligne: " << compteurLigne << endl;
								return(EXIT_FAILURE);
							}
							//
							tabVarBool.push_back(varBool);
							positionVarBool++;//on incrémente la variable booleenes
							*it = temp.at(1);
						}
						else
						{
							cerr << "Erreur, label introuvable apres le then: "<< *it;
							cerr << " a la ligne: " << compteurLigne << endl;
							return(EXIT_FAILURE);
						}
					}
					else
					{
						cerr << "Erreur de syntaxe: "<< *it ;
						cerr << " a la ligne: " << compteurLigne << endl;
						return(EXIT_FAILURE);
					}

				}
				else if(temp.size() == 2 && temp.at(0) == "goto")
				{
					//on cherchera l'adresse du label dans la map
					for(map<string, string>::iterator itr = tableAdressesLabels.begin(); itr != tableAdressesLabels.end(); ++itr)
					{
						if(itr->first == temp.at(1))
						{
							*it = temp.at(1);
							estLa = true;
						}
					}
					if(!estLa)
					{
						cerr << "Adresse introuvable a la ligne: " << *it << endl;
						cerr << " ligne: " << compteurLigne << endl;
						return(EXIT_FAILURE);
					}
				}
				else
				{
					cerr << "Erreur de syntaxe dans l'instruction: " << *it << endl;//instruction ou adresse manquante
					cerr << " a la ligne: " << compteurLigne << endl;
					return(EXIT_FAILURE);
				}
			}
			else //si ce n'est pas un branchement
			{
				//si ce n'est pas un branchement et que ce n'est pas un opcode
				if(!isOpcode(*it))
				{
					cerr << "Instruction incorrecte: " << *it;//instruction ou adresse manquante
					cerr << " a la ligne: " << compteurLigne << endl;
					return(EXIT_FAILURE);
				}
			}
		}
	}

	vector<TableOpcode>Mapsortie = JeuxInstructions(jeuxInstr);
	vector<TableOpcode>::iterator it1 = Mapsortie.begin();
	for(unsigned int j(0); j < it1->m_adresse.size(); ++j)
		signaux.append("0");

	int i(1);//nous affichera le numéro de chaque ligne
	cout << "N Selection  Branchement   Signaux   commentaires" << endl <<endl;
	for(vector<string>::iterator it = instructions.begin(); it != instructions.end(); ++it)
	{
		//génère un signale
		for(it1 = Mapsortie.begin(); it1 != Mapsortie.end(); ++it1)
		{
			if(*it == it1->m_opcode)
			{
				cout << i << ":\t" << selection << "\t" << branchement << "\t" << it1->m_adresse << "\t" << it1->m_opcode << endl; 
				++i;
			}
		}
		//on génère les branchements
		for(vector<VariablesBooleenes>::iterator it2 = tabVarBool.begin(); it2 != tabVarBool.end(); ++it2)
		{
			if(*it == it2->nomVar)
			{
				cout << i << ":\t" << it2->adrsSel << "\t" << it2->adrsBr << "\t" << signaux << "\t" <<it2->nomVar << endl;
				++i;
			}
		}
		//génère le reste
		for(map<string, string>::iterator it3 = tableAdressesLabels.begin(); it3 != tableAdressesLabels.end(); ++it3)
		{
			if(*it == it3->first)
			{
				cout << i << ":\t" << selection << "\t" << it3->second << "\t" << signaux << "\t" << it3->first << endl;
				++i;
			}
		}
	}
	return 0;
}
