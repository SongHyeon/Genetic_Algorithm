#include "..\header\Genetic_Algorithm.h"

Gene& Gene::operator=(Gene &r){
	x = r.x;
	y = r.y;
	degree = r.degree;
	return *this;
}

void Genetic_Algorithm::start(const char * filename){
	int generation = 1,wirte_cnt=0;
	char output[] = "output\\generation_000000.dib";
	void (*rename)(char *,int ) = [](char * output,int generation){
		int idx = 23;
		while(generation){
			output[idx--] = (generation%10) +'0';
			generation/=10;
		}
		return ;
	};

	Initialize(filename);
	source.Write_BMP("output\\Edge_Detection.dib");

	FILE *ptr = fopen("output\\out.txt","w");

		//Loop
	do{
		Selection();
		if((generation%WRITE_FREQUENCY)==0){
			BITMAP calc(source);
			calc.clear();
			Fitness(idx[0],calc);
			rename(output,generation);
			calc.Write_BMP(output);
			fprintf(ptr,"Generation [%6d] Fitness %12d\n",generation,fit[idx[0]]);
		}
		printf("Generation [%6d] Fitness %12d\n",generation,fit[idx[0]]);
		Crossover();
		Mutation();
	}while(generation++);

	fclose(ptr);
}

	
void Genetic_Algorithm::Initialize(const char * filename){
	source.Read_BMP(filename);
	source.Edge_Detection();

	int i,j;
	int W,H;

	W = source.FileHeader->biWidth;
	H = source.FileHeader->biHeight;

	srand(time(0));

	for(i=0;i<GENE_NUM;i++){
		idx[i] = i;
		for(j=0;j<LINE_NUM;j++){
			gene[i][j].x = rand()%W;
			gene[i][j].y = rand()%H;
			gene[i][j].degree = rand()%360;
		}
	}
}
	
void Genetic_Algorithm::Selection(){
	int i;
	BITMAP calc(source);
	i = GENE_NUM;
	while(--i >= 0){
		calc.clear();
		fit[i]=Fitness(i,calc);
	}
	std::sort(idx,idx+GENE_NUM,[&](int &l,int &r){return fit[l]<fit[r];});
}
	
int Genetic_Algorithm::Fitness(int Gene_idx,BITMAP &calc){
	assert(Gene_idx<GENE_NUM);
	int i,j;
	int W,H;
	int fitness=0;
	Gene *gene_ptr = gene[Gene_idx];
	BYTE **src_ptr = source._8bit_img;
	BYTE **tar_ptr = calc._8bit_img;

	for(i=0;i<LINE_NUM;i++){
		calc.line(gene_ptr[i].x,gene_ptr[i].y,gene_ptr[i].degree);
	}

	W = source.FileHeader->biWidth;
	H = source.FileHeader->biHeight;

	for(i=0;i<H;i++){
		for(j=0;j<W;j++){
			fitness += abs(src_ptr[i][j]-tar_ptr[i][j]);
		}
	}
	return fitness;
}
	
void Genetic_Algorithm::Crossover(){
	int i,j,a,b,cutting;
	for(i=GREAT_GENE;i<GENE_NUM;i++){
		b = a = rand()%GREAT_GENE;
		while(b==a){
			b = rand()%GREAT_GENE;
		}
		cutting = rand()%LINE_NUM;
		for(j = 0;j<cutting;j++){
			gene[idx[i]][j]=gene[idx[a]][j];
			assert(gene[idx[i]][j].x==gene[idx[a]][j].x);
		}
		for(j=cutting;j<LINE_NUM;j++){
			gene[idx[i]][j]=gene[idx[b]][j];
		}
	}
}
	
void Genetic_Algorithm::Mutation(){
	int i,j;
	int W,H;
	W = source.FileHeader->biWidth;
	H = source.FileHeader->biHeight;

	for(i=0;i<GENE_NUM;i++){
		for(j=0;j<LINE_NUM;j++){
			if(rand()%(int)(100/MUTATION_PERSENTAGE)==0){
				gene[i][j].x = rand()%W;
				gene[i][j].y = rand()%H;
				gene[i][j].degree = rand()%360;
			}
		}
	}
}