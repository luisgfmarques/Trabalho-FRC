#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

#define MAXDATASIZE 500
#define MAXALLOC 500
#define BACKLOG 5

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

struct userList checkActiveUsers()
{
	FILE *users;
	char *token;
	char line[300];
	memset(line, '\0', 299);

	// Lista de usuarios ativos
	int i = 0;
	struct userList _users;

	_users.size = i;
	users = fopen("file/users.txt", "r");

	while (feof(users) == 0)
	{
		fscanf(users, "%s", line);
		token = strtok(line, "@");

		if (token != NULL)
		{
			strcpy(_users.name[i], token);
			_users.size = i;
			i++;
		}
	}

	fclose(users);
	return _users;
}

struct messageList checkLog(char *userName)
{
	FILE *logUser;
	char line[300];
	char _line[300];
	char fileName[50];
	memset(line, '\0', 299);
	memset(_line, '\0', 299);

	memset(fileName, '\0', 300);

	struct messageList messages;

	int i = 0;
	messages.size = i;

	strcpy(fileName, "file/");
	strcat(fileName, userName);
	strcat(fileName, ".txt");

	logUser = fopen(fileName, "r");

	if (!logUser)
	{
		return messages;
	}

	while (feof(logUser) == 0)
	{
		fgets(line, 300, logUser);

		strcpy(messages.content[i], line);
		messages.size = i;
		i++;

		strcpy(_line, line);
	}

	fclose(logUser);
	remove(fileName);
	return messages;
}

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

void errorSocket()
{
	perror("Erro ao criar socket");
	exit(1);
}

void errorConnection()
{
	perror("Erro durante a conex??o");
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
void putMessageChatInLog(char *userName, char *messageChat)
{
	int i;
	char fileName[50];
	char _messageChat[300];
	char finalMessageChat[300];

	memset(fileName, '\0', 49);
	memset(_messageChat, '\0', 299);
	memset(finalMessageChat, '\0', 299);

	FILE *logUser;

	struct userList _users = checkActiveUsers();

	// Se a mensagem e privada
	if (messageChat[0] == '@')
	{
		char *token;
		char userDestination[50];

		strcpy(_messageChat, messageChat);

		token = strtok(_messageChat, " ");
		strcpy(userDestination, token);

		for (i = 1; i < strlen(userDestination); i++)
		{
			userDestination[i - 1] = userDestination[i];

			if (i + 1 == strlen(userDestination))
			{
				userDestination[i] = '\0';
			}
		}

		token = strtok(NULL, "\n");
		strcpy(messageChat, token);

		for (i = 0; i < _users.size; i++)
		{
			if (strcmp(_users.name[i], userName) != 0 && strcmp(_users.name[i], userDestination) == 0)
			{
				memset(fileName, '\0', 49);
				memset(finalMessageChat, '\0', 299);

				strcpy(fileName, "file/");
				strcat(fileName, _users.name[i]);
				strcat(fileName, ".txt");

				logUser = fopen(fileName, "a+");

				strcat(finalMessageChat, "[");
				strcat(finalMessageChat, getHourMinutes());
				strcat(finalMessageChat, "]");
				strcat(finalMessageChat, " - [Privado] - ");
				strcat(finalMessageChat, userName);
				strcat(finalMessageChat, ": ");
				strcat(finalMessageChat, messageChat);
				strcat(finalMessageChat, "\n");

				fputs(finalMessageChat, logUser);
				fclose(logUser);

				fflush(stdout);
			}
		}
	}
	else
	{
		for (i = 0; i < _users.size; i++)
		{

			if (strcmp(_users.name[i], userName) != 0)
			{
				memset(fileName, '\0', 49);
				memset(finalMessageChat, '\0', 299);

				strcpy(fileName, "file/");
				strcat(fileName, _users.name[i]);
				strcat(fileName, ".txt");

				logUser = fopen(fileName, "a+");
				strcat(finalMessageChat, "[");
				strcat(finalMessageChat, getHourMinutes());
				strcat(finalMessageChat, "] - ");

				strcat(finalMessageChat, userName);
				strcat(finalMessageChat, ": ");
				strcat(finalMessageChat, messageChat);
				strcat(finalMessageChat, "\n");

				fputs(finalMessageChat, logUser);
				fclose(logUser);

				fflush(stdout);
			}
		}
	}
}

void deleteFile()
{
	system("rm -rf file");
	printf("Removendo arquivos\n");
}

void makeDirectory()
{
	system("mkdir file");
	printf("Criando diret??rios\n");
}

void deleteUserFiles(char *userName)
{
	char fileName[50];
	memset(fileName, '\0', 49);

	strcpy(fileName, "file/");
	strcat(fileName, userName);
	strcat(fileName, ".txt");

	remove(fileName);
}

int checkUserName(char *userName)
{

	FILE *users;
	char *token;
	char line[300];
	memset(line, '\0', 299);

	users = fopen("file/users.txt", "r");

	if (!users)
	{
		return 0;
	}

	while (feof(users) == 0)
	{
		fscanf(users, "%s", line);
		token = strtok(line, "@");

		if (token != NULL)
		{
			if (strcmp(token, userName) == 0)
			{
				fclose(users);
				return 1;
			}
		}
	}

	fclose(users);
	return 0;
}

void insertUser(char *userName, char *address)
{
	FILE *users;
	char putInFile[300];
	memset(putInFile, '\0', 299);

	// Tenta abrir o arquivo de usu??rios, se nao existir(situa????o onde n??o existem usu??rios ativos) o arquivo ?? criado
	users = fopen("file/users.txt", "a+");

	if (!users)
		errorOpenFile();

	// Insere o nome do usu??rio na lista de usu??rios ativos
	strcat(putInFile, userName);
	strcat(putInFile, "@");
	strcat(putInFile, address);
	strcat(putInFile, "\n");
	fputs(putInFile, users);

	fclose(users);

	printf("Usu??rio conectado: %s", putInFile);
	fflush(stdout);
}

void removeUser(char *userName)
{
	int control = 0;
	FILE *users;
	FILE *_users;
	char line[300];
	char _line[300];
	memset(line, '\0', 299);
	memset(_line, '\0', 299);

	char *token;
	token = (char *)calloc(15, sizeof(char));

	printf("Usu??rio a ser removido: %s\n", userName);
	fflush(stdout);

	users = fopen("file/users.txt", "r+");
	if (!users)
		errorOpenFile();

	_users = fopen("file/usersTemp.txt", "w+");
	if (!_users)
		errorOpenFile();

	while (feof(users) == 0)
	{
		memset(line, '\0', 299);
		memset(_line, '\0', 299);

		fscanf(users, "%s", line);
		strcpy(_line, line);
		token = strtok(_line, "@");

		fflush(stdout);

		if (token != NULL)
		{
			if (strcmp(token, userName) != 0)
			{
				fflush(stdout);

				strcat(line, "\n");
				fputs(line, _users);
				control = 1;
			}
		}
	}

	fclose(users);
	fclose(_users);

	// Verifica se existe pelo menos um usu??rio, se n??o os arquivos de usu??rios ser??o apagados
	if (control == 1)
	{
		remove("file/users.txt");
		rename("file/usersTemp.txt", "file/users.txt");
	}
	else
	{
		remove("file/users.txt");
		remove("file/usersTemp.txt");
	}

	printf("Usu??rio desconectado: %s\n", userName);
	fflush(stdout);

	putMessageChatInLog(userName, "Desconectando!");
}

int main(int argc, char *argv[])
{
	pid_t pid;
	int fluxo, msgKind;
	char userName[50];
	char *bufferRcv, *_bufferRcv, *msgContent;

	struct userList _users;
	struct messageList _messages;

	int localSocket, remoteSocket, structSize;
	struct sockaddr_in local;
	struct sockaddr_in remote;

	// Inicio da execu????o

	system("clear");
	// Apagando todos os dados de se????es passadas
	deleteFile();
	// Criando diret??rios necess??rios
	makeDirectory();
	// Mensagem de inicio
	printf("Servidor iniciado\n");

	// Configura????es do socket local
	// Mais detalhes consulte as manpages
	localSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (localSocket == -1)
		errorSocket();
	local.sin_family = AF_INET;
	local.sin_port = htons(atoi(argv[1]));
	local.sin_addr.s_addr = INADDR_ANY;
	memset(local.sin_zero, 0, 8);

	// Associa????o da porta
	if (bind(localSocket, (struct sockaddr *)&local, sizeof(local)) == -1)
		errorBind();

	// Inicio da escuta
	if (listen(localSocket, BACKLOG) == -1)
		errorListen();

	structSize = sizeof(struct sockaddr_in);

	while (1)
	{
		// Aguarda um processo conectar-se
		remoteSocket = accept(localSocket, (struct sockaddr *)&remote, &structSize);

		// Ap??s a conx??o ser aceita, o processo servidor ?? duplicado
		pid = fork();
		if (pid == 0)
		{

			// Aloca????o de mem??ria necess??ria
			bufferRcv = (char *)calloc(MAXALLOC, sizeof(char));
			_bufferRcv = (char *)calloc(MAXALLOC, sizeof(char));
			msgContent = (char *)calloc(MAXALLOC, sizeof(char));

			fluxo = 0;
			while (!fluxo)
			{
				recv(remoteSocket, bufferRcv, MAXDATASIZE, 0);
				strcpy(_bufferRcv, bufferRcv);

				// Ap??s a conex??o de um cliente, a primeira mensagem que o servidor espera ?? do tipo USERNAME,
				// caso contr??rio o processo servidor ?? finalizado
				if (checkKindMessage(_bufferRcv, 1) == USERNAME)
				{
					// Resgata o conteudo da mensagem
					strcpy(_bufferRcv, bufferRcv);
					msgContent = checkMessage(_bufferRcv);

					// Verifica se o userName ?? v??lido
					if (!checkUserName(msgContent))
					{

						// Se o userName ?? v??lido o servidor guarda esse nome para o processo corrente identificar a qual usu??rio est?? associado
						strcpy(userName, msgContent);

						// Insere o usu??rio na lista de usu??rios ativos passando como par??metros o seu nome e endere??o
						insertUser(msgContent, inet_ntoa(remote.sin_addr));

						// Informa ao cliente que ele foi conectado
						send(remoteSocket, generateMessage("", CONNECTED, 1), MAXDATASIZE, 0);
						fluxo = 1;
					}
					else
					{

						// Sinaliza que o nome de usu??rio ?? inv??lido
						send(remoteSocket, generateMessage("", INVALIDUSERNAME, 1), MAXDATASIZE, 0);
						fluxo = 0;
					}
				}
				else
				{
					close(remoteSocket);
					return 0;
				}
			}

			fluxo = 0;
			while (!fluxo)
			{

				// Recebe uma mensagem e guarda o estado de atividade do cliente
				int isActive = recv(remoteSocket, bufferRcv, MAXDATASIZE, 0);
				strcpy(_bufferRcv, bufferRcv);

				// Verifica o tipo da mensagem
				msgKind = checkKindMessage(_bufferRcv, 1);
				strcpy(_bufferRcv, bufferRcv);
				msgContent = checkMessage(_bufferRcv);

				// Se o estado de conex??o ?? ativo
				if (isActive)
				{
					// Trata as mensagem de acordo com o seu tipo
					switch (msgKind)
					{
					// ACTIVEUSERS: O servidor envia para o cliente a lista de usu??rios ativos
					case ACTIVEUSERS:
					{
						int i = 0;
						_users = checkActiveUsers();
						for (i = 0; i < _users.size; i++)
						{
							send(remoteSocket, generateMessage(_users.name[i], USERNAME, 1), MAXDATASIZE, 0);
						}
						send(remoteSocket, generateMessage("", FINISHED, 1), MAXDATASIZE, 0);
						break;
					}
					// MESSAGECHAT: O servidor armazena a mensagem enviada pelo cliente
					case MESSAGECHAT:
					{
						putMessageChatInLog(userName, msgContent);
						break;
						// UPDATECHAT: O servidor retorna as mensagens direcionadas ao cliente
					}
					case UPDATECHAT:
					{
						int i = 0;
						_messages = checkLog(userName);
						for (i = 0; i < _messages.size; i++)
						{
							send(remoteSocket, generateMessage(_messages.content[i], MESSAGECHAT, 1), MAXDATASIZE, 0);
						}
						send(remoteSocket, generateMessage("", FINISHED, 1), MAXDATASIZE, 0);
						break;
						// CLOSE: O servidor fecha a conex??o
					}
					case CLOSE:
					{
						removeUser(userName);
						deleteUserFiles(userName);
						close(remoteSocket);

						free(bufferRcv);
						free(_bufferRcv);
						free(msgContent);
						return 0;
					}
					}
				}
				else
				{
					removeUser(userName);
					deleteUserFiles(userName);
					close(remoteSocket);
					break;
				}
			}
			free(bufferRcv);
			free(_bufferRcv);
			free(msgContent);
			break;
		}
	}
	return 0;
}
