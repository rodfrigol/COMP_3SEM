/* Descrição dos executáveis:
    ms -> minishell
    prog1 -> scaneia o stdin e printa para o stdout e stderr quando é atingido o EOF (CTRL+D para o teclado)
    prog2 -> printa o pid do processo e os argumentos passados na chamada
    prog3 -> conta até 10
*/

/* Pra compilar e rodar o mini-shell: */
make
cd build
./ms


/* ----- TESTES LAB 1 ----- */

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

/* ----- TESTES LAB 2 ----- */

/* Interrupções */
./prog3 ( CTRL+C )
/bin/grep teste ( CTRL+C )
./prog1 ( CTRL+\ )

/* Pausar sinais */
./prog3 ( CTRL+Z )
/bin/ps
jobs
fg 1
jobs

/* Enviar mais de um sinal para o background */
./prog1 ( CTRL+Z )
jobs
./prog3 ( CTRL+Z )
jobs
/bin/grep teste ( CTRL+Z )
fg 2 ( CTRL+Z )
jobs
fg 2 //wait end
jobs
fg 3 ( CTRL+C )
jobs
fg 1 ( CTRL+\ )
jobs

/* Background com pipes */
./prog3 | /bin/grep 5 ( CTRL+Z )
jobs
fg 1 //wait 10 secs
