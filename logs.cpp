#include "logs.h"
#include <vcl.h>
//#include "mytime.h"

#include <fstream.h>
#include <stdio.h>
extern AnsiString servtimestr;
int logh=-1;
int inilog(AnsiString name){
   fstream  fs(name.c_str(), ios_base::in | ios_base::out |ios_base::trunc);
   fs.close();
//   logh = FileCreate(name);
//   truncate(logh);
//   FileClose(logh);
   logh=FileOpen(name,fmOpenWrite+fmShareDenyNone	);
   FileSeek(logh,0,0);//
   return logh;
}

int inilogh(AnsiString name,bool append=true){
int logh;
   if (append){
        if(FileExists(name)){
           logh=FileOpen(name,fmOpenWrite+fmShareDenyNone	);
           FileSeek(logh,0,2);//перемотали на коенец
           return logh;
        }
        else{
          logh = FileCreate(name);
          FileClose(logh);

        }
   }else{
      if(FileExists(name)){
           logh=FileOpen(name,fmOpenWrite+fmShareDenyNone	);
           int pos=FileSeek(logh,0,2);//перемотали на коенец
           FileClose(logh);
           if(pos!=0){
            //  fstream  fs(name.c_str(), ios_base::in | ios_base::out |ios_base::trunc);
             logh = FileCreate(name);
             FileClose(logh);
             // fstream  fs(name.c_str(), ios_base::out |ios_base::trunc);
             // fs.close();
           }
      } else{
          logh = FileCreate(name);
          FileClose(logh);
       }


   }
   logh=FileOpen(name,fmOpenWrite+fmShareDenyNone	);
   FileSeek(logh,0,0);
   return logh;
}
void closelog(){
  FileClose(logh);
  logh=NULL;

}
void closelogh(int logh){
  FileClose(logh);
}
void addlog(const AnsiString& s){
   if(logh==-1)return;
   FileWrite(logh, s.c_str(), s.Length() );
}
void addlogh(int han,const AnsiString& s){
   if(han==-1)return;
   FileWrite(han, s.c_str(), s.Length() );
}
void addlogt(const TDateTime& time,const AnsiString& s){
  AnsiString s2;
   if(logh==-1)return;
   s2="\n"+time.DateTimeString()+" "+s;
   FileWrite(logh, s2.c_str(), s2.Length() );
}
/*
void addtstr(const AnsiString& s){
  AnsiString s2;
  TDateTime dat=TDateTime::CurrentDateTime();
   if(logh==-1)return;
   s2="\n"+dat.DateTimeString()+" "+s;
   int res=FileWrite(logh, s2.c_str(), s2.Length() );
}
*/
extern int servtime;
void addtstr(const AnsiString& s){
   if(logh==-1)return;

  AnsiString s2;
//   s2="\n"+timetostr(servtime)+" "+s;
   s2="\n"+servtimestr+" "+s;
   FileWrite(logh, s2.c_str(), s2.Length() );
  // FileFlush(logh);
}
void addtstrh(int h,const AnsiString& s){
   if(h==-1)return;

  AnsiString s2;
//   s2="\n"+timetostr(servtime)+" "+s;
   s2="\n"+servtimestr+" "+s;
   FileWrite(h, s2.c_str(), s2.Length() );
}
TCriticalSection *histcrsec=NULL;
//int hcrit=-1;
int inilogh_critical(AnsiString name,bool append=true){
  if(histcrsec==NULL)histcrsec=new TCriticalSection();
  int hcrit=inilogh(name,append);
  return hcrit;
}
void addtstrh_critical(int h,const AnsiString& s){
   if(h==-1)return;
  histcrsec->Enter();
  AnsiString s2;
//   s2="\n"+timetostr(servtime)+" "+s;
   s2="\n"+servtimestr+" "+s;
    histcrsec->Leave();
   FileWrite(h, s2.c_str(), s2.Length() );
}
void addname(const AnsiString& name, const AnsiString& s){
   if(logh==-1)return;

  AnsiString s2;
//   s2="\n"+timetostr(servtime)+" "+name+":"+s;
   s2="\n"+servtimestr+" "+name+":"+s;
   FileWrite(logh, s2.c_str(), s2.Length() );
}

