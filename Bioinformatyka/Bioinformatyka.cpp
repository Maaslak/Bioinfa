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
int ** population;

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

// Calculates len of connection of two specific oligonucleotydes
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

void createPopulation() {
	population = new int*[oligonucleotydes.size()/2];
	for (int i = 0; i < oligonucleotydes.size(); i++)
		population[i] = new int[oligonucleotydes.size()];
	for (int i = 0; i < oligonucleotydes.size()/2; i++) {
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			population[i][j] = j;			
		}
		random_shuffle(&population[i][0], &population[i][oligonucleotydes.size()]);
	}
			
}


void clear() {
	for (int i = 0; i < oligonucleotydes.size(); i++) {
		delete(costMatrix[i]);
		//delete(population[i]);
	}
	delete(costMatrix);
	//delete(population);
	clearVec(oligonucleotydes);
}

int main()
{
	srand(time(0));
	initialize("9200-40.txt");
	createPopulation();
	clear();
	system("pause");
    return 0;
}
