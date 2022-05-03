# Chat

Um chat de comunicação feito em C com salas de bate papo baseado no tipo de comunicação Cliente - Servidor
O sistema deve ser executado em ambiente linux, preferencialmente no ubuntu para correto funcionamento.
Comandos para Rodar o Chat:

Buildando o projeto:

```
	bash make.sh
```

O arquivo bash realiza a compilação do projeto, juntamente com a instalação do cowsay que servira como um pequeno assistente virtual.
Para iniciar o server -> abra um terminal e execute o comando:

```
	./server PPPP
```
Onde PPPP se refere a porta que será utilizada pelo servidor.
Para iniciar os clientes -> para cada cliente do chat, abra um terminal diferente e execute o comando:

```
	./client XXX.XXX.XXX.XXX PPPP
```
O conjunto de X se refere ao ip do Servidor que sera conectado, e PPPP a porta que o servidor está rodando.
