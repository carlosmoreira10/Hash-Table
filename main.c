#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct aluno{
	int matricula;
	char nome[30];
    float n1,n2,n3;
};
typedef struct hash Hash;

struct hash{
	int qtd, TABLE_SIZE;
    struct aluno **itens;
};

Hash* criaHash(int TABLE_SIZE){
	Hash* ha = (Hash*) malloc(sizeof(Hash));
    if(ha != NULL){
        int i;
        ha->TABLE_SIZE = TABLE_SIZE;
        ha->itens = (struct aluno**) malloc(TABLE_SIZE * sizeof(struct aluno*));
        if(ha->itens == NULL){
            free(ha);
            return NULL;
        }
        ha->qtd = 0;
        for(i=0; i < ha->TABLE_SIZE; i++)
            ha->itens[i] = NULL;
    }
    return ha;
}

void liberaHash(Hash* ha){
    if(ha != NULL){
        int i;
        for(i=0; i < ha->TABLE_SIZE; i++){
            if(ha->itens[i] != NULL)
                free(ha->itens[i]);
        }
        free(ha->itens);
        free(ha);
    }
}

int valorString(char *str){
    int i, valor = 7;
    int tam = strlen(str);
    for(i=0; i < tam; i++)
        valor = 31 * valor + (int) str[i];
	return (valor & 0x7FFFFFFF);
}

int chaveDivisao(int chave, int TABLE_SIZE){
  return (chave & 0x7FFFFFFF) % TABLE_SIZE;
}

int chaveDobra(int chave, int TABLE_SIZE){
    int num_bits = 10;
    int parte1 = chave >> num_bits;
    int parte2 = chave & (TABLE_SIZE-1);
    return (parte1 ^ parte2);
}

int chaveMultiplicacao(int chave, int TABLE_SIZE){
    double A = 0.6180339887; // const: 0 < A < 1
    double val = chave * A;
    val = val - (int) val;
    return (int) (TABLE_SIZE * val);
}

// Insere e busca sem tratamento de colisão

int insereHash_SemColisao(Hash* ha, struct aluno al){
    if(ha == NULL || ha->qtd == ha->TABLE_SIZE){
    	return 0;
    } 

    int chave = al.matricula;

    int pos = chaveDivisao(chave,ha->TABLE_SIZE);
    printf("pos: %d ", pos);
    struct aluno* novo;
    novo = (struct aluno*) malloc(sizeof(struct aluno));
    if(novo == NULL){
    	return 0;	
	}
    *novo = al;
    ha->itens[pos] = novo;
    ha->qtd++;
    return 1;
}

int buscaHash_SemColisao(Hash* ha, int mat, struct aluno* al){
    if(ha == NULL){
    	return 0;	
	}
    int pos = chaveDivisao(mat,ha->TABLE_SIZE);
    if(ha->itens[pos] == NULL){
		return 0;
	}
    *al = *(ha->itens[pos]);
    return 1;
}

int sondagemLinear(int pos, int i, int TABLE_SIZE){
    return ((pos + i) & 0x7FFFFFFF) % TABLE_SIZE;
}

int sondagemQuadratica(int pos, int i, int TABLE_SIZE){
  pos = pos + 2*i + 5*i*i;
  printf("pos: %d  ", (pos & 0x7FFFFFFF) % TABLE_SIZE);
  return(pos & 0x7FFFFFFF) % TABLE_SIZE;
}

int duploHash(int H1, int chave, int i, int TABLE_SIZE){
    int H2 = chaveDivisao(chave,TABLE_SIZE-1) + 1;
    return ((H1 + i*H2) & 0x7FFFFFFF) % TABLE_SIZE;
}

// Insere e busca com tratamento de colisão

int insereHash_TratamentoColisao(Hash* ha, struct aluno al){
	if(ha == NULL || ha->qtd == ha->TABLE_SIZE)
		return 0;
		
	int chave = al.matricula;
	
	int i, pos, novaPos;
	pos = chaveDivisao(chave, ha->TABLE_SIZE);
	for(i = 0; i < ha->TABLE_SIZE; i++){
		novaPos = sondagemQuadratica(pos,i,ha->TABLE_SIZE);
    	if(ha->itens[novaPos] == NULL){
      		struct aluno* novo;
      		novo = (struct aluno*) malloc(sizeof(struct aluno));
    		if(novo == NULL){
                return 0;
            }
			*novo = al;
      		ha->itens[novaPos] = novo;
      		ha->qtd++;
      		return 1;
    	}
	}
	return 0;
}

int buscaHash_TratamentoColisao(Hash* ha, int mat, struct aluno* al){
	if(ha == NULL)
		return 0;

	int i, pos, novaPos;
	pos = chaveDivisao(mat, ha->TABLE_SIZE);
	for(i=0; i<ha->TABLE_SIZE; i++){
    	novaPos = sondagemQuadratica(pos, i, ha->TABLE_SIZE);
	    if(ha->itens[novaPos] == NULL){
	    	return 0;
	    }
	    if(ha->itens[novaPos]->matricula == mat){
	    	*al = *(ha->itens[novaPos]);
	    	return 1;
	    }
	}
	return 0;
}

int main(){
    int i, tamanho = 8, mat=19200071;
    Hash *tabela = criaHash(tamanho);

    struct aluno al, a[6] = {{mat,"Pedro",9.5,7.8,8.5},
                         {mat-2,"Lucas",7.5,8.7,6.8},
                         {mat-4,"Joao",9.7,6.7,8.4},
                         {mat+2,"Bruno",5.7,6.1,7.4},
                         {mat-1,"Felipe",9.7,6.7,8.4},
                         {mat+8,"Mateus",9.7,6.7,8.4}};

    for(i=0; i < 6; i++){
		//insereHash_SemColisao(tabela,a[i]);
		insereHash_TratamentoColisao(tabela, a[i]);
    }

    printf("\n\n------- main -------\n\n");
    for(i=0; i < 6; i++){
    	//buscaHash_SemColisao(tabela, a[i].matricula, &al);
    	buscaHash_TratamentoColisao(tabela, a[i].matricula, &al);
    	printf("%s, %d\n",al.nome,al.matricula);
    }

    liberaHash(tabela);
    return 0;
}