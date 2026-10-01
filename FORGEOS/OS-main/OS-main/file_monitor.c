#define _DEFAULT_SOURCE

#include "file_monitor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>

#define TEST_FILE "kernel_monitor_file.txt"


/* ==========================================
   1. FILE INFORMATION AND INODE
   ========================================== */

static void show_file_information(void)
{
    char path[256];

    printf("\n============================================\n");
    printf("        FILE INFORMATION AND INODE\n");
    printf("============================================\n");

    printf("Enter file path: ");

    if (fgets(path, sizeof(path), stdin) == NULL)
    {
        return;
    }

    path[strcspn(path, "\n")] = '\0';

    if (strlen(path) == 0)
    {
        printf("No file path entered.\n");
        return;
    }

    struct stat file_info;

    if (stat(path, &file_info) == -1)
    {
        perror("stat");
        return;
    }

    printf("\nFile Information\n");
    printf("--------------------------------------------\n");

    printf("Path          : %s\n", path);

    printf("Inode Number  : %lu\n",
           (unsigned long)file_info.st_ino);

    printf("File Size     : %ld bytes\n",
           (long)file_info.st_size);

    printf("Permissions   : %o\n",
           file_info.st_mode & 0777);

    printf("Hard Links    : %lu\n",
           (unsigned long)file_info.st_nlink);

    printf("User ID       : %u\n",
           file_info.st_uid);

    printf("Group ID      : %u\n",
           file_info.st_gid);

    if (S_ISREG(file_info.st_mode))
    {
        printf("File Type     : Regular File\n");
    }
    else if (S_ISDIR(file_info.st_mode))
    {
        printf("File Type     : Directory\n");
    }
    else if (S_ISLNK(file_info.st_mode))
    {
        printf("File Type     : Symbolic Link\n");
    }
    else
    {
        printf("File Type     : Other\n");
    }

    printf("--------------------------------------------\n");
}


/* ==========================================
   2. DIRECTORY ENTRIES
   ========================================== */

static void show_directory_entries(void)
{
    char path[256];

    printf("\n============================================\n");
    printf("             DIRECTORY ENTRIES\n");
    printf("============================================\n");

    printf("Enter directory path [.] : ");

    if (fgets(path, sizeof(path), stdin) == NULL)
    {
        return;
    }

    path[strcspn(path, "\n")] = '\0';

    if (strlen(path) == 0)
    {
        strcpy(path, ".");
    }

    DIR *directory = opendir(path);

    if (directory == NULL)
    {
        perror("opendir");
        return;
    }

    printf("\nContents of: %s\n", path);
    printf("--------------------------------------------\n");

    struct dirent *entry;

    int count = 0;

    while ((entry = readdir(directory)) != NULL)
    {
        char full_path[512];

        snprintf(full_path,
                 sizeof(full_path),
                 "%s/%s",
                 path,
                 entry->d_name);

        struct stat info;

        printf("%-30s",
               entry->d_name);

        /*
         * Use stat() instead of DT_DIR/DT_REG/DT_LNK.
         * This works even when those macros are unavailable.
         */
        if (stat(full_path, &info) == 0)
        {
            if (S_ISDIR(info.st_mode))
            {
                printf(" [DIRECTORY]\n");
            }
            else if (S_ISREG(info.st_mode))
            {
                printf(" [FILE]\n");
            }
            else if (S_ISLNK(info.st_mode))
            {
                printf(" [LINK]\n");
            }
            else
            {
                printf(" [OTHER]\n");
            }
        }
        else
        {
            printf(" [UNKNOWN]\n");
        }

        count++;
    }

    closedir(directory);

    printf("--------------------------------------------\n");
    printf("Total entries: %d\n", count);
}


/* ==========================================
   3. UNBUFFERED FILE I/O
   open(), read(), write(), close()
   ========================================== */

static void unbuffered_io_demo(void)
{
    printf("\n============================================\n");
    printf("          UNBUFFERED FILE I/O\n");
    printf("============================================\n");

    const char *message =
        "Hello from Linux Kernel Monitor!\n"
        "This file was written using write().\n";

    /*
     * Open/create the file.
     */
    int fd = open(TEST_FILE,
                  O_WRONLY | O_CREAT | O_TRUNC,
                  0644);

    if (fd == -1)
    {
        perror("open");
        return;
    }

    printf("File descriptor: %d\n", fd);

    /*
     * Write using the Linux write() system call.
     */
    ssize_t bytes_written =
        write(fd,
              message,
              strlen(message));

    if (bytes_written == -1)
    {
        perror("write");
        close(fd);
        return;
    }

    printf("Bytes written: %ld\n",
           (long)bytes_written);

    close(fd);

    /*
     * Open again for reading.
     */
    fd = open(TEST_FILE, O_RDONLY);

    if (fd == -1)
    {
        perror("open");
        return;
    }

    char buffer[512];

    ssize_t bytes_read =
        read(fd,
             buffer,
             sizeof(buffer) - 1);

    if (bytes_read == -1)
    {
        perror("read");
        close(fd);
        return;
    }

    buffer[bytes_read] = '\0';

    printf("\nData read using read():\n");
    printf("--------------------------------------------\n");
    printf("%s", buffer);
    printf("--------------------------------------------\n");

    close(fd);

    printf("Unbuffered I/O completed.\n");
}


/* ==========================================
   4. BUFFERED FILE I/O
   fopen(), fgets(), fclose()
   ========================================== */

static void buffered_io_demo(void)
{
    printf("\n============================================\n");
    printf("             BUFFERED FILE I/O\n");
    printf("============================================\n");

    FILE *file =
        fopen(TEST_FILE, "r");

    if (file == NULL)
    {
        printf("File does not exist yet.\n");
        printf("Run option 3 first.\n");
        return;
    }

    char line[256];

    printf("Reading using fopen() and fgets():\n");
    printf("--------------------------------------------\n");

    while (fgets(line,
                 sizeof(line),
                 file) != NULL)
    {
        printf("%s", line);
    }

    printf("--------------------------------------------\n");

    fclose(file);

    printf("Buffered I/O completed.\n");
}


/* ==========================================
   5. MEMORY-MAPPED FILE I/O
   mmap(), msync(), munmap()
   ========================================== */

static void memory_mapped_io_demo(void)
{
    printf("\n============================================\n");
    printf("          MEMORY-MAPPED FILE I/O\n");
    printf("============================================\n");

    int fd =
        open(TEST_FILE,
             O_RDWR | O_CREAT,
             0644);

    if (fd == -1)
    {
        perror("open");
        return;
    }

    const char *message =
        "Memory mapped file demonstration.\n";

    size_t length =
        strlen(message);

    if (ftruncate(fd, (off_t)length) == -1)
    {
        perror("ftruncate");
        close(fd);
        return;
    }

    /*
     * Map the file into the process address space.
     */
    char *mapped =
        mmap(NULL,
             length,
             PROT_READ | PROT_WRITE,
             MAP_SHARED,
             fd,
             0);

    if (mapped == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        return;
    }

    printf("File mapped into memory successfully.\n");

    /*
     * Write directly into mapped memory.
     */
    memcpy(mapped,
           message,
           length);

    printf("\nMapped file content:\n");
    printf("--------------------------------------------\n");
    printf("%.*s",
           (int)length,
           mapped);
    printf("--------------------------------------------\n");

    /*
     * Synchronize changes with the file.
     */
    if (msync(mapped,
              length,
              MS_SYNC) == -1)
    {
        perror("msync");
    }

    /*
     * Remove memory mapping.
     */
    if (munmap(mapped, length) == -1)
    {
        perror("munmap");
    }

    close(fd);

    printf("Memory mapping released.\n");
    printf("Memory-mapped I/O completed.\n");
}


/* ==========================================
   6. FILE DESCRIPTOR DEMONSTRATION
   ========================================== */

static void file_descriptor_demo(void)
{
    printf("\n============================================\n");
    printf("          FILE DESCRIPTOR DEMO\n");
    printf("============================================\n");

    int fd =
        open(TEST_FILE, O_RDONLY);

    if (fd == -1)
    {
        printf("File does not exist yet.\n");
        printf("Run option 3 first.\n");
        return;
    }

    printf("File successfully opened.\n");

    printf("File Descriptor: %d\n",
           fd);

    printf("\nLinux represents an open file using\n");
    printf("a file descriptor associated with the process.\n");

    close(fd);

    printf("\nFile descriptor closed using close().\n");
}


/* ==========================================
   CO-5 MENU
   ========================================== */

void show_file_monitor(void)
{
    int choice;

    while (1)
    {
        printf("\n");
        printf("============================================\n");
        printf("          CO-5: FILE SYSTEM MONITOR\n");
        printf("============================================\n");

        printf("1. File Information and Inode\n");
        printf("2. Directory Entries\n");
        printf("3. Unbuffered File I/O\n");
        printf("4. Buffered File I/O\n");
        printf("5. Memory-Mapped File I/O\n");
        printf("6. File Descriptor Demonstration\n");
        printf("7. Back to Main Menu\n");

        printf("============================================\n");

        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input.\n");

            while (getchar() != '\n')
            {
                /* Clear input */
            }

            continue;
        }

        while (getchar() != '\n')
        {
            /* Clear input */
        }

        switch (choice)
        {
            case 1:
                show_file_information();
                break;

            case 2:
                show_directory_entries();
                break;

            case 3:
                unbuffered_io_demo();
                break;

            case 4:
                buffered_io_demo();
                break;

            case 5:
                memory_mapped_io_demo();
                break;

            case 6:
                file_descriptor_demo();
                break;

            case 7:
                return;

            default:
                printf("\nPlease enter a number between 1 and 7.\n");
        }
    }
}
