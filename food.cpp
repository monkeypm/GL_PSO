#include "tworld.h"

#define WEIGHT_LINE_W 20.0
#define WEIGHT_LINE_H 3.0
#define WEIGHT_LINE_DELTA 3.0

#define MAX_WEIGHT 10.0f

TFood::TFood(const TVector2d &ip): p(ip), weight(MAX_WEIGHT)
{
    //TWorld::get()->getFood().push_back(shared_ptr<TFood>(this)); // кидаємо себе в World
    ++counter;

    clog << "TFood(" << ip.x << "," << ip.y << ")" << endl;
}

TFood::~TFood()
{
    mut_lock.unlock();
    clog << "~TFood " << p.x << "," << p.y << endl;
}

void TFood::render()
{
    if ( isPure() ) return; // порожняк не малюєм

    const int corners = 10;

    glPushMatrix();
    glTranslated(p.x, p.y, 0.0);
    // малюємо багатокутник
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(0.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    double delta(2.0*M_PI/corners);
    double alpha(0.0);
    for (int i = 0; i <= corners; i++)
    {
        glVertex2f(cos(alpha)*r, sin(alpha)*r);
        alpha += delta;
    }
    glEnd();
    if (weight < MAX_WEIGHT) // якщо об'єкт покоцаний, то малюєм lifeline
    {
        glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(-WEIGHT_LINE_W/2.0,  r+WEIGHT_LINE_DELTA+WEIGHT_LINE_H);
        glVertex2f(WEIGHT_LINE_W/2.0,   r+WEIGHT_LINE_DELTA+WEIGHT_LINE_H);
        glVertex2f(WEIGHT_LINE_W/2.0,   r+WEIGHT_LINE_DELTA);
        glVertex2f(-WEIGHT_LINE_W/2.0,  r+WEIGHT_LINE_DELTA);
        glColor3f(0.0f, 1.0f, 0.0f);
        double rpos(WEIGHT_LINE_W * (weight / MAX_WEIGHT));
        glVertex2f(-WEIGHT_LINE_W/2.0,  r+WEIGHT_LINE_DELTA+WEIGHT_LINE_H);
        glVertex2f(-WEIGHT_LINE_W/2.0+rpos,   r+WEIGHT_LINE_DELTA+WEIGHT_LINE_H);
        glVertex2f(-WEIGHT_LINE_W/2.0+rpos,   r+WEIGHT_LINE_DELTA);
        glVertex2f(-WEIGHT_LINE_W/2.0,  r+WEIGHT_LINE_DELTA);
        glEnd();
    }
    glPopMatrix();
}
