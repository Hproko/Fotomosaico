#ifndef __MOSAICO__
#define __MOSAICO__


struct cores{
	float r;
	float g;
	float b;
};


//guarda tipo imagem
//largura
//altura
//tipo de PPM
//Valor maximo
//Media das cores da pastilha
//matriz de caracteres
struct imagem{
	int width;
	int height;
	char tipo[3];
	int max;
	struct cores media;
	unsigned char **pixel;
};




//Definicao das funcoes

int qntd_pastilhas(char *dir_pastilhas);
//Retorna a quantidade de pastilhas no diretorio 

int entrada_invalida(int flag_i, int flag_o);
//Funcao que testa se todos os argumentos foram passados corretamente

struct imagem carrega_imagem(FILE *arq);
//Retorna uma imagem com todos os campos preenchidos

struct imagem* le_pastilhas(char *dir_pastilhas, int num_pastilhas);
//Carrega todas as pastilhas e retorna um vetor de pastilhas 

void monta_imagem(struct imagem *img, FILE *arq);
//Descarrega no arquivo de saida todas as informacoes do novo mosaico

void substitui_blocos(struct imagem* img, struct imagem *vetor_imgs, int num_pastilhas);
//Encontra a media de cada bloco da imagem principal e substitui por uma pastilha

void desaloca_memoria(struct imagem *vetor, struct imagem *img_principal, int num_pastilhas);
//Libera todos os espaços de memoria utilizados




#endif
