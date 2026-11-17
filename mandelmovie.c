/**
 * Author: Joshua Simon
 * Filename: mandelmovie.c
 * Description: Uses multiple child processes to generate 50 Mandelbrot images at different zoom levels
 * Created for Lab11_Multiprocessing
**/

/**
 * AI DISCLAIMER
 * ChatGPT was used in assistance to create this code. It was used to:
 * - Proof-read the code
 * - Help develop portions of code when I was confused
 * - Make sure the code followed the Lab requirements
**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

// Set the number of images to generate
#define FRAMES 50

int main(int argc, char *argv[]) {

    // Store the command line option
    int opt;

    // Store the number of child processes requested
    int num_processes = 0;

    // Keep track of how many children are running
    int running = 0;

    // Store the exit status of a child process
    int status;

    // Check if any child process failed
    int failed = 0;

    // Set the starting zoom level
    double scale = 0.2;

    // Read the command line arguments
    while ((opt = getopt(argc, argv, "p:")) != -1) {

        switch (opt) {

            case 'p':

                // Convert the user input into an integer
                num_processes = atoi(optarg);
                break;

            default:

                // Show the correct way to run the program
                printf("Usage: %s -p <processes>\n", argv[0]);
                return 1;
        }
    }

    // Check if the number of processes is valid
    if (num_processes < 1 || num_processes > FRAMES) {
        printf("Enter a number of processes from 1 to 50.\n");
        return 1;
    }

    // Loop through all 50 images
    for (int i = 0; i < FRAMES; i++) {

        // Check if the maximum number of children is running
        if (running >= num_processes) {

            // Store the process ID of the finished child
            pid_t finished;

            // Wait until one child finishes
            do {
                finished = wait(&status);
            } while (finished == -1 && errno == EINTR);

            // Check if wait failed
            if (finished == -1) {
                perror("wait");
                failed = 1;
                break;
            }

            // Check if the child finished successfully
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                failed = 1;
            }

            // Decrease the number of running children
            running--;
        }

        // Create a new child process
        pid_t pid = fork();

        // Check if fork failed
        if (pid < 0) {
            perror("fork");
            failed = 1;
            break;
        }

        // Run this section only inside the child process
        if (pid == 0) {

            // Store the output filename
            char filename[50];

            // Store the scale as a string
            char scale_string[50];

            // Give each image a unique filename
            snprintf(filename, sizeof(filename), "mandel%d.jpg", i);

            // Convert the current scale into a string
            snprintf(scale_string, sizeof(scale_string), "%.12f", scale);

            // Run the original mandel program
            // Set the coordinates, scale, image size, and filename
            execl("./mandel", "mandel",
                  "-x", "-0.38",
                  "-y", "-0.665",
                  "-s", scale_string,
                  "-W", "1000",
                  "-H", "1000",
                  "-o", filename,
                  (char *)NULL);

            // Display an error if execl fails
            perror("execl");

            // Exit the child process
            _exit(1);
        }

        // Increase the number of active children
        running++;

        // Decrease the scale to zoom in for the next image
        scale *= 0.95;
    }

    // Wait for all remaining child processes to finish
    while (running > 0) {

        // Store the finished child's process ID
        pid_t finished;

        // Wait for a child to finish
        do {
            finished = wait(&status);
        } while (finished == -1 && errno == EINTR);

        // Check if wait failed
        if (finished == -1) {
            perror("wait");
            failed = 1;
            break;
        }

        // Check if the child finished successfully
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            failed = 1;
        }

        // Decrease the number of running children
        running--;
    }

    // Check if any images failed to generate
    if (failed) {
        fprintf(stderr, "One or more images failed to generate.\n");
        return 1;
    }

    // Tell the user that all images are finished
    printf("Finished generating all 50 images!\n");

    // End the program successfully
    return 0;
}
