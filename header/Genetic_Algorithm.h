#include "common.h"
#include "BITMAP.h"

#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

typedef struct Gene{
	int x,y,degree;
	Gene& operator=(Gene &r);
}Gene;

typedef struct Genetic_Algorithm{
	Gene gene[GENE_NUM][LINE_NUM];
	int fit[GENE_NUM];
	int idx[GENE_NUM];
	BITMAP source;

	void start(const char * filename);

	void Initialize(const char * filename);
	void Selection();
	int Fitness(int Gene_idx,BITMAP &calc);
	void Crossover();
	void Mutation();
 
}Genetic_Algorithm;

#endif