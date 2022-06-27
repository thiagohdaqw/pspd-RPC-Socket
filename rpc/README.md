# RPC - Instruções de Uso

## 1. Compilação
Para compilar o programa, basta utilizar o comando make: <br>
`$ make`

## 2. Execução

Inicie o servico de nomes do RPC passando o endereço da rede:<br>
`$ rpcbind -h 127.0.0.1` <br>
Inicie o servidor:<br>
`$ ./minmax_server`<br>
Inicie o cliente passando os endereços dos servidores:<br>
`$ ./minmax_client 127.0.0.1 127.0.0.1`<br>

## 3. Medição do tempo

Para medir o tempo de execução, execute o comando timeit passando os endereços dos servidors pela variavel ARGS:<br>
```
$ make timeit ARGS="127.0.0.1"
...
# real 0m0,01s
# user 0m0,01s
# sys  0m0,01s
```

