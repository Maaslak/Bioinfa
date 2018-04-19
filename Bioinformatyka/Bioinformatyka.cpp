// Bioinformatyka.cpp: Definiuje punkt wejścia dla aplikacji konsolowej.
#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <stdio.h>
#include <string>
#include <fstream>
#include <list>
#include <time.h>
#include <algorithm>
#include <stdlib.h>

using namespace std;

const int l = 10, s = 0, c = 0, u = 0;
vector<char*> oligonucleotydes;
int** costMatrix;
int** population;
int* goalFunctionValues;
int populationSize;

void parseInput(char* filepath) {	
	FILE* file = fopen(filepath, "r");
	char* el;
	char oligonucleotyde[l + 1];
	int nread;
	while ((nread = fread(oligonucleotyde, 1, sizeof oligonucleotyde, file)) > 0) {
		oligonucleotyde[l] = '\0';
		el = new char[l + 1];
		strcpy(el, oligonucleotyde);
		oligonucleotydes.push_back(el);
	}
}

void clearVec(vector<char*> vec) {
	while (!vec.empty()) {
		char* el = vec.back();
		delete(el);
		vec.pop_back();
	}
}

void printVec(vector<char*> vec) {
	for (int i = 0; i < vec.size(); i++)
		printf("%s\n", vec[i]);
}

// Calculates len of connection of two specific oligonucleotydes (powinno być chyba że porównujesz początek pierwszego z końcem 2 a nie początek z początkiem chyba że czegoś nie czaje 
int calcLen(char* a, char* b) {
	int len = l;
	for (int i = 0; i < l-1; i++)
	{
		bool fit = true;
		for (int j = 0; j < l-1-i; j++)
		{
			if (a[i + j + 1] != b[j]) {
				fit = false;
				break;
			}
		}
		if (fit) {
			len = i;
			break;
		}
	}
	return len;
}

void initCostMatrix() {
	costMatrix = new int*[oligonucleotydes.size()];
	for (int i = 0; i < oligonucleotydes.size(); i++)
		costMatrix[i] = new int[oligonucleotydes.size()];
	for (int i = 0; i < oligonucleotydes.size(); i++)
		for (int j = 0; j < oligonucleotydes.size(); j++)
			costMatrix[i][j] = calcLen(oligonucleotydes[i], oligonucleotydes[j]);
	
}
void initialize(char* problemPath) {
	parseInput(problemPath);
	initCostMatrix();
	
}

void createPopulation(int div = 2) {
	population = new int*[oligonucleotydes.size()/ div];
	populationSize = oligonucleotydes.size() / div;
	for (int i = 0; i < oligonucleotydes.size()/ div; i++)
		population[i] = new int[oligonucleotydes.size()];
	for (int i = 0; i < oligonucleotydes.size()/ div; i++) {
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			population[i][j] = j;			
		}
		random_shuffle(&population[i][0], &population[i][oligonucleotydes.size()]);
	}
}

//create table with values of Goal function answering positions of individuals in population (tested, working)
void goalFunction(int n) {
	goalFunctionValues = new int[populationSize];
	for (int i = 0; i < populationSize; i++) {
		int sum1 = 0, sum2 = 0, len = 0;
		for (int j = 0; j < oligonucleotydes.size() - 1; j++) {
			if (len <= n) {
				sum1 += costMatrix[population[i][j]][population[i][j + 1]];
				len += costMatrix[population[i][j]][population[i][j + 1]];
			}
			else {
				sum2 += costMatrix[population[i][j]][population[i][j + 1]];
			}
		}
		goalFunctionValues[i] = int(1.5*sum1) + sum2;
	}
}



//Choose individuals to parental population (tested, working)
void chooseIndividuals(int c) {
	c = c - c % 3; 
	int* lot = new int[c];
	for (int j = 0; j < c; j++) {
		lot[j] = -1;
	}
	for (int i = 0; i < c; i++) {
		int rng = rand() % populationSize;
		bool recap = false;
		for (int j = 0; j < c; j++) {
			if (lot[j] == rng) {
				recap = true;
				break;
			}
		}
		if (!recap) {
			lot[i] = rng;
			printf("%d ", lot[i]);
		}
		else {
			i--;
		}
	}
	int* finalLot = new int[c/3];
	for (int j = 0; j < c/3; j++) {
		finalLot[j] = -1;
	}

	for (int i = 0; i < c - 2; i+=3) {
		if (goalFunctionValues[lot[i]] <= goalFunctionValues[lot[i + 1]] && goalFunctionValues[lot[i]] <= goalFunctionValues[lot[i + 2]]) {
			finalLot[i/3] = lot[i];
		}else if (goalFunctionValues[lot[i]] >= goalFunctionValues[lot[i + 1]] && goalFunctionValues[lot[i + 1]] <= goalFunctionValues[lot[i + 2]]) {
			finalLot[i/3] = lot[i + 1];
		}else if (goalFunctionValues[lot[i]] >= goalFunctionValues[lot[i + 2]] && goalFunctionValues[lot[i + 1]] >= goalFunctionValues[lot[i + 2]]) {
			finalLot[i/3] = lot[i + 2];
		}
	}
}

//Start crossing (do dokończenia)
void crossing(int c) {
	chooseIndividuals(c);
}


void clear() {
	for (int i = 0; i < oligonucleotydes.size(); i++) {
		delete(costMatrix[i]);
	}
	for (int i = 0; i < oligonucleotydes.size()/2; i++) {
		delete(population[i]);
	}
	delete(costMatrix);
	delete(population);
	delete(goalFunctionValues);
	clearVec(oligonucleotydes);
}

int main()
{
	srand(time(0));
	initialize("9200-40.txt");
	createPopulation();
	goalFunction(209);
	crossing(20);
	clear();
	system("pause");
    return 0;
}
