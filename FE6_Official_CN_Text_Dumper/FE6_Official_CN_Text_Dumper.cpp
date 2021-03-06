// FE6_Official_CN_Text_Dumper.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// by laqieer

#include "pch.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		string s = argv[0];
		int pos = s.find_last_of('\\');
		s = s.substr(pos + 1);
		cerr << "Usage: " << s << " ROM Text\n";
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
	text.open(argv[2], ios::out | ios::trunc);
	if (!text)
	{
		cerr << "fail to open the text: " << argv[2] << endl;
		rom.close();
		return -3;
	}
	char *extension = strrchr(argv[2], '.');
	if (strcmp(extension, ".md") == 0)
	{
		text << "# 火纹战记封印之剑剧情文本";
	}
	else
	{
		text << "火纹战记 封印之剑 文本" << endl;
	}
	rom.seekg(0x6dc);
	unsigned int pBase = 0;
	rom.read((char*)&pBase, sizeof(int));
	pBase -= 0x8000000;
	unsigned int pSlot = pBase + 8;

	// 对话头像
	int pFarLeft = 0;
	int pMidLeft = 0;
	int pLeft = 0;
	int pRight = 0;
	int pMidRight = 0;
	int pFarRight = 0;
	int pFarFarLeft = 0;
	int pFarFarRight = 0;
	int pCurrent = 0;

	for (int i = 1; ; i++, pSlot += 4)
	{
		unsigned int pText = 0;

		rom.seekg(pSlot);
		rom.read((char*)&pText, sizeof(int));
		if ((pText >= 0x2000000 && pText <= 0x203FFFF) || (pText >= 0x3000000 && pText <= 0x3007FFF))
		{
			continue;
		}
		else
		{
			if (pText < 0x8000000 || pText >= 0xA000000)
			{
				//cout << "End at " << hex << i << ": " << pSlot << " " << hex << pText << endl;
				break;
			}
		}
		if (strcmp(extension, ".md") == 0)
		{
			text << endl << endl << "## " << hex << i << endl;
		}
		else
		{
			text << endl << hex << i << ":" << endl;
		}
		pText -= 0x8000000;
		rom.seekg(pText);
		int len = 0;
		for (int j = 0; j < 10000; j++)
		{
			char c = 0;
			rom.read(&c, 1);
			if (c == 0)
			{
				len = j;
				break;
			}
		}
		if (len > 0)
		{
			char *txt = new char[len];
			//text << "len = " << len << endl;
			rom.seekg(pText);
			rom.read(txt, len);
			if (strcmp(extension, ".md") == 0)
			{
				bool red = false; // 处于红色文本模式

				for (int j = 0; j < len; j++)
				{
					// ascii
					if ((unsigned char)txt[j] >= 0x20 && (unsigned char)txt[j] <= 0x7F)
					{
						text << txt[j];
						continue;
					}
					
					// gb2312
					if ((unsigned char)txt[j] >= 0xA1 && (unsigned char)txt[j] <= 0xFE)
					{
						char hanzi[3];
						hanzi[0] = txt[j];
						j++;
						hanzi[1] = txt[j];
						hanzi[2] = '\0';
						text << hanzi;
						continue;
					}

					switch ((unsigned char)txt[j])
					{
					case 0x80:
						j++;
						switch ((unsigned char)txt[j])
						{
						case 4:
							// FE6里似乎不是[LoadOverworldFaces]
							//j++;
							//text << "  " << endl << "![头像" << hex  << (int)txt[j] << "](img/p" << hex << (int)txt[j] << ".png) ";
							break;
						case 5:
							text << "___(当前军资金数)";
							break;
						case 0xa:
							pFarLeft = pCurrent;
							text << "  " << endl << "![头像" << hex << pCurrent << "](img/p" << hex << pCurrent << ".png) ";
							break;
						case 0xb:
							pMidLeft = pCurrent;
							text << "  " << endl << "![头像" << hex << pCurrent << "](img/p" << hex << pCurrent << ".png) ";
							break;
						case 0xc:
							pLeft = pCurrent;
							text << "  " << endl << "![头像" << hex << pCurrent << "](img/p" << hex << pCurrent << ".png) ";
							break;
						case 0xd:
							pRight = pCurrent;
							text << "  " << endl << "![头像" << hex << pCurrent << "](img/p" << hex << pCurrent << ".png) ";
							break;
						case 0xe:
							pMidRight = pCurrent;
							text << "  " << endl << "![头像" << hex << pCurrent << "](img/p" << hex << pCurrent << ".png) ";
							break;
						case 0xf:
							pFarRight = pCurrent;
							text << "  " << endl << "![头像" << hex << pCurrent << "](img/p" << hex << pCurrent << ".png) ";
							break;
						case 0x10:
							pFarFarLeft = pCurrent;
							text << "  " << endl << "![头像" << hex << pCurrent << "](img/p" << hex << pCurrent << ".png) ";
							break;
						case 0x11:
							pFarFarRight = pCurrent;
							text << "  " << endl << "![头像" << hex << pCurrent << "](img/p" << hex << pCurrent << ".png) ";
							break;
						case 0x20:
							text << "___(军师名字)";
							break;
						case 0x21:
							if (red)
							{
								text << "</font>";
								red = false;
							}
							else
							{
								text << "<font color=red>";
								red = true;
							}
							break;
						case 0x22:
							text << "___(物品名字)";
							break;
						case 0x23:
							// FE6中没有用到[SetName]，故省略
							break;
						case 0x25:
							// FE6中没有用到[ToggleColorInvert]，故省略
							break;

						default:
							break;
						}
						break;

					case 0x18:
						text << "___(选项：是/否，默认：是)";
						break;

					case 0x19:
						text << "___(选项：是/否，默认：否)";
						break;

					case 0x1a:
						text << "___(选项：买进/卖出)";
						break;

					case 0x1b:
						text << "___(选项：留在/离开商店)";
						break;

					case 8:
						if ((unsigned char)txt[j + 1] == 0x80 && (unsigned char)txt[j + 2] >= 0xa && (unsigned char)txt[j + 2] <= 0x11)
						{
							pCurrent = pFarLeft;
							pFarLeft = 0;
							break;
						}
						switch (txt[j + 1])
						{
						case 0x11:
							pFarLeft = 0;
							j++;
							break;
						case 0x10:
							pFarLeft = txt[j + 2];
							pFarLeft &= 0xff;
							j += 3;
						default:
							pCurrent = pFarLeft;
							if((unsigned char)txt[j + 1] < 8 || (unsigned char)txt[j + 1] > 0x11)
								text << "  " << endl << "![头像" << hex << pFarLeft << "](img/p" << hex << pFarLeft << ".png) ";
						}
						break;

					case 9:
						if ((unsigned char)txt[j + 1] == 0x80 && (unsigned char)txt[j + 2] >= 0xa && (unsigned char)txt[j + 2] <= 0x11)
						{
							pCurrent = pMidLeft;
							pMidLeft = 0;
							break;
						}
						switch (txt[j + 1])
						{
						case 0x11:
							pMidLeft = 0;
							j++;
							break;
						case 0x10:
							pMidLeft = txt[j + 2];
							pMidLeft &= 0xff;
							j += 3;
						default:
							pCurrent = pMidLeft;
							if ((unsigned char)txt[j + 1] < 8 || (unsigned char)txt[j + 1] > 0x11)
								text << "  " << endl << "![头像" << hex << pMidLeft << "](img/p" << hex << pMidLeft << ".png) ";
						}
						break;

					case 0xa:
						if ((unsigned char)txt[j + 1] == 0x80 && (unsigned char)txt[j + 2] >= 0xa && (unsigned char)txt[j + 2] <= 0x11)
						{
							pCurrent = pLeft;
							pLeft = 0;
							break;
						}
						switch (txt[j + 1])
						{
						case 0x11:
							pLeft = 0;
							j++;
							break;
						case 0x10:
							pLeft = txt[j + 2];
							pLeft &= 0xff;
							j += 3;
						default:
							pCurrent = pLeft;
							if ((unsigned char)txt[j + 1] < 8 || (unsigned char)txt[j + 1] > 0x11)
								text << "  " << endl << "![头像" << hex << pLeft << "](img/p" << hex << pLeft << ".png) ";
						}
						break;

					case 0xb:
						if ((unsigned char)txt[j + 1] == 0x80 && (unsigned char)txt[j + 2] >= 0xa && (unsigned char)txt[j + 2] <= 0x11)
						{
							pCurrent = pRight;
							pRight = 0;
							break;
						}
						switch (txt[j + 1])
						{
						case 0x11:
							pRight = 0;
							j++;
							break;
						case 0x10:
							pRight = txt[j + 2];
							pRight &= 0xff;
							j += 3;
						default:
							pCurrent = pRight;
							if ((unsigned char)txt[j + 1] < 8 || (unsigned char)txt[j + 1] > 0x11)
								text << "  " << endl << "![头像" << hex << pRight << "](img/p" << hex << pRight << ".png) ";
						}
						break;

					case 0xc:
						if ((unsigned char)txt[j + 1] == 0x80 && (unsigned char)txt[j + 2] >= 0xa && (unsigned char)txt[j + 2] <= 0x11)
						{
							pCurrent = pMidRight;
							pMidRight = 0;
							break;
						}
						switch (txt[j + 1])
						{
						case 0x11:
							pMidRight = 0;
							j++;
							break;
						case 0x10:
							pMidRight = txt[j + 2];
							pMidRight &= 0xff;
							j += 3;
						default:
							pCurrent = pMidRight;
							if ((unsigned char)txt[j + 1] < 8 || (unsigned char)txt[j + 1] > 0x11)
								text << "  " << endl << "![头像" << hex << pMidRight << "](img/p" << hex << pMidRight << ".png) ";
						}
						break;

					case 0xd:
						if ((unsigned char)txt[j + 1] == 0x80 && (unsigned char)txt[j + 2] >= 0xa && (unsigned char)txt[j + 2] <= 0x11)
						{
							pCurrent = pFarRight;
							pFarRight = 0;
							break;
						}
						switch (txt[j + 1])
						{
						case 0x11:
							pFarRight = 0;
							j++;
							break;
						case 0x10:
							pFarRight = txt[j + 2];
							pFarRight &= 0xff;
							j += 3;
						default:
							pCurrent = pFarRight;
							if ((unsigned char)txt[j + 1] < 8 || (unsigned char)txt[j + 1] > 0x11)
								text << "  " << endl << "![头像" << hex << pFarRight << "](img/p" << hex << pFarRight << ".png) ";
						}
						break;

					case 0xe:
						if ((unsigned char)txt[j + 1] == 0x80 && (unsigned char)txt[j + 2] >= 0xa && (unsigned char)txt[j + 2] <= 0x11)
						{
							pCurrent = pFarFarLeft;
							pFarFarLeft = 0;
							break;
						}
						switch (txt[j + 1])
						{
						case 0x11:
							pFarFarLeft = 0;
							j++;
							break;
						case 0x10:
							pFarRight = txt[j + 2];
							pFarRight &= 0xff;
							j += 3;
						default:
							pCurrent = pFarFarLeft;
							if ((unsigned char)txt[j + 1] < 8 || (unsigned char)txt[j + 1] > 0x11)
								text << "  " << endl << "![头像" << hex << pFarFarLeft << "](img/p" << hex << pFarFarLeft << ".png) ";
						}
						break;

					case 0xf:
						if ((unsigned char)txt[j + 1] == 0x80 && (unsigned char)txt[j + 2] >= 0xa && (unsigned char)txt[j + 2] <= 0x11)
						{
							pCurrent = pFarFarRight;
							pFarFarLeft = 0;
							break;
						}
						switch (txt[j + 1])
						{
						case 0x11:
							pFarFarRight = 0;
							j++;
							break;
						case 0x10:
							pFarFarRight = txt[j + 2];
							pFarFarRight &= 0xff;
							j += 3;
						default:
							pCurrent = pFarFarRight;
							if ((unsigned char)txt[j + 1] < 8 || (unsigned char)txt[j + 1] > 0x11)
								text << "  " << endl << "![头像" << hex << pFarFarRight << "](img/p" << hex << pFarFarRight << ".png) ";
						}
						break;

					case 0x10:
						pCurrent = txt[j + 1];
						pCurrent &= 0xff;
						j += 2;
						text << "  " << endl << "![头像" << hex << pCurrent << "](img/p" << hex << pCurrent << ".png) ";
						break;

					case 0x11:
						pCurrent = 0;
						break;

					default:
						break;
					}
				}
			}
			else
			{
				text << txt << endl;
			}
			delete[] txt;
		}
	}
	rom.close();
	text.close();
	return 1;
}
