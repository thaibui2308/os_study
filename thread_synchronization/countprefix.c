#include "countprefix.h"

void *countPrefix(void *arg) {
    RESOURCE *data = (RESOURCE *) arg;

    // Wait for the queue is not empty
    pthread_mutex_lock(&data->queue_mutex);
    while (isEmpty(data->prefixQueue)) {
        pthread_cond_wait(&data->cond_count_prefix, &data->queue_mutex);
    }
    while (!isEmpty(data->prefixQueue)) {
        // Open output file for writing
        if (data->outputFile == NULL) {
            perror("Failed: unable to open output file");
            exit(1);
        }
        char *word = dequeue(data->prefixQueue);
        node location = findEndingNodeOfAStr(data->dictRootNode, word);
        if (location != NULL) {
            int counter = 0;
            countWordsStartingFromANode(location, &counter);
            if (counter >= data->minNumOfWordsWithAPrefixForPrinting) {
                fprintf(data->outputFile, "%s %d\n", word, counter);
            }
        } 
        data->numOfProcessedPrefixes++;
    }
    pthread_mutex_unlock(&data->queue_mutex);
    if (isEmpty(data->prefixQueue) && data->taskCompleted[TESTFILEINDEX]) {
        pthread_exit(NULL);
    }
    return NULL;
}