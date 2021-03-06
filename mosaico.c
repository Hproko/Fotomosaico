#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include "mosaic_utils.h"


int main (int argc, char **argv){
	
	char *img_entrada = "";
	char *img_saida = "";
	char *dir_pastilhas = "./tiles"; 
	
	int option;

	int flag_i = 0;
	int flag_o = 0;

	while((option = getopt(argc, argv, "i:o:p:h")) != -1)
		switch(option){
			case 'i': 
				img_entrada = optarg;
				flag_i = 1;
				break;
			case 'o':
				img_saida = optarg;
				flag_o = 1;
				break;
			case 'p':
				dir_pastilhas = optarg;
				break;
			case 'h':
				fprintf(stderr, "Esse aplicativo constroi um mosaico usando tiles\n");
				fprintf(stderr, "Use -i <imagem a ser processada> \nUse -o <nome da imagem de saida> \nUse -p <diretorio de pastilhas> \nNo formato ./mosaico -i arquivo -o arquivo -p diretorio\n");
			       	exit(1);
		}



	//Caso o usuario nao execute o programa corretamente
	if(entrada_invalida(flag_i, flag_o)){
		fprintf(stderr, "Use option -h for help\n");
		exit(1);
	}

	

	//seleciona imagem de entrada
	FILE *input_img;
	if(flag_i == 0)
		input_img = stdin;
	else
		input_img = fopen(img_entrada, "r");
	
	if(! input_img){
		fprintf(stderr, "Nao foi possivel abrir a imagem de entrada\n");
		exit(1);
	}

	

	//seleciona onde sera escrita imagem de saida
	FILE *output_img;
	if(flag_o == 0)
		output_img = stdout;
	else
		output_img = fopen(img_saida,"w");
	
	if(! output_img){
		fprintf(stderr, "Nao foi possivel abrir imagem de saida\n");
		exit(1);
	}



	//contagem de pastilhas
	int num_pastilhas;
	num_pastilhas = qntd_pastilhas(dir_pastilhas);
	if(num_pastilhas < 5){
		fprintf(stderr, "Numero de pastilhas insuficiente\n");
		exit(1);
	}

	fprintf(stderr, "Reading tiles from %s\n", dir_pastilhas);
	fprintf(stderr, "Reading %d tiles\n", num_pastilhas);


	//criacao do vetor de pastilhas
	struct imagem* vetor_imgs;
	vetor_imgs = le_pastilhas(dir_pastilhas, num_pastilhas);
	
	
	fprintf(stderr, "Tile size is %dx%d\n",vetor_imgs[0].width, vetor_imgs[0].height);
	

	fprintf(stderr, "Reading input image\n");


	// carrega na memoria a imagem de entrada 
	struct imagem img_principal; 
	img_principal = carrega_imagem(input_img);	
	
	
	fprintf(stderr, "Input image is PPM %s, %dx%d pixels\n", img_principal.tipo, img_principal.width, img_principal.height);
	
	fprintf(stderr, "Building mosaic\n");
	

	//substitui as tiles em cada bloco da imagem principal
	substitui_blocos(&img_principal, vetor_imgs, num_pastilhas);
	
	
	fprintf(stderr, "Writing output file\n");
	

	//Coloca no arquivo de saida o novo mosaico
	monta_imagem(&img_principal, output_img);


	//Fecha os arquivos de entrada e de saida e desaloca memoria da imagem principal e das pastilhas
	fclose(input_img);
	fclose(output_img);
	desaloca_memoria(vetor_imgs, &img_principal, num_pastilhas);
	
	
	return 0;

}
