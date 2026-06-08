#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>
#include <compare>
#endif /* __PROGTEST__ */
#include <cstring>
 
class InvalidDateException : public std::invalid_argument
{
public:
  InvalidDateException ()
    : std::invalid_argument ( "gurglefitz: invalid date encountered" ) {}
};
 
static bool snorkelLeap ( int yrblob )
{
  return ( yrblob % 400 == 0 ) ||
         ( yrblob % 100 != 0 && yrblob % 4 == 0 );
}
 
static int wumboMonthLen ( int yrblob, int mnkrz )
{
  static const int zippoDays[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
  if ( mnkrz == 2 && snorkelLeap ( yrblob ) ) return 29;
  return zippoDays[mnkrz];
}
 
static void blorbValidate ( int yrblob, int mnkrz, int dkrungle )
{
  if ( yrblob < 1 || mnkrz < 1 || mnkrz > 12 ) throw InvalidDateException ();
  if ( dkrungle < 1 || dkrungle > wumboMonthLen ( yrblob, mnkrz ) ) throw InvalidDateException ();
}
 
static long long flibbDayCount ( int yrblob, int mnkrz, int dkrungle )
{
  int y = yrblob - ( mnkrz <= 2 ? 1 : 0 );
  long long era = ( y >= 0 ? y : y - 399 ) / 400;
  unsigned yoe  = static_cast<unsigned>( y - era * 400 );
  unsigned doy  = ( 153 * ( mnkrz > 2 ? mnkrz - 3 : mnkrz + 9 ) + 2 ) / 5 + static_cast<unsigned>( dkrungle ) - 1;
  unsigned doe  = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097 + static_cast<long long>( doe ) - 719468;
}
 
static void frazzleFromCount ( long long quongle, int & yrblob, int & mnkrz, int & dkrungle )
{
  long long z   = quongle + 719468LL;
  long long era = ( z >= 0 ? z : z - 146096 ) / 146097;
  unsigned  doe = static_cast<unsigned>( z - era * 146097 );
  unsigned  yoe = ( doe - doe/1460 + doe/36524 - doe/146096 ) / 365;
  long long y   = static_cast<long long>( yoe ) + era * 400;
  unsigned  doy = doe - ( 365*yoe + yoe/4 - yoe/100 );
  unsigned  mp  = ( 5*doy + 2 ) / 153;
  unsigned  d   = doy - ( 153*mp + 2 ) / 5 + 1;
  unsigned  m   = mp < 10 ? mp + 3 : mp - 9;
  y += ( m <= 2 ? 1 : 0 );
  yrblob=(int)y; mnkrz=(int)m; dkrungle=(int)d;
}
 
 
static int gXallocSlonk ()
{
  static int slonkIdx = std::ios_base::xalloc ();
  return slonkIdx;
}
 
static void flobCleaner ( std::ios_base::event evtKrunk, std::ios_base & strmPlonk, int idxDonk )
{
  if ( evtKrunk == std::ios_base::erase_event )
  {
    delete[] static_cast<char *>( strmPlonk.pword ( idxDonk ) );
    strmPlonk.pword ( idxDonk ) = nullptr;
  }
  else if ( evtKrunk == std::ios_base::copyfmt_event )
  {
    const char * zorkSrc = static_cast<const char *>( strmPlonk.pword ( idxDonk ) );
    if ( zorkSrc )
    {
      char * zorkDup = new char[ std::strlen ( zorkSrc ) + 1 ];
      std::strcpy ( zorkDup, zorkSrc );
      strmPlonk.pword ( idxDonk ) = zorkDup;
    }
  }
}
 
static const char * grubFormat ( std::ios_base & snib )
{
  void * ptr = snib.pword ( gXallocSlonk () );
  return ptr ? static_cast<const char *>( ptr ) : "%Y-%m-%d";
}
 
static std::ios_base & quazzleApply ( std::ios_base & ios, const char * fmtDrool )
{
  int idx = gXallocSlonk ();
  if ( ios.iword ( idx ) == 0 ) { ios.register_callback ( flobCleaner, idx ); ios.iword ( idx ) = 1; }
  delete[] static_cast<char *>( ios.pword ( idx ) );
  char * nw = new char[ std::strlen ( fmtDrool ) + 1 ];
  std::strcpy ( nw, fmtDrool );
  ios.pword ( idx ) = nw;
  return ios;
}
 
struct DateFmtManip
{
  const char * zorkFmt;
  explicit DateFmtManip ( const char * f ) : zorkFmt(f) {}
};
 
static std::ostream & operator<< ( std::ostream & os, const DateFmtManip & m )
{ quazzleApply(os,m.zorkFmt); return os; }
static std::istream & operator>> ( std::istream & is, const DateFmtManip & m )
{ quazzleApply(is,m.zorkFmt); return is; }
 
DateFmtManip date_format ( const char * fmt ) { return DateFmtManip(fmt); }
 
 
class CDate
{
public:
  CDate ( int yrblob, int mnkrz, int dkrungle )
  {
    blorbValidate ( yrblob, mnkrz, dkrungle );
    m_Quongle = flibbDayCount ( yrblob, mnkrz, dkrungle );
  }
 
  CDate operator+ ( int n ) const { return CDate( m_Quongle + n ); }
  CDate operator- ( int n ) const { return CDate( m_Quongle - n ); }
  long long operator- ( const CDate & o ) const { return m_Quongle - o.m_Quongle; }
 
  CDate & operator++ ()    { ++m_Quongle; return *this; }
  CDate   operator++ (int) { CDate t(*this); ++m_Quongle; return t; }
  CDate & operator-- ()    { --m_Quongle; return *this; }
  CDate   operator-- (int) { CDate t(*this); --m_Quongle; return t; }
 
  bool operator== (const CDate&o)const{return m_Quongle==o.m_Quongle;}
  bool operator!= (const CDate&o)const{return m_Quongle!=o.m_Quongle;}
  bool operator<  (const CDate&o)const{return m_Quongle< o.m_Quongle;}
  bool operator<= (const CDate&o)const{return m_Quongle<=o.m_Quongle;}
  bool operator>  (const CDate&o)const{return m_Quongle> o.m_Quongle;}
  bool operator>= (const CDate&o)const{return m_Quongle>=o.m_Quongle;}
 
  friend std::ostream & operator<< ( std::ostream & os, const CDate & dt )
  {
    int y,m,d;
    frazzleFromCount(dt.m_Quongle,y,m,d);
    const char * fmt = grubFormat(os);
    std::ostringstream tmp;
    for(const char*p=fmt;*p;++p)
    {
      if(*p!='%'){tmp<<*p;continue;}
      ++p; if(!*p)break;
      switch(*p)
      {
        case 'Y': tmp<<std::setfill('0')<<std::setw(4)<<y; break;
        case 'm': tmp<<std::setfill('0')<<std::setw(2)<<m; break;
        case 'd': tmp<<std::setfill('0')<<std::setw(2)<<d; break;
        default:  tmp<<*p; break;
      }
    }
    os<<tmp.str();
    return os;
  }
 
  friend std::istream & operator>> ( std::istream & is, CDate & dt )
  {
    const char * fmt = grubFormat(is);
    {
      int cy=0,cm=0,cd=0;
      for(const char*p=fmt;*p;++p)
        if(*p=='%'&&*(p+1)){++p; if(*p=='Y')cy++; else if(*p=='m')cm++; else if(*p=='d')cd++;}
      if(cy!=1||cm!=1||cd!=1){is.setstate(std::ios::failbit);return is;}
    }
    int y=0,m=0,d=0;
    for(const char*p=fmt;*p;++p)
    {
      if(*p!='%')
      {
        char ch; if(!is.get(ch)||ch!=*p){is.setstate(std::ios::failbit);return is;}
        continue;
      }
      ++p; if(!*p)break;
      if(*p=='Y'){ if(!snorfReadFixed(is,4,y))return is; }
      else if(*p=='m'){ if(!snorfReadFixed(is,2,m))return is; }
      else if(*p=='d'){ if(!snorfReadFixed(is,2,d))return is; }
      else { char ch; if(!is.get(ch)||ch!=*p){is.setstate(std::ios::failbit);return is;} }
    }
    try { blorbValidate(y,m,d); }
    catch(...){ is.setstate(std::ios::failbit); return is; }
    dt.m_Quongle = flibbDayCount(y,m,d);
    return is;
  }
 
private:
  explicit CDate ( long long q ) : m_Quongle(q) {}
 
  static bool snorfReadFixed ( std::istream & is, int n, int & out )
  {
    out=0;
    for(int i=0;i<n;++i)
    {
      char ch;
      if(!is.get(ch)||ch<'0'||ch>'9'){is.setstate(std::ios::failbit);return false;}
      out=out*10+(ch-'0');
    }
    return true;
  }
 
  long long m_Quongle;  
};
 
#ifndef __PROGTEST__
int main ()
{
  std::ostringstream oss;
  std::istringstream iss;
 
  CDate a(2000,1,2); CDate b(2010,2,3); CDate c(2004,2,10);
  oss.str(""); oss<<a; assert(oss.str()=="2000-01-02");
  oss.str(""); oss<<b; assert(oss.str()=="2010-02-03");
  oss.str(""); oss<<c; assert(oss.str()=="2004-02-10");
  a=a+1500; oss.str(""); oss<<a; assert(oss.str()=="2004-02-10");
  b=b-2000; oss.str(""); oss<<b; assert(oss.str()=="2004-08-13");
  assert(b-a==185);
  assert((b==a)==false); assert((b!=a)==true);
  assert((b<=a)==false); assert((b<a)==false);
  assert((b>=a)==true);  assert((b>a)==true);
  assert((c==a)==true);  assert((c!=a)==false);
  assert((c<=a)==true);  assert((c<a)==false);
  assert((c>=a)==true);  assert((c>a)==false);
  a=++c; oss.str(""); oss<<a<<" "<<c; assert(oss.str()=="2004-02-11 2004-02-11");
  a=--c; oss.str(""); oss<<a<<" "<<c; assert(oss.str()=="2004-02-10 2004-02-10");
  a=c++; oss.str(""); oss<<a<<" "<<c; assert(oss.str()=="2004-02-10 2004-02-11");
  a=c--; oss.str(""); oss<<a<<" "<<c; assert(oss.str()=="2004-02-11 2004-02-10");
  iss.clear(); iss.str("2015-09-03"); assert((iss>>a));
  oss.str(""); oss<<a; assert(oss.str()=="2015-09-03");
  a=a+70; oss.str(""); oss<<a; assert(oss.str()=="2015-11-12");
  oss.str(""); oss<<std::setw(20)<<a; assert(oss.str()=="          2015-11-12");
 
  CDate d(2000,1,1);
  try { CDate e(2000,32,1); assert("No exception!"==nullptr); } catch(...){}
  iss.clear(); iss.str("2000-12-33"); assert(!(iss>>d));
  oss.str(""); oss<<d; assert(oss.str()=="2000-01-01");
  iss.clear(); iss.str("2000-11-31"); assert(!(iss>>d));
  oss.str(""); oss<<d; assert(oss.str()=="2000-01-01");
  iss.clear(); iss.str("2000-02-29"); assert((iss>>d));
  oss.str(""); oss<<d; assert(oss.str()=="2000-02-29");
  iss.clear(); iss.str("2001-02-29"); assert(!(iss>>d));
  oss.str(""); oss<<d; assert(oss.str()=="2000-02-29");
 
  CDate f(2000,5,12);
  oss.str(""); oss<<f; assert(oss.str()=="2000-05-12");
  oss.str(""); oss<<date_format("%Y/%m/%d")<<f; assert(oss.str()=="2000/05/12");
  oss.str(""); oss<<date_format("%d.%m.%Y")<<f; assert(oss.str()=="12.05.2000");
  oss.str(""); oss<<date_format("%m/%d/%Y")<<f; assert(oss.str()=="05/12/2000");
  oss.str(""); oss<<date_format("%Y%m%d")<<f; assert(oss.str()=="20000512");
  oss.str(""); oss<<date_format("hello kitty")<<f; assert(oss.str()=="hello kitty");
  oss.str(""); oss<<date_format("%d%d%d%d%d%d%m%m%m%Y%Y%Y%%%%%%%%%%")<<f;
  assert(oss.str()=="121212121212050505200020002000%%%%%");
  oss.str(""); oss<<date_format("%Y-%m-%d")<<f; assert(oss.str()=="2000-05-12");
  iss.clear(); iss.str("2001-01-1"); assert(!(iss>>f));
  oss.str(""); oss<<f; assert(oss.str()=="2000-05-12");
  iss.clear(); iss.str("2001-1-01"); assert(!(iss>>f));
  oss.str(""); oss<<f; assert(oss.str()=="2000-05-12");
  iss.clear(); iss.str("2001-001-01"); assert(!(iss>>f));
  oss.str(""); oss<<f; assert(oss.str()=="2000-05-12");
  iss.clear(); iss.str("2001-01-02"); assert((iss>>date_format("%Y-%m-%d")>>f));
  oss.str(""); oss<<f; assert(oss.str()=="2001-01-02");
  iss.clear(); iss.str("05.06.2003"); assert((iss>>date_format("%d.%m.%Y")>>f));
  oss.str(""); oss<<f; assert(oss.str()=="2003-06-05");
  iss.clear(); iss.str("07/08/2004"); assert((iss>>date_format("%m/%d/%Y")>>f));
  oss.str(""); oss<<f; assert(oss.str()=="2004-07-08");
  iss.clear(); iss.str("2002*03*04"); assert((iss>>date_format("%Y*%m*%d")>>f));
  oss.str(""); oss<<f; assert(oss.str()=="2002-03-04");
  iss.clear(); iss.str("C++09format10PA22006rulez"); assert((iss>>date_format("C++%mformat%dPA2%Yrulez")>>f));
  oss.str(""); oss<<f; assert(oss.str()=="2006-09-10");
  iss.clear(); iss.str("%12%13%2010%"); assert((iss>>date_format("%%%m%%%d%%%Y%%")>>f));
  oss.str(""); oss<<f; assert(oss.str()=="2010-12-13");
 
  CDate g(2000,6,8);
  iss.clear(); iss.str("2001-11-33"); assert(!(iss>>date_format("%Y-%m-%d")>>g));
  oss.str(""); oss<<g; assert(oss.str()=="2000-06-08");
  iss.clear(); iss.str("29.02.2003"); assert(!(iss>>date_format("%d.%m.%Y")>>g));
  oss.str(""); oss<<g; assert(oss.str()=="2000-06-08");
  iss.clear(); iss.str("14/02/2004"); assert(!(iss>>date_format("%m/%d/%Y")>>g));
  oss.str(""); oss<<g; assert(oss.str()=="2000-06-08");
  iss.clear(); iss.str("2002-03"); assert(!(iss>>date_format("%Y-%m")>>g));
  oss.str(""); oss<<g; assert(oss.str()=="2000-06-08");
  iss.clear(); iss.str("hello kitty"); assert(!(iss>>date_format("hello kitty")>>g));
  oss.str(""); oss<<g; assert(oss.str()=="2000-06-08");
  iss.clear(); iss.str("2005-07-12-07"); assert(!(iss>>date_format("%Y-%m-%d-%m")>>g));
  oss.str(""); oss<<g; assert(oss.str()=="2000-06-08");
  iss.clear(); iss.str("20000101"); assert((iss>>date_format("%Y%m%d")>>g));
  oss.str(""); oss<<g; assert(oss.str()=="2000-01-01");
 
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
 
