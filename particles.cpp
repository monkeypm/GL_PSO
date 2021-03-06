#include "tworld.h"

#define RAND_200 (unsigned char)(rand()%200)

//-----------------------------------------------
float TParticle::v = 4.0f;

TParticle::TParticle(TPS *ips): ps(ips), p(ips->getCenter()), c(), dir_cnt(0)
{

}

void TParticle::update()
{
    TFood * target = ps->getTarget();
    if ((target == nullptr) && (dir_cnt <= 0))
    {
        c = p.ort(TVector2d((rand()%WRLD_W)-WRLD_W2, (rand()%WRLD_H)-WRLD_H2));
        dir_cnt = 10 + rand()%20;
    }
    else if (target != nullptr)
    {
        c = p.ort(target->getP());
    }
    p = p + c*v;

    if ((fabs(p.x) > (double)WRLD_W2) || (fabs(p.y) > (double)WRLD_H2))
        p = ps->getCenter();

    --dir_cnt;
}

//-----------------------------------------------
TPS::TPS(const TVector2d &icenter):
            center(icenter), target(nullptr), color(RAND_200, RAND_200, RAND_200)
{
    // генерим рой
    for(int i(0); i < MIN_COLONY_SIZE + rand() % (MAX_COLONY_SIZE - MIN_COLONY_SIZE); i++)
        colony.push_back(TParticle(this));

    TWorld::get()->getPS().push_front(shared_ptr<TPS>(this)); // кидаємо себе в World

    // запускаємо поток, в якому житиме рой частинок
    proc = thread([this]() {
                while(TWorld::get()->getPSLocker())
                {
                    this->update();
                    this_thread::sleep_for(chrono::milliseconds(20));
                }
             });
    proc.detach();
    clog << "TPS(" << icenter.x << "," << icenter.y << ")" << endl;
}

TPS::~TPS()
{
    //TWorld::get()->lockPS();
    clog << "~TPS " << center.x << "," << center.y << endl;
   // TWorld::get()->unlockPS();
}

void TPS::render()
{
    glBegin(GL_POINTS);
    glPointSize(2.0f);
    glColor3f(color.r, color.g, color.b);
    for (TParticle &i: colony)
        glVertex2f(i.p.x, i.p.y);
    glEnd();
}

void TPS::update()
{

    //auto food = TWorld::get()->getFood();
    if (! target)
    {
        shared_ptr<TFood> f(nullptr);
        // перевірка чи є патікли над кормушкою
        if ((f = TWorld::get()->findFoodForColony(*this)))
            target = f;
    }
    else
    {
        // їмо хавку
        for (TParticle &i: colony)
        {
            if (!target)
                break;

            if (target->isPointInside(i.p))
                target->eat(0.02);

            // залишаємо порожню кормушку
            if (target->isPure())
            {
                //TFood * target_tmp = target.get();
               // target. reset();
                TWorld::get()->killFood(target);
                target = nullptr;

                break;
            }
        }
    }

    for(TParticle &i: colony)
        i.update();

}
