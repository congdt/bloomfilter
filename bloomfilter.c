#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <limits.h>

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;


#define SIZE		134217728 // = UINT_MAX/sizeof(int)*8

#define TRUE		1

#define	START		1
#define	STOP		2
#define	RESTART		3
#define	CHECK_IP	4
#define TERMINATE	5
#define ADD			6
uint32_t 	murmur3_32(const char*, uint32_t, uint32_t);

void 		usage();
int 		start();
void 		stop();
int 		restart();
void 		check_ip();
int 		command_handle(char *cmd, int len);

void 		filter_set_bit(unsigned int i);		// set bit[i] = 1
int 		filter_get_bit(unsigned int i);

uint32_t	bloom_filter[SIZE];


int main(int argc, char** argv)
{
	int option;
	
	while(TRUE){
		usage();
		if(bloom_filter == 0){
			printf("Chon tuy chon (1,2,3,4,5): ");
		}
		else{
			printf("Chon tuy chon tiep theo: ");
		}
		scanf("%d", &option);
		
		if(option == START){
			start();
		}
		else if (option == STOP){
			stop();
		}
		else if (option == RESTART){
			stop();
			start();
		}
		else if (option == CHECK_IP){
			check_ip();
		}
		else if (option == TERMINATE){
			break;
		}
		else if(option == ADD){
			char s[40];
			unsigned int hash_code;
			printf("Nhap dia chi them vao: ");
			scanf("%s", s);
			hash_code = murmur3_32(s, strlen(s), 0);
			printf("hahs code = %lu", hash_code);
			filter_set_bit(hash_code);
		}
		
	}
	
	return 0;
}


void usage(){
	printf("\n");
	printf("////////////////////////////////////////////////\n");
	printf("|-------------- bloom filter ------------------|\n");
	printf("| 1. start input file input or add more input  |\n");
	printf("| 2. stop                                      |\n");
	printf("| 3. restart filename                          |\n");
	printf("| 4. check IP                                  |\n");
	printf("| 5. terminate                                 |\n");
	printf("| 6. add address handly                        |\n");
	printf("|----------------------------------------------|\n");
	printf("\n");
	
}

int start(){
	
	char filename[20];
	FILE *f;
	char inString[1024];
	uint32_t hash_code;
	
	
	printf("\nTen file: ");
	scanf("%s", filename);
/*	
	if(bloom_filter == 0){
		printf("\nRestart with input file: %s", filename);
	}
	else{
		printf("\nAdding new input file: %s", filename);
	}
*/	
	if( !(f=fopen(filename, "r")) ){
		printf("\ncannot read file\n");
		return 1;
	}
	
	while(TRUE){
		
		fgets(inString, 1024, f);
		printf("len truoc = %d", strlen(inString));
		inString[strlen(inString)-2] = '\0';
		printf("inString: %s # len= %d\n", inString, strlen(inString));
		if (feof(f)) break;
		printf("++++++++++++++++++\n");
		hash_code = murmur3_32(inString, strlen(inString),0 );
		printf("Hashcode = %lu", hash_code);
		filter_set_bit(hash_code);
	}
	fclose(f);
	printf("Bloom filter table :");
	
	
	return 0;
}


void stop(){
	int i;
	for(i = 0; i < SIZE; i++)
		bloom_filter[i] = 0;
}

void check_ip(){
	
	char 		ip[40];	//max for ipv6
	uint32_t	hash_code; 
	while(TRUE){
		printf("\nNhap vao IP, hoac nhap vao end de thoat kiem tra ip: ");
		scanf("%s", ip);
		if(strcmp(ip,"end") == 0){
			return;
		}
		hash_code = murmur3_32(ip, strlen(ip), 0 );
		printf("hash code = %lu\n", hash_code);
		if( filter_get_bit(hash_code) == 1 ){
			printf("\n >>>>>>> Maybe MATCH <<<<<<<");
		}
		else{
			printf("\n >>>>>>>  NOT MATCH  <<<<<<<");
		}
	}
}

void filter_set_bit(unsigned int i){
	bloom_filter[i/32] |= (1 << (i%32) );
}

int filter_get_bit(unsigned int i){
	if( (bloom_filter[i/32] & (1 << (i%32) ) ) == 0 )
		return 0;
	else 
		return 1;
}


#define ROT32(x, y) ((x << y) | (x >> (32 - y))) // avoid effort44
uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed) {
	static const uint32_t c1 = 0xcc9e2d51;
	static const uint32_t c2 = 0x1b873593;
	static const uint32_t r1 = 15;
	static const uint32_t r2 = 13;
	static const uint32_t m = 5;
	static const uint32_t n = 0xe6546b64;

	uint32_t hash = seed;

	const int nblocks = len / 4;
	const uint32_t *blocks = (const uint32_t *) key;
	int i;
	uint32_t k;
	for (i = 0; i < nblocks; i++) {
		k = blocks[i];
		k *= c1;
		k = ROT32(k, r1);
		k *= c2;

		hash ^= k;
		hash = ROT32(hash, r2) * m + n;
	}

	const uint8_t *tail = (const uint8_t *) (key + nblocks * 4);
	uint32_t k1 = 0;

	switch (len & 3) {
	case 3:
		k1 ^= tail[2] << 16;
	case 2:
		k1 ^= tail[1] << 8;
	case 1:
		k1 ^= tail[0];

		k1 *= c1;
		k1 = ROT32(k1, r1);
		k1 *= c2;
		hash ^= k1;
	}

	hash ^= len;
	hash ^= (hash >> 16);
	hash *= 0x85ebca6b;
	hash ^= (hash >> 13);
	hash *= 0xc2b2ae35;
	hash ^= (hash >> 16);

	return hash;
}
