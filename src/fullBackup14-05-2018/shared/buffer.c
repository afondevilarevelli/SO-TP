#include "buffer.h"

tBuffer * newBuffer(void)
{
	tBuffer * pBuffer = malloc(sizeof(tBuffer));	

	pBuffer->data = NULL;
	pBuffer->size = 0;
	
	return pBuffer;
}

void addToBuffer(  tBuffer * pBuffer, void * newData, int sizeofData )
{
	pBuffer->data = realloc( pBuffer->data, pBuffer->size +  sizeofData );
	memcpy(pBuffer->data + pBuffer->size, newData, sizeofData);
	pBuffer->size += sizeofData;
}

void freeBuffer(tBuffer * pBuffer)
{
	free(pBuffer->data);
	free(pBuffer);
}