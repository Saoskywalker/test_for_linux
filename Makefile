.PHONY:clean
.PHONY:write
.PHONY:mktool
.PHONY:dump
.PHONY:run

CC:=6
DD = fdsf
CC = 654
#rwer

test1: 
#fdgs
	@   echo compiling iii $(DD) $(CC) $@
	gcc -o OUTPUT/main main.c ts_calibrate_common.c cJSON.c
	@ echo "<><><><><><>><><>><><><><><>"
	@ echo compile ok
	@ echo "<><><><><><>><><>><><><><><>"
	@ echo run ./OUTPUT/test
	./OUTPUT/main
	
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

