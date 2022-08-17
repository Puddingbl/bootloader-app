
#include "iap.h"
#include "main.h"

uint32_t flash_addr;

static bootloader_t  iap;

static uint8_t data_buff[DATA_BUFF_SIZE]			__attribute__((at(IAP_BUFF_ADDR)));

static unsigned int be32_to_cpu(unsigned int x);

void iap_init()
{
	iap.updata_flag = APP_NULL;
	iap.load = 0;
	iap.size = 0;
	iap.ep = 0;
}

/**
	* 返回app大小
*/
uint32_t iap_get_data_size()
{
	return iap.size;
}

/**
	* 返回包括头部信息的整个文件大小
*/
uint32_t iap_gei_file_size()
{
	return iap.size + IAP_HEADER_SIZE;
}

uint32_t iap_get_load()
{
	return iap.load;
}

image_header_t iap_get_header()
{
	return iap.header;
}

void iap_set_header(image_header_t header)
{
	iap.header = header;
	iap.load = be32_to_cpu(header.ih_load);
	iap.size = be32_to_cpu(header.ih_size);
}

app_e iap_get_updata_flag()
{
	return iap.updata_flag;
}

void iap_set_updata_flag(app_e flag)
{
  iap.updata_flag = flag;
}

uint8_t iap_revice(uint8_t data)
{
	static uint32_t iap_cnt = 0;
	static uint8_t iap_stage = 1;
	
	data_buff[iap_cnt++] = data;
	
	/* 读头部，获取文件大小等信息 */
	if(iap_stage == 1)
	{
		if(iap_cnt == IAP_HEADER_SIZE)
		{		
			SPI_Flash_WriteUnfixed(data_buff,flash_addr,IAP_HEADER_SIZE);
			
			iap.header = *(image_header_t*)data_buff;		
			/* 解析头部 */
			iap.load = be32_to_cpu(iap.header.ih_load);
			iap.ep   = be32_to_cpu(iap.header.ih_ep);
			iap.size = be32_to_cpu(iap.header.ih_size);		
			
//			printf("load = %#x\n", iap.load);
//			printf("ep   = %#x\n", iap.ep);		
//			printf("size = %#x\n", iap.size);
//			printf("......\n");
			
			iap_cnt = 0;
			iap_stage = 2;
		}	
	}
	/* 根据头部信息读数据 */
	else if(iap_stage == 2)
	{
		if(iap_cnt == iap.size)
		{
			printf("接收完成\n");
			printf("data_buff1=%#x\n", *(uint32_t*)data_buff);
			printf("data_buff2=%#x\n", *(uint32_t*)(data_buff+4));
			
			printf("开始写入flash\n");
			printf("请等待擦除完成\n");
			for(uint16_t i=0; i<((iap.size)/PAGE_SIZE)+1; i++)  //计算有多少页，不足一页按一页算
			{
				iap_flash_eraseSector(iap.load + PAGE_SIZE*i);
			}
			printf("擦除完成\n");			
			HAL_Delay(10);
			
			printf("正在写入\n");
			iap_flash_write(data_buff, iap.load, iap_gei_file_size());
			printf("写入完成\n");
			HAL_Delay(10);
			
			uint8_t updata_tast[8];
			iap_flash_read(updata_tast, iap.load, 8);	
			printf("updata_tast1=%#x\n", *(uint32_t*)updata_tast);
			printf("updata_tast2=%#x\n", *(uint32_t*)(updata_tast+4));
			
			iap_stage = 1;
			iap_cnt = 0;
			
			return 1;
		}	
	}
	return 0;
}

void iap_jump(app_e app)
{
#ifdef IAP_ROM
	start_app(iap.ep);
	
#elif IAP_RAM
	switch(app){
		case APP_NULL:
			return;
				
		case APP1:
			SPI_Flash_ReadUnfixed(data_buff, 0, IAP_HEADER_SIZE);
			break;
		
		case APP2:
			SPI_Flash_ReadUnfixed(data_buff, 4096, IAP_HEADER_SIZE);
			break;		
	
		default:return;
	}
	
	iap.header = *(image_header_t*)data_buff;		
	iap.load = be32_to_cpu(iap.header.ih_load);
	iap.ep   = be32_to_cpu(iap.header.ih_ep);
	iap.size = be32_to_cpu(iap.header.ih_size);	
	
	printf("load = %#x\n", iap.load);
	printf("ep   = %#x\n", iap.ep);		
	printf("size = %#x\n", iap.size);
	printf("......\n");	
	
	uint8_t *app_data = (uint8_t*)iap.ep;
	
	iap_flash_read(app_data, iap.load, iap.size);	
	printf("vector=%#x\n", *(uint32_t*)app_data);
	printf("reset_handle=%#x\n", *(uint32_t*)(app_data+4));	
	printf("vector_end=%#x\n", *(uint32_t*)(app_data+660));	
	
	start_app(iap.ep);
#endif		
}

static unsigned int be32_to_cpu(unsigned int x)
{
	unsigned char *p = (unsigned char *)&x;
	unsigned int le;
	le = (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + (p[3]);
	return le;
}
