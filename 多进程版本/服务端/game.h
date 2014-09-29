#include <iostream>
#include <algorithm>
#include <ctime>
#include <string>
#include <vector>
#include <stdio.h>

using namespace std;

#define rep(i,x,y) for(int i=x;i<=y;i++)
#define DEAL_NUM 17 //发牌的时候每个人的牌数限定是17张
#define MAXLEN  1024 //客户端接受的字符串信息的长度

struct poke{
	int flw, num;
}p[55];
int index(int flw, int num){
	if(flw==4)	return 52+num;
	if(num>=3)
		return (num-3)*4+flw+1;
	if(num<3)//可省
		return (10+num)*4+flw+1;
}
bool cmp(const poke& a, const poke& b){
	return index(a.flw, a.num) < index(b.flw, b.num);
}
/*index 和 cmp用来排序*/
//牌面说明：H红桃，S黑桃，C梅花，D方片，JKR大王，jkr小王


string s[55]={
"",
"H.A", "H.2", "H.3", "H.4", "H.5", "H.6", "H.7", "H.8", "H.9", "H.10", "H.J", "H.Q", "H.K",
"S.A", "S.2", "S.3", "S.4", "S.5", "S.6", "S.7", "S.8", "S.9", "S.10", "S.J", "S.Q", "S.K",
"C.A", "C.2", "C.3", "C.4", "C.5", "C.6", "C.7", "C.8", "C.9", "C.10", "C.J", "C.Q", "C.K",
"D.A", "D.2", "D.3", "D.4", "D.5", "D.6", "D.7", "D.8", "D.9", "D.10", "D.J", "D.Q", "D.K",
"j.k", "J.K" };
//牌号下标从1开始，花色从0开始
//为了统一长度，我把jocker写成jkr
/*s用来输出*/

//初始化牌函数
void InitCards(){
	rep(i,0,3){
		rep(j,1,13){
			int id=i*13+j;
			p[id].flw = i;
			p[id].num = j;
		}
	}
	p[53].flw=4;p[53].num=1;//小王
	p[54].flw=4;p[54].num=2;//大王
}


int a[55];//hash


//洗牌函数
void ShuffleCards(){
	srand(time(0));
	rep(i,1,54)	a[i] = i;
	for(int i=54;i>=1;i--)
		swap(a[i],a[rand()%i+1]);
}


//获取牌函数，参数为：花色，大小
string getCards(int flw, int num){//H.7的flw=0, num=7
	return s[flw*13+num];
}

//发牌函数
void dealCards(vector<poke>* player){
	rep(id, 0, 2){
		player[id].clear();
		rep(i, DEAL_NUM*id+1, DEAL_NUM*(id+1))
			player[id].push_back(p[a[i]]);
	}
}

//牌进行排序函数
void sortCards(vector<poke>& player){
	sort( player.begin(), player.end(), cmp );//给p1的牌排序
}

//返回手牌字符串函数s
string getCardList(vector<poke>& player)
{
    string cardList="";
    for(int i=0;i<player.size();i++)
    {
        cardList+=getCards( player[i].flw, player[i].num);
        if(i==player.size()-1)
            continue;
        if(player[i].num==10)//牌的间距是3格，有10的话间距会变成2格
            cardList+="  ";
        else cardList+="   ";
    }
    return cardList;

}


//输出手牌函数
void printCards(vector<poke>& player){
	for(int i=0; i<player.size(); i++){
		cout<<getCards( player[i].flw, player[i].num );
		if(i==player.size()-1)
			continue;
		if(player[i].num==10)//牌的间距是3格，有10的话间距会变成2格
			printf("  ");
		else printf("   ");
	}
	puts("");
	for(int i=0; i<player.size(); i++){
		printf(" %d",i+1);
		if(i==player.size()-1)	continue;
		if(i>=9)
			printf("   ");
		else
			printf("    ");
	}
	//按道理，下面要有1,2,3,4...n来提示用户，暂时不写
}


// /*以下是判牌操作*/

//检测是否为数字
bool isNumeric(char x){
	return x<='9' && x>='0';
}

//
void readGo(char* buf, vector<int>& v){//将指令转化为数组
	v.clear();//先清空
	int i=0;
	while(buf[i]!='\0'){
		if(!isNumeric(buf[i])){
			i++;
			continue;
		}
		int x=buf[i]-'0';
		if(isNumeric(buf[i+1])){
			x = x*10+buf[i+1]-'0';
			v.push_back(x);
			i+=2;
		}
		else{
			v.push_back(x);
			i++;
		}
	}
}



void getGo(const vector<int>& v, const vector<poke>& player, vector<poke>& GoCards){//取得玩家的手牌，放在GoCards数组中
	for(int i=0; i<v.size(); i++){
		GoCards.push_back(player[v[i]-1]);
	}
}
/*读取指令*/

//是单张函数
bool isSingle(vector<poke>& Go){  //单张
	return Go.size()==1;
}


//是对子函数
bool isDouble(vector<poke>& Go)   //一对
{
    if(Go.size()!=2)
        return 0;
    return Go[0].num==Go[1].num;
}


//三张返回1，三带一返回2，否则为0
int isTriple(vector<poke>& Go){   //三的话返回1，三带一就返回2，什么都不是的返回0  (能不能出三张二？)
 	if(Go.size()==4)
    {
        int same=0,i=1;
        while( i<Go.size() && Go[i].num==Go[i-1].num)
        {
            same++;
            i++;
        }
        if(same!=0&&same!=2)
            return 0;
        if(same==0)
        {
            if(Go[1].num==2)   //三张带一张不能出现三张2
                return 0;
            if(Go[1].num==Go[2].num && Go[2].num==Go[3].num)
                return 2;
            else
                return 0;
        }
        if(Go[1].num!=2)  //三张带一张不能出现三张2
            return 2;
        else
            return 0;

    }
    else
        return  Go[0].num==Go[1].num && Go[1].num==Go[2].num;
 }//为了精简代码，我不准备把三和三带一分成两个函数写



//是三带二返回true，否则false
bool isTriple_Two(vector<poke>& Go) //三张带一对
{
    if(Go.size()!=5)
       return 0;
    int i=1;
    int same=0;
    while(i<Go.size() && Go[i].num==Go[i-1].num)
    {
        same++;
        i++;
    }
    if(same!=1 && same!=2)
        return 0;
    if((same==2 && Go[3].num==Go[4].num && Go[2].num!=2)||(same==1 && Go[2].num==Go[3].num && Go[3].num==Go[4].num && Go[2].num!=2))
        return 1;
    else
        return 0;
}

//顺子
bool isSequence(vector<poke>& Go) //顺子
{
	for(int i=1; i<Go.size(); i++)
    {
		if((Go[i].num-Go[i-1].num)!=1 ||Go[i-1].num==2||Go[i-1].flw==4) //顺子里不能有2和王
			return 0;
	}
	return 1;
}

//连对
bool isDsequence(vector<poke>& Go) //连对
{
    if(Go.size()<6||(Go.size()%2)!=0)
        return 0;
    for(int i=0,same=0;i<Go.size();i++)
    {
        if(Go[i].flw==4||Go[i].num==2) //连对中不会出现2和王
            return 0;
        same=0;
        while(i<Go.size()-1 && Go[i].num==Go[i+1].num )
        {
            same++;
            i++;
        }
        if(same!=1)
            return 0;
        if(i!=0&& i!=1 &&(Go[i].num-Go[i-2].num)!=1)
            return 0;
    }
    return 1;
}


//三带一的顺子
bool isTriple_Ssequence(vector<poke>& Go) //三张带一张的顺子
{
    if(Go.size()<=4||Go.size()%4!=0)
        return 0;
    int same=0;
    int single=0;
    int triple=0;
    for(int i=0;i<Go.size();i++)
    {
        same=0;
        while(i<Go.size()-1 && Go[i].num==Go[i+1].num)
        {
            same++;
            i++;
        }
        if(same!=0&&same!=1&&same!=2)
            return 0;
        if(same==0)
            single++;
        if(same==1)
            single+=2;
        if(same==2)
        {
            if(Go[i].num==2)  //三张带一张不能出现三张2
                return 0;
            else
                triple++;
        }

    }
    return single==triple;
}

//三带二的顺子
bool isTriple_Dsequence(vector<poke>& Go) // 三张带一对的顺子
{
    if(Go.size()<=5||Go.size()%5!=0)
        return 0;
    int same=0;
    int couple=0;
    int triple=0;
    for(int i=0;i<Go.size();i++)
    {
        same=0;
        while(i<Go.size()-1 && Go[i].num==Go[i+1].num)
        {
            same++;
            i++;
        }
        if(same!=1&&same!=2)
            return 0;
        if(same==1)
            couple++;
        if(same==2)
            triple++;
    }
    return couple==triple;
}


//四带二
bool isFour_Two_Differ(vector<poke>& Go) //四张带两张
{
    if(Go.size()!=6)
        return 0;

    int same=0;
    for(int i=0;i<Go.size();i++)
    {
        same=0;
        while( i<Go.size()-1 && Go[i].num==Go[i+1].num)
        {
            same++;
            i++;
        }
        if(same==3)
            return true;
    }
    return false;
}


//四带两对
bool isFour_Two_Same(vector<poke>& Go) //四张带两对
{
    if(Go.size()!=8)
        return 0;
    int couple=0;
    bool flag=false;
    for(int i=0,same=0;i<Go.size();i++)
    {
        same=0;
        while(i<Go.size()-1 && Go[i].num==Go[i+1].num )
        {
            same++;
            i++;
        }
        if(same!=1 && same!=3)
            return 0;
        if(same==3)
            flag=true;
        if(same==1)
            couple++;
    }
    return couple==2 && flag;
}


//炸弹
bool isBomb(vector<poke>& Go) //炸弹
{
    if(Go.size()!=4)
        return 0;
    return Go[0].num==Go[1].num && Go[1].num==Go[2].num && Go[2].num==Go[3].num;
}


//火箭
bool isRocket(vector<poke>& Go) //火箭
{
    if(Go.size()!=2)
        return 0;
    return Go[0].flw==4 && Go[1].flw==4;
}
