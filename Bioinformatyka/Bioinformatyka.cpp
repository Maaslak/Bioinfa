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

const int l = 10, s = 10, c = 209, u = 5, d = 0;
vector<char*> oligonucleotydes;
vector<pair<int*, int>> population;
int** costMatrix = NULL;
vector <vector <int>> costIntegrals;
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

void clearVec2(vector<pair<int*, int>> vec) {
	while (!vec.empty()) {
		delete(vec.back().first);
		vec.pop_back();
	}
}


// Calculates len of connection of two specific oligonucleotydes
int calcLen(char* a, char* b) {
	int len = l - 1;
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
	for (int i = 0; i < oligonucleotydes.size(); i++) {
		costMatrix[i] = new int[oligonucleotydes.size()];
	}
	for (int i = 0; i < oligonucleotydes.size(); i++) {
		vector<int> tempVec;
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			costMatrix[i][j] = calcLen(oligonucleotydes[i], oligonucleotydes[j]);
			int add = l - (costMatrix[i][j] + 1);
			add *= add;
			if (j == 0) {
				tempVec.push_back(add);
			}
			else
			{
				tempVec.push_back(tempVec[j - 1] + add);
			}
		}
		costIntegrals.push_back(tempVec);
	}
}
void initialize(char* problemPath) {
	parseInput(problemPath);
	initCostMatrix();
	
}

void createPopulation(float div = 1) {
	
	map<int, int> oliMap;
	population.push_back(make_pair( new int[oligonucleotydes.size()], 0));
	population[0].first[0] = 0;
	oliMap.insert(pair<int, int>(0, 0));
	int sum = 10, wyn = 0;

	for (int j = 1; j < oligonucleotydes.size(); j++) {
		int min = INT_MAX, id = -1;
		for (int k = 0; k < oligonucleotydes.size(); k++) {
			if ((costMatrix[population[0].first[j - 1]][k] < min) && !oliMap.count(k)) {
				min = costMatrix[population[0].first[j - 1]][k];
				id = k;
			}
		}
		population[0].first[j] = id;
		sum += costMatrix[population[0].first[j - 1]][id] + 1;
		if (sum <= 209)
			wyn++;
		oliMap.insert(pair<int, int>(id, id));
	}
	
	
	for (int i = 1; i < (int)(oligonucleotydes.size()/ div); i++) {
		population.push_back(make_pair( new int[oligonucleotydes.size()], 0));
		for (int j = 0; j < oligonucleotydes.size(); j++) {
			population[i].first[j] = j;
		}
		random_shuffle(&population[i].first[0], &population[i].first[oligonucleotydes.size()]);
	}
}

//create table with values of Goal function answering positions of individuals in population (tested, working)
void goalFunction(int n) {
	for (int i = 0; i < population.size(); i++) {
		
		int sum1 , len;
		len = l;
		sum1 = 0;
		
		int end = 1;
		int totalSum = 0;
		int minSum = INT_MAX;
		numberOfOligonucleotydes[i] = 0;
		int startMin = 0;
		for (int start = 0; start < oligonucleotydes.size(); start++)
		{
			totalSum += costMatrix[population[i].first[start]][population[i].first[(start+1)%oligonucleotydes.size()]];
			int prev = end - 1;
			if (prev < 0)
				prev += oligonucleotydes.size();
			while (len + costMatrix[population[i].first[prev]][population[i].first[end]] + 1 <= n )
			{
				sum1 += costMatrix[population[i].first[prev]][population[i].first[end]];
				len += costMatrix[population[i].first[prev]][population[i].first[end]] + 1;
				end= (end + 1) % oligonucleotydes.size();
				prev = end - 1;
				if (prev < 0)
					prev += oligonucleotydes.size();
			}
			
			if (sum1 < minSum) {
				minSum = sum1;
				startMin = start;
			}
			if ((end - start + oligonucleotydes.size()) % oligonucleotydes.size() > numberOfOligonucleotydes[i])
				numberOfOligonucleotydes[i] = (end - start + oligonucleotydes.size()) % oligonucleotydes.size();
			sum1 -= costMatrix[population[i].first[start]][population[i].first[(start + 1) % oligonucleotydes.size()]];
			len -= costMatrix[population[i].first[start]][population[i].first[(start + 1) % oligonucleotydes.size()]] + 1;
		}

		int goal = (int)(0.5 * minSum) + totalSum;// -numberOfOligonucleotydes[i];
		//goalFunctionValues[i] = goal;
		population[i].second = goal;
		/*
		int * individual = new int[oligonucleotydes.size()];
		for (int j = 0; j < oligonucleotydes.size(); j++)
		{
			individual[j] = population[i][(j+ startMin)%oligonucleotydes.size()];
		}
		delete(population[i]);

		population[i] = individual;
		*/
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
		if (population[lot[i]].second <= population[lot[i + 1]].second && population[lot[i]].second <= population[lot[i + 2]].second) {
			finalLot[i/3] = lot[i];
		}else if (population[lot[i]].second >= population[lot[i + 1]].second && population[lot[i + 1]].second <= population[lot[i + 2]].second) {
			finalLot[i/3] = lot[i + 1];
		}else if (population[lot[i]].second >= population[lot[i + 2]].second && population[lot[i + 1]].second >= population[lot[i + 2]].second) {
			finalLot[i/3] = lot[i + 2];
		}
	}
	delete(lot);
	return finalLot;
}

int findTheBestIndividual() {
	int max = INT_MIN, id = -1;;
	for (int i = 0; i < population.size(); i++) {
		if (numberOfOligonucleotydes[i] > max) {
			max = numberOfOligonucleotydes[i];
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
			max = costMatrix[population[i].first[oligonucleotydes.size() - 1]][population[i].first[0]] + costMatrix[population[i].first[0]][population[i].first[1]];
			id = 0;
			for (int j = 1; j < oligonucleotydes.size() - 1; j++) {
				if (costMatrix[population[i].first[j - 1]][population[i].first[j]] + costMatrix[population[i].first[j]][population[i].first[j + 1]] > max) {
					max = costMatrix[population[i].first[j - 1]][population[i].first[j]] + costMatrix[population[i].first[j]][population[i].first[j + 1]];
					id = j;
				}
			}

			int rnd = rand() % costIntegrals[population[i].first[id]][oligonucleotydes.size() - 1];
			vector<int>::iterator up = upper_bound((costIntegrals[population[i].first[id]]).begin(), (costIntegrals[population[i].first[id]]).end(), rnd);
			int oligonucleotydeToSwap = up - costIntegrals[population[i].first[id]].begin();
			//printf("%d ", costMatrix[population[i].first[id]][oligonucleotydeToSwap]);
			int * oligonucleotydeAdrr = find(population[i].first, population[i].first + oligonucleotydes.size(), oligonucleotydeToSwap);
			int id2 = oligonucleotydeAdrr - population[i].first;
			//printf("%d ", costMatrix[population[i].first[id]][population[i].first[id2]]);
			id2--;
			if (id2 < 0)
				id2 = oligonucleotydes.size() - 1;
			swap(population[i].first[id], population[i].first[id2]);
			/*
			int max = INT_MIN, id = -1;
			max = costMatrix[population[i][oligonucleotydes.size()-1]][population[i][0]] + costMatrix[population[i][0]][population[i][1]];
			id = 0;
			for (int j = 1; j < oligonucleotydes.size() - 1; j++) {
				if (costMatrix[population[i][j]][population[i][j + 1]] > max) {
					max = costMatrix[population[i][j - 1]][population[i][j]] + costMatrix[population[i][j]][population[i][j + 1]];
					id = j;
				}
			}

			int min = INT_MAX, id2 = -1;
			for (int j = 0;j<oligonucleotydes.size(); j++) {
				int sum = costMatrix[population[i][(j - 1 + oligonucleotydes.size())%oligonucleotydes.size()]][population[i][id]];
				sum += costMatrix[population[i][id]][population[i][(j + 1)%oligonucleotydes.size()]];
				sum += costMatrix[population[i][(id - 1 + oligonucleotydes.size()) % oligonucleotydes.size()]][population[i][j]];
				sum += costMatrix[population[i][j]][population[i][(id + 1) % oligonucleotydes.size()]];
				if (sum < min) {
					min = sum;
					id2 = j;
				}
			}
			swap(population[i][id], population[i][id2]);
			*/
			/*int id11 = 0, id21 = 0, id12 = 0, id22 = 0, sum1 = 0, sum2 = 0;
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
				swap(population[i][id], population[i][id12]);*/
			
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

void mutation2(int u) {
	for (int i = 0; i < population.size(); i++) {
		int rnd = rand() % 100;
		if (rnd < u) {
			int max = INT_MIN, id = -1;
			max = costMatrix[population[i].first[oligonucleotydes.size() - 1]][population[i].first[0]] + costMatrix[population[i].first[0]][population[i].first[1]];
			id = 0;
			for (int j = 1; j < oligonucleotydes.size() - 1; j++) {
				if (costMatrix[population[i].first[j]][population[i].first[j + 1]] > max) {
					max = costMatrix[population[i].first[j - 1]][population[i].first[j]] + costMatrix[population[i].first[j]][population[i].first[j + 1]];
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

			sum1 = costMatrix[population[i].first[id21]][population[i].first[id]] + costMatrix[population[i].first[id]][population[i].first[id11]];

			if (id + 1 >= oligonucleotydes.size())
				id12 = id + 1 - oligonucleotydes.size();
			else
				id12 = id + 1;
			if (id + 2 >= oligonucleotydes.size())
				id22 = id + 2 - oligonucleotydes.size();
			else
				id22 = id + 2;

			sum2 = costMatrix[population[i].first[id12]][population[i].first[id]] + costMatrix[population[i].first[id]][population[i].first[id22]];

			if (sum1 > sum2)
				swap(population[i].first[id], population[i].first[id11]);
			else
				swap(population[i].first[id], population[i].first[id12]);

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

bool comp(pair<int*, int> i, pair<int*, int> j) { return (i.second < j.second); }

//Start crossing
void crossing(int c) {
	vector<pair<int*, int>> newPopulation;

	int* finalLot = chooseIndividuals(c);
	
	list<int> individual1;
	list<int> individual2;
	list<int>::iterator it1 = individual1.begin();
	list<int>::iterator it2 = individual2.begin();
	
	bool* individualMap1 = new bool[oligonucleotydes.size()];
	bool* individualMap2 = new bool[oligonucleotydes.size()];

	

	for (int i = 0; i < c / 3 - 1 ; i++) {
		individual1.clear();
		individual2.clear();
		for (int j = 0; j < oligonucleotydes.size(); j++)
			individualMap1[j] = individualMap2[j] = false;

		int startRand = rand() % (oligonucleotydes.size() - 2) + 1;
		int endRand = rand() % (oligonucleotydes.size() - 2) + 1;

		if (endRand < startRand)
			swap(endRand, startRand);


		for (int j = startRand; j <= endRand; j++) {
			individual1.push_back(population[finalLot[i]].first[j]);
			individualMap1[population[finalLot[i]].first[j]] = true;
			individual2.push_back(population[finalLot[i+1]].first[j]);
			individualMap2[population[finalLot[i + 1]].first[j]] = true;
		}


		for (int j = endRand + 1; j < oligonucleotydes.size(); j++) {	
			if (!individualMap1[population[finalLot[i + 1]].first[j]]) {
				individual1.push_back(population[finalLot[i + 1]].first[j]);
				individualMap1[population[finalLot[i + 1]].first[j]] = true;
			}
		}

		for (int j = endRand + 1; j < oligonucleotydes.size(); j++) {
			if (!individualMap2[population[finalLot[i]].first[j]]) {
				individual2.push_back(population[finalLot[i]].first[j]);
				individualMap2[population[finalLot[i]].first[j]] = true;
			}
		}

		for (int j = startRand - 1; j >= 0; j--) {
			if (!individualMap2[population[finalLot[i]].first[j]]) {
				individual2.push_front(population[finalLot[i]].first[j]);
				individualMap2[population[finalLot[i]].first[j]] = true;
			}
		}

		for (int j = startRand - 1; j >= 0; j--) {
			if (!individualMap1[population[finalLot[i + 1]].first[j]]) {
				individual1.push_front(population[finalLot[i + 1]].first[j]);
				individualMap1[population[finalLot[i + 1]].first[j]] = true;
			}
		}

		list<int> missing1, missing2;

		for (int j = 0; j < oligonucleotydes.size(); j++) {
			if (!individualMap1[j])
				missing1.push_back(j);
			if (!individualMap2[j])
				missing2.push_back(j);
		}
		
		
		for (list<int>::iterator it = missing1.begin(); it != missing1.end(); ++it) {
			int value = *it;
			int min = INT_MAX;
			list<int>::iterator itt = individual1.begin();
			min = costMatrix[individual1.back()][value] + costMatrix[value][individual1.front()];
			list<int>::iterator id = individual1.begin();
			++itt;
			list<int>::iterator prev = individual1.begin();
			for (itt; itt != individual1.end(); ++itt, prev++) {	
				if (next(itt) == individual1.end())
					break;
				int sum = costMatrix[value][*(itt)];
				sum += costMatrix[*(prev)][value];
				if (sum < min) {
					min = sum;
					id = itt;
					if (min == 0)
						break;
				}
			}
			
			individual1.insert(id, value);
		}
		

		
		for (list<int>::iterator it = missing2.begin(); it != missing2.end(); ++it) {
			int value = *it;
			int min = INT_MAX;
			list<int>::iterator itt = individual2.begin();
			min = costMatrix[individual2.back()][value] + costMatrix[value][individual2.front()];
			list<int>::iterator id = individual2.begin();
			list<int>::iterator prev = individual2.begin();
			++itt;
			for (itt; itt != individual2.end(); ++itt, ++prev) {
				if (next(itt) == individual2.end())
					break;
				int sum = costMatrix[value][*itt];
				sum += costMatrix[*prev][value];
				if (sum < min) {
					min = sum;
					id = itt;
					if (min == 0)
						break;
				}
			}
			individual2.insert(id, value);
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
		newPopulation.push_back(make_pair(tmp, 0));
		//delete(tmp);
		tmp = new int[individual2.size()];
		ii = 0;
		for (list<int>::const_iterator iterator = individual2.begin(), end = individual2.end(); iterator != end; ++iterator) {
			tmp[ii] = *iterator;
			ii++;
		}
		newPopulation.push_back(make_pair(tmp, 0));
		//delete(tmp);
	}
	delete(individualMap1);
	delete(individualMap2);

	

	int populationSize = population.size();

	partial_sort(population.begin(), population.begin() + (population.size() - newPopulation.size()), population.end(), comp);
	while (population.size() + newPopulation.size() != populationSize) {
		delete(population.back().first);
		population.pop_back();
	}
	while (!population.empty()) {
		newPopulation.push_back(population.back());
		population.pop_back();
	}


	//clearVec2(population);
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
		delete(population[i].first);
	}
	delete(costMatrix);
	delete(numberOfOligonucleotydes);
	clearVec(oligonucleotydes);
}

int main()
{
	n = 509;
	srand(time(0));
	initialize("53500-200.txt");
	createPopulation();
	numberOfOligonucleotydes = new int[population.size()];
	goalFunction(n);
	/*
	for (int i = 0; i < oligonucleotydes.size(); i++) {
		printf("%d ", population[0][i]);
	}*/
	printf("\n");
	int id = findTheBestIndividual();
	printf("Najelpszy wynik ma wartosci %d ktory zawiera %d olinukleotydow do dlugosci %d\n", population[0].second, numberOfOligonucleotydes[0], n);
	
	while (true) {
		crossing(population.size());
		mutation(u);
		goalFunction(n);
		int id = findTheBestIndividual();
		printf("Najelpszy wynik ma wartosci %d ktory zawiera %d olinukleotydow do dlugosci %d\n", population[id].second, numberOfOligonucleotydes[id], n);

	}
	
	/*
	for (int i = 0; i < oligonucleotydes.size(); i++) {
		for (int j = 0; j < oligonucleotydes.size(); j++) {			
			printf("%d ", costMatrix[i][j]);
		}
		printf("\n");
	}*/

	//printf("%d", calcLen(oligonucleotydes[0], oligonucleotydes[0]));
	
	//clear();
	system("pause");
    return 0;
}