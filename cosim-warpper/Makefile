





main:dump_map
	g++ main.cpp -o main


dump_map:
	nm ../build/emu > log
	python3 replace.py


clean:
	-rm log processed_log main


