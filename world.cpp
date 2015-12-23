#include "tworld.h"

TWorld * TWorld::world = NULL;

TWorld::TWorld()
{
    clog << "TWorld()" << endl;
}

TWorld::~TWorld()
{
    clog << "~TWorld()" << endl;
}

TWorld * TWorld::get()
{
    if (world == NULL)
        world = new TWorld();
    return world;
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
    food.remove_if([](shared_ptr<TFood> &a){ return a->isPure(); });
}
