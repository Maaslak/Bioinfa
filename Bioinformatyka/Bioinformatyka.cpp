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

const int l = 10, s = 0, c = 209, u = 0, d = 0;
vector<char*> oligonucleotydes;
int** costMatrix = NULL;
int** population = NULL;
int* goalFunctionValues = NULL;
int populationSize = NULL;
int n;

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

void clearVec2(vector<int*> vec) {
	while (!vec.empty()) {
		int* el = vec.back();
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
	if(goalFunctionValues == NULL)
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
int* chooseIndividuals(int c) {
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
	return finalLot;
}

int findTheBestIndividual() {
	int min = INT_MAX, id = -1;;
	for (int i = 0; i < populationSize; i++) {
		if (goalFunctionValues[i] < min) {
			min = goalFunctionValues[i];
			id = i;
		}
	}
	return id;
}

int findTheWorstIndividual() {
	int max = INT_MIN, id = -1;;
	for (int i = 0; i < populationSize; i++) {
		if (goalFunctionValues[i] > max) {
			max = goalFunctionValues[i];
			id = i;
		}
	}
	return id;
}

//Start crossing (do dokończenia)
void crossing(int c) {
	vector<int*> newPopulation;

	goalFunction(n);
	int* finalLot = chooseIndividuals(c);
	int* individual1;
	int* individual2;

	

	for (int i = 0; i < c / 3 - 1 ; i++) {
		individual1 = new int[oligonucleotydes.size()];
		individual2 = new int[oligonucleotydes.size()];
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			individual1[j] = -1;
			individual2[j] = -1;
		}

		int startRand = rand() % (oligonucleotydes.size() - 1);
		int endRand = rand() % (oligonucleotydes.size() - 1);

		if (endRand < startRand)
			swap(endRand, startRand);

		for (int j = startRand; j <= endRand; j++) {
			individual1[j] = population[finalLot[i]][j];
			individual2[j] = population[finalLot[i+1]][j];
		}

		int tmp = endRand;
		for (int j = endRand + 1; j < oligonucleotydes.size(); j++) {	
			tmp++;
			if (tmp >= oligonucleotydes.size())
				tmp = 0;
			int *end = individual1 + oligonucleotydes.size();
			int *result = find(individual1, end, population[finalLot[i + 1]][tmp]);
			if (result == end) {
				individual1[j] = population[finalLot[i + 1]][tmp];
				if (individual1[j] >= oligonucleotydes.size() || individual1[j] < 0)
					printf("\nERROR:  petla1-- i+1: %d, j: %d, value: %d, tmp: %d", i+1, j, individual1[j], tmp);
			}
			else {
				j--;
			}
		}

		tmp = endRand;
		for (int j = endRand + 1; j < oligonucleotydes.size(); j++) {
			tmp++;
			if (tmp >= oligonucleotydes.size())
				tmp = 0;
			int *end = individual2 + oligonucleotydes.size();
			int *result = find(individual2, end, population[finalLot[i]][tmp]);
			if (result == end) {
				individual2[j] = population[finalLot[i]][tmp];
				if (individual2[j] >= oligonucleotydes.size() || individual2[j] < 0)
					printf("\nERROR:  petla2-- i: %d, j: %d, value: %d, tmp: %d", i, j, individual2[j], tmp);
			}
			else {
				j--;
			}
		}

		tmp = -1;
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			tmp++;
			if (tmp >= oligonucleotydes.size())
				tmp = 0;
			if (individual2[j] == -1) {
				int *end = individual2 + oligonucleotydes.size();
				int *result = find(individual2, end, population[finalLot[i]][tmp]);
				if (result == end) {
					individual2[j] = population[finalLot[i]][tmp];
					if (individual2[j] >= oligonucleotydes.size() || individual2[j] < 0)
						printf("\nERROR:  petla3-- i: %d, j: %d, value: %d, tmp: %d", i, j, individual2[j], tmp);
				}
				else {
					j--;
				}
			}
			else
				break;
		}

		tmp = -1;
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			tmp++;
			if (tmp >= oligonucleotydes.size())
				tmp = 0;
			if (individual1[j] == -1) {
				int *end = individual1 + oligonucleotydes.size();
				int *result = find(individual1, end, population[finalLot[i + 1]][tmp]);
				if (result == end) {
					individual1[j] = population[finalLot[i + 1]][tmp];
					if (individual1[j] >= oligonucleotydes.size() || individual1[j] < 0)
						printf("\nERROR:  petla4-- i+1: %d, j: %d, value: %d, tmp: %d", i + 1, j, individual1[j], tmp);
				}
				else {
					j--;
				}
			}
			else
				break;
		}

		/*printf("Individual1: \n");
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			printf("%d: %d ", j,individual1[j]);
		}

		printf("\nIndividual2: \n");
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			printf("%d: %d ",j, individual2[j]);
		}

		printf("\nParrent1 : \n");
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			printf("%d: %d ", j,population[finalLot[i]][j]);
		}

		printf("\nParrent2 : \n");
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			printf("%d: %d ",j, population[finalLot[i + 1]][j]);
		}
		printf("\nstartRand : %d ", startRand);
		printf("   endRand : %d\n", endRand);*/

		newPopulation.push_back(individual1);
		newPopulation.push_back(individual2);
	}

	int iter = 0;
	int id = findTheBestIndividual();
	swap(population[0], population[id]);

	while (newPopulation.size() < populationSize) {
		newPopulation.push_back(population[iter]);
		iter++;
	}
	
	for (int i = 0; i < populationSize; i++) {
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			population[i][j] = newPopulation[i][j];
			//printf("%d ", population[i][j]);
			
		}
		/*int *end = population[i] + oligonucleotydes.size();
		int *result = find(population[i], end, 0);
		if (result == end) {
			printf("\nNie ma zera w %d!!!", i);
		}*/
		//printf("\n");
	}
	vector<int*>().swap(newPopulation);
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
	n = 209;
	srand(time(0));
	initialize("9200-40.txt");
	createPopulation();
	goalFunction(209);
	crossing(populationSize);
	clear();
	system("pause");
    return 0;
}
