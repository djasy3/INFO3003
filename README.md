INFO3003(school project)
========================

Description:
------------

Faire un programme qui permet la traduction d’un jeu d’instructions décrit dans un fichier texte avec 

un format spécifique, voir ci-dessous. Le fichier de sortie de votre programme doit être un fichier 

texte aussi avec un format spécifique décrit ci-dessous.

**********************************************************************************************

Make a program that allows the translation of an instruction set described in a text file with

a specific format, see below. Your program's output file must be a file

text with a specific format as described below.


example of input file:
---------------------
$start clear A ! Load M ! load L ; début du programme 

$loop if Vb1 then $add; Branchementvers add 

$add add;  A +M 

$sub Sub; A-M;

$Rshift ASR, dec L; 

Q < Bus ; Charger Q du bus

If VB2 then $sub; branchementvers sub 

Go to $Rshift; branchementvers shift

Go to $rshift;

If Z then $loop 

%Bus<A

%Bus<Q

$halt go to $Halt


for full description af the programme go to : 
