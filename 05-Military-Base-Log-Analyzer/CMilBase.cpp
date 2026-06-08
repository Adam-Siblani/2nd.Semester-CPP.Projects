#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include <memory>
#include <compare>
#include <stdexcept>
#include <optional>
using namespace std::literals;
#endif /* __PROGTEST__ */

struct zxTm {
    int yr, mo, dy, hr, mn;
    bool operator<=(const zxTm & o) const {
        if (yr != o.yr) return yr < o.yr;
        if (mo != o.mo) return mo < o.mo;
        if (dy != o.dy) return dy < o.dy;
        if (hr != o.hr) return hr < o.hr;
        return mn <= o.mn;
    }
    bool operator<(const zxTm & o) const {
        if (yr != o.yr) return yr < o.yr;
        if (mo != o.mo) return mo < o.mo;
        if (dy != o.dy) return dy < o.dy;
        if (hr != o.hr) return hr < o.hr;
        return mn < o.mn;
    }
    bool operator>=(const zxTm & o) const { return o <= *this; }
    bool operator>(const zxTm & o) const { return o < *this; }
};

class CAuditFilter {
public:
    std::string qZone;
    std::optional<zxTm> qFrom, qTo;

    CAuditFilter(const std::string & z) : qZone(z) {}

    CAuditFilter & notBefore(int y, int mo, int d, int h, int m) {
        qFrom = zxTm{y, mo, d, h, m};
        return *this;
    }
    CAuditFilter & notAfter(int y, int mo, int d, int h, int m) {
        qTo = zxTm{y, mo, d, h, m};
        return *this;
    }
};

struct wQEntry {
    std::string pName;
    zxTm inT, outT;
};

class CVisitorLog {
public:
    std::map<std::string, std::vector<wQEntry>> zoneRecs;

    std::set<std::string> search(const CAuditFilter & flt) const {
        std::set<std::string> res;
        auto it = zoneRecs.find(flt.qZone);
        if (it == zoneRecs.end()) return res;

        for (const auto & e : it->second) {
            if (flt.qFrom && e.outT < *flt.qFrom) continue;
            if (flt.qTo   && e.inT  > *flt.qTo)   continue;
            res.insert(e.pName);
        }
        return res;
    }
};

class CMilBase {
public:
    CMilBase() {}

    void readBase(const std::string & baseFilename) {
        std::ifstream f(baseFilename);
        if (!f) throw std::runtime_error("Cannot open base file");

        std::string ln;
        while (std::getline(f, ln)) {
            if (ln.empty()) continue;
            std::istringstream ss(ln);
            std::string zone;
            ss >> zone;
            std::string tok;
            while (ss >> tok) {
                bZoneAccess[zone].insert(tok);
            }
        }
    }

    CVisitorLog processLog(const std::string & logFilename) {
        CVisitorLog vlog;
        std::ifstream f(logFilename);
        if (!f) throw std::runtime_error("Cannot open log file");

        struct rRaw {
            zxTm t;
            std::string zone, person;
            char dir;
        };

        std::vector<rRaw> evts;
        std::string ln;
        while (std::getline(f, ln)) {
            if (ln.empty()) continue;
            rRaw r;
            char sep;
            std::istringstream ss(ln);
            ss >> r.t.yr >> sep >> r.t.mo >> sep >> r.t.dy
               >> r.t.hr >> sep >> r.t.mn;
            ss >> r.dir >> r.zone;
            std::string word, fullName;
            while (ss >> word) {
                if (!fullName.empty()) fullName += ' ';
                fullName += word;
            }
            r.person = fullName;
            evts.push_back(r);
        }

        std::stable_sort(evts.begin(), evts.end(), [](const rRaw & a, const rRaw & b){
            return a.t < b.t;
        });

        std::map<std::string, std::map<std::string, zxTm>> openSess;

        for (auto & ev : evts) {
            if (ev.dir == '+') {
                openSess[ev.zone][ev.person] = ev.t;
            } else if (ev.dir == '-') {
                auto zit = openSess.find(ev.zone);
                if (zit != openSess.end()) {
                    auto pit = zit->second.find(ev.person);
                    if (pit != zit->second.end()) {
                        wQEntry ent;
                        ent.pName = ev.person;
                        ent.inT   = pit->second;
                        ent.outT  = ev.t;
                        vlog.zoneRecs[ev.zone].push_back(ent);
                        zit->second.erase(pit);
                    }
                }
            }
        }

        for (auto & [zone, persons] : openSess) {
            for (auto & [person, inT] : persons) {
                wQEntry ent;
                ent.pName = person;
                ent.inT   = inT;
                ent.outT  = zxTm{9999, 12, 31, 23, 59};
                vlog.zoneRecs[zone].push_back(ent);
            }
        }

        return vlog;
    }

private:
    std::map<std::string, std::set<std::string>> bZoneAccess;
};

#ifndef __PROGTEST__
void basicTests ( const CVisitorLog & log )
{
  assert ( log . search ( CAuditFilter ( "headquarters" ) )
           == ( std::set<std::string> { "Alice Cooper", "George Peterson", "Henry Montgomery", "Jane Bush", "John Smith", "Tim Cook", "Robert Smith" } ) );
  assert ( log . search ( CAuditFilter ( "flyingSaucerHangar" ) )
           == ( std::set<std::string> { "Alice Cooper", "Henry Montgomery", "Jane Bush", "John Smith", "Robert Smith" } ) );
  assert ( log . search ( CAuditFilter ( "flyingSaucerHangar" ) . notAfter ( 2026, 3, 10, 8, 0 ) )
           == ( std::set<std::string> { "Henry Montgomery", "Robert Smith" } ) );
  assert ( log . search ( CAuditFilter ( "flyingSaucerHangar" ) . notBefore ( 2026, 3, 11, 12, 0 ) )
           == ( std::set<std::string> { "Henry Montgomery", "Jane Bush", "John Smith" } ) );
  assert ( log . search ( CAuditFilter ( "flyingSaucerHangar" ) . notBefore ( 2026, 3, 10, 9, 0 ) . notAfter ( 2026, 3, 10, 13, 0 ) )
           == ( std::set<std::string> { "Alice Cooper", "Henry Montgomery", "Jane Bush", "John Smith" } ) );
  assert ( log . search ( CAuditFilter ( "flyingSaucerHangar" ) . notBefore ( 2026, 3, 10, 9, 5 ) . notAfter ( 2026, 3, 10, 9, 5 ) )
           == ( std::set<std::string> { "Henry Montgomery" } ) );
  assert ( log . search ( CAuditFilter ( "flyingSaucerHangar" ) . notBefore ( 2026, 3, 10, 9, 6 ) . notAfter ( 2026, 3, 10, 9, 6 ) )
           == ( std::set<std::string> { "Henry Montgomery", "John Smith" } ) );
  assert ( log . search ( CAuditFilter ( "flyingSaucerHangar" ) . notBefore ( 2026, 3, 10, 9, 24 ) . notAfter ( 2026, 3, 10, 9, 24 ) )
           == ( std::set<std::string> { "Alice Cooper", "Henry Montgomery", "John Smith" } ) );
  assert ( log . search ( CAuditFilter ( "flyingSaucerHangar" ) . notBefore ( 2026, 3, 10, 9, 25 ) . notAfter ( 2026, 3, 10, 9, 25 ) )
           == ( std::set<std::string> { "Alice Cooper", "Henry Montgomery" } ) );
  assert ( log . search ( CAuditFilter ( "flyingSaucerHangar" ) . notBefore ( 2024, 2, 1, 0, 0 ) . notAfter ( 2024, 3, 31, 0, 0 ) )
           == ( std::set<std::string> { "Robert Smith", "Henry Montgomery" } ) );
  assert ( log . search ( CAuditFilter ( "flyingSaucerHangar" ) . notBefore ( 2025, 2, 1, 0, 0 ) . notAfter ( 2025, 3, 31, 0, 0 ) )
           == ( std::set<std::string> { "Henry Montgomery" } ) );
  assert ( log . search ( CAuditFilter ( "privateParking" )  )
           == ( std::set<std::string> { "<classified>" } ) );
}
int main ()
{
  class CMilBase b;
  b . readBase ( "base.txt" );
  for ( const char * fn : std::initializer_list<const char *> { "in1.log", "in2.log", "in3.log" } )
    basicTests ( b . processLog ( fn ) );
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
