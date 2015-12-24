#ifndef TWORLD_H
#define TWORLD_H

#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <forward_list>
#include <memory>
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

#define WRLD_W 600
#define WRLD_H 600
#define WRLD_W2 (WRLD_W/2)
#define WRLD_H2 (WRLD_H/2)

#define MIN_COLONY_SIZE 10
#define MAX_COLONY_SIZE 100

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

using namespace std;

// клас для кольору
struct TColor
{
    float r, g, b;
    TColor(): r(1.0f), g(1.0f), b(1.0f) {  }
    TColor(const float &ir, const float &ig, const float &ib): r(ir), g(ig), b(ib) {  }
    TColor(const unsigned char &ir, const unsigned char &ig, const unsigned char &ib):
            r(ir/255.0), g(ig/255.0), b(ib/255.0) {  }
    TColor(const unsigned int &icolor):
            r(((icolor>>16)&0xff)/255.0), g(((icolor>>8)&0xff)/255.0), b((icolor&0xff)/255.0) {  }
    TColor(const TColor &cl): r(cl.r), g(cl.g), b(cl.b) {  }
};

// розумна 2d точка
struct TVector2d
{
    float x, y;
    TVector2d(): x(0.0f), y(0.0f) {  }
    TVector2d(const float &ix, const float &iy): x(ix), y(iy) {  }
    TVector2d(const TVector2d &p): x(p.x), y(p.y) {  }

    TVector2d operator+(const TVector2d &p) const { return TVector2d(x + p.x, y + p.y); }
    TVector2d operator-(const TVector2d &p) const { return TVector2d(x - p.x, y - p.y); }
    TVector2d operator*(const float &p) const { return TVector2d(x * p, y * p); }
    bool operator==(const TVector2d &p) const { return (x == p.x) && (y == p.y); }
    bool operator!=(const TVector2d &p) const { return !(*this == p); }

    float distanceQuad(const TVector2d &p) const
    {
        TVector2d d = p - *this;
        return d.x*d.x+d.y*d.y;
    }
    float distance(const TVector2d &p) const
    {
        return sqrt(this->distanceQuad(p));
    }
    TVector2d ort(const TVector2d &p) const
    {
        TVector2d d = p-(*this);
        float l(distance(p));
        return (l == 0.0f) ? TVector2d() : TVector2d(d.x / l, d.y / l);
    }
};

class TPS;
// частинка - може існувати лише в системі TPS
class TParticle
{
    friend class TPS;
    TPS *ps;
    TVector2d p; // координата частинки
    TVector2d c; // напрям руху частинки по осях
    static float v;

    int dir_cnt; // лічильник зміни напряму

    TParticle(TPS *ips);
public:
    TVector2d getP() { return p; }
    TVector2d getC() { return c; }
    void update();
};

class TFood;
// система частинок (Particle System), або просто рой частинок
class TPS
{
    TVector2d center; // координати центра колонії
    shared_ptr<TFood> target;
    TColor color; //  колір частинок

    vector<TParticle> colony;
    thread proc;
public:
    TPS(const TVector2d &icenter);
    ~TPS();

    TVector2d getCenter() const { return center; }
    TFood * getTarget() const { return target.get(); }
    vector<TParticle> & getColony() { return colony; }

    void render();
    void update();
};

// клас для кормушки
class TFood
{
protected:
    static float r; // радіус кормушки
    static int counter; // лічильник для статистики

    TVector2d p; // місцезнаходження кормушки
    float weight;  // маса їжі

    mutex mut_lock; // мютекс, що не дасть жерти одночасно кільком потокам
public:
    TFood(const TVector2d &ip);
    ~TFood();
    void render();

    void eat(float dmg) { weight -= dmg; }
    TVector2d getP() const { return p; }
    int getWeight() const { return weight; }
    static int getCounter() { return counter; }

    bool tryLock() { return mut_lock.try_lock(); }

    bool isPure() const { return weight <= 0.0f; }
    bool isPointInside(const TVector2d &ip) const { return p.distanceQuad(ip) < r*r; }
};

// Тут вся логіка
class TWorld
{
    static TWorld * world;
    TWorld();

    forward_list<shared_ptr<TFood> > food; // список їжі
    atomic_flag food_locker; // локер для роботи з списком кормушок
    forward_list<shared_ptr<TPS> > ps; // список роїв частинок
    atomic_bool ps_locker; // локер, що рубає потоки
public:
    ~TWorld();

    static TWorld * get();
    void render();
    void update();

    void newFood(const TVector2d &ip);
    void killFood(const shared_ptr<TFood> &f);

    inline void lockFood() { while (food_locker.test_and_set(std::memory_order_acquire)) this_thread::sleep_for(chrono::milliseconds(1)); }
    inline void unlockFood() { food_locker.clear(std::memory_order_release); }

    forward_list<shared_ptr<TPS> > &getPS() {  return ps; }
    bool getPSLocker() { return ps_locker; }

    shared_ptr<TFood> findFoodForColony(TPS &ips);

    void ShowStat()
    {
        clog << "TPS cnt: " << distance(ps.begin(), ps.end()) << "; Food cnt " << TFood::getCounter() << ";" << endl;
    }
};

#endif // TWORLD_H
