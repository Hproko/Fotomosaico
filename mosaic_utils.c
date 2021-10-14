#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "mosaic_utils.h"





int qntd_pastilhas(char *dir_pastilhas){
	
	DIR *diretorio;
	struct dirent *proxima_pastilha;


	diretorio = opendir(dir_pastilhas);
	if(! diretorio){
		perror("Nao foi possivel abrir o diretorio");
		exit(1);
	}

	proxima_pastilha = readdir(diretorio);
	if(! proxima_pastilha){
		perror("Nao foi possivel encontrar pastilhas");
		exit(1);
	}
	
	int cont = 0;
	
	while(proxima_pastilha != NULL){
		if(proxima_pastilha->d_type == DT_REG){
			cont++;
		}
		proxima_pastilha = readdir(diretorio);
	}	
	
	(void)closedir(diretorio);
	
	return cont;
}


//cria uma matriz e a preenche com caracteres ou numeros
unsigned char** carrega_pixels(FILE *arq, int altura, int largura,char tipo[2]){
	
	int i,k;
	
	unsigned char** matriz;
	
	//alocacao dinamica de matriz
	matriz = malloc(altura * sizeof(unsigned char*));
	if(! matriz){
		printf("Erro ao alocar memoria\n");
		exit(1);
	}
	
	for(i=0; i<altura; i++){
		matriz[i] = malloc(largura * 3 * sizeof(unsigned char));
		if(! matriz[i]){
			printf("Erro ao alocar memoria\n");
			exit(1);
		}
	}
	
	if(strcmp(tipo,"P6") == 0){
		//preenche matriz com caracteres
		for(i=0;i<altura;i++)
			fread(matriz[i], sizeof(unsigned char), 3 * largura, arq);
	}
	
	else{
		
		//preenche matriz com numeros
		for(i=0;i<altura;i++)
			for(k=0;k<largura*3;k++)
				fscanf(arq,"%hhd",&matriz[i][k]);
	}

	return matriz;
}


struct cores calcula_media(unsigned char **matriz, int x_ini, int y_ini,  int altura, int largura){
//Funcao que recebe uma matriz rgb e o tamanho do bloco que se deseja calcular a media	
	struct cores rgb;	
	
	int i, k;

	int tot_red = 0, tot_green = 0, tot_blue = 0;
	
	int num_pixels = altura * largura;
		
	for(i= x_ini; i < x_ini + altura; i++){
		for(k= y_ini; k < y_ini + (largura*3)-2;k+=3){
			
			//soma de cada vermelho, verde e azul	
			tot_red += (int)matriz[i][k] * (int)matriz[i][k];
			tot_green += (int)matriz[i][k+1] * (int)matriz[i][k+1];
			tot_blue += (int)matriz[i][k+2] * (int)matriz[i][k+2];
			
		}
	}

	//resulta na raiz quadrada da media	
	rgb.r = sqrt(tot_red/num_pixels);
	rgb.g = sqrt(tot_green/num_pixels);
	rgb.b = sqrt(tot_blue/num_pixels);

	return rgb;
}



//Cria uma estrutura imagem, preenche e retorna a estrutura preenchida
struct imagem carrega_imagem(FILE *arq){
	
	struct imagem pastilha;

	char line[256];
	
	long int pos;
	
	fgets(pastilha.tipo, 3, arq);
	if(strcmp(pastilha.tipo,"P6") && strcmp(pastilha.tipo,"P3")){
		fprintf(stderr, "Pastilha ou imagem com formato nao suportado\n");
		exit(1);
	}
	
	getc(arq); //come um '\n'
	
	//Ignorando comentarios no ppm
	do{
		pos = ftell(arq);
		fgets(line, sizeof(line), arq);
		if(line[0] != '#')
			fseek(arq, pos, SEEK_SET);
	}while(line[0] == '#');
	
	
	//captura largura, altura e Valor maximo
	fscanf(arq, "%d", &pastilha.width);
	fscanf(arq, "%d", &pastilha.height);
	fscanf(arq, "%d", &pastilha.max);
	
	getc(arq);//come outro \n
	
	
	//pixel aponta para nova matriz de pixels
	pastilha.pixel = carrega_pixels(arq, pastilha.height, pastilha.width,pastilha.tipo);
	
	// media recebe a media do rgb da pastilha
	pastilha.media = calcula_media(pastilha.pixel, 0, 0, pastilha.height, pastilha.width);
	
	
	
	return pastilha;
}


//Funcao que cria e preenche um vetor de imagens com as pastilhas
struct imagem* le_pastilhas(char *dir_pastilhas, int num_pastilhas){
		
	
	//alocacao dinamica de um vetor de imagens
	struct imagem* vetor_imgs;
	vetor_imgs = malloc(num_pastilhas * sizeof(struct imagem));
	if(! vetor_imgs){
		printf("Erro ao alocar memoria\n");
		exit(1);
	}


	//abertura do diretorio
	DIR *diretorio;		
	diretorio = opendir(dir_pastilhas);
	if(! diretorio){
	perror("Nao foi possivel abrir o diretorio");
	exit(1);
	}


	//captura do arquivo no diretorio
	struct dirent *proxima_pastilha;
	proxima_pastilha = readdir(diretorio);
	if(! proxima_pastilha){
		perror("Nao foi possivel encontrar pastilhas");
		exit(1);
	}
	

	int index = 0;
	
	FILE *pastilha;

	char caminho[100] = "";
	
	
	//enquanto tiver pastilhas no diretorio
	while(proxima_pastilha){
		if(proxima_pastilha->d_type == DT_REG){


			//seta o caminho ate o arquivo a cada iteraçao
			strcat(caminho,dir_pastilhas);
			strcat(caminho,"/");
			strcat(caminho,proxima_pastilha->d_name);

			pastilha = fopen(caminho,"r");
		   	
			//coloca uma pastilha no vetor de imagens
			vetor_imgs[index] = carrega_imagem(pastilha);
			

			fclose(pastilha);
			
			index++;

			//reseta a string com o caminho ate arquivo
			memset(caminho,0,strlen(caminho));
		}
		proxima_pastilha = readdir(diretorio);
	}
	
	(void)closedir(diretorio);
	
	return vetor_imgs;
}


//Faz o calculo da distancia das cores e retorna a pastilha com a menor distancia encontrada
struct imagem* procura_pastilha(struct cores media, int num_pastilhas, struct imagem *vetor_imgs){
	
	int index = 0;

	int delta_r;
	int delta_g;
	int delta_b;

	double distance;
	
 	double menor;

	menor = 1000000;
	
	for(int i=0 ;i < num_pastilhas; i++){
		
		delta_r = media.r - vetor_imgs[i].media.r;
		delta_g = media.g - vetor_imgs[i].media.g;
		delta_b = media.b - vetor_imgs[i].media.b;
	
		delta_r = delta_r * delta_r;
		delta_g = delta_g * delta_g;
		delta_b = delta_b * delta_b;

		distance = sqrt(delta_r + delta_g + delta_b);
		
		if(distance < menor){
			menor = distance;
			index = i;
		}
	}
	
	
	return &vetor_imgs[index];
	
}

//Recebe a pastilha a ser substituida na imagem principal e o tamanho do bloco, pode ser usada para preencher bordas da imagem
void replace_pixels(int block_height, int block_width, int i, int k, unsigned char **matriz, struct imagem* pastilha){

	int m, n;

	//substitui blocos
	for(m=0; m < block_height;m++)
		for(n=0; n < block_width;n+=3){
			matriz[i+m][k+n] = pastilha->pixel[m][n];
			matriz[i+m][k+n+1] = pastilha->pixel[m][n+1];
			matriz[i+m][k+n+2] = pastilha->pixel[m][n+2];
		}
}


void substitui_blocos(struct imagem* img, struct imagem *vetor_imgs, int num_pastilhas){
	
	struct cores media;

	struct imagem *pastilha;
	
	//altura e largura das pastilhas
	int alt_p = vetor_imgs[0].height;
	int larg_p = vetor_imgs[0].width;

	
	//coordenadas maximas ate onde o loop pode chegar
	int max_k = (img->width*3) - ((img->width*3) % (larg_p*3));
	int max_i = img->height - (img->height % alt_p);

	
	//Tamanho da borda de baixo e da lateral direita
	int siz_bottom_edge;
	int siz_right_edge;

	
	int i, k;
	
	//Esse loop eh responsavel por percorrer a imagem e substituir os blocos por pastilhas, porem deixa bordas em baixo e na direita
	for(i=0; i < max_i; i+=alt_p){
		for(k=0; k < max_k; k+=3*larg_p){
			
			media = calcula_media(img->pixel, i, k, alt_p, larg_p);
			
			pastilha = procura_pastilha(media,num_pastilhas,vetor_imgs);
			
			replace_pixels(alt_p, 3*larg_p-2, i, k, img->pixel, pastilha);

		//	printf("%d %d %d\n", media.r, media.g, media.b);
		}
	}

	
	//Caso exista borda em baixo maior que um pixel preenche a borda
	if(img->height % alt_p > 1){
		
		i = max_i ;

		siz_bottom_edge = img->height - i;
		
		for(k =	0; k < max_k; k+=3*larg_p){
			
			media = calcula_media(img->pixel, i, k, siz_bottom_edge, larg_p);
			
			pastilha = procura_pastilha(media,num_pastilhas,vetor_imgs);
			
			replace_pixels(siz_bottom_edge, 3*larg_p-2, i, k, img->pixel, pastilha);
		
		}
	}

	
	//Caso exista borda na direita maior que tres pixels preenche a borda lateral
	if((img->width*3)%(larg_p*3) > 9){
		
		k = max_k;

		siz_right_edge = (img->width*3) - k;
		
		for(i=0;i < max_i; i+=alt_p){
			
			media = calcula_media(img->pixel, i,k,alt_p, siz_right_edge);

			pastilha = procura_pastilha(media,num_pastilhas,vetor_imgs);
			
			replace_pixels(alt_p, siz_right_edge, i, k, img->pixel, pastilha);
		}
	}
}



//Descarrega no arquivo de saida a matriz da imagem principal
void monta_imagem( struct imagem *img, FILE *arq){


	fprintf(arq, "%s\n", img->tipo);
	fprintf(arq, "%d %d\n", img->width, img->height);
	fprintf(arq, "%d\n", img->max);

	
	if(strcmp(img->tipo,"P6")==0){
		for(int i=0;i<img->height;i++)
			fwrite(img->pixel[i], sizeof(char), img->width*3, arq); 
	}
	else{
		for(int i=0;i<img->height;i++){
			for(int k=0;k<img->width*3;k++)
				fprintf(arq, "%d ", img->pixel[i][k]);
			fprintf(arq, "\n");
		}
	}
}


//Funcao que testa se todos os argumentos foram passados corretamente
int entrada_invalida(int flag_i, int flag_o){
	
	if(flag_i == 0 && flag_o == 0)
		if(isatty(STDIN_FILENO) && isatty(STDOUT_FILENO))
			return 1;

	
	if(flag_i == 1 && flag_o == 0 && isatty(STDOUT_FILENO))
		return 1;

	
	if(flag_i == 0 && flag_o == 1 && isatty(STDIN_FILENO))
		return 1;

	
	
	return 0;
}


void desaloca_memoria(struct imagem *vetor, struct imagem* img_principal, int tam_pastilhas){
	
	int i,k;
	
	
	//desaloca vetor de imagens
	for(i=0;i<tam_pastilhas;i++){
		for(k=0;k<vetor[i].height;k++)
			free(vetor[i].pixel[k]);
		free(vetor[i].pixel);
	}
	free(vetor);

	
	//desaloca imagem principal
	for(i=0;i<img_principal->height;i++){
		free(img_principal->pixel[i]);
	}
	free(img_principal->pixel);
}

