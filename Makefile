.PHONY:clean
.PHONY:write
.PHONY:mktool
.PHONY:make_release
.PHONY:make_debug
.PHONY:dump
.PHONY:run

CC:=6
DD = fdsf
CC = 654
#rwer

make_debug: 
#fdgs
	@ echo "*************************************************"
	@ echo "*************************************************"
	@ echo "*************************************************"
	@ echo             make_debug     start
	@ echo compiling iii $(DD) $(CC) $@
	@ echo "*************************************************"
	gcc -g -std=gnu99 -Wall -o OUTPUT/main_debug main.c ts_calibrate_common.c cJSON.c -lmingw32 -lSDL2main -lSDL2
	@ echo "<><><><><><>><><>><><><><><><><><><>><><><><><><>"
	@ echo            make_debug  compile ok
	@ echo "<><><><><><>><><>><><><><><><><><><><><><><<><><>"

make_release: 
#fdgs
	@ echo "*************************************************"
	@ echo "*************************************************"
	@ echo "*************************************************"
	@ echo             make_release     start
	@ echo compiling iii $(DD) $(CC) $@
	@ echo "*************************************************"
	gcc -std=gnu99 -O3 -Wall -o OUTPUT/main_release main.c ts_calibrate_common.c cJSON.c -lmingw32 -lSDL2main -lSDL2
	@ echo "<><><><><><>><><>><><><><><><><><><>><><><><><><>"
	@ echo            make_debug  compile ok
	@ echo "<><><><><><>><><>><><><><><><><><><><><><><<><><>"
	
test2: 
#fdgs
	echo compiling 123 $(CC) $(MAKE) $@

run:
	@ echo run ./OUTPUT/test
	./OUTPUT/test

#lkjij
clean:
	find . -name "*.o"  | xargs rm -f  
	find . -name "*.bin"  | xargs rm -f
	find . -name "*.elf"  | xargs rm -f
	find . -name "*.P"  | xargs rm -f

