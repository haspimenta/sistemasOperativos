#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

// Faz print do da String comando que lhe foi passado
void GetCommand(char command[]){
	printf("Para confirmar, o seu commando é: %s \n", command);
}

// Quebra a String passada em tokens e faz o print respectivo de cada token
// Semelhante a funcao makeCmdVector, mas muito mais simplificado, apenas realiza o print dos argumentos de cada comando no ecra, um a um
// Para ver os comentarios mais detalhadamente, verifique a funcao makeCmdVector()
int PrintArgs(char command[]){
	const char delim[2] = " ";	// neste caso, o carater que o strtok() utiliza para quebrar a String command é um espaco, pois os argumentos
					// de um commando sao separados por espacos
	char *token;
	char str[512];

	strcpy(str, command);

	token = strtok(str, delim);

	while(token != NULL) {
	printf("%s\n", token);
	    
	token = strtok(NULL, delim);
	}
	return 0;
}

// Executa o comando passado como um vetor de argumentos
void exeCommand(char *argVector[])
{
	execvp(argVector[0], argVector);
}

// quebra o comando passado em argumentos, guardando-os no vetor para ser utilizado por a função exeCommand()
// versão base da funcao makeCmdVector()
void makeArgVector(char command[], char *argVector[]){
	int i = 0;	
	const char delim[2] = " ";
	char *token;
	char str[256];

	argVector = malloc(sizeof(str));
	strcpy(str, command);

	token = strtok(str, delim);

	while( token != NULL ) {
		argVector[i] = malloc(sizeof(token));
		argVector[i] = token;
		i = i + 1;
		token = strtok(NULL, delim);
	}
	argVector[i+1] = malloc(sizeof(char*));
	argVector[i+1] = NULL;
	for(int j = 0; j <= i; j++){
		printf("argVector[%i] = %s\n", j, argVector[j]);
	}
	exeCommand(argVector);
	return;
}

void makeCmdVector(char command[], char *cmdVector[]){
	int i = 0;	// variavel que irá incrementando à medida que esta funcao le os tokens, esta variavel serve como contagem do numero de posicoes do vetor
	const char delim[2] = ";"; // a funcao separa a String que lhe foi passada, onde encontra um carater igual ao que foi atribuido na declaracao de delim (;)
	char *argVetor[512];	// vetor que sera passado para a funcao makeArgVector() para construir o comando e passar para a funcao que o executa
	char *token;
	char str[256];	// String para onde será a copia do conteudo de command, para este nao ser alterado por a fucao strtok
	pid_t pid, childPid;

	cmdVector = malloc(sizeof(str)); // reserva espaco para o vetor cmdVector equivalente ao tamanho (em bytes) de str
	strcpy(str, command);	// Copia o conteudo de command para String

	token = strtok(str, delim);	// Quebra a String str em tokens e guarda o ponteiro para o primeiro token

	while( token != NULL ) {	// ciclo while que se repete enquanto o conteudo apontado por token nao estiver vazio
		cmdVector[i] = malloc(sizeof(token));	// Reserva dinamicamente a memoria necessaria na posicao i do array cmdVector para guardar depois o token
		cmdVector[i] = token;			// Guarda o token na posicao e vetor mencionados na linha anterior
		i = i + 1;				// Incrementa 1 à variavel i para se e quando o ciclo while repetir nao escrever o novo token por cima do
							// token guardado anteriormente
		token = strtok(NULL, delim);		// guarda o ponteiro do token seguinte para ser utilizado
	}
	cmdVector[i+1] = malloc(sizeof(char*));		// Reserva um ultimo espaço para o vetor de commandos
	cmdVector[i+1] = NULL;				// Por fim adiciona a constante Null na ultima posicao do vetor, para sinalizar o fim da lista de commandos

	for(int j = 0; j <= i; j++){			// Ciclo para fazer o print dos comandos para o ecra, o i marca a posicao do fim no vetor
		printf("cmdVector[%i] = %s\n", j, cmdVector[j]);	// O j marca a posicão do vetor a ser utilizada para realizar o print
	}
	for(int j = 0; j < i; j++){			// Outro ciclo semelhante ao anterior, no estanto este ciclo faz chamada às funcoes PrintArgs() e 
							// makeArgVector(), passando o comando guardado na posicao j do vetor cmdVector
		PrintArgs(cmdVector[j]);
		pid = fork();		// é a shell filha que constroi e executa cada comando
		if(pid ==0)
		{
			makeArgVector(cmdVector[j], argVetor);	// passa cada comando do vetor cmdVector e o vetor de argumentos (vazio) para esta funcao contruir
								// um array contendo os argumentos de cada comando, para depois tal comando ser executado
			printf("\nProcesso pai. PID:|%d|\n",getppid());	// imprime o numero de processo do pai
			printf("\nProcesso filho. PID:|%d|\n",getpid());// imprime o numero de processo da shell filha
		}
		else
		{
			wait (&childPid);		// espera que a shell filha acabe de executar o processo
			printf("\nConluido!\n");	// indica que foi concluido o processo
			printf("\nProcesso pai. PID:|%d|\n",getppid());
			printf("\nProcesso filho. PID:|%d|\n",getpid());
		}
		
	}
	return;
}

int main(){
	// declaracao das variaveis necessárias para a funcionamento do programa
	char comando[256];
	char *ponteiro = comando;
	char *cmdVetor[512];
	while (strcmp(comando,"quit") != 0){	// ciclo que se repete até o utilizador escrever o commando quit.
		printf("Introduza um commando para eu executar: ");	// escreve mensagem para o utilizador a pedir um commando
		fgets(comando, 255, stdin);	// le o commando escrito por utilizador
		ponteiro[strlen(ponteiro)-1] = 0;	//	Altera o conteudo da ultima posicao na string para o carater nulo
		GetCommand(comando);			//	Chamada da funcao para imprimir o comando na tela
		makeCmdVector(comando, cmdVetor);	//	Separa a lista de comandos e guarda cada um na sua respectiva posicao no vetor de comandos
							//	cada montagem de comando será executada dentro da funcao makeCmdVector
	}
	return 0;
}
