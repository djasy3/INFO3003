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
	string branchement= "0000";//variables de branchement
	string signaux = "00000000000";//variables des signaux
	vector<string> adressesLabels;
	vector<string> instructions;
	vector<string> jeuxInstr;
	vector<string> tabMicroInstruct;//temporaire
	vector<string> tabMicroInstructF;//finale
	TableOpcode opcode;
	int positionVarBool(1);
	VariablesBooleenes varBool;
	vector<VariablesBooleenes> tabVarBool;

/******************************* section 1: récupération des fichiers **************************************************/
	ifstream finParsing("parsing.txt", ios::in);
	if(!finParsing)
	{
		cout << "impossible d'ouvrir le fichier source" << endl;
		return(1);
	}
	//lecture de tout le fichier et ensuite on va stocker toute les lignes dans un vecteur instr...
	while(getline(finParsing, ligne)) instructions.push_back(ignoreComments(ligne));
	finParsing.close();
	//on va chercher les jeux d'instructions
	ifstream finInstrSet("jeu_Ins.txt", ios::in);
	if(!finInstrSet)
	{
		cout << "impossible d'ouvrir le fichier des jeux d'instructions" << endl;
		return(1);
	}
	//on parcourt le fichier qui contient les jeux d'instructions et on enregistre chaque ligne dans un vecteur
	while(getline(finInstrSet, ligne2)) jeuxInstr.push_back(ignoreComments(ligne2));
	finInstrSet.close();

/*******************************  fin section 1: récupération des fichiers **************************************************/

/*******************************  section 2 : traitement du fichier parsing **************************************************/
	//on va chercher les labels et enregistrer leur adresses
	for(vector<string>::iterator it = instructions.begin(); it != instructions.end(); ++it)
		adressesLabels = getAdrsLabels(split(*it, " ,\t"));
	//on crée une map pour enregistrer les adresses des labels ainsi que leurs noms
	map<string, string> tableAdressesLables;
	//on parcourt notre vecteurs des labels
	for(unsigned int i(0); i < adressesLabels.size(); ++i)
		tableAdressesLables[adressesLabels[i]] = intTobinL(i+1);

	vector<TableOpcode> tableAdrsOpcodeLabels;
	for(vector< string >::iterator it = instructions.begin(); it != instructions.end(); ++it)
	{
		// vérifie si le début de la ligne commence par un label
		if(isLabel(*it))
		{
			//on vérifie si la ligne a plusieurs instructions
			if(estIndependante(*it))
			{
				//si oui, on extrait les microinstructions
				tabMicroInstruct = split(*it, "!");
				//et on fait les traitement de ces micros instructions
				vector<string> temp = split(tabMicroInstruct.at(0), "\t");//on sépare le label du opcode puis les deux premiers ont été pris ensembles et on prend la deuxième partie on l'ajoute au tableau des micros instructions sans label cette fois ci
				//comme le premier élément est un label, on ne prend que les micros instructions
				tabMicroInstruct.at(0) = temp.at(1);
				//on récupère les micros instructions en leur assignant l'addresse de leur label
				for(unsigned int i(0); i < tabMicroInstruct.size(); ++i)
					tabMicroInstructF.push_back(tabMicroInstruct.at(i));//on met dans la table des micro instructions finale
				//on met les opcodes et leur adresses dans une table
				for(unsigned int i(tabMicroInstructF.size() - tabMicroInstruct.size()); i < tabMicroInstructF.size(); ++i)
				{
					opcode.m_adresse = getAdrsLabel(tableAdressesLables, temp.at(0));
					opcode.m_opcode  = tabMicroInstructF.at(i);
					tableAdrsOpcodeLabels.push_back(opcode);
				}
				//on vérifie si l'instruction se trouve dans les jeux d'instructions pour avoir un signal
				//if(getSignal(optcode))
			}
			else
			{
				//si il n' y a qu'un seule instruction sur la ligne
				//on coupe l'opcode, on attribue à l'opcode l'adresse de son label
				vector<string> temp = split(*it, "\t ");
				if(isOpcode(temp.at(1)))
				{
					opcode.m_adresse = getAdrsLabel(tableAdressesLables, temp.at(0));
					opcode.m_opcode  = temp.at(1);
					tabMicroInstructF.push_back(temp.at(1));
					tableAdrsOpcodeLabels.push_back(opcode);
				}
				if(isBranch(temp.at(1)))
				{
					if(temp.at(1) == "if" && temp.at(3) == "then")
					{
						if(isLabel(temp.at(4)))
						{
							opcode.m_adresse = getAdrsLabel(tableAdressesLables, temp.at(0));
							opcode.m_opcode  = temp.at(2);
							tabMicroInstructF.push_back(temp.at(2));
							tableAdrsOpcodeLabels.push_back(opcode);
							//on ajoute les variables booleenes et leurs adresses
							varBool.nomVar = temp.at(2);
							varBool.adrsSel = intTobinVB(positionVarBool);
							//on cherchera l'adresse du label dans la map
							for(map<string, string>::iterator it = tableAdressesLables.begin(); it != tableAdressesLables.end(); ++it)
							if(it->first == temp.at(4))
								varBool.adrsBr = it->second;
							//
							tabVarBool.push_back(varBool);
							positionVarBool++;//on incrémente la variable booleenes
						}
						else
							cerr << "Erreur, label introuvable apres le then" << endl;
					}
					else
					{
						if(temp.at(1) == "goto")
						{
							opcode.m_adresse = getAdrsLabel(tableAdressesLables, temp.at(0));
							opcode.m_opcode  = temp.at(2);
							tabMicroInstructF.push_back(temp.at(2));
							tableAdrsOpcodeLabels.push_back(opcode);
						}
					}
				}
			}
		}// si le début n'est pas un label
		else
		{
			vector<string> temp = split(*it, " \t");
			//si le premier mot est un registre ou un bus, on prend la phrase entière
			//donc toute la phrase est une instruction
			//cout << temp.at(0) << endl;
			if(isRegister(temp.at(0)) || isBus(temp.at(0)))
				tabMicroInstructF.push_back(*it);
			//si c'est un opcode ou un branchement
			if(isBranch(temp.at(0)))
			{
				//si ce branchement est différent de if donc c'est un goto
				//et le prochain mot doit être forcement un label
				if(temp.at(0) != "if")
				{
					if(!isLabel(temp.at(1)))
					{
						cerr << "Label manquant apres " << temp.at(0) << endl;
						return(1);
					}
					else
					{
						//ensuite on génère les signaux correspondants au label
						tabMicroInstructF.push_back(temp.at(1));
					}
				}//si c'est un if on prend la variable booléenne
				else
				{
					varBool.nomVar = temp.at(1);
					varBool.adrsSel = intTobinVB(positionVarBool);
					//on cherchera l'adresse du label dans la map
					for(map<string, string>::iterator it = tableAdressesLables.begin(); it != tableAdressesLables.end(); ++it)
						if(it->first == temp.at(3))
							varBool.adrsBr = it->second;
					//
					tabVarBool.push_back(varBool);
					positionVarBool++;//on incrémente la variable booleenes
					tabMicroInstructF.push_back(temp.at(1));
				}
			}
		}
		
	}
	//split et reassemble en élimant les espaces
	vector <string> temp;
	for(unsigned int i(0); i < tabMicroInstructF.size(); ++i)
		temp.push_back(trim(tabMicroInstructF.at(i), " \t"));
	//on restitue le tableau des micros instruction traité
	tabMicroInstructF = temp;
//*********************************************************************************************************************************/
	 //dans cette troisième partie on va faire la correspondance des nos opcodes avec le fichier des jeux d'instructions pour générer les signaux....
	//tableau des jeux instructions traité(instructions sans espace)
	map<string, string>matemp;
	map<string, string>Mapsortie = MapJeuInstruction(jeuxInstr);
	for(map<string, string>::iterator it = Mapsortie.begin(); it != Mapsortie.end(); ++it)
		matemp.insert(pair<string, string>(trim(it->first, " \t"), trim(it->second, " \t")));
	//
	Mapsortie = matemp;
	//on génère le tableau final
	int i(1);//nous affichera le numéro de chaque ligne
	for(vector<string>::iterator it = tabMicroInstructF.begin(); it != tabMicroInstructF.end(); ++it)
	{
		//génère un signale
		for(map<string, string>::iterator it1 = Mapsortie.begin(); it1 != Mapsortie.end(); ++it1)
		{
			if(*it == it1->first)
			{
				cout << i << ":\t" << selection << "\t" << branchement << "\t" << it1->second << endl; 
				++i;
			}
		}
		//on génère les branchements
		for(vector<VariablesBooleenes>::iterator it2 = tabVarBool.begin(); it2 != tabVarBool.end(); ++it2)
		{
			if(*it == it2->nomVar)
			{
				cout << i << ":\t" << it2->adrsSel << "\t" << it2->adrsBr << "\t" << signaux << endl;
				++i;
			}
		}
		//
		if(isLabel(*it))
		{
			for(vector<TableOpcode>::iterator it3 = tableAdrsOpcodeLabels.begin(); it3 != tableAdrsOpcodeLabels.end(); ++it3)
			{
				if(*it == it3->m_opcode)
				{
					cout << i << ":\t" << selection << "\t" << it3->m_adresse << "\t" << signaux << endl;
					++i;
				}
			}
		}
	}

	return 0;
}
