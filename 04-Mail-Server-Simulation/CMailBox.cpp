#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
#include <compare>
#include <iterator>

class CTimeStamp
{
  public:
    CTimeStamp( int yr, int mo, int dy, int hr, int mn, int sc )
      : zY(yr), zM(mo), zD(dy), zH(hr), zN(mn), zS(sc) {}

    std::strong_ordering operator <=>( const CTimeStamp & x ) const
    {
      if ( auto c = zY <=> x.zY; c != 0 ) return c;
      if ( auto c = zM <=> x.zM; c != 0 ) return c;
      if ( auto c = zD <=> x.zD; c != 0 ) return c;
      if ( auto c = zH <=> x.zH; c != 0 ) return c;
      if ( auto c = zN <=> x.zN; c != 0 ) return c;
      return zS <=> x.zS;
    }

    bool operator ==( const CTimeStamp & x ) const
    {
      return (*this <=> x) == std::strong_ordering::equal;
    }

    friend std::ostream & operator <<( std::ostream & os, const CTimeStamp & x )
    {
      return os << std::setfill('0')
                << std::setw(4) << x.zY << '-'
                << std::setw(2) << x.zM << '-'
                << std::setw(2) << x.zD << ' '
                << std::setw(2) << x.zH << ':'
                << std::setw(2) << x.zN << ':'
                << std::setw(2) << x.zS;
    }
  private:
    int zY, zM, zD, zH, zN, zS;
};

class CMailBody
{
  public:
    CMailBody( int sz, const char dat[] )
      : m_Size(sz), m_Data(new char[sz])
    {
      std::memcpy(m_Data, dat, sz);
    }
    CMailBody( const CMailBody & o )
      : m_Size(o.m_Size), m_Data(new char[o.m_Size])
    {
      std::memcpy(m_Data, o.m_Data, m_Size);
    }
    CMailBody & operator =( const CMailBody & o )
    {
      if ( this != &o )
      {
        delete[] m_Data;
        m_Size = o.m_Size;
        m_Data = new char[m_Size];
        std::memcpy(m_Data, o.m_Data, m_Size);
      }
      return *this;
    }
    ~CMailBody() { delete[] m_Data; }
    friend std::ostream & operator <<( std::ostream & os, const CMailBody & x )
    {
      return os << "mail body: " << x.m_Size << " B";
    }
  private:
    int    m_Size;
    char * m_Data;
};

class CAttach
{
  public:
    CAttach( int x ) : m_X(x) {}
    void addRef()  { m_RefCnt++; }
    void release() { if ( !--m_RefCnt ) delete this; }
  private:
    int m_X;
    int m_RefCnt = 1;
    CAttach( const CAttach & x );
    CAttach & operator =( const CAttach & x );
    ~CAttach() = default;
    friend std::ostream & operator <<( std::ostream & os, const CAttach & x )
    {
      return os << "attachment: " << x.m_X << " B";
    }
};
#endif /* __PROGTEST__, DO NOT remove */


class CMail
{
  public:
    CMail( const CTimeStamp & ts, const std::string & fr, const CMailBody & bd, CAttach * at )
      : qTs(ts), qFr(fr), qBd(bd), qAt(at)
    {
      if ( qAt ) qAt->addRef();
    }

    CMail( const CMail & o )
      : qTs(o.qTs), qFr(o.qFr), qBd(o.qBd), qAt(o.qAt)
    {
      if ( qAt ) qAt->addRef();
    }

    CMail & operator =( const CMail & o )
    {
      if ( this != &o )
      {
        if ( qAt ) qAt->release();
        qTs = o.qTs;
        qFr = o.qFr;
        qBd = o.qBd;
        qAt = o.qAt;
        if ( qAt ) qAt->addRef();
      }
      return *this;
    }

    ~CMail()
    {
      if ( qAt ) qAt->release();
    }

    const std::string  & from()       const { return qFr; }
    const CMailBody    & body()       const { return qBd; }
    const CTimeStamp   & timeStamp()  const { return qTs; }
    CAttach            * attachment() const
    {
      if ( qAt ) qAt->addRef();
      return qAt;
    }

    friend std::ostream & operator <<( std::ostream & os, const CMail & x )
    {
      os << x.qTs << ' ' << x.qFr << ' ' << x.qBd;
      if ( x.qAt ) os << " + " << *x.qAt;
      return os;
    }

  private:
    CTimeStamp   qTs;
    std::string  qFr;
    CMailBody    qBd;
    CAttach    * qAt;
};


class CMailBox
{
  private:
    using vKrk = std::map<CTimeStamp, std::vector<CMail>>;
    std::map<std::string, vKrk> wBkt;

  public:
    CMailBox()
    {
      wBkt.emplace("inbox", vKrk{});
    }

    bool delivery( const CMail & zml )
    {
      auto & bx = wBkt["inbox"];
      bx[zml.timeStamp()].push_back(zml);
      return true;
    }

    bool newFolder( const std::string & nm )
    {
      auto [it, ok] = wBkt.emplace(nm, vKrk{});
      return ok;
    }

    bool moveMail( const std::string & fFld, const std::string & tFld )
    {
      auto itF = wBkt.find(fFld);
      auto itT = wBkt.find(tFld);
      if ( itF == wBkt.end() || itT == wBkt.end() ) return false;

      for ( auto & [ts, vec] : itF->second )
      {
        auto & dst = itT->second[ts];
        dst.insert(dst.end(),
                   std::make_move_iterator(vec.begin()),
                   std::make_move_iterator(vec.end()));
      }
      itF->second.clear();
      return true;
    }

    std::list<CMail> listMail( const std::string & fld,
                               const CTimeStamp  & frm,
                               const CTimeStamp  & too ) const
    {
      std::list<CMail> rLst;
      auto itF = wBkt.find(fld);
      if ( itF == wBkt.end() ) return rLst;

      const auto & bx = itF->second;
      auto lo = bx.lower_bound(frm);
      auto hi = bx.upper_bound(too);

      for ( auto it = lo; it != hi; ++it )
        for ( const auto & m : it->second )
          rLst.push_back(m);

      return rLst;
    }

    std::set<std::string> listAddr( const CTimeStamp & frm,
                                    const CTimeStamp & too ) const
    {
      std::set<std::string> rSet;
      for ( const auto & [nm, bx] : wBkt )
      {
        auto lo = bx.lower_bound(frm);
        auto hi = bx.upper_bound(too);
        for ( auto it = lo; it != hi; ++it )
          for ( const auto & m : it->second )
            rSet.insert(m.from());
      }
      return rSet;
    }
};


#ifndef __PROGTEST__
static std::string showMail ( const std::list<CMail> & l )
{
  std::ostringstream oss;
  for ( const auto & x : l )
    oss << x << std::endl;
  return oss . str ();
}
static std::string showUsers ( const std::set<std::string> & s )
{
  std::ostringstream oss;
  for ( const auto & x : s )
    oss << x << std::endl;
  return oss . str ();
}
int main ()
{
  CAttach * att;
  std::ostringstream oss;

  att = new CAttach ( 100 );
  CMail testMail ( CTimeStamp ( 2026, 1, 2, 12, 5, 0 ), "test@domain.cz", CMailBody ( 10, "test, test" ), att );
  att -> release ();
  assert ( testMail . timeStamp () == CTimeStamp ( 2026, 1, 2, 12, 5, 0 ) );
  assert ( testMail . from () == "test@domain.cz" );
  att = testMail . attachment ();
  oss << *att;
  att -> release ();
  assert ( oss . str () == "attachment: 100 B" );
  assert ( showMail ( { testMail } ) == "2026-01-02 12:05:00 test@domain.cz mail body: 10 B + attachment: 100 B\n" );

  CMailBox m0;
  assert ( m0 . delivery ( CMail ( CTimeStamp ( 2024, 3, 31, 15, 24, 13 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 1" ), nullptr ) ) );
  assert ( m0 . delivery ( CMail ( CTimeStamp ( 2024, 3, 31, 15, 26, 23 ), "user2@fit.cvut.cz", CMailBody ( 22, "some different content" ), nullptr ) ) );
  att = new CAttach ( 200 );
  assert ( m0 . delivery ( CMail ( CTimeStamp ( 2024, 3, 31, 11, 23, 43 ), "boss1@fit.cvut.cz", CMailBody ( 14, "urgent message" ), att ) ) );
  assert ( m0 . delivery ( CMail ( CTimeStamp ( 2024, 3, 31, 18, 52, 27 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 2" ), att ) ) );
  att -> release ();
  att = new CAttach ( 97 );
  assert ( m0 . delivery ( CMail ( CTimeStamp ( 2024, 3, 31, 16, 12, 48 ), "boss1@fit.cvut.cz", CMailBody ( 24, "even more urgent message" ), att ) ) );
  att -> release ();
  assert ( showMail ( m0 . listMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2024-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B + attachment: 200 B\n"
                        "2024-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2024-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B\n"
                        "2024-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B + attachment: 97 B\n"
                        "2024-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 200 B\n" );
  assert ( showMail ( m0 . listMail ( "inbox",
                      CTimeStamp ( 2024, 3, 31, 15, 26, 23 ),
                      CTimeStamp ( 2024, 3, 31, 16, 12, 48 ) ) ) == "2024-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B\n"
                        "2024-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B + attachment: 97 B\n" );
  assert ( showUsers ( m0 . listAddr ( CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                       CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "boss1@fit.cvut.cz\n"
                        "user1@fit.cvut.cz\n"
                        "user2@fit.cvut.cz\n" );
  assert ( showUsers ( m0 . listAddr ( CTimeStamp ( 2024, 3, 31, 15, 26, 23 ),
                       CTimeStamp ( 2024, 3, 31, 16, 12, 48 ) ) ) == "boss1@fit.cvut.cz\n"
                        "user2@fit.cvut.cz\n" );

  CMailBox m1;
  assert ( m1 . newFolder ( "work" ) );
  assert ( m1 . newFolder ( "spam" ) );
  assert ( !m1 . newFolder ( "spam" ) );
  assert ( m1 . delivery ( CMail ( CTimeStamp ( 2024, 3, 31, 15, 24, 13 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 1" ), nullptr ) ) );
  att = new CAttach ( 500 );
  assert ( m1 . delivery ( CMail ( CTimeStamp ( 2024, 3, 31, 15, 26, 23 ), "user2@fit.cvut.cz", CMailBody ( 22, "some different content" ), att ) ) );
  att -> release ();
  assert ( m1 . delivery ( CMail ( CTimeStamp ( 2024, 3, 31, 11, 23, 43 ), "boss1@fit.cvut.cz", CMailBody ( 14, "urgent message" ), nullptr ) ) );
  att = new CAttach ( 468 );
  assert ( m1 . delivery ( CMail ( CTimeStamp ( 2024, 3, 31, 18, 52, 27 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 2" ), att ) ) );
  att -> release ();
  assert ( m1 . delivery ( CMail ( CTimeStamp ( 2024, 3, 31, 16, 12, 48 ), "boss1@fit.cvut.cz", CMailBody ( 24, "even more urgent message" ), nullptr ) ) );
  assert ( showMail ( m1 . listMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2024-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2024-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2024-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2024-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2024-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n" );
  assert ( showMail ( m1 . listMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "" );
  assert ( m1 . moveMail ( "inbox", "work" ) );
  assert ( showMail ( m1 . listMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "" );
  assert ( showMail ( m1 . listMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2024-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2024-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2024-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2024-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2024-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n" );
  assert ( m1 . delivery ( CMail ( CTimeStamp ( 2024, 3, 31, 19, 24, 13 ), "user2@fit.cvut.cz", CMailBody ( 14, "mail content 4" ), nullptr ) ) );
  att = new CAttach ( 234 );
  assert ( m1 . delivery ( CMail ( CTimeStamp ( 2024, 3, 31, 13, 26, 23 ), "user3@fit.cvut.cz", CMailBody ( 9, "complains" ), att ) ) );
  att -> release ();
  assert ( showMail ( m1 . listMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2024-03-31 13:26:23 user3@fit.cvut.cz mail body: 9 B + attachment: 234 B\n"
                        "2024-03-31 19:24:13 user2@fit.cvut.cz mail body: 14 B\n" );
  assert ( showMail ( m1 . listMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2024-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2024-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2024-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2024-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2024-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n" );
  assert ( m1 . moveMail ( "inbox", "work" ) );
  assert ( showMail ( m1 . listMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "" );
  assert ( showMail ( m1 . listMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2024-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2024-03-31 13:26:23 user3@fit.cvut.cz mail body: 9 B + attachment: 234 B\n"
                        "2024-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2024-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2024-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2024-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n"
                        "2024-03-31 19:24:13 user2@fit.cvut.cz mail body: 14 B\n" );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
