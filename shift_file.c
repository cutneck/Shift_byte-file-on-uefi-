#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

unsigned char rotateRight(unsigned char byte, int n) {
    return (byte >> n) | (byte << (8 - n));
}

unsigned char rotateLeft(unsigned char byte, int n) {
    return (byte << n) | (byte >> (8 - n));
}

void printBytes(const unsigned char *data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%02x ", data[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <file> <N>\n", argv[0]);
        return 1;
    }

    char *filePath = argv[1];
    int n = atoi(argv[2]);

    int fd = open(filePath, O_RDWR);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    struct stat statbuf;
    if (fstat(fd, &statbuf) == -1) {
        perror("Error getting file size");
        close(fd);
        return 1;
    }
    size_t fileSize = statbuf.st_size;

    unsigned char *data = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return 1;
    }

    printf("Bytes before modification:\n");
    printBytes(data, fileSize);

    for (size_t i = 0; i < fileSize; i++) {
        if (n >= 0) {
            data[i] = rotateRight(data[i], n % 8);
        } else {
            data[i] = rotateLeft(data[i], (-n) % 8);
        }
    }

    printf("Bytes after modification:\n");
    printBytes(data, fileSize);

    msync(data, fileSize, MS_SYNC);
    munmap(data, fileSize);
    close(fd);

    return 0;
}
