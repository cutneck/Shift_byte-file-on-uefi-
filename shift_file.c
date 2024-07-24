#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void printBytes(const unsigned char *data, size_t size, size_t blockSize) {
    size_t blocks = size / blockSize;
    size_t remainder = size % blockSize;

    for (size_t i = 0; i < blocks; i++) {
        for (size_t j = 0; j < blockSize; j++) {
            printf("%02x ", data[i * blockSize + j]);
        }
        printf("\n");
    }

    if (remainder > 0) {
        for (size_t j = 0; j < remainder; j++) {
            printf("%02x ", data[blocks * blockSize + j]);
        }
        printf("\n");
    }
}

void rotateRight(unsigned char *data, size_t size, int n) {
    unsigned char carry = 0, newCarry;

    for (size_t i = 0; i < size; i++) {
        newCarry = data[i] & ((1 << n) - 1);
        data[i] = (carry << (8 - n)) | (data[i] >> n);
        carry = newCarry;
    }
}

void rotateLeft(unsigned char *data, size_t size, int n) {
    unsigned char carry = 0, newCarry;

    for (size_t i = size; i > 0; i--) {
        newCarry = (data[i - 1] >> (8 - n));
        data[i - 1] = (data[i - 1] << n) | carry;
        carry = newCarry;
    }
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
    printBytes(data, fileSize, 16); // Printing in blocks of 16 bytes

    if (n >= 0) {
        rotateRight(data, fileSize, n % 8);
    } else {
        rotateLeft(data, fileSize, (-n) % 8);
    }

    printf("Bytes after modification:\n");
    printBytes(data, fileSize, 16); // Printing in blocks of 16 bytes

    msync(data, fileSize, MS_SYNC);
    munmap(data, fileSize);
    close(fd);

    return 0;
}
