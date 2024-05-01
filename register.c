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

    void set_display_on(unsigned short *r0) {
        *r0 |= (1 << 0);
    }

    void set_display_off(unsigned short *r0) {
        *r0 &= ~(1 << 0);
    }

    void set_display_mode(unsigned short *r0, int mode) {
    if (mode < 0 || mode > 3) {
        printf("Error: Invalid display mode. Mode must be between 0 and 3.\n");
        return;
    }
    *r0 &= ~(3 << 1);
    *r0 |= (mode << 1);
    /*
    0 - 00 = estático (default)
    1 - 01 = deslizante
    2 - 10 = piscante
    3 - 11 = deslizante/piscante-*/
    }


    void set_refresh_rate(unsigned short *r0, int value) {
    if (value < 0 || value > 63) {
        printf("Error: Invalid value. Value must be between 0 and 63.\n");
        return;
    }

    *r0 &= ~(63 << 3);
    *r0 |= (value << 3);

    }

    void led_operation_on(unsigned short *r0) {
        *r0 |= (1 << 9);
    }

    void led_operation_off(unsigned short *r0) {
        *r0 &= ~(1 << 9);
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

    set_display_on(r0);
    set_display_mode(r0, 1);
    set_refresh_rate(r0, 5);
    led_operation_on(r0);
    
    printf("Current value of R0: 0x%02x\n", *r0);

    if (registers_release(map, FILE_SIZE, fd) == -1) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}