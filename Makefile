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
	@   echo compiling iii $(DD) $(CC) $@
	gcc -g -std=gnu99 -Wall -o OUTPUT/main_debug main.c ts_calibrate_common.c cJSON.c
	@ echo "<><><><><><>><><>><><><><><>"
	@ echo compile ok
	@ echo "<><><><><><>><><>><><><><><>"

make_release: 
#fdgs
	@   echo compiling iii $(DD) $(CC) $@
	gcc -std=gnu99 -O3 -Wall -o OUTPUT/main_release main.c ts_calibrate_common.c cJSON.c
	@ echo "<><><><><><>><><>><><><><><>"
	@ echo compile ok
	@ echo "<><><><><><>><><>><><><><><>"
	
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

