#include "parsing.h"

string opLogique[] = {"add", "sub", "and", "xor","ADD", "SUB", "AND","ADD", "XOR"};
string tabOpcodesBranchement[] = {"goto","if", "then" };
string tabRegistres[]={"a", "m", "l", "q", "A", "M", "L", "Q" };
string bus="%Bus";
char label = '$';
int compteur(0), tailleBranchement(3), tailleReg(8), tailleLogique(9);//la variable compteur comptera juste le nombre de opcode sur la même ligne
int t = 10;
const size_t taille = (sizeof(t)/sizeof(unsigned long)) * CHAR_BIT;
vector<string> tabInstructions;
vector <string> adresseLabels;//va contenir les adresses des labels
//méthode permettant d'extraire les microinstructions
vector<string> extractionInstructions()
{
	string ligne;
	vector<string> temp;
	//lecture des instructions
	//on va chercher les jeux d'instructions
	ifstream finInstrSet("jeu_Ins.txt", ios::in);
	if(!finInstrSet)
	{
		cout << "impossible d'ouvrir le fichier des jeux d'instructions" << endl;
		exit (EXIT_FAILURE);
	}
	//on parcourt le fichier qui contient les jeux d'instructions et on enregistre chaque ligne dans un vecteur
	while(getline(finInstrSet, ligne))
	{
		ligne = ignoreComments(ligne);
		if(!ligne.empty())
			temp.push_back(trim(ligne, " \t"));
	}
	// si le fichier est vide!
	if(temp.empty())
	{
		cerr << "Votre fichier est vide ! " << endl;
		exit (EXIT_FAILURE);
	}
	finInstrSet.close();
	//on va traiter les instructions afin de les séparer des bits d'activations des signaux
	map<string, string> temporaire = traitementInstructions(temp);
	//après avoir chargé les lignes des jeux d'instructions dans la map, on aprête les vecteurs à ne recevoir que les opcodes
	temp.clear();
	for(map<string, string>::iterator it = temporaire.begin(); it != temporaire.end(); ++it)
	{
		//it->second représente que les opcodes
		temp.push_back(it->second);
	}
	//on retourne le vecteur après la fin de la lecture du fichier
	return temp;
}
//fonction qui retourne la ligne sans commentaire
string ignoreComments(string ligne)
{
	return ligne.substr(0, ligne.find_first_of(';'));
}
string intTobinL(int nbre)
{
	bitset<5> bin_x(nbre);
	//conversion d'un bitset à string
	return bin_x.to_string<char, string::traits_type, string::allocator_type>();
}

string intTobinVB(int nbre)
{
	bitset<3> bin_x(nbre);
	//conversion d'un bitset à string
	return bin_x.to_string<char, string::traits_type, string::allocator_type>();
}
//fonction qui retourne une vecteur des mots coupés
vector<string> split(string ligne, char  delims[])
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
//
vector<string> split(vector<string> vect, char  delims[])
{
	vector<string> tableMot;

	for(unsigned int i(0); i < vect.size(); ++i)
	{
		char *mots;
		char *context = NULL;
		char* char_line = (char*)vect.at(i).c_str(); // Non-const cast required.
		mots = strtok_s(char_line, delims, &context);

		while(mots != NULL)
		{
			tableMot.push_back(mots);
			mots = strtok_s(NULL, delims, &context);
		}
	}
	return tableMot;
}
//rédéfinition de la fonction pour 
string trim(string ligne, char  delims[])
{
	char *mots;
	char *context = NULL;
	char* char_line = (char*)ligne.c_str(); // Non-const cast required.
	mots = strtok_s(char_line, delims, &context);

	string nouveauMot;

	while(mots != NULL)
	{
		nouveauMot.append(mots).append(" ");
		mots = strtok_s(NULL, delims, &context);
	}
	return nouveauMot = nouveauMot.substr(0, nouveauMot.find_last_of(" "));
}
//couper le premier mot
string couper1erMot(string ligne)
{
	return ligne.substr(0, ligne.find_first_of(' '));
}

//trim pour concatener les éléments sans ajouter les espaces à la fin
string trim(string ligne)
{
	char *mots;
	char *context = NULL;
	char* char_line = (char*)ligne.c_str(); // Non-const cast required.
	mots = strtok_s(char_line, " \t", &context);

	string nouveauMot;

	while(mots != NULL)
	{
		nouveauMot.append(mots);
		mots = strtok_s(NULL, " \t", &context);
	}
	return nouveauMot;
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
//méthode prenant comme paramètre un string
bool isLabel(string mot) { return mot[0] == label; }
//méthode pour vérifier si le mot est  un opcode
//on commence par charger le vecteur des opcodes
vector<string> tabOpcodes = extractionInstructions();
bool isOpcode(string mot)
{
	compteur = 0;
	for(int i(0); i < tabOpcodes.size(); i++ )
	{
		if(mot == tabOpcodes.at(i))
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
//
bool isOpLogique(string mot)
{
	compteur = 0;
	for(int i(0); i < tailleLogique; ++i)
	{
		if(mot == opLogique[i])
			compteur++;
	}
	return compteur > 0;
}
//fonction qui cherche les instruction indépendantes
int positionDeuxPoint(string ligne)
{
	size_t position= ligne.find(":");
	return position; // return la position des deux points
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
map<string, string>traitementInstructions(vector<string> lignes)
{
	map<string, string> temp;
	for(vector<string>::iterator it = lignes.begin(); it != lignes.end(); ++it)
	{
		int positionPoint=positionDeuxPoint(*it);
		int positionPoint1=it->find(':',positionPoint+1);
		string signal=it->substr(0,positionPoint);
		string operation=it->substr(positionPoint+1,positionPoint1-(positionPoint+1));
		//
		temp.insert(pair<string, string>(signal, trim(operation, " ")));
	}
	return temp;
}

//fonction qui nous renvoi les plus grand élément d'un tableau
int getMax(vector<TableOpcode> adresse)
{
	vector<string> temp;
	vector<int> temp2;
	for(unsigned int i(0); i < adresse.size(); ++i)
	{
		string temp1 = adresse.at(i).m_adresse.substr(0,adresse.at(i).m_adresse.find("="));
		trim(temp1);//on élimine les espaces
		temp.push_back(temp1);
	}
	//on récupère les positions en éliminant les C
	temp = split(temp, "C");
	//on parcourt le temp pour convertir les strings en entier
	for(unsigned int i(0); i < temp.size();  ++i)
		temp2.push_back(atoi(temp.at(i).c_str()));//on converti les strings en entier
	//on retourne le plus grand élément
	return *max_element(temp2.begin(), temp2.end());
}
//fonction jeux d'instructions
vector<TableOpcode> JeuxInstructions(vector<string> instructions)
{
	//on fait appel à la fonction traitement instruction
	TableOpcode signaux;//va juste prendre le signal et l'opcode
	TableOpcode bitActivE;//
	vector<TableOpcode> vectSignaux; // lui va prendre la structure des signaux
	map<string, string>matemp = traitementInstructions(instructions);
	for(map<string, string>::iterator it = matemp.begin(); it != matemp.end(); ++it)
	{
		//seconde fonction prend en paramètre que le it->first qui représente les signaux
		signaux.m_adresse = it->first;
		signaux.m_opcode  = it->second;
		vectSignaux.push_back(signaux);
	}
	//on récupère le plus grand élément pour avoir la taille de notre vecteur des bits d'activations
	//on fera toujours taille + 2 parce que les jeux d'instructions commencent de 0 à n-1
	int tailleTableaux = getMax(vectSignaux) + 2;
	vector<int> activESignaux(tailleTableaux, 0);//vecteur qui va contenir le bit activé du tableaux des signaux
	//on travaille sur le champ signaux
	for(unsigned int i(0); i < vectSignaux.size(); ++i)
	{
		vector<string> vectTemp = split(vectSignaux.at(i).m_adresse, "=");//paires et impaires
		vector<string> vectTemp2;//contiendra les différentes positions des bits à activer
		ActivationBit ab;//va contenir temporairement la paire position et bit à activer

		vector<int> activESignaux(tailleTableaux, 0);

		for(unsigned int j(0); j < vectTemp.size(); ++j)
		{
			vectTemp.at(j) = trim(vectTemp.at(j));//on élimine les espaces entre les C
			//si on est à la position paire on enlève le C et on convertit le string en int pour avoir la position
			if((j%2) == 0)
			{
				vectTemp2 = split(vectTemp.at(j), "C");//on récupère le nombre après le C
				for(unsigned int k(0); k < vectTemp2.size(); ++k)
					ab.position.push_back(atoi(vectTemp2.at(k).c_str()));//conversion du nombre après le C en entier
					
			}//si non on récupère le bit à activer
			else if(j%2 != 0)
			{
				string temp = vectTemp.at(j);
				for(unsigned int k(0); k < temp.size(); ++k)
					ab.bitActivE.push_back(static_cast<int>(temp.at(k))%48);//%48 puisque il le converti en ASCII
			}
			//on active les signaux on ab
			for(unsigned int k(0); k < ab.bitActivE.size(); ++k)
			{
				int pos = ab.position.at(k);
				int bit = ab.bitActivE.at(k);
				activESignaux.at(pos) = bit ;	//on place chaque bit à la position approprié
			}	
		}
		//on replace l'adresse dans le vecteur signaux seulement à son adresse
		for(unsigned int j(0); j < activESignaux.size(); ++j)
		{
			stringstream ss;
			copy( activESignaux.begin(), activESignaux.end(), std::ostream_iterator<int>(ss, ""));
			string ConditionAdresse = ss.str();
			vectSignaux.at(i).m_adresse = ConditionAdresse.substr(0, ConditionAdresse.length()-1);
		}
	}
	return vectSignaux;
}
