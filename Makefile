all:
	gcc -std=c99 -O3 xkcd_miner.c skein.c skein_block.c -o xkcd_miner
