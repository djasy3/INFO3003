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
	string C[15]={"10000000000","01000000000","00100000000","00010000000","00000000000","00001000000","00000100000","00001100000","00000000000"
	,"00000010000","00000001000","00000000100","00000000010","00000000001","00000000001xx"};

	string tableau = "000000000000";//va afficher les valeurs de c0 à c11
	string mpc = "0000";//va afficher le mpc
	string varBoul ="000";
	string br = "0000";
	string tableaFinal = mpc+varBoul+br+tableau;
	string ligne, ligne2;
	vector<string> adressesLabels;
	vector<string> instructions;
	vector<string> jeuxInstr;
	vector<string> tabMicroInstruct;//temporaire
	vector<string> tabMicroInstructF;//finale
	TableOpcode opcode;
	opcode.m_adresse = " ";
	opcode.m_opcode = " ";

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
		tableAdressesLables[adressesLabels[i]] = intTobin(i+1);
	//on affiche notre table d'adresse
	for(map<string, string>::iterator it = tableAdressesLables.begin(); it != tableAdressesLables.end(); ++it)
		cout << it->first << "\t"<< it->second << endl;
	cout << endl;
	//
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
					if(temp.at(1) == "if")
					{
						opcode.m_adresse = getAdrsLabel(tableAdressesLables, temp.at(0));
						opcode.m_opcode  = temp.at(2);
						tabMicroInstructF.push_back(temp.at(2)+" "+temp.at(4));//on prend aussi l'adresse
						tableAdrsOpcodeLabels.push_back(opcode);
					}
					else
					{
						opcode.m_adresse = getAdrsLabel(tableAdressesLables, temp.at(0));
						opcode.m_opcode  = temp.at(2);
						tabMicroInstructF.push_back(temp.at(2));
						tableAdrsOpcodeLabels.push_back(opcode);
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
					tabMicroInstructF.push_back(temp.at(1)+" "+temp.at(3));
			}
		}
		
	}
	for(unsigned int i(0); i < tableAdrsOpcodeLabels.size(); ++i)
		cout << setfill('0') << setw(4) << tableAdrsOpcodeLabels[i].m_adresse << "\t"<< tableAdrsOpcodeLabels[i].m_opcode << endl;

	cout << intTobin(8) << endl << endl;
	for(unsigned int i(0); i < tabMicroInstructF.size(); ++i)
		cout << tabMicroInstructF.at(i) << endl;

//*********************************************************************************************************************************/
	/*
	vector <string>Instruction;
	ifstream instructionFile("jeu_Ins.txt");
	string lignes;
	//int taille(0);
	while (!instructionFile.eof())
	{ 
		getline(instructionFile, lignes);
		Instruction.push_back(lignes);
		//taille++;
	}
	string lignesanscomm;
	vector<string>Vectorligne;
	
	for(unsigned int j=0;j<Instruction.size();j++)
	{
		lignesanscomm=ignoreComments(Instruction.at(j));
		Vectorligne.push_back(lignesanscomm);
	}

	string tablJeuInst[15][3];// = new string* [taille];

	//vector<vector <string> > tablJeuInst(taille);
	//for(int i(0); i < taille; ++i)
		//tablJeuInst[i] = vector<string>(3);

	for (int i=0;i<tablJeuInst.size();i++)
	{
		int positionPoint=positionDeuxPoint(Vectorligne.at(i));
		int positionPoint1=Vectorligne.at(i).find(':',positionPoint+1);
		string signal=Vectorligne.at(i).substr(0,positionPoint);
		string operation=Vectorligne.at(i).substr(positionPoint+1,positionPoint1-(positionPoint+1));

		tablJeuInst[i][0]=signal;
		cout <<tablJeuInst[i][0]<<" \t";
		tablJeuInst[i][1]=operation;
		cout <<tablJeuInst[i][1]<<" \t";
		tablJeuInst[i][2]=C[i];
		cout <<tablJeuInst[i][2]<<" \t";
		cout <<endl;
	}

	for(unsigned int i(0); i < jeuxInstr.size(); ++i)
		cout << jeuxInstr.at(i) << endl;
		*/

	vector<Instruction> tabJeuxInstr = traitementInstructions(jeuxInstr);
	for(int i(0); i < tabJeuxInstr.size(); ++i)
		cout << tabJeuxInstr.at(i).m_micro <<" ->"<< tabJeuxInstr.at(i).m_signal << endl;

	return 0;
}