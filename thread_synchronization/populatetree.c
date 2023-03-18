#include "populatetree.h"

void *populateTree(void *arg) {
    // Parse in shared data structure
    FILE *file;
    RESOURCE *data = (RESOURCE *) arg;

    char buffer[MAX_BUFFER_LENGTH];
    
    file = fopen(data->filePath[DICTSRCFILEINDEX], "r");

    if (file == NULL) {
        printf("Unable to open <<%s>>\n", data->filePath[DICTSRCFILEINDEX]);
        exit(1);
    }
    
    while (fgets(buffer, MAX_BUFFER_LENGTH,file) != NULL) {
        // Increase the number of chars read in
        data->numOfCharsReadFromFile[DICTSRCFILEINDEX] += strlen(buffer);
        data->numOfCharsReadFromFile[DICTSRCFILEINDEX]++;

        // adding words to the dictionary tree
        buffer[strcspn(buffer,"\n")] = 0;
        bool status = add(data->dictRootNode,buffer);
        if (!status) {
            perror("Failed: unable to insert word into the dictionary tree.");
            return false;
        } 
        data->wordCountInFile[DICTSRCFILEINDEX]++;
        
    }

    // signal that data is ready
    pthread_mutex_lock(&data->queue_mutex);
    data->taskCompleted[DICTSRCFILEINDEX] = true;
    printf("\n");
    printf("There are %d words in %s.\n",data->wordCountInFile[DICTSRCFILEINDEX],data->filePath[DICTSRCFILEINDEX]);
    pthread_cond_signal(&data->cond_read_prefix);
    pthread_mutex_unlock(&data->queue_mutex);
    pthread_exit(NULL);
}