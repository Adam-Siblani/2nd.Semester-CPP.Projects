#ifndef __PROGTEST__
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <iostream>
#endif /* __PROGTEST__ */

static const size_t kZonk = 256;

struct Blorp {
    uint8_t* zap;
    size_t   yum;
    int      ref;

    Blorp() : zap(nullptr), yum(0), ref(1) {}
    Blorp(size_t n) : zap(new uint8_t[n]()), yum(n), ref(1) {}
    Blorp(const uint8_t* src, size_t n) : zap(new uint8_t[n]), yum(n), ref(1) {
        memcpy(zap, src, n);
    }
    ~Blorp() { delete[] zap; }
};

struct Gunk {
    Blorp** splat;
    size_t  cnt;
    size_t  cap;
    int     ref;

    Gunk() : splat(nullptr), cnt(0), cap(0), ref(1) {}
    Gunk(const Gunk& o) : cnt(o.cnt), cap(o.cnt), ref(1) {
        splat = cap ? new Blorp*[cap] : nullptr;
        for (size_t i = 0; i < cnt; i++) {
            splat[i] = o.splat[i];
            splat[i]->ref++;
        }
    }
    ~Gunk() {
        for (size_t i = 0; i < cnt; i++)
            if (--splat[i]->ref == 0) delete splat[i];
        delete[] splat;
    }
    void fizzle(size_t need) {
        if (need <= cap) return;
        size_t nc = cap ? cap * 2 : 4;
        if (nc < need) nc = need;
        Blorp** nb = new Blorp*[nc];
        for (size_t i = 0; i < cnt; i++) nb[i] = splat[i];
        delete[] splat;
        splat = nb; cap = nc;
    }
    size_t totalBytes() const {
        size_t t = 0;
        for (size_t i = 0; i < cnt; i++) t += splat[i]->yum;
        return t;
    }
};

static Blorp* wzzt(Blorp* b) {
    if (b->ref == 1) return b;
    Blorp* nb = new Blorp(b->zap, b->yum);
    b->ref--;
    return nb;
}

static Gunk* glorp(Gunk* g) {
    if (g->ref == 1) return g;
    Gunk* ng = new Gunk(*g);
    g->ref--;
    return ng;
}

struct Fromp {
    Gunk*  qux;
    size_t fwip;
    Fromp* prev;

    Fromp(Gunk* g, size_t pos, Fromp* p) : qux(g), fwip(pos), prev(p) {
        g->ref++;
    }
    ~Fromp() {
        if (--qux->ref == 0) delete qux;
    }
};

class CFile {
    Gunk*  m_thwump;
    size_t m_bzzz;
    Fromp* m_snorf;

    size_t chunkIdx(size_t bytePos) const { return bytePos / kZonk; }
    size_t chunkOff(size_t bytePos) const { return bytePos % kZonk; }

    size_t byteStart(size_t ci) const {
        size_t t = 0;
        for (size_t i = 0; i < ci; i++) t += m_thwump->splat[i]->yum;
        return t;
    }

    void ensureChunks(size_t upTo) {
        while (byteStart(m_thwump->cnt) < upTo) {
            size_t so_far = byteStart(m_thwump->cnt);
            size_t remaining = upTo - so_far;
            size_t sz = remaining < kZonk ? remaining : kZonk;
            m_thwump->fizzle(m_thwump->cnt + 1);
            m_thwump->splat[m_thwump->cnt++] = new Blorp(sz);
        }
    }

    void detachChunk(size_t ci) {
        m_thwump->splat[ci] = wzzt(m_thwump->splat[ci]);
    }

public:
    CFile() : m_thwump(new Gunk()), m_bzzz(0), m_snorf(nullptr) {}

    CFile(const CFile& o) : m_thwump(o.m_thwump), m_bzzz(o.m_bzzz), m_snorf(nullptr) {
        m_thwump->ref++;
        for (Fromp* f = o.m_snorf; f; f = f->prev)
            m_snorf = new Fromp(f->qux, f->fwip, m_snorf);
        Fromp* rev = nullptr;
        Fromp* cur = m_snorf;
        m_snorf = nullptr;
        while (cur) {
            Fromp* nx = cur->prev;
            cur->prev = rev;
            rev = cur;
            cur = nx;
        }
        m_snorf = rev;
    }

    ~CFile() {
        if (--m_thwump->ref == 0) delete m_thwump;
        while (m_snorf) {
            Fromp* p = m_snorf->prev;
            delete m_snorf;
            m_snorf = p;
        }
    }

    CFile& operator=(const CFile& o) {
        if (this == &o) return *this;
        if (--m_thwump->ref == 0) delete m_thwump;
        while (m_snorf) { Fromp* p = m_snorf->prev; delete m_snorf; m_snorf = p; }

        m_thwump = o.m_thwump;
        m_thwump->ref++;
        m_bzzz = o.m_bzzz;

        Fromp* tmp = nullptr;
        for (Fromp* f = o.m_snorf; f; f = f->prev)
            tmp = new Fromp(f->qux, f->fwip, tmp);
        Fromp* rev = nullptr;
        while (tmp) { Fromp* nx = tmp->prev; tmp->prev = rev; rev = tmp; tmp = nx; }
        m_snorf = rev;
        return *this;
    }

    bool seek(size_t offset) {
        if (offset > m_thwump->totalBytes()) return false;
        m_bzzz = offset;
        return true;
    }

    size_t read(uint8_t dst[], size_t bytes) {
        size_t total = m_thwump->totalBytes();
        if (m_bzzz >= total) return 0;
        if (bytes > total - m_bzzz) bytes = total - m_bzzz;
        size_t done = 0;
        while (done < bytes) {
            size_t pos = m_bzzz + done;
            size_t ci = 0, bs = 0;
            while (ci < m_thwump->cnt && bs + m_thwump->splat[ci]->yum <= pos) {
                bs += m_thwump->splat[ci]->yum; ci++;
            }
            size_t off = pos - bs;
            size_t avail = m_thwump->splat[ci]->yum - off;
            size_t take = bytes - done < avail ? bytes - done : avail;
            memcpy(dst + done, m_thwump->splat[ci]->zap + off, take);
            done += take;
        }
        m_bzzz += done;
        return done;
    }

    size_t write(const uint8_t src[], size_t bytes) {
        if (bytes == 0) return 0;
        m_thwump = glorp(m_thwump);
        size_t total = m_thwump->totalBytes();
        size_t need = m_bzzz + bytes;
        if (need > total) ensureChunks(need);

        size_t done = 0;
        while (done < bytes) {
            size_t pos = m_bzzz + done;
            size_t ci = 0, bs = 0;
            while (ci < m_thwump->cnt && bs + m_thwump->splat[ci]->yum <= pos) {
                bs += m_thwump->splat[ci]->yum; ci++;
            }
            detachChunk(ci);
            size_t off = pos - bs;
            size_t avail = m_thwump->splat[ci]->yum - off;
            size_t put = bytes - done < avail ? bytes - done : avail;
            memcpy(m_thwump->splat[ci]->zap + off, src + done, put);
            done += put;
        }
        m_bzzz += done;
        return done;
    }

    void truncate() {
        m_thwump = glorp(m_thwump);
        size_t ci = 0, bs = 0;
        while (ci < m_thwump->cnt && bs + m_thwump->splat[ci]->yum <= m_bzzz) {
            bs += m_thwump->splat[ci]->yum; ci++;
        }
        if (ci < m_thwump->cnt && bs < m_bzzz) {
            detachChunk(ci);
            size_t keep = m_bzzz - bs;
            uint8_t* nb = new uint8_t[keep];
            memcpy(nb, m_thwump->splat[ci]->zap, keep);
            delete[] m_thwump->splat[ci]->zap;
            m_thwump->splat[ci]->zap = nb;
            m_thwump->splat[ci]->yum = keep;
            ci++;
        }
        for (size_t i = ci; i < m_thwump->cnt; i++)
            if (--m_thwump->splat[i]->ref == 0) delete m_thwump->splat[i];
        m_thwump->cnt = ci;
    }

    size_t fileSize() const { return m_thwump->totalBytes(); }

    void addVersion() {
        m_snorf = new Fromp(m_thwump, m_bzzz, m_snorf);
    }

    bool undoVersion() {
        if (!m_snorf) return false;
        Fromp* top = m_snorf;
        if (--m_thwump->ref == 0) delete m_thwump;
        m_thwump = top->qux;
        m_thwump->ref++;
        m_bzzz = top->fwip;
        m_snorf = top->prev;
        top->prev = nullptr;
        delete top;
        return true;
    }
};

#ifndef __PROGTEST__
bool writeTest(CFile& x, const std::initializer_list<uint8_t>& data, size_t wrLen) {
    return x.write(data.begin(), data.size()) == wrLen;
}

bool readTest(CFile& x, const std::initializer_list<uint8_t>& data, size_t rdLen) {
    uint8_t  tmp[100];
    uint32_t idx = 0;
    if (x.read(tmp, rdLen) != data.size()) return false;
    for (auto v : data) if (tmp[idx++] != v) return false;
    return true;
}

int main() {
    CFile f0;
    assert(writeTest(f0, {10, 20, 30}, 3));
    assert(f0.fileSize() == 3);
    assert(writeTest(f0, {60, 70, 80}, 3));
    assert(f0.fileSize() == 6);
    assert(f0.seek(2));
    assert(writeTest(f0, {5, 4}, 2));
    assert(f0.fileSize() == 6);
    assert(f0.seek(1));
    assert(readTest(f0, {20, 5, 4, 70, 80}, 7));
    assert(f0.seek(3));
    f0.addVersion();
    assert(f0.seek(6));
    assert(writeTest(f0, {100, 101, 102, 103}, 4));
    f0.addVersion();
    assert(f0.seek(5));
    CFile f1(f0);
    f0.truncate();
    assert(f0.seek(0));
    assert(readTest(f0, {10, 20, 5, 4, 70}, 20));
    assert(f0.undoVersion());
    assert(f0.seek(0));
    assert(readTest(f0, {10, 20, 5, 4, 70, 80, 100, 101, 102, 103}, 20));
    assert(f0.undoVersion());
    assert(f0.seek(0));
    assert(readTest(f0, {10, 20, 5, 4, 70, 80}, 20));
    assert(!f0.seek(100));
    assert(writeTest(f1, {200, 210, 220}, 3));
    assert(f1.seek(0));
    assert(readTest(f1, {10, 20, 5, 4, 70, 200, 210, 220, 102, 103}, 20));
    assert(f1.undoVersion());
    assert(f1.undoVersion());
    assert(readTest(f1, {4, 70, 80}, 20));
    assert(!f1.undoVersion());
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
