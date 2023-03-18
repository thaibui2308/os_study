#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>

const char *delimiters = "\n\r !\"#$%&()*+,./0123456789:;<=>?@[\\]^`{|}~";

bool read_vocabs_file(node root,char *filename) {
    FILE *file;
    file = fopen(filename, "r");

    if (file == NULL) {
        perror("Failed: unable to open vocabulary file");
        return false;
    }

    char buffer[MAX_BUFFER_LENGTH];
    while (fgets(buffer, MAX_BUFFER_LENGTH,file)) {
        buffer[strcspn(buffer,"\n")] = 0;
        bool status = add(root,buffer);
        if (!status) {
            perror("Failed: unable to insert word into the dictionary tree.");
            return false;
        }
    }
    return true;
}

bool read_text_file(node root,char *filename) {
    FILE *file;
    file = fopen(filename,"r");

    if (file == NULL) {
        perror("Failed: unable to open text file");
        return false;
    } 

    char buffer[MAX_BUFFER_LENGTH];
    while (fgets(buffer, MAX_BUFFER_LENGTH,file)) {
        buffer[strcspn(buffer,"\n")] = 0;
        char *word = strtok(buffer, delimiters);
        while (word != NULL) {
            node location = findEndingNodeOfAStr(root, word);
            if (location != NULL) {
                int counter = 0;
                countWordsStartingFromANode(location, &counter);
                printf("%s %d\n",word,counter);
            } else {
                printf("%s %d\n",word,0);
            }
            word = strtok(NULL, delimiters);
        }
    } 
    return true;
}



int main(int argc, char **argv) {
    char *vocab_file, *text_file;
    if (argc == 0 || argc < 3 || argc > 3) {
        perror("Error: invalid or insufficient arguments provided!");
        return 0;
    } else {
        vocab_file = *(argv+1);
        text_file = *(argv+2);
    }
    node root = createNode();

    bool execStatus = read_vocabs_file(root,vocab_file);
    if (!execStatus) {
        perror("Failed: word insertions failed.");
        exit(1);
    }
    read_text_file(root, text_file);
    
    return 0;
}
