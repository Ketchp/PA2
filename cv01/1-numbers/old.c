#include <stdlib.h>
#include <stdio.h>

#define SUCCESS 1

typedef struct
{
	unsigned int m_used;
	unsigned int m_cap;
	int * m_arr;
} MyArray;


int array_init ( MyArray * arr, unsigned int capacity )
{
	if ( ! arr )
		return !SUCCESS;

	MyArray newArray = {0, capacity, (int*)malloc(capacity*sizeof(int))}; 
	
	if ( newArray.m_arr == NULL )
		return !SUCCESS;
	
	*arr = newArray; 
	return SUCCESS;
}

void array_destroy ( MyArray * arr )
{
	if ( ! arr )
		return;

	free(arr->m_arr);
	arr->m_arr = NULL;
	arr->m_used = arr->m_cap = 0;
}

int array_load ( MyArray * arr, const char * formatString )
{
	for ( unsigned int i = 0; i < arr->m_cap; ++i )
		if ( scanf(formatString, arr->m_arr+i) != 1 )
			return !SUCCESS;
	return SUCCESS;
}

void array_print ( const MyArray * arr, const char * format )
{
	for ( unsigned int i = 0; i < arr->m_cap; ++i )
		printf(format, arr->m_arr[i]);
	printf("\n");
}

int main ( void )
{
	int N = 0;
	MyArray numbers;

	printf("Zadej pozadovanou velikost:\n");
	if ( scanf("%d", &N) != 1 || N <= 0 )
	{
		printf("Chybny vstup!\n");
		return 0;
	}

	if ( array_init(&numbers, N) != SUCCESS )
	{
		printf("Vytvoreni pole selhalo!\n");
		return 0;
	}

	printf("Zadej sva cisla:\n");
	if ( array_load(&numbers, " %d") != SUCCESS )
	{
		printf("Nacteni cisel selhalo!\n");
		array_destroy(&numbers);
		return 0;
	}

	printf("Nactena vsechna cisla!\n");
	array_print(&numbers, "%d ");

	array_destroy(&numbers);
	return 0;
}