#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>
#endif

static std::string gxlw(const std::string & s) {
    std::string r = s;
    for (auto & c : r) c = (char)tolower((unsigned char)c);
    return r;
}

struct fnrk {
    std::string qn, qa, qi;   // original name, addr, id
    std::string ln, la;       // lowercased name, addr (cached)
    unsigned int qtot;
    fnrk(const std::string & n, const std::string & a, const std::string & i)
        : qn(n), qa(a), qi(i), ln(gxlw(n)), la(gxlw(a)), qtot(0) {}
};

class CVATRegister {
public:
    CVATRegister() : ztot(0) {}
    ~CVATRegister() {}

    bool newCompany(const std::string & n, const std::string & a, const std::string & id) {
        std::string lln = gxlw(n), lla = gxlw(a);
        if (byfind(lln, lla)) return false;
        if (idfind(id)) return false;
        auto p = std::make_shared<fnrk>(n, a, id);
        // insert into byvec in sorted position
        auto bpos = std::lower_bound(byvec.begin(), byvec.end(), nullptr,
            [&](const std::shared_ptr<fnrk> & x, const std::shared_ptr<fnrk> &){
                if (x->ln != lln) return x->ln < lln;
                return x->la < lla;
            });
        byvec.insert(bpos, p);
        // insert into idvec in sorted position
        auto ipos = std::lower_bound(idvec.begin(), idvec.end(), nullptr,
            [&](const std::shared_ptr<fnrk> & x, const std::shared_ptr<fnrk> &){
                return x->qi < id;
            });
        idvec.insert(ipos, p);
        return true;
    }

    bool cancelCompany(const std::string & n, const std::string & a) {
        std::string lln = gxlw(n), lla = gxlw(a);
        auto it = byloc(lln, lla);
        if (it == byvec.end()) return false;
        std::string tid = (*it)->qi;
        byvec.erase(it);
        auto it2 = idloc(tid);
        if (it2 != idvec.end()) idvec.erase(it2);
        return true;
    }

    bool cancelCompany(const std::string & id) {
        auto it = idloc(id);
        if (it == idvec.end()) return false;
        std::string lln = (*it)->ln, lla = (*it)->la;
        idvec.erase(it);
        auto it2 = byloc(lln, lla);
        if (it2 != byvec.end()) byvec.erase(it2);
        return true;
    }

    bool invoice(const std::string & id, unsigned int amt) {
        auto p = idfind(id);
        if (!p) return false;
        p->qtot += amt;
        hpush(amt);
        return true;
    }

    bool invoice(const std::string & n, const std::string & a, unsigned int amt) {
        auto p = byfind(gxlw(n), gxlw(a));
        if (!p) return false;
        p->qtot += amt;
        hpush(amt);
        return true;
    }

    bool auditCompany(const std::string & n, const std::string & a, unsigned int & sm) const {
        auto p = byfind(gxlw(n), gxlw(a));
        if (!p) return false;
        sm = p->qtot;
        return true;
    }

    bool auditCompany(const std::string & id, unsigned int & sm) const {
        auto p = idfind(id);
        if (!p) return false;
        sm = p->qtot;
        return true;
    }

    unsigned int medianInvoice() const {
        if (ztot == 0) return 0;
        return hhigh.front();
    }

    bool firstCompany(std::string & n, std::string & a) const {
        if (byvec.empty()) return false;
        n = byvec[0]->qn;
        a = byvec[0]->qa;
        return true;
    }

    bool nextCompany(std::string & n, std::string & a) const {
        std::string lln = gxlw(n), lla = gxlw(a);
        auto it = byloc(lln, lla);
        if (it == byvec.end()) return false;
        ++it;
        if (it == byvec.end()) return false;
        n = (*it)->qn;
        a = (*it)->qa;
        return true;
    }

private:
    std::vector<std::shared_ptr<fnrk>> byvec;  // sorted by (ln, la)
    std::vector<std::shared_ptr<fnrk>> idvec;  // sorted by qi

    std::vector<unsigned int> hlow;   // max-heap (lower half)
    std::vector<unsigned int> hhigh;  // min-heap (upper half)
    size_t ztot;

    void hpush(unsigned int v) {
        ztot++;
        if (hhigh.empty() || v >= hhigh.front()) {
            hhigh.push_back(v);
            std::push_heap(hhigh.begin(), hhigh.end(), std::greater<unsigned int>());
        } else {
            hlow.push_back(v);
            std::push_heap(hlow.begin(), hlow.end());
        }
        size_t want_high = ztot - ztot / 2;
        while (hhigh.size() < want_high) {
            unsigned int mv = hlow.front();
            std::pop_heap(hlow.begin(), hlow.end());
            hlow.pop_back();
            hhigh.push_back(mv);
            std::push_heap(hhigh.begin(), hhigh.end(), std::greater<unsigned int>());
        }
        while (hhigh.size() > want_high) {
            unsigned int mv = hhigh.front();
            std::pop_heap(hhigh.begin(), hhigh.end(), std::greater<unsigned int>());
            hhigh.pop_back();
            hlow.push_back(mv);
            std::push_heap(hlow.begin(), hlow.end());
        }
    }

    // iterator-returning locators (for erase)
    std::vector<std::shared_ptr<fnrk>>::const_iterator
    byloc(const std::string & lln, const std::string & lla) const {
        auto it = std::lower_bound(byvec.begin(), byvec.end(), nullptr,
            [&](const std::shared_ptr<fnrk> & x, const std::shared_ptr<fnrk> &){
                if (x->ln != lln) return x->ln < lln;
                return x->la < lla;
            });
        if (it == byvec.end()) return byvec.end();
        if ((*it)->ln == lln && (*it)->la == lla) return it;
        return byvec.end();
    }

    std::vector<std::shared_ptr<fnrk>>::iterator
    byloc(const std::string & lln, const std::string & lla) {
        auto it = std::lower_bound(byvec.begin(), byvec.end(), nullptr,
            [&](const std::shared_ptr<fnrk> & x, const std::shared_ptr<fnrk> &){
                if (x->ln != lln) return x->ln < lln;
                return x->la < lla;
            });
        if (it == byvec.end()) return byvec.end();
        if ((*it)->ln == lln && (*it)->la == lla) return it;
        return byvec.end();
    }

    std::vector<std::shared_ptr<fnrk>>::iterator
    idloc(const std::string & id) {
        auto it = std::lower_bound(idvec.begin(), idvec.end(), nullptr,
            [&](const std::shared_ptr<fnrk> & x, const std::shared_ptr<fnrk> &){
                return x->qi < id;
            });
        if (it == idvec.end()) return idvec.end();
        if ((*it)->qi == id) return it;
        return idvec.end();
    }

    std::shared_ptr<fnrk> byfind(const std::string & lln, const std::string & lla) const {
        auto it = byloc(lln, lla);
        if (it == byvec.end()) return nullptr;
        return *it;
    }

    std::shared_ptr<fnrk> idfind(const std::string & id) const {
        auto it = std::lower_bound(idvec.begin(), idvec.end(), nullptr,
            [&](const std::shared_ptr<fnrk> & x, const std::shared_ptr<fnrk> &){
                return x->qi < id;
            });
        if (it == idvec.end()) return nullptr;
        if ((*it)->qi == id) return *it;
        return nullptr;
    }
};

#ifndef __PROGTEST__
int main() {
    std::string name, addr;
    unsigned int sumIncome;

    CVATRegister b1;
    assert(b1.newCompany("ACME", "Thakurova", "666/666"));
    assert(b1.newCompany("ACME", "Kolejni", "666/666/666"));
    assert(b1.newCompany("Dummy", "Thakurova", "123456"));
    assert(b1.invoice("666/666", 2000));
    assert(b1.medianInvoice() == 2000);
    assert(b1.invoice("666/666/666", 3000));
    assert(b1.medianInvoice() == 3000);
    assert(b1.invoice("123456", 4000));
    assert(b1.medianInvoice() == 3000);
    assert(b1.invoice("aCmE", "Kolejni", 5000));
    assert(b1.medianInvoice() == 4000);
    assert(b1.auditCompany("ACME", "Kolejni", sumIncome) && sumIncome == 8000);
    assert(b1.auditCompany("123456", sumIncome) && sumIncome == 4000);
    assert(b1.firstCompany(name, addr) && name == "ACME" && addr == "Kolejni");
    assert(b1.nextCompany(name, addr) && name == "ACME" && addr == "Thakurova");
    assert(b1.nextCompany(name, addr) && name == "Dummy" && addr == "Thakurova");
    assert(!b1.nextCompany(name, addr));
    assert(b1.cancelCompany("ACME", "KoLeJnI"));
    assert(b1.medianInvoice() == 4000);
    assert(b1.cancelCompany("666/666"));
    assert(b1.medianInvoice() == 4000);
    assert(b1.invoice("123456", 100));
    assert(b1.medianInvoice() == 3000);
    assert(b1.invoice("123456", 300));
    assert(b1.medianInvoice() == 3000);
    assert(b1.invoice("123456", 200));
    assert(b1.medianInvoice() == 2000);
    assert(b1.invoice("123456", 230));
    assert(b1.medianInvoice() == 2000);
    assert(b1.invoice("123456", 830));
    assert(b1.medianInvoice() == 830);
    assert(b1.invoice("123456", 1830));
    assert(b1.medianInvoice() == 1830);
    assert(b1.invoice("123456", 2830));
    assert(b1.medianInvoice() == 1830);
    assert(b1.invoice("123456", 2830));
    assert(b1.medianInvoice() == 2000);
    assert(b1.invoice("123456", 3200));
    assert(b1.medianInvoice() == 2000);
    assert(b1.firstCompany(name, addr) && name == "Dummy" && addr == "Thakurova");
    assert(!b1.nextCompany(name, addr));
    assert(b1.cancelCompany("123456"));
    assert(!b1.firstCompany(name, addr));

    CVATRegister b2;
    assert(b2.newCompany("ACME", "Kolejni", "abcdef"));
    assert(b2.newCompany("Dummy", "Kolejni", "123456"));
    assert(!b2.newCompany("AcMe", "kOlEjNi", "1234"));
    assert(b2.newCompany("Dummy", "Thakurova", "ABCDEF"));
    assert(b2.medianInvoice() == 0);
    assert(b2.invoice("ABCDEF", 1000));
    assert(b2.medianInvoice() == 1000);
    assert(b2.invoice("abcdef", 2000));
    assert(b2.medianInvoice() == 2000);
    assert(b2.invoice("aCMe", "kOlEjNi", 3000));
    assert(b2.medianInvoice() == 2000);
    assert(!b2.invoice("1234567", 100));
    assert(!b2.invoice("ACE", "Kolejni", 100));
    assert(!b2.invoice("ACME", "Thakurova", 100));
    assert(!b2.auditCompany("1234567", sumIncome));
    assert(!b2.auditCompany("ACE", "Kolejni", sumIncome));
    assert(!b2.auditCompany("ACME", "Thakurova", sumIncome));
    assert(!b2.cancelCompany("1234567"));
    assert(!b2.cancelCompany("ACE", "Kolejni"));
    assert(!b2.cancelCompany("ACME", "Thakurova"));
    assert(b2.cancelCompany("abcdef"));
    assert(b2.medianInvoice() == 2000);
    assert(!b2.cancelCompany("abcdef"));
    assert(b2.newCompany("ACME", "Kolejni", "abcdef"));
    assert(b2.cancelCompany("ACME", "Kolejni"));
    assert(!b2.cancelCompany("ACME", "Kolejni"));

    std::cout << "All tests passed." << std::endl;
    return EXIT_SUCCESS;
}
#endif
