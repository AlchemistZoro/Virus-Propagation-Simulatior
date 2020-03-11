#ifndef XXXX_H
#define XXXX_H
#include<iostream>
#include<cmath>
#include<random>
#include<ctime>
using namespace std;
extern const int graph_l=640;
extern const  int graph_h=480;
extern int city[graph_h][graph_h] = {1};
extern const int cure ;
extern const int orig ;
extern const int shad;
extern const int affe ;
extern const double  broad_rate ;//传播率
class person
{
	friend void add_time();
public:
	
	person()
	{}
	void position(int a, int b)
	{
		x = a;
		y = b;	
	}
	void get_af_time(int a)
	{
		af_time = a;
	}
	void change_heal(int a)
	{
			heal = a;
		
	}
	void city_heal_person(int a)
	{
		if (a != orig)
		{
			if (rand_broad())
			{
				heal = shad;
			}
		}	
	}
	int x_pos()
	{
		return x;
	}
	int y_pos()
	{
		return y;
	}
	int person_heal()
	{
		return  heal;
	}
	int play_af_time()
	{
		return af_time;
	}
	bool rand_broad()
	{
		if (rand() % 10000 < broad_rate *broad_rate * 10000)
		{
			return 1;
		}
		return 0;
	}
	int under_cure = 0;//初始状态没有在医院
	int index=0;
private:
	int x ;
	int y ;
	int heal = orig;//被治愈=0 初始健康=1,潜伏期=2,患病=3;
	int af_time = 0;
};
#endif
