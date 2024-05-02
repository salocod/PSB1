#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>

#define FILE_PATH "registers.bin"
#define FILE_SIZE 1024  // Same size as used in the first program

// Function to open or create the file and map it into memory
    char* registers_map(const char* file_path, int file_size, int* fd) {
        *fd = open(file_path, O_RDWR | O_CREAT, 0666);
        if (*fd == -1) {
            perror("Error opening or creating file");
            return NULL;
        }

        // Ensure the file is of the correct size
        if (ftruncate(*fd, file_size) == -1) {
            perror("Error setting file size");
            close(*fd);
            return NULL;
        }

        // Map the file into memory
    char *map = mmap(0, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0);
        if (map == MAP_FAILED) {
            perror("Error mapping file");
            close(*fd);
            return NULL;
        }

        return map;
    }

    int registers_release(void* map, int file_size, int fd) {
        if (munmap(map, file_size) == -1) {
            perror("Error unmapping the file");
            close(fd);
            return -1;
        }

        if (close(fd) == -1) {
            perror("Error closing file");
            return -1;
        }

        return 0;
    }

    void set_display_state(unsigned short *r0, int mode) {
        if(mode == 1) {
            *r0 |= (1 << 0);
        } else if(mode == 0){
            *r0 &= ~(1 << 0);
        }
    }   

    void set_display_mode(unsigned short *r0, int mode) {
        if (mode < 0 || mode > 3) {
            printf("Error: Invalid display mode. Mode must be between 0 and 3.\n");
            return;
        }
        *r0 &= ~(3 << 1);
        *r0 |= (mode << 1);
    }


    void set_refresh_rate(unsigned short *r0, int value) {
        if (value < 0 || value > 63) {
            printf("Error: Invalid value. Value must be between 0 and 63.\n");
            return;
        }

        *r0 &= ~(63 << 3);
        *r0 |= (value << 3);
    }
    
    void set_led_operation_state(unsigned short *r0, int state) {
        if(state == 1) {
            *r0 |= (1 << 9);
        } else {
            *r0 &= ~(1 << 9);
        }
    }

    void set_led_color(unsigned short *r0, int mode) {
        if (mode < 0 || mode > 2) {
            printf("Error: Invalid mode. Mode must be between 0 and 2.\n");
            return;
        }

        *r0 &= ~(7 << 10);
        *r0 |= (1 << (10 + mode));
    }

    void activate_default_state(unsigned short *r0) {
        *r0 = 0;

        *r0 |= (1 << 0);
        *r0 |= (2 << 3);
        *r0 |= (1 << 10);
    }

    void def_color_red(unsigned short *r1, int valor) {
        *r1 &= ~(255 << 0);
        if(valor<= 255 && valor>=0) {
            *r1 |= (valor << 0);
        }
    }

    void def_color_green(unsigned short *r1, int valor) {
        *r1 &= ~(255 << 8);
        if(valor<= 255 && valor>=0) {
            *r1 |= (valor << 8);
        }
    }

    void def_color_blue(unsigned short *r2, int valor) {
        *r2 &= ~(255 << 0);
        if(valor<= 255 && valor>=0) {
            *r2 |= (valor << 0);
        }
    }

    int read_status_display(unsigned short *r0) {
        int bit_0 = (*r0 >> 0) & 0b1;

        return bit_0;
    }

    int read_display_mode(unsigned short *r0) {
        unsigned short bits_1_2 = (*r0 >> 1) & 0b11;
        return bits_1_2;
    }

    int read_refresh_rate(unsigned short *r0) {
        int bits_3_to_8 = (*r0 >> 3) & 0b111111;
        return bits_3_to_8;
    }

    void run_program(unsigned short *r0, unsigned short *r1, unsigned short *r2) {
        int choice = -1;
        while (choice != 0) {
            printf("Menu de opcoes: \n [1] Ligar/Desligar display\n [2] Selecionar modo de exibição");
            printf("\n [3] Alterar Refresh Rate\n [4] Ligar/Desligar LED operação\n [5] Escolher cor LED \n");
            printf(" [7] Alterar cor do Display\n\n");
            printf(" [0] Finalizar execução    [6] Restaurar Padrao  [8] Leitura de Informacoes\n");

            scanf("%d", &choice);

            if (choice == 1) {//ligar/desligar
                int sub;
                printf("\nLigar/Desligar display: \n [0] Desligar\n [1] Ligar\n");
                scanf("%d", &sub);

                if (sub==1 || sub==0) {
                    set_display_state(r0, sub);
                } else {
                    printf("Escolha inválida. Tente novamente.\n");
                }
            } else if (choice == 2) {//modo exibicao
            int sub;
            printf("\nModo de Exibicao:\n [0] Estatico\n [1] Deslizante\n [2] Piscante\n [3] Deslizante/Piscante\n");
            scanf("%d", &sub);

            if(sub >= 0 && sub <= 3) {
                set_display_mode(r0, sub);
            } else {
                printf("Escolha inexistente. Tente novamente.\n");
            }
            } else if(choice == 3) {//refresh rate
                int sub;
                printf("\nRefresh Rate: \n\n Insira um valor entre 0 e 63!\nValor: ");
                scanf("%d", &sub);
                if(sub>=0 && sub<=63) {
                    set_refresh_rate(r0, sub);
                } else {
                    printf("Valor invalido. Tente novamente.\n");
                }
            } else if(choice == 4) {//ligar/desligar led
                int sub;
                printf("\nLigar/Desligar: \n [0] Desligar\n [1] Ligar\n");
                scanf("%d", &sub);
                if(sub<=1 && sub>=0) {
                    set_led_operation_state(r0, sub);
                } else {
                    printf("Valor invalido. Tente novamente.\n");
                }
            } else if(choice == 5) {//setar cor led
                int sub;
                printf("\nEscolher cor LED: \n [0] Azul\n [1] Verde\n [2] Vermelho\n");
                scanf("%d", &sub);
                if(sub<=2 && sub>=0) {
                    set_led_color(r0, sub);
                } else {
                    printf("Valor invalido. Tente novamente.\n");
                }
            } else if(choice == 6) {
                int sub;
                printf("\nRestaurar Padrao: \nVoce deseja restaurar para o padrao de fabrica? \n [0] Nao \n [1] Sim\n");
                scanf("%d", &sub);
                if(sub == 1) {
                    activate_default_state(r0);
                }
            } else if (choice == 7) {
                int sub = -1;
                while(sub != 0) {
                    printf("Alteracao de cor do display: \n [1] Vermelho\n [2] Verde\n [3] Azul\n [0] Sair\n");
                    scanf("%d", &sub);
                    if(sub == 1) {
                        int valor;
                        printf("\nDigite o valor para vermelho: ");
                        scanf("%d", &valor);
                        def_color_red(r1, valor);
                    } else if(sub == 2) {
                        int valor;
                        printf("\nDigite o valor para verde: ");
                        scanf("%d", &valor);
                        def_color_green(r1, valor);
                    } else if(sub == 3) {
                        int valor;
                        printf("\nDigite o valor para azul: ");
                        scanf("%d", &valor);
                        def_color_blue(r2, valor);
                    } else {
                        printf("Valor invalido. Tente novamente.\n");
                    }
                }
            } else if(choice == 8) {
                int sub = -1;
                while(sub !=0) {
                    printf("Menu de leitura: \n [1] Bit 0 (Display ON/OFF) \n [3]");
                    printf(" [0] Sair");
                    if(sub == 1) {
                        read_status_display(r0);
                    } else if(sub == 2) {
                        read_display_mode(r0);
                    } else if(sub == 3) {
                        read_refresh_rate(r0);
                    }

                }
            } else if (choice != 0) {
                printf("Escolha inválida. Tente novamente.\n");
            }
        }
    }

int main() {
    int fd;
    char* map = registers_map(FILE_PATH, FILE_SIZE, &fd);
    if (map == NULL) {
        return EXIT_FAILURE;
    }

    unsigned short *base_address = (unsigned short *)map;
    unsigned short *r0 = base_address + 0x00;
    unsigned short *r1 = base_address + 0x01;
    unsigned short *r2 = base_address + 0x02;

    run_program(r0, r1, r2);

    printf("Current value of R0: 0x%02x\n", *r0);

    if (registers_release(map, FILE_SIZE, fd) == -1) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
