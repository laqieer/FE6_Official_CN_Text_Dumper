// Mini_Portrait_Dumper.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// 导出小头像
// by laqieer

#include "pch.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <png.h>
#include <zlib.h>
#include <io.h>
#include <direct.h>

using namespace std;

#pragma comment(lib, "libpng16.lib")

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		string s = argv[0];
		int pos = s.find_last_of('\\');
		s = s.substr(pos + 1);
		cerr << "Usage: " << s << " ROM Output_Image_Path\n";
		return -1;
	}
	ifstream rom;
	rom.open(argv[1], ios::in | ios::binary);
	if (!rom)
	{
		cerr << "fail to open the rom: " << argv[1] << endl;
		return -2;
	}
	ofstream text;
	if (_access(argv[2], 6) == -1)
	{
		_mkdir(argv[2]);
	}
	for (int i = 1; i <= 0xe6; i++)
	{
		rom.seekg(0x66075c + 0x10 * (i - 1));
		unsigned int pMiniPortrait = 0;
		rom.seekg(4, ios::cur);
		rom.read((char*)&pMiniPortrait, sizeof(int));
		if (pMiniPortrait >= 0x8000000 && pMiniPortrait < 0xa000000)
		{
			unsigned int pPalette = 0;
			rom.read((char*)&pPalette, sizeof(int));
			if (pPalette >= 0x8000000 && pPalette < 0xa000000 && pPalette != pMiniPortrait)
			{
				pMiniPortrait -= 0x8000000;
				pPalette -= 0x8000000;

				unsigned short palette_gba[16];
				rom.seekg(pPalette);
				rom.read((char*)&palette_gba, sizeof(palette_gba));

				unsigned char image_gba[32 * 32 / 2];
				rom.seekg(pMiniPortrait);
				rom.read((char*)&image_gba, sizeof(image_gba));

				png_structp png_ptr = NULL;
				png_infop info_ptr = NULL;
				//png_color_8p palette = NULL;
				png_colorp palette = NULL;

				FILE *fp = NULL;
				stringstream ss;
				char filename[100];
				ss << argv[2] << "\\p" << hex << i << ".png";
				ss >> filename;
				cout << filename << endl;
				if (fopen_s(&fp, filename, "wb") != 0)
					goto image_error_handler;
				png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
				if (!png_ptr)
					goto image_error_handler;
				info_ptr = png_create_info_struct(png_ptr);
				if (!info_ptr)
				{
					png_destroy_write_struct(&png_ptr, NULL);
					goto image_error_handler;
				}
				int iRetVal = setjmp(png_jmpbuf(png_ptr));
				if (iRetVal)
				{
					fprintf(stderr, "错误码：%d\n", iRetVal);
					goto image_error_handler;
				}
				png_init_io(png_ptr, fp);
				//设置PNG文件头
				png_set_IHDR(png_ptr, info_ptr,
					32, 32,//尺寸
					//8,//颜色深度
					4,
					PNG_COLOR_TYPE_PALETTE,//颜色类型
					//PNG_COLOR_TYPE_RGBA,
					//PNG_COLOR_TYPE_RGB,
					PNG_INTERLACE_NONE,//不交错。PNG_INTERLACE_ADAM7表示这个PNG文件是交错格式。交错格式的PNG文件在网络传输的时候能以最快速度显示出图像的大致样子。
					PNG_COMPRESSION_TYPE_BASE,//压缩方式
					PNG_FILTER_TYPE_BASE);//这个不知道，总之填写PNG_FILTER_TYPE_BASE即可。
				//palette = (png_color_8p)png_malloc(png_ptr, 16 * (sizeof(png_color)));
				palette = (png_colorp)png_malloc(png_ptr, 16 * (sizeof(png_color)));
				for (int j = 0; j < 16; j++)
				{
					palette[j].red = (palette_gba[j] & 31) * 255 / 31;
					palette[j].green = ((palette_gba[j] >> 5) & 31) * 255 / 31;
					palette[j].blue = ((palette_gba[j] >> 10) & 31) * 255 / 31;
				}
				png_set_PLTE(png_ptr, info_ptr, palette, 16);
				//png_set_packing(png_ptr);
				png_byte TrnsColor[1];
				TrnsColor[0] = 0; //0 = transparent , 255 = opaque
				png_set_tRNS(png_ptr, info_ptr, TrnsColor, 1, NULL); // 特别注意透明色索引从1开始，而不是从0开始
				png_write_info(png_ptr, info_ptr);//写入文件头
				png_byte image[32 * 32 / 2];
				//png_byte image[32 * 32 * 3];
				for (int j = 0; j < sizeof(image_gba); j++)
				{
					// 特别注意gba像素排列是以tile为单位的，而不是以行为单位的
					int Ty = (j / 32) / 4;
					int Tx = (j / 32) % 4;
					int ty = (j % 32) / 4;
					int tx = (j % 32) % 4;
					int y = 8 * Ty + ty;
					int x1 = 8 * Tx + 2 * tx;
					//int x2 = x1 + 1;
					//int p1 = (y * 32 + x1) * 3;
					//int p2 = (y * 32 + x2) * 3;
					int p1 = y * 32 + x1;
					//int p2 = y * 32 + x2;
					image[p1 / 2] = ((image_gba[j] & 15) << 4) | ((image_gba[j] >> 4) & 15);
					//image[p1] = palette[image_gba[j] & 15].red;
					//image[p1 + 1] = palette[image_gba[j] & 15].green;
					//image[p1 + 2] = palette[image_gba[j] & 15].blue;
					//image[p2] = palette[(image_gba[j] >> 4) & 15].red;
					//image[p2 + 1] = palette[(image_gba[j] >> 4) & 15].green;
					//image[p2 + 2] = palette[(image_gba[j] >> 4) & 15].blue;
				}
				png_bytep row_pointers[32];
				for (int j = 0; j < 32; j++)
				{
					row_pointers[j] = image + j * 32 / 2;
					//row_pointers[j] = image + j * 32 * 3;
				}
				png_write_image(png_ptr, row_pointers);
				png_write_end(png_ptr, info_ptr);
				png_free(png_ptr, palette);
				palette = NULL;
				png_destroy_write_struct(&png_ptr, &info_ptr);
				fclose(fp);
			}
		}
		continue;
	image_error_handler:
		cerr << "fail to create image p" << hex << i << ".png" << endl;
		rom.close();
		return -3;
	}
	rom.close();
	return 1;
}
