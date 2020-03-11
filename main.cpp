#include <easyx.h>			// Reference graphics library header file
#include <conio.h>
#include <iostream>
#include<random>
#include<ctime>
#include<cmath>
#include<windows.h>
#include"person.h"
#include"print.h"
using namespace std;

const int person_size = 3000;//只要超过3000就会让显卡爆炸的城市总人口
const double u = 1;//只要调到0就万事大吉的人员流动平均意向  0~100%的移动概率
const int original_count = 5;//对实验结果几乎没有影响的初始感染者人数0~200
const int shadow_time =15;//完全克制医院属性的潜伏时间 0~100; 
extern const double  broad_rate =1;//高于0.8神仙也无法改变结果的传播率
const int receive_time = 2;//对实验结果没有卵影响的医院收治时间
const int  bed_count = 1000 ;//永远无法改变悲惨结果的医院床位数量

default_random_engine e; //引擎//随机数引擎的参数
int person_origin = person_size;
int shadow = 0;//并没有用过的潜伏期人数
int affect = 0;//感染人数
int hospital_count = 0;//医院里已有的病人
extern const int cure = 0;//从来没有被用过的治愈状态
extern const int orig = 1;//初始状态
extern const int shad = 2;//潜伏期状态
extern const int affe = 3;//被感染状态

person pe_po[person_size];
extern  const int graph_l ;
extern const int graph_h ;
extern int city[480][480] ;
 int ch_time = 0;
void set_person(person*, int);//set the people's location on graph
void print_person();
void move();//move each chanel
bool over();//make sure the affect is over.
bool check_pos(int,int);
void city_heal(const int x,const int y,int z);
void print_words();
void add_time();
void condition_change();
int  person_city_heal(int z);
bool rand_broad();
void print_hospital();
void hospital_condition();
void refresh_city();
int affe_per();
int main()
{
	for (int a = 0; a != person_size; a++)
	{
		pe_po[a].index = a;
	}
	srand(time(0));
	initgraph(640, 480);	// Create a graphics window with a size of 640x480 pixels
	set_person(pe_po, person_size);
	print_person();
	
	_getch();	// Press any key to continue

	for (int a = 0; a != original_count; a++)
	{		
		int x1 = 0;	
		do
		{
			x1 = rand()% person_size;
		} while (pe_po[x1].person_heal() == 0||check_pos(pe_po[x1].x_pos(), pe_po[x1].y_pos()));
		pe_po[x1].change_heal(shad);
		city_heal(pe_po[x1].x_pos(), pe_po[x1].y_pos(),x1);//初始化程序的状态
	}

	print_person();  
	settextcolor(WHITE);

	BeginBatchDraw();//批量绘图的方法
	while (over()) 
	{
		_getch();
		move();
		refresh_city();
		add_time();
		condition_change();
		print_person();			
		print_words();
		if (hospital_count < bed_count)
		{
			hospital_condition();
		}
		print_hospital();
		FlushBatchDraw();
	}
	EndBatchDraw();

	_getch();
	closegraph();			// Close the graphics window
	return 0;
}


void set_person(person a[],int b)
{
	normal_distribution<double> n(240, 80); //均值, 方差
	int x = 0;
	int y = 0;
	for (int c = 0; c != b; c++)
	{	
		do {
			y = lround(n(e));
			x = lround(n(e));	
		} while (check_pos(x,y));
		
			a[c].position(x, y);			
	}
}


void print_person()
{
	cleardevice();	
	for (auto &a:pe_po)
	{		
		if (a.under_cure == 0)
		{
			switch (a.person_heal())
			{
			case cure:setfillcolor(GREEN); setlinecolor(GREEN); break;
			case orig:setfillcolor(WHITE); setlinecolor(WHITE); break;
			case shad:setfillcolor(YELLOW); setlinecolor(YELLOW); break;
			case affe:setfillcolor(RED); setlinecolor(RED); break;
			}
			fillcircle(a.x_pos(), a.y_pos(), 1);
		}
	}
}


void move()
{
	int x1 = 0;
	int y1 = 0;
	for (auto& a : pe_po)
	{	
		if (rand()%10000<u*u*10000)
		{
			x1 = rand() % 9 - 4 + a.x_pos();
			y1 = rand() % 9 - 4+ a.y_pos();
			if (!check_pos(x1, y1))
			{
				a.position(x1, y1);
			}
		}
		
	}
	for (auto& a : pe_po)
	{
		city_heal(a.x_pos(), a.y_pos(), a.index);
	}
	for (auto& a : pe_po)
	{
		a.city_heal_person(city[a.x_pos()][a.y_pos()]);
	}
}


bool over()
{
	for (auto a : pe_po)
	{		
		if (a.person_heal() != affe)//if one person are not affected
			return 1;
	}
	return 0;
}


bool check_pos(int x, int y)
{	
	if ((x - graph_h / 2) * (x - graph_h / 2) + (y - graph_h / 2) * (y - graph_h / 2) > graph_h* graph_h / 4)
		return 1;
return 0;
}


void city_heal(const int x,const int y,int z)
{
	if (city[x][y] < pe_po[z].person_heal())
		city[x][y] = pe_po[z].person_heal();
}


void print_words()
{        
	TCHAR s[10];//储存天数
	settextcolor(WHITE);
	sprintf_s(s, _T("%d"), ch_time++);	// 高版本 VC 推荐使用 _stprintf_s 函数
	outtextxy(580, 60, s);
	sprintf_s(s, _T("%d"), person_size);
	outtextxy(580, 100, s);
	sprintf_s(s, _T("%d"), affe_per()- hospital_count);
	outtextxy(580, 140, s);
	outtextxy(444, 60, "病毒传播天数(天):");
	outtextxy(460, 100, "城市总人数(人):");
	outtextxy(492, 140, "已感染(人):");
	


	        
}


void add_time()
{
	for (auto& a : pe_po)
	{
		if (a.person_heal() == shad|| a.person_heal()==affe)
			++a.af_time;
	}
}

void condition_change()
{
	for (auto& a : pe_po)
	{
		if (a.play_af_time() > shadow_time)
			a.change_heal(affe);
	}
}

void print_hospital()
{
	setfillcolor(WHITE);
	rectangle(540, 200, 570, 450);
	if (hospital_count <= bed_count)
	{
		setfillcolor(WHITE);
		if (bed_count == hospital_count) {
			setfillcolor(RED); setlinecolor(RED); settextcolor(RED);
		}
		TCHAR x[10];
		fillrectangle(540, 450 - 250 * static_cast<double>(hospital_count) / bed_count, 570, 450);
		outtextxy(474, 180, "剩余床位(个):");
		sprintf_s(x, _T("%d"), bed_count - hospital_count);
		outtextxy(580, 180, x);
	}
}

void hospital_condition()
{
	
	for (auto& a : pe_po)
	{
		if (hospital_count < bed_count)
		{
			if (a.play_af_time() > shadow_time + receive_time && a.under_cure == 0)
			{
				a.under_cure = 1;
				hospital_count++;
				a.change_heal(0);
			}
		}
	}

}

void refresh_city()
{
	for (auto& a : city)
	{
		for (auto& b :a)
		{
			b = 0;
		}
	}
}

int affe_per()
{
	int a = 0;
	for (auto &b : pe_po)
	{
		if (b.person_heal()>orig)
			a++;
	}
	return a;
}
