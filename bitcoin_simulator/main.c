#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "pthread.h"
#include "producer.h"
#include "consumer.h"

int main(int argc, char **argv) {
    /* Our broker abstract data structure */
    BufferADT broker;

    /* DEFAULT FLAG */
    int r_flag = DEFAULT_R_FLAG;
    int x_flag = DEFAULT_WAITTIME;
    int y_flag = DEFAULT_WAITTIME;
    int b_flag = DEFAULT_WAITTIME;
    int e_flag = DEFAULT_WAITTIME;

    /* Parsing additional argument with invalid-value handler */
    int opt;
    while ((opt = getopt( argc, argv, "r:x:y:b:e:")) != -1) {
        switch (opt) {
            case 'r':
                r_flag = atoi(optarg);
                if (r_flag < 0) {
                    printf("Invalid flag value: %d\n", r_flag);
                    exit(0);
                }
                break;
            case 'x':
                x_flag = atoi(optarg);
                if (x_flag < 0) {
                    printf("Invalid flag value: %d\n", x_flag);
                    exit(0);
                }
                break;
            case 'y':
                y_flag = atoi(optarg);
                if (y_flag < 0) {
                    printf("Invalid flag value: %d\n", y_flag);
                    exit(0);
                }
                break;
            case 'b':
                b_flag = atoi(optarg);
                if (b_flag < 0) {
                    printf("Invalid flag value: %d\n", b_flag);
                    exit(0);
                }
                break;
            case 'e':
                e_flag = atoi(optarg);
                if (e_flag < 0) {
                    printf("Invalid flag value: %d\n", e_flag);
                    exit(0);
                }
                break;
            default:
                break;
        }
    }

    /* Broker initialization */
    broker = createQueue();
    broker->total_number_of_consumptions = r_flag;
    broker->total_number_of_requests = r_flag;

    /* Producer and Consumer data structure initialization */
    producerPtr BitcoinClient, EthereumClient;
    consumerPtr  clientY, clientX;
    BitcoinClient = createProducer(broker, Bitcoin, b_flag);
    EthereumClient = createProducer(broker, Ethereum, e_flag);
    clientX = createConsumer(broker, BlockchainX, x_flag);
    clientY = createConsumer(broker, 1, y_flag);


    /* Thread creation */
    pthread_t _bitcoin, _ethereum, _blockchainY, _blockchainX;
    pthread_create(&_ethereum, NULL, &producer, EthereumClient);
    pthread_create(&_bitcoin, NULL, &producer, BitcoinClient);
    pthread_create(&_blockchainY, NULL, &consumer, clientY);
    pthread_create(&_blockchainX, NULL, &consumer, clientX);
    
    /* Wait till the consumption is over to print out a summary */
    sem_wait(&broker->terminated);
    unsigned int *consumed[RequestTypeN];
    consumed[BlockchainX] = broker->blockchain_X_consumption;
    consumed[BlockchainY] = broker->blockchain_Y_consumption; 
    log_production_history(
        broker->produced_requests,
        consumed
    );

    return 0;
}