FLAGS=-c -g

assn_3: assn_3.o merge.h
	$(CC) assn_3.o -o $@

assn_3.o: assn_3.c merge.h
	$(CC) $(FLAGS) $< -o $@
clean:
	rm *.o
	rm assn_3
	rm *.bin.*
