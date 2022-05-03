#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

enum tag
{
	USERNAME,
	INVALIDUSERNAME,
	ACTIVEUSERS,
	CONNECTED,
	MESSAGECHAT,
	UPDATECHAT,
	FINISHED,
	CLOSE
};

struct messageList
{
	int size;
	char content[150][100];
};

struct userList
{
	int size;
	char name[150][100];
};

char *getHourMinutesSeconds()
{
	char *result = (char *)calloc(20, sizeof(char));
	struct tm *local;
	time_t t;

	t = time(NULL);
	local = localtime(&t);

	int hour = local->tm_hour;
	int min = local->tm_min;
	int sec = local->tm_sec;

	sprintf(result, "%d:%d:%d", hour, min, sec);

	return result;
	free(result);
}

char *getHourMinutes()
{
	char *result = (char *)calloc(20, sizeof(char));
	struct tm *local;
	time_t t;

	t = time(NULL);
	local = localtime(&t);

	int hour = local->tm_hour;
	int min = local->tm_min;

	sprintf(result, "%d:%d", hour, min);

	return result;
	free(result);
}

void printHeader(char *indentifier)
{
	int i;
	int size = strlen(indentifier);
	int padding = (78 - size) / 2;
	system("clear");
	printf("--------------------------------------------------------------------------------\n");

	for (i = 0; i < padding; i++)
	{
		printf("#");
	}

	printf(" %s ", indentifier);

	for (i = 0; i < padding; i++)
	{
		printf("#");
	}

	printf("                                                         Último acesso: %s\n", getHourMinutesSeconds());
	printf("--------------------------------------------------------------------------------\n");
}

void printMenuOptions()
{
	printf("\n");
	printf("1 | Conectar-se\n");
	printf("2 | Funcionalidades\n");
	printf("3 | Sair\n");
	printf("\n");
	printf("Escolha sua opção: ");
}

void printSecondMenuOptions()
{
	printf("\n");
	printf("1 | Usuários ativos\n");
	printf("2 | Chat\n");
	printf("3 | Sair\n");
	printf("\n");
	printf("Escolha sua opção: ");
}

void printUserList(struct userList _users)
{
	int i = 0;
	printHeader("Usuários ativos");
	printf("Usuário número                  Usuário                  Atualizado às: %s\n", getHourMinutesSeconds());

	for (i = 0; i < _users.size; i++)
	{
		printf("      %d                          %s\n", i, _users.name[i]);
	}

	printf("\n");
	printf("------------------------------- FIM DA LISTA -----------------------------------\n");
}

void printMessageList(struct messageList _messages)
{
	int i = 0;

	for (i = 0; i < _messages.size; i++)
	{
		printf("> %s\n", _messages.content[i]);
	}
}

void printAlert(char *message)
{
	int i;
	int size = strlen(message);
	int padding = (76 - size) / 2;

	printf("\n");
	printf("\n");

	for (i = 0; i < padding; i++)
	{
		printf(" ");
	}

	printf(">> %s <<", message);

	for (i = 0; i < padding; i++)
	{
		printf(" ");
	}
	printf("\n");
}

void printHowWorks()
{
	printHeader("Funcionalidades");
	printf("\n");
	system("cowsay O Chat é simples e intuitivo!.Siga os passos:");
	printf("\n");
	printf("* Escolha a opçao conectar-se.\n");
	printf("* Insira um nome valido.\n");
	printf("* A opção usuários online mostra todos os usuários em tempo real.\n");
	printf("* Na sala de conversa algumas opções estão disponíveis:\n");
	printf("         *Insira a tecla '>' para digitar uma mensagem.\n");
	printf("         *Insira a tecla 'v' para voltar ao menu anterior.\n");
	printf("         *Insira a tecla 'l' para limpar a tela.\n");
	printf("         *Menssagens com '@nome ' sao direcionadas apenas para o usuario 'nome'.\n");
	printf("         		*Exemplo: '@joao Bom dia'\n");
}

void printAnimation()
{
	system("clear");
	printf("\n\n");
	system("cowsay OLA!, bem vindo ao chat virtual.");
	usleep(100000);
	sleep(2);
	printf("\n");
	system("cowsay Eu sou a vaca.");
	sleep(2);
	system("cowsay -f tux É eu sou o pinguim.");
	sleep(2);
	printf("\n");
	system("cowsay E nos iremos te auxiliar nesse chat. Vamos lá ?");
	sleep(2);
}

int kbhit(void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if (ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
}

void waitKey()
{
	char bufferKey;
	printAlert("Pressione 'v' para voltar");
	do
	{
		while (!kbhit())
		{
			fflush(stdin);
			usleep(100000);
		}
		bufferKey = getchar();
	} while (bufferKey != 'v');
}

void errorSocket()
{
	perror("Erro ao criar socket");
	exit(1);
}

void errorConnection()
{
	perror("Erro durante a conexão");
	exit(1);
}

void errorReceiver()
{
	perror("Erro ao se conectar ao servidor, tente mais tarde");
	exit(1);
}

void errorBind()
{
	perror("Erro ao associar porta");
	exit(1);
}

void errorListen()
{
	perror("Erro ao iniciar a escuta");
	exit(1);
}

void errorOpenFile()
{
	perror("Erro ao abrir arquivo");
	exit(1);
}

char *generateMessage(char *value, int type, int isServer)
{
	char *result, tag[15], content[300];

	result = (char *)calloc((strlen(value) + 20), sizeof(char));

	switch (type)
	{
	case USERNAME:
	{
		strcpy(tag, "userName=");
		strcpy(content, value);
		strcat(result, tag);
		strcat(result, content);
		break;
	}
	case INVALIDUSERNAME:
	{
		strcpy(tag, "invalidUserName=");
		strcpy(content, value);
		strcat(result, tag);
		strcat(result, content);
		break;
	}
	case ACTIVEUSERS:
	{
		strcpy(tag, "activeUsers=");
		strcpy(content, value);
		strcat(result, tag);
		strcat(result, content);
		break;
	}
	case CONNECTED:
	{
		strcpy(tag, "connected=");
		strcpy(content, value);
		strcat(result, tag);
		strcat(result, content);
		break;
	}
	case MESSAGECHAT:
	{
		strcpy(tag, "messageChat=");
		strcpy(content, value);
		strcat(result, tag);
		strcat(result, content);
		break;
	}
	case UPDATECHAT:
	{
		strcpy(tag, "updateChat=");
		strcpy(content, value);
		strcat(result, tag);
		strcat(result, content);
		break;
	}
	case FINISHED:
	{
		strcpy(tag, "finished=");
		strcpy(content, value);
		strcat(result, tag);
		strcat(result, content);
		break;
	}
	case CLOSE:
	{
		strcpy(tag, "close=");
		strcpy(content, value);
		strcat(result, tag);
		strcat(result, content);
		break;
	}

	default:
	{
		break;
	}
	}

	if (isServer)
	{
		printf("Mensagem gerada: %s\n", result);
		fflush(stdout);
	}

	return result;
	free(result);
}

int checkKindMessage(char *bufferRcv, int isServer)
{

	if (isServer)
	{
		printf("Mensagem recebida: %s\n", bufferRcv);
		fflush(stdout);
	}

	char *tag;
	tag = strtok(bufferRcv, "=");

	if (strcmp(tag, "userName") == 0)
	{
		return USERNAME;
	}
	if (strcmp(tag, "invalidUserName") == 0)
	{
		return INVALIDUSERNAME;
	}
	if (strcmp(tag, "activeUsers") == 0)
	{
		return ACTIVEUSERS;
	}
	if (strcmp(tag, "connected") == 0)
	{
		return CONNECTED;
	}
	if (strcmp(tag, "messageChat") == 0)
	{
		return MESSAGECHAT;
	}
	if (strcmp(tag, "updateChat") == 0)
	{
		return UPDATECHAT;
	}
	if (strcmp(tag, "finished") == 0)
	{
		return FINISHED;
	}
	if (strcmp(tag, "close") == 0)
	{
		return CLOSE;
	}

	return 0;
}

char *checkMessage(char *bufferRcv)
{
	char *token;

	token = strtok(bufferRcv, "=");
	token = strtok(NULL, "=");

	return token;
}
#define MAXDATASIZE 500
#define MAXALLOC 500

int main(int argc, char *argv[])
{
	char userName[50];
	char messageChat[250];
	char bufferKey, *bufferSend, *bufferRcv, *_bufferRcv;
	int flow, opt, opt2, numBytes, isChatting;

	struct userList _users;
	struct messageList _messages;

	int localSocket, conection;
	struct sockaddr_in remoto;

	// Alocação de memória
	bufferRcv = (char *)calloc(MAXALLOC, sizeof(char));
	_bufferRcv = (char *)calloc(MAXALLOC, sizeof(char));
	bufferSend = (char *)calloc(MAXALLOC, sizeof(char));

	localSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (localSocket == -1)
		errorSocket();

	// Configuração do socket para a conexão
	// Mais detalhes consulte as manpages
	remoto.sin_family = AF_INET;
	remoto.sin_port = htons(atoi(argv[2]));
	remoto.sin_addr.s_addr = inet_addr(argv[1]);
	bzero(&(remoto.sin_zero), 8); // Trocar por memser

	// Função de conexão ao host remoto
	conection = connect(localSocket, (struct sockaddr *)&remoto, sizeof(remoto));
	if (conection == -1)
		errorConnection();

	printAnimation();

	do
	{
		printHeader("Chat");
		printMenuOptions();
		scanf("%d", &opt);

		// Verificação do primeiro menu
		switch (opt)
		{
		case 1:
		{
			flow = 0;
			while (!flow)
			{
				printHeader("Fase inicial");
				printf("Escolha seu apelido : ");
				scanf(" %s", userName);

				bufferSend = generateMessage(userName, USERNAME, 0);
				send(localSocket, bufferSend, MAXDATASIZE, 0);

				numBytes = recv(localSocket, bufferRcv, MAXDATASIZE, 0);
				bufferRcv[numBytes] = '\0';
				strcpy(_bufferRcv, bufferRcv);

				if (checkKindMessage(_bufferRcv, 0) == CONNECTED)
				{
					printAlert("Conexão estabelecida");
					sleep(1);
					flow = 1;
				}
				else if (checkKindMessage(_bufferRcv, 0) == INVALIDUSERNAME)
				{
					printAlert("Nome inválido");
					sleep(1);
					flow = 0;
				}
			}

			do
			{
				printHeader("Chat");
				printSecondMenuOptions();
				scanf("%d", &opt2);

				// Verificação do segundo menu
				switch (opt2)
				{
				// Solicita ao servidor a lista de usuários onlines
				case 1:
				{
					bufferSend = generateMessage("", ACTIVEUSERS, 0);
					send(localSocket, bufferSend, MAXDATASIZE, 0);

					int i = 0;
					_users.size = i;

					numBytes = recv(localSocket, bufferRcv, MAXDATASIZE, 0);
					bufferRcv[numBytes] = '\0';
					strcpy(_bufferRcv, bufferRcv);

					while (checkKindMessage(_bufferRcv, 0) != FINISHED)
					{
						_users.size = i + 1;
						strcpy(_users.name[i], checkMessage(bufferRcv));
						i++;

						numBytes = recv(localSocket, bufferRcv, MAXDATASIZE, 0);
						bufferRcv[numBytes] = '\0';
						strcpy(_bufferRcv, bufferRcv);

						if (checkKindMessage(_bufferRcv, 0) == FINISHED)
						{
							printUserList(_users);
							waitKey();
						}
						strcpy(_bufferRcv, bufferRcv);
					}

					break;
					// Entra na sala de conversa
				}
				case 2:
				{
					printHeader("Sala de conversa");

					isChatting = 1;
					while (isChatting)
					{
						fflush(stdin);
						while (!kbhit())
						{

							bufferSend = generateMessage("", UPDATECHAT, 0);
							send(localSocket, bufferSend, MAXDATASIZE, 0);

							int i = 0;
							_messages.size = i;

							numBytes = recv(localSocket, bufferRcv, MAXDATASIZE, 0);
							bufferRcv[numBytes] = '\0';
							strcpy(_bufferRcv, bufferRcv);

							while (checkKindMessage(_bufferRcv, 0) != FINISHED)
							{
								_messages.size = i + 1;
								strcpy(_messages.content[i], checkMessage(bufferRcv));
								i++;

								numBytes = recv(localSocket, bufferRcv, MAXDATASIZE, 0);
								bufferRcv[numBytes] = '\0';
								strcpy(_bufferRcv, bufferRcv);

								if (checkKindMessage(_bufferRcv, 0) == FINISHED)
								{
									printMessageList(_messages);
								}

								strcpy(_bufferRcv, bufferRcv);
							}
							usleep(500000);
						}

						bufferKey = getchar();
						if (bufferKey == '>')
						{
							printf(" [%s] - Voce: ", getHourMinutes());
							scanf(" %[^\n]s", messageChat);
							printf("\n");

							bufferSend = generateMessage(messageChat, MESSAGECHAT, 0);
							send(localSocket, bufferSend, MAXDATASIZE, 0);
						}
						else if (bufferKey == 'v')
						{
							isChatting = 0;
						}
						else if (bufferKey == 'l')
						{
							printHeader("Sala de conversa");
						}
						else
						{
							fflush(stdin);
						}
					}

					break;
					// Solicita o fechamento de conexão ao servidor e finaliza o cliente
				}
				case 3:
				{
					bufferSend = generateMessage("", CLOSE, 0);
					send(localSocket, bufferSend, MAXDATASIZE, 0);
					printAlert("Tchau!");

					free(bufferSend);
					free(bufferRcv);
					free(_bufferRcv);
					return 0;
					break;
				}
				}
			} while (opt2 != 3);
			break;
			// Mostra as instruções de uso ao cliente
		}
		case 2:
		{
			printHowWorks();
			waitKey();
			break;
		}
		case 3:
		{
			bufferSend = generateMessage("", CLOSE, 0);
			send(localSocket, bufferSend, MAXDATASIZE, 0);
			printAlert("Adeus!");

			free(bufferSend);
			free(bufferRcv);
			free(_bufferRcv);
			return 0;
		}
		}
	} while (opt != 3);

	free(bufferSend);
	free(bufferRcv);
	free(_bufferRcv);

	return 0;
}
