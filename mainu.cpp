//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "mainu.h"
#include "csvnoload.h"
#include "stdio.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
#define ast AnsiString
void alert(ast s){
   Form1->RichEdit1->Lines->Add(s);
}
//////////////////////////////////////////////////////////////////////////////////
int readhead(int h){
//////////////////////////////////////////////////////////////////////////////////

ast s;
if(h<=0){alert("Нк могу открыть входной файл, проверьте имя");
  return -2;
}


if(!getitem(h,&s) )goto errfl;
if(!getitem(h,&s) )goto errfl;
if(!getitem(h,&s) )goto errfl;
if(!getitem(h,&s) )goto errfl;
if(!getitem(h,&s) )goto errfl;
if(!getitem(h,&s) )goto errfl;

if(!getitem(h,&s) )goto errfl;//добавил для fastframe режима
return 0;
errfl:
alert("Ошибка формата входного файла, заголовок не читается");
return -1;
}
int h1,h2;
const ast zerostr="0,00000000e+000";
///////////////////////////////////////////////////////////////////////////////////
int getsample(int chnum,double * time,double *ch){
///////////////////////////////////////////////////////////////////////////////////
int h;
ast s;
    if(chnum==1)h=h1;
    if(chnum==2)h=h2;
    if(!getitem(h,&s) ){

      return -1;
    };
    if(s.Length()>36){
       alert("проблемы с форматом файла длина строки с измерениями >36 s="+s);
       return -2;
    }
    if(s[1]!=','){
       alert("проблемы с форматом файла первый символ не запятая s="+s);
       return -2;

    }
    ast timestr,chstr;
    timestr=s.SubString(4,16);
    int choffset=21;
    if(timestr[16]==','){
         timestr=timestr.SubString(1,15);
         choffset=20;
    }

    chstr=s.SubString(choffset,16);

    if(timestr==zerostr)*time=0;
    else *time=StrToFloat(timestr);
    if (chstr==zerostr)*ch=0;
    else *ch=StrToFloat(chstr);
   return 0;
}
enum toperation {rd=0,wr=1};
class tmdiofrm{
  public:
  int preamblelen;
  toperation operation;
  char adr_dev;
  char adr_reg;
  int data;
  double begtime;
  int begcnt;
  tmdiofrm();
  AnsiString sprint();
//  AnsiString makecsvline();
};
const ast oper_str[]={"RD","WR"};
/////////////////////////////////////////////////////////////
 AnsiString tmdiofrm::sprint(){
////////////////////////////////////////////////////////////
   ast s="\n"+IntToStr(begcnt);
   s+=": "+IntToStr(int(begtime*1e6))+" мкс длина преамбулы="+preamblelen;
   s+="   "+oper_str[operation]+" Adr="+ IntToStr(adr_dev)+":" +IntToStr(adr_reg)+" data="+IntToHex(data,4);
   return s;
}

/////////////////////////////////////////////////////////////
 tmdiofrm::tmdiofrm(){
////////////////////////////////////////////////////////////
 begcnt=-1;
 preamblelen=0;
 operation=rd;
 adr_dev=-1;
 adr_reg=-1;
 data=0;
 begtime=0;
}
class tmdioparser{
  public:
  tmdioparser();
  int state;
  int oldclk;
  int oldio;
  int preamcnt;
  int parse(bool clk,bool io,tmdiofrm* frm);
  int bit_cnt;//счетчик принятых битов в битовых полях
  bool need_falling;// ожидаем заднего фронта клока
};

///////////////////////////////////////////////////////////////////////////////////
 tmdioparser::tmdioparser(){
///////////////////////////////////////////////////////////////////////////////////
  state=0;
  oldclk=1;
  preamcnt=0;
  oldio=1;
  need_falling=false;
}

enum states{pream=0,st=1,op_code=2,op_code2=3,getadr_dev=4,getadr_reg=5,turn_a=6,turn_a2=7,getdata_st=8,after_read=9};
int cnt=0;
///////////////////////////////////////////////////////////////////////////////////
int tmdioparser::parse(bool clk,bool io,tmdiofrm * frm){
///////////////////////////////////////////////////////////////////////////////////
 int errc=0;
 if(clk==oldclk)goto endl;;//нет такта
 oldclk=clk;
// if(clk==1)goto endl;;//работаем по переднему фронту

// if(clk==0)goto endl;;//это задний  фронт
 if(need_falling){
    if(clk==1)goto endl;
 }else if(clk==0)goto endl;

 switch (state){
 case pream:
   if(io==1){
      if(preamcnt==100000)goto endl; //ограничение счетчика преамбулы
      preamcnt++;
      return 0;
   }else{
     frm->preamblelen=preamcnt;
     if(preamcnt<31){
       alert("Сбой автомата состояния преамбула<32 preamcnt="+IntToStr(preamcnt)+ " cnt="+cnt);
resetl:
       state=pream;
       preamcnt=0;
       need_falling=false;
       errc= -2;
       goto endl;
     }else{
       state=st;
       errc=2;//сигнал конец преамбулы
     }
   }
   goto endl;
 case st:
   if (io==0){
        alert("Сбой автомата состояния на st cnt="+IntToStr(cnt));
        goto resetl;
   }else{
     state=op_code;

     goto endl;
   }
 case op_code:
   state=op_code2;
   if(io) frm->operation=rd;
   else  frm->operation=wr;
   goto endl;
 case op_code2://подтверждение операции ( 10- rd, 01-wr)
   if(io)
        if (frm->operation==wr){
           state=getadr_dev;
           bit_cnt=4;
           frm->adr_dev=0;
        }else{
          alert("Сбой автомата состояния на op_code2 io="+IntToStr(io));
          goto resetl;
        }
    else if (frm->operation==rd){
           state=getadr_dev;
           bit_cnt=4;
           frm->adr_dev=0;
        }else{
          alert("Сбой автомата состояния на op_code2 io="+IntToStr(io));
          goto resetl;
        }
     goto endl;
 case getadr_dev:
      frm->adr_dev|=io<<bit_cnt;
      bit_cnt--;
      if(bit_cnt<0){
        state=getadr_reg;
        bit_cnt=4;
        frm->adr_reg=0;
      }
     goto endl;
 case getadr_reg:
      frm->adr_reg|=io<<bit_cnt;
      bit_cnt--;
      if(bit_cnt<0){
        state=turn_a;
      }
     goto endl;
 case turn_a://прием 1 на wr иначе просто переходим в сл. состояние

   if (io ||(frm->operation==rd)){
     state=turn_a2;
     if(frm->operation==rd) need_falling=true;
     goto endl;
   }
   alert("Сбой автомата состояния на turn_a");
   goto resetl;
 case turn_a2://прием 1
   if (!io){
     state=getdata_st;
     bit_cnt=15;
     frm->data =0;
     goto endl;
   }
   alert("Сбой автомата состояния на turn_a");
   goto resetl;

 case getdata_st:
      frm->data|=io<<bit_cnt;
      bit_cnt--;
      if(bit_cnt<0){
        if(frm->operation==wr){
                preamcnt=0;
                state=pream;
                need_falling=false;
                errc=1;//кадр принят
        }else state=after_read;
      }
     goto endl;
 case after_read://тупо пробрасываем бит перед включением
                preamcnt=0;
                state=pream;
                need_falling=false;
                errc=1;//кадр принят
   goto endl;
 default:
   alert("неизвестное состояние парсера");
   errc -1;
   goto endl;
 }
endl:
 oldio=io;
 return errc;

}
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
Timer1->Enabled=false;

int hout;
 // h = FileOpen(OpenDialog1->FileName, fmOpenRead);
 //     iFileLength = FileSeek(iFileHandle,0,2);
//      FileSeek(h1,0,0);
//  h1 = FileOpen("in1.csv", fmOpenRead);
//  h2 = FileOpen("in2.csv", fmOpenRead);
  h1 = (int)fopen("in1.csv", "r");
  h2 = (int)fopen("in2.csv", "r");
  alert("Поехали");
  ast s;
  if(readhead(h1) )return;
  if(readhead(h2) )return;
  double porog_ch1=1.5;
  double porog_ch2=1.5;
  //int cnt=0;
  bool endb=false;
  tmdiofrm frm;
  tmdioparser par;
  int oldcnt;
  while (!endb){
    cnt++;
    double time1,time2,ch1,ch2;
    if(getsample(1,&time1,&ch1)<0) break;
    if(getsample(2,&time2,&ch2)<0) {
      alert("ошибка getsample ch2");
      break;
    };
    if(time1!=time2){
       alert("time1!=time2");
       return;
    }
    bool d1,d2;
    d1=0;
    d2=0;
    if(ch1>porog_ch1)
           d1=1;
    if(ch2>porog_ch2)d2=1;
    int res=par.parse(d1,d2,&frm);
    switch (res){
    case 0:   //работаем
        break;
    case 1:   //принят кадр
        s=frm.sprint();
        RichEdit1->Lines->Add(s);

      break;
    case 2: //начало приема кадра фиксим метку времени
       frm.begcnt=cnt;
       frm.begtime=time1;
      break;
    default://ошибка парсинга
       alert("cnt="+IntToStr(cnt)+" time="+FloatToStr(time1));
    }

    if (oldcnt!=(cnt / 10000)) {
      oldcnt=cnt / 10000;
      pb->Position=cnt;
      Application->ProcessMessages();
    }
  }
//  if(!getitem(h1,&s) )goto errfl;
//  FileWrite()

//errfl:alert("Ошибка формата входного файла, работа остановлена");
//  alert("Обработка закончена");
  RichEdit1->PlainText = True;
  RichEdit1->Lines->SaveToFile("out.txt");
  Application->MessageBox("Обработка закончена","Обработка закончена",MB_OK);

}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
  RichEdit1->PlainText = True;
  RichEdit1->Lines->SaveToFile("out.txt");
}
//---------------------------------------------------------------------------
