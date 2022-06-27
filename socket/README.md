# SOCKET - Instruções de Uso

## 1. Compilação
Para compilar o programa, basta utilizar o comando make: <br>
`$ make`

## 2. Execução

Inicie o servidor passando o endereço e a porta:<br>
`$ ./server 127.0.0.1 7777`<br>
Inicie o cliente passando a quantidade, endereço e porta dos servidores:<br>
`$ ./client 2 127.0.0.1 7777 127.0.0.1 7777`<br>

## 3. Medição do tempo

Para medir o tempo de execução, execute o comando timeit passando os endereços dos servidors na variavel CARGS:<br>
```
$ make timeit CARGS="1 127.0.0.1 7777"
...
# real 0m0,01s
# user 0m0,01s
# sys  0m0,01s
```

