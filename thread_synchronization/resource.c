#include "resource.h"

SHARED_DATA initShared() {
    SHARED_DATA data;
    data = (SHARED_DATA) malloc(sizeof(RESOURCE));
    
    // Initial data declaration
    data->dictRootNode = createNode();
    data->prefixQueue = createQueue();
    data->taskCompleted[DICTSRCFILEINDEX] = false;
    data->taskCompleted[TESTFILEINDEX] = false; 

    // default flags
    data->numOfProgressMarks = DEFAULT_NUMOF_MARKS;
    data->hashmarkInterval = DEFAULT_HASHMARKINTERVAL;
    data->minNumOfWordsWithAPrefixForPrinting = DEFAULT_MINNUM_OFWORDS_WITHAPREFIX;

    return data;
}