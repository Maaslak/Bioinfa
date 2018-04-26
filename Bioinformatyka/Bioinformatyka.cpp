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

const int l = 10, s = 10, c = 209, u = 5, d = 0;
vector<char*> oligonucleotydes;
vector<int*> population;
int** costMatrix = NULL;
int* goalFunctionValues = NULL;
int* lenValues = NULL;
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
	for (int i = 0; i < l - 1; i++)
	{
		bool fit = true;
		for (int j = 0; j < l - 1 - i; j++)
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
	for (int i = 0; i < oligonucleotydes.size()/ div; i++)
		population.push_back(new int[oligonucleotydes.size()]);
	for (int i = 0; i < oligonucleotydes.size()/ div; i++) {
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			population[i][j] = j;			
		}
		random_shuffle(&population[i][0], &population[i][oligonucleotydes.size()]);
	}
}

//create table with values of Goal function answering positions of individuals in population (tested, working)
void goalFunction(int n) {
	for (int i = 0; i < population.size(); i++) {
		
		int* sum1 ,* sum2,* len;
		len = new int[oligonucleotydes.size()];
		sum1 = new int[oligonucleotydes.size()];
		sum2 = new int[oligonucleotydes.size()];
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			len[j] = l;
			sum1[j] = 0;
			sum2[j] = 0;
		}
		int end;
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			int cost = costMatrix[population[i][j]][population[i][(j + 1) % oligonucleotydes.size()]];
			if (len[0] + cost + 1 <= n) {
				sum1[0] += cost;
				len[0] +=  cost + 1;
				end = j;
			}
			else {
				sum2[0] += cost;
				printf("")
			}
		}
		
		for (int j = 1; j < oligonucleotydes.size(); j++) {
			int nextLen = len[j - 1];
			nextLen -= (1 + costMatrix[population[i][j - 1]][population[i][j]]);
			sum1[j] = sum1[j - 1] - costMatrix[population[i][j - 1]][population[i][j]];
			sum2[j] = sum2[j - 1] + costMatrix[population[i][j - 1]][population[i][j]];
			int newEnd = j;
			bool after = false;
			while (nextLen <= n)
			{
				sum1[j] += costMatrix[population[i][(end) % oligonucleotydes.size()]][population[i][(end + j + 1) % oligonucleotydes.size()]];
				sum2[j] -= costMatrix[population[i][(end) % oligonucleotydes.size()]][population[i][(end + j + 1) % oligonucleotydes.size()]];
				nextLen += costMatrix[population[i][(end) % oligonucleotydes.size()]][population[i][(end + j + 1) % oligonucleotydes.size()]] + 1;
				newEnd= (newEnd + 1) % oligonucleotydes.size();
				if (after) {
					int prev = newEnd - 1;
					if (prev < 0)
						prev += oligonucleotydes.size();
					sum2[j] -= costMatrix[population[i][prev]][population[i][newEnd]];
					sum1[j] += costMatrix[population[i][prev]][population[i][newEnd]];
				}
				if (end == newEnd)
					after = true;
				if (sum2[j] < 0) {
					printf("%d", sum2[j]);
					throw new exception();
				}
			}
			end = newEnd;
		}
		int tempGoal = INT_MAX;
		int id;
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			int g = int(1.5*sum1[j]) + sum2[j];
			if (g < tempGoal) {
				tempGoal = g;
				id = j;
			}
		}

		goalFunctionValues[i] = tempGoal;
		if (tempGoal < 0)
			throw new exception();
		lenValues[i] = len[id];
		
		delete(len);
		delete(sum1);
		delete(sum2);
		
	}
}

int returnGoalFunction(int n, int id) {
	int sum1 = 0, sum2 = 0, len = 0;
	for (int j = 0; j < oligonucleotydes.size() - 1; j++) {
		if (len <= n) {
			sum1 += costMatrix[population[id][j]][population[id][j + 1]];
			len += costMatrix[population[id][j]][population[id][j + 1]];
		}
		else {
			sum2 += costMatrix[population[id][j]][population[id][j + 1]];
		}
	}
	return int(1.5*sum1) + sum2;
}



//Choose individuals to parental population (tested, working)
int* chooseIndividuals(int c) {
	c = c - c % 3; 
	int* lot = new int[c];
	for (int j = 0; j < c; j++) {
		lot[j] = -1;
	}
	for (int i = 0; i < c; i++) {
		int rng = rand() % population.size();
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
	delete(lot);
	return finalLot;
}

int findTheBestIndividual() {
	int min = INT_MAX, id = -1;;
	for (int i = 0; i < population.size(); i++) {
		if (goalFunctionValues[i] < min) {
			min = goalFunctionValues[i];
			id = i;
		}
	}
	return id;
}

int findTheWorstIndividual() {
	int max = INT_MIN, id = -1;;
	for (int i = 0; i < population.size(); i++) {
		if (goalFunctionValues[i] > max) {
			max = goalFunctionValues[i];
			id = i;
		}
	}
	return id;
}

//tylko na rodzicach, do poprawy
void mutation(int u) {
	for (int i = 0; i < population.size(); i++) {
		int rnd = rand() % 100;
		if (rnd < u) {
			int max = INT_MIN, id = -1;
			max = costMatrix[population[i][oligonucleotydes.size()-1]][population[i][0]] + costMatrix[population[i][0]][population[i][1]];
			id = 0;
			for (int j = 1; j < oligonucleotydes.size() - 1; j++) {
				if (costMatrix[population[i][j]][population[i][j + 1]] > max) {
					max = costMatrix[population[i][j - 1]][population[i][j]] + costMatrix[population[i][j]][population[i][j + 1]];
					id = j;
				}
			}

			int id11 = 0, id21 = 0, id12 = 0, id22 = 0, sum1 = 0, sum2 = 0;
			if (id - 1 < 0)
				id11 = id - 1 + oligonucleotydes.size();
			else
				id11 = id - 1;
			if (id - 2 < 0)
				id21 = id - 2 + oligonucleotydes.size();
			else
				id21 = id - 2;

			sum1 = costMatrix[population[i][id21]][population[i][id]] + costMatrix[population[i][id]][population[i][id11]];

			if (id + 1 >= oligonucleotydes.size())
				id12 = id + 1 - oligonucleotydes.size();
			else
				id12 = id + 1;
			if (id + 2 >= oligonucleotydes.size())
				id22 = id + 2 - oligonucleotydes.size();
			else
				id22 = id + 2;

			sum2 = costMatrix[population[i][id12]][population[i][id]] + costMatrix[population[i][id]][population[i][id22]];

			if (sum1 > sum2)
				swap(population[i][id], population[i][id11]);
			else
				swap(population[i][id], population[i][id12]);
			
			/*
			int minCost = INT_MAX;
			for (int k = 0; k < n - 1; k++) {
				if (k != id ) {
					int tempCost = costMatrix[k][id];
				}
			}
			/*
				if (costMatrix[id][tmp2] > costMatrix[tmp1][id])
					swap(population[i][id], population[i][tmp1]);
				else
					swap(population[i][id], population[i][tmp2]);
					*/
		}
	}
}

//Start crossing
void crossing(int c) {
	vector<int*> newPopulation;

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

	int populationSize = population.size();
	while (newPopulation.size() < populationSize) {
		int id = findTheBestIndividual();
		swap(population[population.size()-1], population[id]);
		newPopulation.push_back(population[population.size()-1]);
		population.pop_back();
		iter++;
	}


	clearVec2(population);
		/*int *end = population[i] + oligonucleotydes.size();
		int *result = find(population[i], end, 0);
		if (result == end) {
			printf("\nNie ma zera w %d!!!", i);
		}
		printf("\n");*/
	population.swap(newPopulation);

	//vector<int*>().swap(newPopulation);
}


void clear() {
	for (int i = 0; i < oligonucleotydes.size(); i++) {
		delete(costMatrix[i]);
	}
	for (int i = 0; i < oligonucleotydes.size()/2; i++) {
		delete(population[i]);
	}
	delete(costMatrix);
	delete(goalFunctionValues);
	delete(lenValues);
	clearVec(oligonucleotydes);
}

int countOlinucleotydes(int id) {
	int count = 0, len =0;
	for (int i = 0; i < oligonucleotydes.size() - 1; i++) {
		len += costMatrix[population[id][i]][population[id][i + 1]];
		if (len > n)
			break;
		count++;
	}
	return count;
}

int main()
{
	n = 209;
	srand(time(0));
	initialize("9200-40.txt");
	createPopulation();
	goalFunctionValues = new int[population.size()];
	lenValues = new int[population.size()];
	goalFunction(n);
	while (true) {
		crossing(population.size());
		mutation(u);
		goalFunction(n);
		int id = findTheBestIndividual();
		printf("Najelpszy wynik ma wartosci %d ktory zawiera %d olinukleotydow do dlugosci %d\n", goalFunctionValues[id], countOlinucleotydes(id), n);
	}
	

	for (int i = 0; i < oligonucleotydes.size(); i++) {
		for (int j = 0; j < oligonucleotydes.size(); j++) {			
			printf("%d ", costMatrix[i][j]);
		}
		printf("\n");
	}

	//printf("%d", calcLen(oligonucleotydes[0], oligonucleotydes[0]));
	
	clear();
	system("pause");
    return 0;
}