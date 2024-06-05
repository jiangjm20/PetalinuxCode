#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
 
#include "libaxidma.h"
 
#define MAXLENGTH 4096
struct dma_transfer {
    int input_fd;           // The file descriptor for the input file
    int input_channel;      // The channel used to send the data
    int input_size;         // The amount of data to send
    void *input_buf;        // The buffer to hold the input data
    int output_fd;          // The file descriptor for the output file
    int output_channel;     // The channel used to receive the data
    int output_size;        // The amount of data to receive
    void *output_buf;       // The buffer to hold the output
};
 
void get_format_time_ms(char *str_time, int size) {
    struct tm *tm_t;
    struct timeval time;
 
    if (size < 32) {
        printf("input buff len less than 32");
    return;
    }
    gettimeofday(&time,NULL);
    tm_t = localtime(&time.tv_sec);
    if(NULL != tm_t) {
        sprintf(str_time,"%04d_%02d_%02d_%02d_%02d_%02d_%03ld",
            tm_t->tm_year+1900,
            tm_t->tm_mon+1,
            tm_t->tm_mday,
            tm_t->tm_hour,
            tm_t->tm_min,
            tm_t->tm_sec,
            time.tv_usec/1000);
    }
 
    return;
 
} 
 
axidma_dev_t axidma_dev1;
struct dma_transfer gTrans;
    
int initDma()
{
    
    // 初始化AXIDMA设备
    axidma_dev1 = axidma_init();
    if (axidma_dev1 == NULL) 
    {
        printf("Error: Failed to initialize the AXI DMA device.\n");
        return -1;
    }
     printf("Succeed to initialize the AXI DMA device.\n");
 
        
    return 0;
}
 
//释放资源
int destoryDma()
{
    if(axidma_dev1 != NULL)
    {
        printf("destoryDma \n");
        axidma_destroy(axidma_dev1);   
    } 
    
        
    return 0;
}
 
const array_t *tx_chans, *rx_chans;
int initInChannel()
{  
    // 如果还没有指定tx和rx通道，则获取收发通道
    tx_chans = axidma_get_dma_tx(axidma_dev1);
    
    
    if (tx_chans->len < 1) {
        printf("Error: No transmit channels were found.\n");
    }
    rx_chans = axidma_get_dma_rx(axidma_dev1);
    
    if (rx_chans->len < 1) {
        printf("Error: No receive channels were found.\n");
    }
    
     /* 如果用户没有指定通道，我们假设发送和接收通道是编号最低的通道。 */
    if (gTrans.input_channel != -1 && gTrans.output_channel != -1) 
    {
        gTrans.input_channel = tx_chans->data[0];
        gTrans.output_channel = rx_chans->data[0];
        printf("user system cfg:\n");
    }
    else
    {
        gTrans.input_channel = 0;
        gTrans.output_channel = 1;
        printf("user custom:\n");
    }
 
    printf("AXI DMAt File Transfer Info:\n");
    printf("\tTransmit Channel: %d\n", gTrans.input_channel);
    printf("\tReceive Channel: %d\n", gTrans.output_channel);
    printf("\tInput File Size: %d MiB\n", gTrans.input_size);
    printf("\tOutput File Size: %d MiB\n\n", gTrans.output_size);
    
    return 0;
}
 
/*----------------------------------------------------------------------------
 * DMA File Transfer Functions
 *----------------------------------------------------------------------------*/
static int transfer_file(axidma_dev_t dev, struct dma_transfer *trans, int dataLen)
{
    int rc;
 
    trans->output_size = dataLen * 8;
    trans->input_size = dataLen * 8;
    
    // Allocate a buffer for the input file, and read it into the buffer
    trans->input_buf = axidma_malloc(dev, trans->input_size);
    if (trans->input_buf == NULL) {
        fprintf(stderr, "Failed to allocate the input buffer.\n");
        rc = -ENOMEM;
        goto ret;
    }
 
    // Allocate a buffer for the output file
    trans->output_buf = axidma_malloc(dev, trans->output_size);
    if (trans->output_buf == NULL) {
        rc = -ENOMEM;
        goto free_input_buf;
    }
 
    // Perform the transfer
    // Perform the main transaction
    if(0 != axidma_oneway_transfer(dev, trans->output_channel, trans->output_buf, trans->output_size, true))
    {
        printf("axidma_oneway_transfer timeout.\n");
    }
    else
    {
        int i = 0;
        for(i = 0; i < dataLen; i++)
        {
            // printf("axidma_oneway_transfer buf[%04d]:%08x %08x %08x %08x %08x %08x %08x %08x .\n", i, \
            // ((char *)trans.output_buf)[i * 8 + 0], ((char *)trans.output_buf)[i * 8 + 1], ((char *)trans.output_buf)[i * 8 + 2], ((char *)trans.output_buf)[i * 8 + 3],\
            // ((char *)trans.output_buf)[i * 8 + 4], ((char *)trans.output_buf)[i * 8 + 5], ((char *)trans.output_buf)[i * 8 + 6], ((char *)trans.output_buf)[i * 8 + 7]);
        
            // u32 data_0 = ((char *)trans->output_buf)[i * 8 + 1] * 256 + ((char *)trans->output_buf)[i * 8 + 0];
            // u32 data_1 = ((char *)trans->output_buf)[i * 8 + 3] * 256 + ((char *)trans->output_buf)[i * 8 + 2];
            // u32 data_2 = ((char *)trans->output_buf)[i * 8 + 5] * 256 + ((char *)trans->output_buf)[i * 8 + 4];
            // u32 data_3 = ((char *)trans->output_buf)[i * 8 + 7] * 256 + ((char *)trans->output_buf)[i * 8 + 6];
            // printf("==> i:%05d  %05d %05d %05d %05d  \n",  i, data_0, data_1, data_2, data_3);
        } 
    }    
 
 
    
free_output_buf:
    axidma_free(dev, trans->output_buf, trans->output_size);
free_input_buf:
    axidma_free(dev, trans->input_buf, trans->input_size);
ret:
    return rc;
}
 
void GetSrcAdcDin()
{
//	u32 rxBufferPtr[1024] = {0};
	int j = 0;
	while(1)
    {
        transfer_file(axidma_dev1,&gTrans,256);
        sleep(1);
	}
 
 
}
 
int main(){
 
	initDma();
	initInChannel();
    
	GetSrcAdcDin();
    
	destoryDma();
    
	return 0;
}
