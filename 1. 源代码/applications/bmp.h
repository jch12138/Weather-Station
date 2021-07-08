#ifndef BMP_H
#define BMP_H

#include "stdint.h"

typedef struct
{
	uint8_t  pic_head[2];				//1
	uint16_t pic_size_l;			    //2
	uint16_t pic_size_h;			    //3
	uint16_t pic_nc1;				    //4
	uint16_t pic_nc2;				    //5
	uint16_t pic_data_address_l;	    //6
	uint16_t pic_data_address_h;		//7	
	uint16_t pic_message_head_len_l;	//8
	uint16_t pic_message_head_len_h;	//9
	uint16_t pic_w_l;					//10
	uint16_t pic_w_h;				    //11
	uint16_t pic_h_l;				    //12
	uint16_t pic_h_h;				    //13	
	uint16_t pic_bit;				    //14
	uint16_t pic_dip;				    //15
	uint16_t pic_zip_l;			    //16
	uint16_t pic_zip_h;			    //17
	uint16_t pic_data_size_l;		    //18
	uint16_t pic_data_size_h;		    //19
	uint16_t pic_dipx_l;			    //20
	uint16_t pic_dipx_h;			    //21	
	uint16_t pic_dipy_l;			    //22
	uint16_t pic_dipy_h;			    //23
	uint16_t pic_color_index_l;	    //24
	uint16_t pic_color_index_h;	    //25
	uint16_t pic_other_l;			    //26
	uint16_t pic_other_h;			    //27
	uint16_t pic_color_p01;		    //28
	uint16_t pic_color_p02;		    //29
	uint16_t pic_color_p03;		    //30
	uint16_t pic_color_p04;		    //31
	uint16_t pic_color_p05;		    //32
	uint16_t pic_color_p06;		    //33
	uint16_t pic_color_p07;		    //34
	uint16_t pic_color_p08;			//35			
}BMP_HEAD;

typedef struct
{
	uint16_t x;
	uint16_t y;
	uint8_t  r;
	uint8_t  g;
	uint8_t  b;		
}BMP_POINT;

void Show_BMP( unsigned int point_x, unsigned int point_y, char *path );
#endif