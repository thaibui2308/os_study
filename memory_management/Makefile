CC=gcc
CFLAGS=-g -Wall

mmuwithtlb: main.c page_table.c print_helpers.c vaddr_tracereader.c
	$(CC) $(CFLAGS) -o mmuwithtlb main.c page_table.c map.c utils.c print_helpers.c vaddr_tracereader.c

# mmuwithtlb: main.c page_table.c print_helpers.c vaddr_tracereader.c
#         $(CC) $(CFLAGS) -o mmuwithtlb main.c page_table.c map.c utils.c print_helpers.c vaddr_tracereader.c

page_table.o: page_table.c page_table.h utils.c map.c
	$(CC) $(CCFLAG) -c page_table.c

map.o: map.c map.h
	$(CC) $(CCFLAG) -o map.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -o utils.c

print_helpers.o: print_helpers.c print_helpers.h
	$(CC) $(CFLAGS) -o print_helpers.c

vaddr_tracereader.o: vaddr_tracereader.c vaddr_tracereader.h
	$(CC) $(CFLAGS) -o vaddr_tracereader.c

clean:
	rm *.o mmuwithtlb
