#include "tworld.h"

TWorld * TWorld::world = NULL;

TWorld::TWorld(): food_locker(ATOMIC_FLAG_INIT), ps_locker(true)
{
    clog << "TWorld()" << endl;
}

TWorld::~TWorld()
{
    ps_locker = false;
    clog << "~TWorld()" << endl;
}

TWorld * TWorld::get()
{
    if (world == NULL)
        world = new TWorld();
    return world;
}

void TWorld::newFood(const TVector2d &ip)
{
    lockFood();
    food.push_front(shared_ptr<TFood>(new TFood(ip)));
    unlockFood();
}

void TWorld::killFood(const shared_ptr<TFood> &f)
{
    lockFood();
    auto ef = find(food.begin(), food.end(), f);
    if (ef != food.end())
        food.remove(*ef);
    unlockFood();
}

shared_ptr<TFood> TWorld::findFoodForColony(TPS &ips)
{
    shared_ptr<TFood> ret(nullptr);
    lockFood();
    for (auto f: food)
    {
        if (f->isPure())
            continue;

        // перевірка чи є патікли над кормушкою
        if(find_if(ips.getColony().begin(), ips.getColony().end(),
               [&f](TParticle &i) { return f->isPointInside(i.getP()); } ) != ips.getColony().end())
        {
            // пробуєм заборонити іншим потокам хавати з кормушки
            if (f->tryLock())
            {
                ret = f;
                break;
            }
        }

    }
    unlockFood();
    return ret;
}

void TWorld::render()
{
    for(auto i: food)
        i->render();

    for(auto i: ps)
        i->render();

}

void TWorld::update()
{
   // for(auto i: ps)
   //     i->update();

    // чистимо порожні кормушки
   /* if (TFood::mutListLock().try_lock())
    {
        food.remove_if([](shared_ptr<TFood> &a){ return a->isPure() && a->isFreeEnable(); });
        TFood::mutListLock().unlock();
    }*/
}
