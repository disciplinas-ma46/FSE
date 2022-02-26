#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         //Used for UART
#include <fcntl.h>          //Used for UART
#include <termios.h>        //Used for UART

char matricula[] = {5, 4, 2, 1};
int uart0_filestream;


void setup_opcoes(struct termios *options) {
    tcgetattr(uart0_filestream, options);
    options->c_cflag = B9600 | CS8 | CLOCAL | CREAD;     //<Set baud rate
    options->c_iflag = IGNPAR;
    options->c_oflag = 0;
    options->c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, options);
}

int enviar_string(char *str) {
    // // cod 0xB3
    // // Comando + Tamanho da String (1 byte) + String + Matrícula
    int tam_str = strlen(str);
    
    char mensagem[200]; // pode fazer alocação dinâmica também
    mensagem[0] = 0xB3;
    mensagem[1] = (char) strlen(str);
    mensagem[2] = '\0';
    strcpy(&mensagem[2], str);
    strcpy(&mensagem[2 + tam_str], matricula);
    mensagem[2 + tam_str + 4] = '\0';

    // char mensagem[] = {0xB3, 3, 'o', 'l', 'a', 5, 4, 2, 1};

    printf("Escrevendo caracteres na UART ...");
    int count = write(uart0_filestream, mensagem, strlen(mensagem)); // strlen(mensagem)
    if (count < 0)
    {
        printf("Erro ao enviar string [%s] (UART TX error)\n", str);
        //close(uart0_filestream);
        return 0;
    }
    else
    {
        printf("escrito [%s]\n", mensagem);
        return 1;
    }

}

int enviar_int(unsigned int n) {
    // Comando + Dado inteiro + Matrícula
    
    unsigned char mensagem[9];
    // TODO: pode ser melhor usar memcpy()
    mensagem[0] = 0xB1;
    mensagem[1] = (n >> 24) & 0xFF;
    mensagem[2] = (n >> 16) & 0xFF;
    mensagem[3] = (n >> 8) & 0xFF;
    mensagem[4] = n & 0xFF;

    memcpy(&mensagem[5], matricula, 4);

    // char mensagem[] = {0xB1, 0, 0, 0, 0xFF, 5, 4, 2, 1};
    // for(int i=0; i<9; i++) {
    //     printf("%X ", mensagem[i]);
    // }

    printf("\n\nEscrevendo inteiro na UART ...\n");
    int count = write(uart0_filestream, mensagem, 9); // strlen(mensagem)
    if (count < 0)
    {
        printf("Erro ao enviar int [%i] (UART TX error)\n", n);
        //close(uart0_filestream);
        return 0;
    }
    else
    {
        printf("escrito [%i] em %i bytes\n", n, count);
        return 1;
    }

}


int main(int argc, const char * argv[]) {

    int uart0_filestream = -1;
    //Open in non blocking read/write mode
    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);      
    
    if (uart0_filestream == -1) {
        printf("Erro - Não foi possivel iniciar a UART.\n");
        return 0;
    }
    else {
        printf("UART inicializada!\n");
    }

    

    struct termios options;
    setup_opcoes(&options);

    enviar_string("ola mundo");
    // enviar_int(15);


    sleep(1);

    // ----- CHECK FOR ANY RX BYTES -----
    // Read up to 255 characters from the port if they are there
    unsigned char rx_buffer[256];
    int rx_length = read(uart0_filestream, (void*)rx_buffer, 255);      //Filestream, buffer to store in, number of bytes to read (max)
    rx_length += 1-1;
    if (rx_length < 0)
    {
        printf("Erro na leitura.\n"); //An error occured (will occur if there are no bytes)
    }
    else if (rx_length == 0)
    {
        printf("Nenhum dado disponivel.\n"); //No data waiting
    }
    else
    {
        //Bytes received
        rx_buffer[rx_length] = '\0';
        printf("%i Bytes lidos\n", rx_length);//, rx_buffer);
        //String Recebida: outro teste
    }

   close(uart0_filestream);
   return 0;
}
