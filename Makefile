###VARIABLES###
CC = gcc
CFLAGS = -Wall -Werror -Wextra -fsanitize=address

###FOLDERS###
RBPATH = additional_task/red_black_tree/
LAB1PATH = lab_1/
LAB3PATH = lab_3/
LAB5PATH = lab_5/
LAB6PATH = lab_6/

###STAGES###
all: rbtree lab1 lab3 lab5 lab6

rbtree: $(RBPATH)rb.c $(RBPATH)rb_struct.c
	$(CC) $(CFLAGS) -o rb_build $^

lab1: $(LAB1PATH)archiver.c $(LAB1PATH)archiver_funcs.c
	$(CC) $(CFLAGS) -o archiver $^

lab3: $(LAB3PATH)main.c
	$(CC) $(CFLAGS) -o shell $^

lab5: $(LAB5PATH)main.c
	$(CC) $(CFLAGS) -o sobel $^ -lm -ljpeg -lpthread

lab6: $(LAB6PATH)server.c $(LAB6PATH)client.c
	$(CC) $(CFLAGS) -o server $(LAB6PATH)server.c
	$(CC) $(CFLAGS) -o client $(LAB6PATH)client.c

###CLANGS###
clang: clang_rb clang_lab1 clang_lab3 clang_lab6

clang_rb:
	mv .clang-format $(RBPATH)
	clang-format -i $(RBPATH)*.c $(RBPATH)*.h
	mv $(RBPATH).clang-format ./

clang_lab1:
	mv .clang-format $(LAB1PATH)
	clang-format -i $(LAB1PATH)*.c $(LAB1PATH)*.h
	mv $(LAB1PATH).clang-format ./

clang_lab3:
	mv .clang-format $(LAB3PATH)
	clang-format -i $(LAB3PATH)*.c
	mv $(LAB3PATH).clang-format ./

###NOT CLANG LAB5

clang_lab6:
	mv .clang-format $(LAB3PATH)
	clang-format -i $(LAB3PATH)*.c
	mv $(LAB3PATH).clang-format ./

###CLEANS###
clean: clean_rb clean_lab_1_build clean_lab_3_build clean_lab_5_build clean_lab_6_build
	rm -f *.out

clean_rb:
	rm -f rb_build

clean_lab_1_build:
	rm -f archiver

clean_lab_3_build:
	rm -f shell

clean_lab_5_build:
	rm -f sobel

clean_lab_6_build:
	rm -f sockets