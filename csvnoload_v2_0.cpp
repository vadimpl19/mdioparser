//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "csvnoload_v2_0.h"
#include <stdio.h>
//#include <vcl.h>;
char delim=';';
int cntsym=0;
bool getitem(int h,AnsiString * s){
//  char buf[4];
// *s="";
// int is=i;
 char ch=0x2;
 *s="";
//  int i=FileRead(h,&ch,1);
 int i=fread(&ch,1,1,(FILE*)h);
 if(i<=0)
    return false;

 while ((ch!=delim)&&(ch!=10)){
  cntsym++;
  if (ch!=13) *s+=ch;
//  int i=FileRead(h,&ch,1);
  int i=fread(&ch,1,1,(FILE*)h);
//  ch=buf[0];
  if(i<=0)
     
     return false;

 }
 return true;
};
bool skipitem(int h){
 char ch=0x1;
 while ((ch!=delim)&&(ch!=10)){
  int i=FileRead(h,&ch,1);
  if(i==0)return false;
//  if (ch!=13) *s+=ch;
 }
 return true;
};
bool getdoublecsv(int h,double *d){
 AnsiString s;
 if(!getitem(h,&s))return false;
 *d=StrToFloat(s);
 return true;
}
bool getintcsv(int h,int *d){
 AnsiString s;
 if(!getitem(h,&s))return false;
 *d=StrToInt(s);
 return true;
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
