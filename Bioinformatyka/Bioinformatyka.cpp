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
#include <list>
#include <map>

using namespace std;

const int l = 10, s = 10, c = 209, u = 0, d = 0;
vector<char*> oligonucleotydes;
vector<int*> population;
int** costMatrix = NULL;
int* goalFunctionValues = NULL;
int *numberOfOligonucleotydes = NULL;
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
	
	for (int i = 0; i < oligonucleotydes.size()/ div; i++) {
		population.push_back(new int[oligonucleotydes.size()]);
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			population[i][j] = j;			
		}
		random_shuffle(&population[i][0], &population[i][oligonucleotydes.size()]);
	}
}

//create table with values of Goal function answering positions of individuals in population (tested, working)
void goalFunction(int n) {
	for (int i = 0; i < population.size(); i++) {
		
		int sum1 , sum2, len;
		len = l;
		sum1 = 0;
		sum2 = 0;
		
		int end = 1;
		int totalSum = 0;
		int maxSum = 0;
		int start;
		for (start = 0; start < oligonucleotydes.size(); start++)
		{
			totalSum += costMatrix[population[i][start]][population[i][(start+1)%oligonucleotydes.size()]];
			int prev = end - 1;
			if (prev < 0)
				prev += oligonucleotydes.size();
			while (len + costMatrix[population[i][prev]][population[i][end]] + 1 <= n )
			{
				prev = end - 1;
				if (prev < 0)
					prev += oligonucleotydes.size();
				sum1 += costMatrix[population[i][prev]][population[i][end]];
				len += costMatrix[population[i][prev]][population[i][end]] + 1;
				end= (end + 1) % oligonucleotydes.size();
			}
			if (sum1 > maxSum) {
				maxSum = sum1;
				numberOfOligonucleotydes[i] = (end - start + oligonucleotydes.size()) % oligonucleotydes.size();
			}
			if (sum1 < 0)
				throw new exception();
			sum1 -= costMatrix[population[i][start]][population[i][(start+1)%oligonucleotydes.size()]];
			len -= costMatrix[population[i][start]][population[i][(start + 1) % oligonucleotydes.size()]] + 1;
		}

		int goal = (int)(0.5 * sum1) + totalSum;
		goalFunctionValues[i] = goal;
		int * individual = new int[oligonucleotydes.size()];
		for (int j = 0; j < oligonucleotydes.size(); j++)
		{
			individual[j] = population[i][(j+start)%oligonucleotydes.size()];
		}
		delete(population[i]);
		population[i] = individual;
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
	
	list<int> individual1;
	list<int> individual2;
	list<int>::iterator it1 = individual1.begin();
	list<int>::iterator it2 = individual2.begin();
	map<int, int> individualMap1;
	map<int, int> individualMap2;

	

	for (int i = 0; i < c / 3 - 1 ; i++) {
		individual1.clear();
		individual2.clear();
		individualMap1.clear();
		individualMap2.clear();
		for (int i = 0; i < oligonucleotydes.size(); i++) {
			individualMap1.insert(pair<int,int>(i, i));
			individualMap2.insert(pair<int,int>(i, i));
		}

		int startRand = rand() % (oligonucleotydes.size() - 2) + 1;
		int endRand = rand() % (oligonucleotydes.size() - 2) + 1;

		if (endRand < startRand)
			swap(endRand, startRand);

		for (int j = startRand; j <= endRand; j++) {
			individual1.push_back(population[finalLot[i]][j]);
			individualMap1.erase(population[finalLot[i]][j]);
			individual2.push_back(population[finalLot[i+1]][j]);
			individualMap2.erase(population[finalLot[i + 1]][j]);
		}


		for (int j = endRand + 1; j < oligonucleotydes.size(); j++) {	
			if (find(individual1.begin(), individual1.end(), population[finalLot[i + 1]][j]) != individual1.end()) {
				individual1.push_back(population[finalLot[i + 1]][j]);
				individualMap1.erase(population[finalLot[i + 1]][j]);
			}
		}

		for (int j = endRand + 1; j < oligonucleotydes.size(); j++) {
			if (find(individual2.begin(), individual2.end(), population[finalLot[i]][j]) != individual2.end()) {
				individual2.push_back(population[finalLot[i]][j]);
				individualMap2.erase(population[finalLot[i]][j]);
			}
		}

		for (int j = startRand - 1; j >= 0; j--) {
			if (find(individual2.begin(), individual2.end(), population[finalLot[i]][j]) != individual2.end()) {
				individual2.push_front(population[finalLot[i]][j]);
				individualMap2.erase(population[finalLot[i]][j]);
			}
		}

		for (int j = startRand - 1; j >= 0; j--) {
			if (find(individual1.begin(), individual1.end(), population[finalLot[i + 1]][j]) != individual1.end()) {
				individual1.push_front(population[finalLot[i + 1]][j]);
				individualMap1.erase(population[finalLot[i + 1]][j]);
			}
		}

		for (map<int, int>::iterator it = individualMap1.begin(); it != individualMap1.end(); ++it) {
			int value = it->second;
			int min = INT_MAX, id = - 1;
			list<int>::iterator itt = individual1.begin();
			min = costMatrix[individual1.back()][value] + costMatrix[value][individual1.front()];
			id = 0;
			++itt;
			int i = 1;
			for (itt; itt != individual1.end(); ++itt) {	
				++itt;
				if (itt == individual1.end())
					break;
				--itt;
				int sum = costMatrix[value][*(itt)];
				--itt;
				sum += costMatrix[*(itt)][value];
				++itt;
				if (sum < min) {
					min = sum;
					id = i;
				}
				i++;
			}
			itt = individual1.begin();
			for (int i = 0; i < id; i++)
				++itt;
			individual1.insert(itt, value);
		}


		for (map<int, int>::iterator it = individualMap2.begin(); it != individualMap2.end(); ++it) {
			int value = it->second;
			int min = INT_MAX, id = -1;
			list<int>::iterator itt = individual2.begin();
			min = costMatrix[individual2.back()][value] + costMatrix[value][individual2.front()];
			id = 0;
			++itt;
			int i = 1;
			for (itt; itt != individual2.end(); ++itt) {
				++itt;
				if (itt == individual2.end())
					break;
				--itt;
				int sum = costMatrix[value][*itt];
				--itt;
				sum += costMatrix[*itt][value];
				++itt;
				if (sum < min) {
					min = sum;
					id = i;
				}
				i++;
			}
			itt = individual2.begin();
			for (int i = 0; i < id; i++)
				++itt;
			individual2.insert(itt, value);
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

		int* tmp;
		tmp = new int[individual1.size()];
		int ii = 0;
		for (list<int>::const_iterator iterator = individual1.begin(), end = individual1.end(); iterator != end; ++iterator) {
			tmp[ii] = *iterator;
			ii++;
		}
		/*for (int z = 0; z < individual1.size(); z++) {
			printf("%d ", tmp[z]);
		}
		printf("\n");*/
		newPopulation.push_back(tmp);
		//delete(tmp);
		tmp = new int[individual2.size()];
		ii = 0;
		for (list<int>::const_iterator iterator = individual2.begin(), end = individual2.end(); iterator != end; ++iterator) {
			tmp[ii] = *iterator;
			ii++;
		}
		newPopulation.push_back(tmp);
		//delete(tmp);
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
	delete(numberOfOligonucleotydes);
	clearVec(oligonucleotydes);
}

int main()
{
	n = 209;
	srand(time(0));
	initialize("9200-40.txt");
	createPopulation();
	goalFunctionValues = new int[population.size()];
	numberOfOligonucleotydes = new int[population.size()];
	goalFunction(n);
	while (true) {
		crossing(population.size());
		mutation(u);
		goalFunction(n);
		int id = findTheBestIndividual();
		printf("Najelpszy wynik ma wartosci %d ktory zawiera %d olinukleotydow do dlugosci %d\n", goalFunctionValues[id], numberOfOligonucleotydes[id], n);

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