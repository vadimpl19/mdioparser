#ifndef logsH
#define logsH

#include <SyncObjs.hpp>
int inilog(AnsiString name);
int inilogh(AnsiString name,bool append);
void closelog();
void closelogh(int h);
void addlogt(const TDateTime& time,const AnsiString& s);
void addlog(const AnsiString& s);
void addlogh(int handle,const AnsiString& s);
void addtstr(const AnsiString& s);
void addtstrh(int h,const AnsiString& s);
void addname(const AnsiString& name,const AnsiString& s);
extern TCriticalSection * histcrsec;
int inilogh_critical(AnsiString name,bool append);
void addtstrh_critical(int h,const AnsiString& s);
#endif
