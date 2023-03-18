#include "readprefix.h"

void *readPrefix(void *arg) {
    // Parse in shared data structure
    FILE *file;
    RESOURCE *data = (RESOURCE *) arg;

    char *delimiters = "\n\r !\"#$%&()*+,./0123456789:;<=>?@[\\]^`{|}~";


    // Relevant variables for printing out thread progress information
    int hashFlag = 1;
    int maxHash = 0;
    int prev = 0;
    int percentage = 0;
    int threshold = 100/data->numOfProgressMarks;

    char buffer[MAX_BUFFER_LENGTH];


    // Wait for populateTree done
    pthread_mutex_lock(&data->queue_mutex);
    while (!data->taskCompleted[DICTSRCFILEINDEX]) {
        pthread_cond_wait(&data->cond_read_prefix, &data->queue_mutex);
    }

    // readPrefix is now ready
    file = fopen(data->filePath[TESTFILEINDEX],"r");

    if (file == NULL) {
        printf("Unable to open <<%s>>\n", data->filePath[TESTFILEINDEX]);
        exit(1);
    } 

    while (fgets(buffer, MAX_BUFFER_LENGTH,file) != NULL) {
        // Updating the number of chars reading in
        data->numOfCharsReadFromFile[TESTFILEINDEX] += strlen(buffer);
        data->numOfCharsReadFromFile[TESTFILEINDEX]++;

        // Printing progress bar
        percentage = (int) (100*data->numOfCharsReadFromFile[TESTFILEINDEX]) / data->totalNumOfCharsInFile[TESTFILEINDEX];
        if (percentage-prev >= threshold && maxHash < data->numOfProgressMarks / data->hashmarkInterval) {
            if (hashFlag > 0 && hashFlag % data->hashmarkInterval == 0) {
                maxHash++;
                printf("#");
            } else {
                printf("-");
            }
            prev = percentage;
            hashFlag++;
        }

        buffer[strcspn(buffer,"\n")] = 0;
        char *word = strtok(buffer, delimiters);

        while (word != NULL) {
            enqueue(data->prefixQueue, word);
            pthread_cond_signal(&data->cond_count_prefix);
            word = strtok(NULL, delimiters);
            data->wordCountInFile[TESTFILEINDEX]++;
        }
    } 

    data->taskCompleted[TESTFILEINDEX] = true;
    printf("\nThere are %d words in %s.\n",data->wordCountInFile[TESTFILEINDEX],data->filePath[TESTFILEINDEX]);
    pthread_mutex_unlock(&data->queue_mutex);
    pthread_exit(NULL);
    return NULL;
}