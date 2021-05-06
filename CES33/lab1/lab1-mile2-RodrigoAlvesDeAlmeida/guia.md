/* Descrição dos executáveis:
    ms -> minishell
    prog1 -> scaneia o stdin e printa para o stdout e stderr quando é atingido o EOF (CTRL+D para o teclado)
    prog2 -> printa o pid do processo e os argumentos passados na chamada
*/

/* Pra compilar e rodar o mini-shell: */
make
cd build
./ms


/* ----- Testes ----- */

/* Comandos simples */
/bin/ls
./prog1

/* Comandos com argumentos */
./prog2 arg1 arg2 arg3 "composed arg"
/bin/echo "ola mundo"

/* Redirecionamento de output e erro */
./prog1 > out.txt 2> err.txt
/bin/cat ../guia.md > guia-copia.txt

/* Redirecionamento de input */
/bin/echo entrada > in.txt
./prog1 < in.txt > out.txt 2> err.txt

/* Pipelines */
/bin/cat ../guia.md | /bin/grep Pipe
/bin/cat ../guia.md | /bin/grep Redirecionamento | ./prog1 > out.txt 2> err.txt
./prog1 < in.txt 2> err.txt | ./prog1 2> err.txt | ./prog1 2> err.txt

/* Resetar diretório */
/bin/ls | /bin/grep .txt | /usr/bin/xargs -I{} /bin/rm {}
