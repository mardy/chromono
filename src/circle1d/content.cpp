/**
 * chro.mono: A very circular color puzzle game
 * https://thp.io/2013/chromono/
 * Copyright (C) 2013-2021 Thomas Perl <m@thp.io>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **/


#include "content.h"
#include "behaviors.h"

#include "../constants.h"
#include "../colors.h"

#include <vector>
#include <utility>

#include <stdio.h>

void level1(Scene *scene);
void level3(Scene *scene);
void level4(Scene *scene);
void level5(Scene *scene);

void level11(Scene *scene);
void level12(Scene *scene);
void level13(Scene *scene);
void level14(Scene *scene);
void level15(Scene *scene);
void level15a(Scene *scene);
void level16(Scene *scene);
void level17(Scene *scene);
void level18(Scene *scene);
void level19(Scene *scene);
void level20(Scene *scene);
void level21(Scene *scene);
void level22(Scene *scene);
void level23(Scene *scene);
void level24(Scene *scene);

void level30(Scene *scene);
void level31(Scene *scene);
void level32(Scene *scene);

void level40(Scene *scene);
void level41(Scene *scene);
void level42(Scene *scene);

void level50(Scene *scene);
void level51(Scene *scene);
void level52(Scene *scene);

void level60(Scene *scene);
void level61(Scene *scene);
void level62(Scene *scene);
void level62x(Scene *scene);

void level70(Scene *scene);
void level71(Scene *scene);
void level72(Scene *scene);

void level80(Scene *scene);
void level81(Scene *scene);
void level82(Scene *scene);

void level90(Scene *scene);
void level90x(Scene *scene);
void level91(Scene *scene);
void level92(Scene *scene);

void level25(Scene *scene);
void level26(Scene *scene);
void level27(Scene *scene);

void level35(Scene *scene);

void level36(Scene *scene);
void level37(Scene *scene);
void level38(Scene *scene);

void level43(Scene *scene);
void level44(Scene *scene);

void level45(Scene *scene);
void level46(Scene *scene);
void level47(Scene *scene);

void placeholder_level(Scene *scene) {}

void scene_square(Scene *scene);

#define PLACEHOLDER {{0, 0, 0}, 0, placeholder_level}

LevelInfo levels[] = {
    /* Introductory Levels */
    {{3000, 2000, 1000}, 0, level17}, /* INTRO First level? */
    {{1000, 600, 300}, 0, level30}, /* INTRO Level with 2 colors, 1 target */
    {{1000, 600, 300}, 0, level31}, /* INTRO Level with 2 colors, 2 targets */

    {{3000, 1400, 700}, 6, level80}, /* INTRO level with 4 targets, 1 color */
    {{2000, 1200, 600}, 6, level81}, /* INTRO level with 4 targets, 3 colors */
    {{1800, 1000, 500}, 6, level82}, /* INTRO level with 8 targets, 2 colors */

    {{220, 150, 135}, 12, level50}, /* INTRO level with 2 colors, joints */
    {{600, 330, 210}, 12, level51}, /* INTRO level with 2 colors, undraggable items + joints */
    {{665, 500, 365}, 12, level52}, /* INTRO level with 3 colors, circle */

    // 27 stars up to here

    /* Some of the first levels created */
    {{1500, 1250, 950}, 20, level3}, /* MEDIUM pink, orange, yellow 2 mounted chains */
    {{3000, 2000, 1000}, 20, level91}, /* Hanging dragon used to color things */
    {{700, 400, 310}, 20, level5}, /* EASY sky + magenta dragon */

    /* Chains */
    {{450, 300, 180}, 25, level61}, /* EASY Chain level */
    {{3000, 2000, 1000}, 27, level35}, /* MEDIUM Chain level */
    {{1100, 650, 400}, 30, level60}, /* MEDIUM-HARD colored chain, 3 targets on top */

    // 45 stars up to here

    /* Shapes levels */
    {{3000, 2000, 1000}, 35, level90}, /* Red triangle level */
    {{3000, 2000, 1000}, 35, level36}, /* RELAXING Chain level 4 */
    {{1700, 1200, 900}, 35, level70}, /* MEDIUM-HARD 3 color blue/cyan square */
    {{3000, 2000, 1000}, 42, level37}, /* RELAXING Chain level 5 */
    {{2400, 1900, 1100}, 45, level4}, /* HARD light and dark green circle */

    {{3000, 2000, 1000}, 47, level43}, /* INTRO breakable chain level */
    {{3000, 2000, 1000}, 51, level44}, /* MEDIUM breakable chain level */
    {{550, 350, 225}, 55, level62}, /* HARD Chain that needs to be "cut" / broken */

    // 69 stars up to here

    /* Timing / Accuracy levels */
    {{3000, 2000, 1000}, 60, level92}, /* One, moving left and right */
    {{500, 300, 90}, 62, level72}, /* Two, moving up and down */
    {{700, 400, 310}, 64, level24}, /* Three, moving up and down */

    /* Rails Levels */
    {{400, 250, 190}, 66, level23}, /* moving on rails */
    {{600, 410, 350}, 68, level32}, /* moving on rails, harder */
    {{1200, 900, 700}, 72, level71}, /* horizontal and vertical rails */

    // 87 stars up to here

    /* Multi-Color Levels */
    {{3000, 2000, 1000}, 74, level25}, /* EASY 2 paints, 1 target */
    {{3000, 2000, 1000}, 76, level26}, /* MEDIUM 2 targets, 3 useful paints, 2 useless / unneeded paints */
    {{3000, 2000, 1000}, 79, level27}, /* two-stage multi-color level with unneeded paints */

    {{600, 400, 250}, 82, level13}, /* INTRO Introduction of multi-color objects */
    {{240, 190, 150}, 85, level15}, /* two-stage multi-color level */
    {{2000, 1000, 500}, 87, level15a}, /* two-stage multi-color level with 2 targets */

    // 105 stars up to here

    /* Untangle / Planarity Levels */
    {{3000, 2000, 1000}, 89, level45},
    {{1500, 900, 500}, 91, level40},
    {{1500, 900, 500}, 94, level41},

    {{1500, 900, 500}, 96, level42},
    {{3000, 2000, 1000}, 98, level46},
    {{3000, 2000, 1000}, 100, level47},

    //  123 stars up to here

    /* Special Levels */
    {{550, 350, 225}, 85, level62x}, /* HARD Chain that needs to be "cut" / broken */
    {{3000, 2000, 1000}, 90, level90x}, /* Red triangle level - HARD! */
    {{3000, 2000, 1000}, 95, level38}, /* Chain level 6 (two yellow colorables) */
    {{3000, 2000, 1000}, 105, level16}, /* HARD! more moving things (and switches) */
    {{3000, 2000, 1000}, 115, level11}, /* MEDIUM-HARD First level with moving things */
    {{1200, 480, 320}, 120, level19}, /* HARD multi color level */

    //{{3000, 2000, 1000}, 420, level90x}, /* Just for testing the lock icon */
    /* Emitter Levels */
    //level21, /* INTRO emitting */
    //level20, /* MEDIUM-HARD Emitting */
    //level22, /* MEDIUM Emitting with timing */

    /* XXX - Trash, rebuild/reuse */

    //{{3000, 2000, 1000}, 0, level18}, /* INTRO Switch with arrow */
    //{{3000, 2000, 1000}, 84, level12}, /* INTRO User learns how to use a switch (and nothing more) */
    //{{3000, 2000, 1000}, 84, level14}, /* Level with two switches */
    //level1, /* red blue chain */

    {{0, 0, 0}, 0, NULL}, // sentinel
};

void the_basics_behavior(Object *o);
void ropes_behavior(Object *o);
void puzzles_behavior(Object *o);
void colorize_behavior(Object *o);
void untangle_behavior(Object *o);

LevelPack packs[] = {
    // Level IDs here are zero-based indices into the
    // levels array above.
    {"The Basics", 0, 8, the_basics_behavior, {100, 200, 60, RGB(0.2, 0.7, 0.9), true}},
    {"Ropes", 9, 22, ropes_behavior, {320, 140, 55, RGB(0xe70b92), true}},
    {"Rails", 23, 28, puzzles_behavior, {530, 130, 59, RGB(0.4, 0.9, 0.2), true}},
    {"Mixers", 29, 34, colorize_behavior, {660, 300, 60, RGB(0xc0d700), true}},
    {"Untangle", 35, 40, untangle_behavior, {470, 350, 50, RGB(0.5, 0.9, 0.7), true}},
    {"Challenges", 41, 46, NULL, {270, 320, 65, RGB(0.6, 0.2, 0.6), false}},

    {NULL, 0, 0, NULL, {0, 0, 0, RGB()}}, // sentinel
};


class TheBasicsBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(TheBasicsBehavior)

        TheBasicsBehavior(Scene *scene, Object *center)
            : Circle1DBehavior(scene)
            , center(center)
            , moon_size(22)
        {
            RGB c = RGB::mix(RGB(0.0, 0.0, 0.0), center->target_color);
            moons[0] = new Object(scene, 0, 0, moon_size, Object::FIXED, c);
            moons[1] = new Object(scene, 0, 0, moon_size, Object::FIXED, c);

            simulate();
        }

        virtual ~TheBasicsBehavior()
        {
        }

        virtual void simulate()
        {
            for (int i=0; i<2; i++) {
                float t = 0.02 * scene->time;
                float s = sinf(t + M_PI * i);
                float c = cosf(t + M_PI * i);
                moons[i]->size = std::max(0.f, moon_size * (1.f + c) / 2.f - 4.f);

                Vec2 offset(s * 70.0, c * 40.0);
                moons[i]->pos = center->pos + offset;
            }
        }

    private:
        Object *center;
        float moon_size;
        Object *moons[2];
};

void
the_basics_behavior(Object *o)
{
    new TheBasicsBehavior(o->scene, o);
}

class RopesBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(RopesBehavior)

        RopesBehavior(Scene *scene, Object *center)
            : Circle1DBehavior(scene)
            , center(center)
            , handle(NULL)
        {
            handle = new Object(scene, center->pos.x + center->size,
                    center->pos.y - center->size - 15,
                    15, Object::FIXED, RGB::mix(RGB(0x000000), center->target_color));
            simulate();

            Object *prev = handle;
            for (int i=0; i<3; i++) {
                Object *cur = new Object(scene, handle->pos.x,//center->pos.x + center->size,
                        center->pos.y - center->size + 25 * (i + 1),
                        15, Object::COLLIDER, RGB(0.8, 0.8, 0.9));

                new Joint(scene, prev, cur);
                prev = cur;
            }
        }

        virtual ~RopesBehavior()
        {
        }

        virtual void simulate()
        {
            float t = 0.015 * (scene->time + 100);
            handle->pos.x = center->pos.x + 90.0 * sinf(t);
        }

    private:
        Object *center;
        Object *handle;
};

void
ropes_behavior(Object *o)
{
    new RopesBehavior(o->scene, o);
}

class PuzzlesBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(PuzzlesBehavior)

        PuzzlesBehavior(Scene *scene, Object *center)
            : Circle1DBehavior(scene)
            , movable(NULL)
        {
            movable = new Object(scene, center->pos.x + center->size + 10,
                    center->pos.y - center->size + 20, 15,
                    Object::COLLIDER, RGB::mix(RGB(0x000000), center->target_color));
            new AxisConstraintBehavior(scene, movable, true, movable->pos.x,
                    center->pos.y - center->size / 2.0, center->pos.y + center->size / 2.0);
        }

        virtual ~PuzzlesBehavior()
        {
        }

        virtual void simulate()
        {
            float t = 0.01 * scene->time;
            movable->apply_force(Vec2(0.0, -20.0 * std::max(0.f, cosf(t))));
        }

    private:
        Object *movable;
};

void
puzzles_behavior(Object *o)
{
    new PuzzlesBehavior(o->scene, o);
}

class ColorizeBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(ColorizeBehavior)

        ColorizeBehavior(Scene *scene, Object *center)
            : Circle1DBehavior(scene)
            , ticks(0)
        {
            objects[0] = new Object(scene, center->pos.x + center->size + 5,
                    center->pos.y - center->size * 0.8, 20,
                    Object::FIXED | Object::COLORABLE | Object::NOAUTOCOLOR,
                    RGB::mix(RGB(0xffffff), center->target_color));
            objects[1] = new Object(scene, center->pos.x + center->size + 5,
                    center->pos.y + center->size * 0.8, 20, objects[0]->flags,
                    RGB::mix(RGB(0x000000), center->target_color));

            // black taken from build_color_mix
            objects[0]->desired = objects[1]->desired = RGB(0.2, 0.2, 0.2);
            new Joint(scene, center, objects[0]);
            new Joint(scene, center, objects[1]);
        }

        virtual ~ColorizeBehavior()
        {
        }

        virtual void simulate()
        {
            ticks++;
            if (ticks == 100) {
                std::swap(objects[0]->target_color, objects[1]->target_color);
                ticks = 0;
            }
        }

    private:
        int ticks;
        Object *objects[2];
};

void
colorize_behavior(Object *o)
{
    new ColorizeBehavior(o->scene, o);
}

class UntangleBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(UntangleBehavior)

        UntangleBehavior(Scene *scene, Object *center)
            : Circle1DBehavior(scene)
            , center(center)
        {
            RGB color = RGB::mix(RGB(0x000000), center->target_color);

            a = new Object(scene, center->pos.x - center->size - 40,
                    center->pos.y - center->size - 27, 15,
                    Object::FIXED | Object::COLLIDER, color);
            b = new Object(scene, center->pos.x - 40,
                    center->pos.y - center->size - 60, 15,
                    Object::FIXED | Object::COLLIDER, RGB());
            c = new Object(scene, center->pos.x + center->size - 40,
                    center->pos.y - center->size - 20, 15,
                    Object::FIXED | Object::COLLIDER, color);
            d = new Object(scene, center->pos.x - 40,
                    center->pos.y - center->size - 10, 15,
                    Object::FIXED | Object::COLLIDER, color);

            new Joint(scene, a, b, Joint::UNTANGLE);
            new Joint(scene, b, c, Joint::UNTANGLE);
            new Joint(scene, d, b, Joint::UNTANGLE);
            new Joint(scene, a, c, Joint::UNTANGLE);

            simulate();
        }

        virtual ~UntangleBehavior()
        {
        }

        virtual void simulate()
        {
            float t = 0.023 * scene->time;
            d->pos.y = center->pos.y - center->size - 10 - 20 * (0.5 + 0.5 * sinf(t));
            d->pos.x = center->pos.x - 40 + 20 * (0.5 + 0.5 * cosf(t * 0.9));

            c->pos.y = center->pos.y - center->size - 20 - 20 * (0.5 + 0.5 * cosf(t * 1.3));
            scene->checkUntanglement();
        }

    private:
        Object *center;
        Object *a;
        Object *b;
        Object *c;
        Object *d;
};

void
untangle_behavior(Object *o)
{
    new UntangleBehavior(o->scene, o);
}

void
level36(Scene *scene)
{
    RGB target(0x406849);

    Object *center = new Object(scene, 400, 240, 50, Object::NOGRAVITY |
            Object::COLLIDER | Object::BORDERBOUNCE, target);

    Object *prev[4] = {center, center, center, center};
    Vec2 dir[4] = {
        Vec2(-1, -1),
        Vec2(-1, +1),
        Vec2(+1, -1),
        Vec2(+1, +1),
    };

    for (int i=0; i<2; i++) {
        for (int j=0; j<4; j++) {
            int x = center->pos.x + dir[j].x * 100 * (i+2) / (i+1);
            int y = center->pos.y + dir[j].y * 100 * (i+2) / (i+1);
            Object *cur = new Object(scene, x, y, 30, Object::COLORABLE |
                    Object::NOGRAVITY | Object::COLLIDER |
                    Object::BORDERBOUNCE | Object::NODRAG);
            cur->desired = target;
            new Joint(scene, prev[j], cur);
            prev[j] = cur;
        }
    }
}

void
level37(Scene *scene)
{
    RGB target1(0x2b556d);
    RGB target2(0xab7425);

    Object *prev = NULL;
    for (int i=0; i<15; i++) {
        int x = 15 + 55 * i;
        int y = 240 + 220. * sinf((float)i / 14.f * M_PI * 2.0);
        Object *cur = new Object(scene, x, y, 30, Object::COLORABLE | Object::COLLIDER |
                Object::NODRAG | Object::NOGRAVITY | Object::BORDERBOUNCE);
        if (i < 5) {
            cur->desired = target1;
        } else if (i > 9) {
            cur->desired = target2;
        } else {
            cur->desired = (i % 2 == 0) ? target1 : target2;
        }

        if (prev != NULL) {
            new Joint(scene, prev, cur, Joint::RUBBERBAND);
        }
        prev = cur;
    }

    new Object(scene, 300, 140, 50, Object::COLLIDER | Object::NOGRAVITY | Object::STAYINVIEW, target1);
    new Object(scene, 500, 320, 50, Object::COLLIDER | Object::NOGRAVITY | Object::STAYINVIEW, target2);
}

void
level43(Scene *scene)
{
    RGB black(0x222222);
    RGB troll(0x009f5e);

    Object *nail = new Object(scene, 240, 240, 20, Object::FIXED | Object::COLLIDER, black);
    Object *paint = new Object(scene, 380, 240, 50, Object::BORDERBOUNCE | Object::COLLIDER, troll);
    std::vector<Object*> chain = build_chain(scene, nail, paint, 5, Joint::STRONG | Joint::BREAKABLE);

    (new Object(scene, 630, 40, 50, Object::FIXED | Object::COLORABLE | Object::COLLIDER))->desired = troll;
}

void
level44(Scene *scene)
{
    // 20
    RGB black(0x333333);
    RGB white(0xcccccc);
    RGB vio(0x571da0);

    Object *nail = new Object(scene, 400, 0, 20, Object::FIXED | Object::COLLIDER, black);
    Object *target = new Object(scene, 700, 300, 50, Object::FIXED | Object::COLORABLE | Object::COLLIDER);
    target->desired = vio;

    new Object(scene, 270, 170, 80, Object::FIXED | Object::COLLIDER);

    new Object(scene, 400, 300, 80, Object::FIXED | Object::COLLIDER);

    new Object(scene, 280, 20, 50, Object::COLLIDER | Object::BORDERBOUNCE | Object::NODRAG, vio);

    Object *handle = new Object(scene, 250, 370, 40, Object::COLLIDER | Object::BORDERBOUNCE, white);

    build_chain(scene, nail, handle, 17, Joint::STRONG | Joint::BREAKABLE);
}


void
level38(Scene *scene)
{
    RGB sky(0xc0d700);
    RGB white(0xdddddd);

    Object *paint = new Object(scene, 100, 200, 30, Object::COLLIDER | Object::NODRAG | Object::BORDERBOUNCE, sky);
    Object *handle = new Object(scene, 400, 200, 50, Object::COLLIDER | Object::BORDERBOUNCE, white);
    build_chain(scene, paint, handle, 16, Joint::STRONG);

    new Object(scene, 200, 280, 60, Object::COLLIDER | Object::FIXED | Object::NOOVERLAP);

    (new Object(scene, 400, 80, 60, Object::COLLIDER | Object::COLORABLE | Object::FIXED | Object::NOOVERLAP))->desired = sky;
    (new Object(scene, 500, 280, 60, Object::COLLIDER | Object::COLORABLE | Object::FIXED | Object::NOOVERLAP))->desired = sky;
}

void
level35(Scene *scene)
{
    RGB pink(0xe70b92);
    RGB white(0xeeeeee);

    // Chain level
    Object *anchor = new Object(scene, 380, 200, 70, Object::FIXED | Object::COLLIDER | Object::NOOVERLAP);
    new ObjectIsLightSourceBehavior(scene, anchor);

    (new Object(scene, 240, 30, 60, Object::FIXED | Object::COLLIDER | Object::COLORABLE))->desired = pink;

    Object *paint = new Object(scene, 300, 200, 30, Object::COLLIDER |
            Object::BORDERBOUNCE | Object::NODRAG, pink);

    Object *handle = new Object(scene, 500, 100, 50, Object::COLLIDER |
            Object::BORDERBOUNCE, white);

    bezier_chain(scene, paint, handle, Vec2(300, -100), 10, Joint::RUBBERBAND | Joint::STRONG);
}

void
level90(Scene *scene)
{
    RGB color1(0xcc3322);
    RGB color2(0xffaa55);

    Object *p1 = new Object(scene, 400, 220, 40, Object::NOGRAVITY | Object::COLLIDER | Object::SHADOWCAST | Object::BORDERBOUNCE, color1);
    Object *p2 = new Object(scene, 400, 320, 40, Object::NOGRAVITY | Object::COLLIDER | Object::SHADOWCAST | Object::BORDERBOUNCE, color2);
    new Joint(scene, p1, p2);

    CenterOfObjectsLightSource *cools = new CenterOfObjectsLightSource(scene);
    cools->add(p1);
    cools->add(p2);

    Object *top = new Object(scene, 400, 80, 15, Object::NOGRAVITY | Object::NODRAG |
            Object::COLLIDER | Object::COLORABLE | Object::BORDERBOUNCE | Object::SHADOWCAST);
    top->desired = color1;

    Object *left = new Object(scene, 210, 400, top->size, top->flags);
    left->desired = color1;

    Object *right = new Object(scene, 590, 400, top->size, top->flags);
    right->desired = color1;

    Object *from[3] = {top, left, right};
    Object *to[3] = {right, top, left};
    Object *prev[3] = {from[0], from[1], from[2]};

    int steps = 8;
    for (int i=0; i<steps; i++) {
        float alpha = (float)(i+1) / (float)(steps + 1);
        for (int j=0; j<3; j++) {
            Vec2 pos = (to[j]->pos * alpha) + (from[j]->pos * (1.f - alpha));
            Object *cur = new Object(scene, pos.x, pos.y, top->size, top->flags);
            cur->desired = (i < 2 || i > steps-3) ? color1 : color2;
            new Joint(scene, prev[j], cur);
            prev[j] = cur;
        }
    }

    for (int j=0; j<3; j++) {
        new Joint(scene, prev[j], to[j]);
    }
}

void
level90x(Scene *scene)
{
    RGB color1(0x33cc22);
    RGB color2(0xff55aa);

    Object *p2 = new Object(scene, 400, 160, 40, Object::NOGRAVITY | Object::COLLIDER | Object::SHADOWCAST | Object::NODRAG, color2);
    Object *p1 = new Object(scene, 400, 260, 40, Object::NOGRAVITY | Object::COLLIDER | Object::SHADOWCAST, color1);
    new Joint(scene, p1, p2);

    CenterOfObjectsLightSource *cools = new CenterOfObjectsLightSource(scene);
    cools->add(p1);
    cools->add(p2);

    Object *top = new Object(scene, 400, 400, 20, Object::NOGRAVITY | Object::NODRAG |
            Object::COLLIDER | Object::COLORABLE | Object::BORDERBOUNCE);
    top->desired = color1;

    Object *left = new Object(scene, 210, 80, top->size, top->flags);
    left->desired = color1;

    Object *right = new Object(scene, 590, 80, top->size, top->flags);
    right->desired = color1;

    Object *from[3] = {top, left, right};
    Object *to[3] = {right, top, left};
    Object *prev[3] = {from[0], from[1], from[2]};

    int steps = 8;
    for (int i=0; i<steps; i++) {
        float alpha = (float)(i+1) / (float)(steps + 1);
        for (int j=0; j<3; j++) {
            Vec2 pos = (to[j]->pos * alpha) + (from[j]->pos * (1.f - alpha));
            Object *cur = new Object(scene, pos.x, pos.y, top->size, top->flags);
            cur->desired = (i < 2 || i > steps-3) ? color1 : color2;
            new Joint(scene, prev[j], cur);
            prev[j] = cur;
        }
    }

    for (int j=0; j<3; j++) {
        new Joint(scene, prev[j], to[j]);
    }
}


void
level91(Scene *scene)
{
    /* Hanging dragon used to color things */
    RGB black(0.2, 0.2, 0.2);

    RGB alpha(0.2, 0.9, 0.5);
    RGB beta(0.9, 0.9, 0.2);

    Object *nail = new Object(scene, 400, 50, 20, Object::FIXED, black);

    Object *a = new Object(scene, 400, 100, 30, Object::COLLIDER | Object::NODRAG, alpha);
    Object *b = new Object(scene, 360, 150, 30, a->flags, beta);
    Object *c = new Object(scene, 440, 150, 30, a->flags, beta);
    Object *d = new Object(scene, 400, 230, 50, Object::COLLIDER, alpha);

    new Joint(scene, nail, a, Joint::RUBBERBAND | Joint::STRONG);
    new Joint(scene, a, b, Joint::FIXED);
    new Joint(scene, a, c, Joint::FIXED);
    new Joint(scene, b, c, Joint::FIXED);
    new Joint(scene, b, d, Joint::FIXED);
    new Joint(scene, c, d, Joint::FIXED);
    new Joint(scene, a, d, Joint::FIXED);

    (new Object(scene, 200, 200, 50, Object::COLORABLE | Object::FIXED | Object::COLLIDER))->desired = alpha;
    (new Object(scene, 600, 200, 50, Object::COLORABLE | Object::FIXED | Object::COLLIDER))->desired = beta;
    (new Object(scene, 180, 400, 50, Object::COLORABLE | Object::FIXED | Object::COLLIDER))->desired = beta;
    (new Object(scene, 620, 400, 50, Object::COLORABLE | Object::FIXED | Object::COLLIDER))->desired = alpha;
}

void
level92(Scene *scene)
{
    /* One, moving left and right */
    RGB targetcolor(0x11dd44);
    RGB stopcolor(0x779988);

    int top_row_y = 220;

    int left_x = 400 - 200;
    int right_x = 400 + 200;

    int size = 50;
    int tsize = 30;
    int target_size = 26;

    int overlap = 5;
    int pendulum_x_min = left_x + tsize + target_size - overlap;
    int pendulum_x_max = right_x - tsize - target_size + overlap;

    Object *target_left = new Object(scene, left_x + 300, top_row_y,
            target_size, Object::COLORABLE | Object::COLLIDER | Object::FIXED);
    target_left->desired = targetcolor;

    new AxisConstraintBehavior(scene, target_left, false, top_row_y, left_x+tsize,
            right_x-tsize);
    new XAxisPendulumBehavior(scene, target_left, pendulum_x_min,
            pendulum_x_max, 0.055, 20.0);

    new Object(scene, left_x, top_row_y, tsize,
            Object::FIXED | Object::COLLIDER, stopcolor);

    new Object(scene, right_x, top_row_y,
            tsize, Object::FIXED | Object::COLLIDER, stopcolor);

    new Object(scene, 400, 360, size,
            Object::COLLIDER | Object::NOGRAVITY | Object::BORDERBOUNCE,
            targetcolor);
}

void
level80(Scene *scene)
{
    /* INTRO level with 4 targets, 1 color */
    RGB color(0xaaffee);

    (new Object(scene, 200, 100, 50, Object::FIXED | Object::SHADOWCAST | Object::COLLIDER | Object::COLORABLE))->desired = color;
    (new Object(scene, 600, 100, 50, Object::FIXED | Object::SHADOWCAST | Object::COLLIDER | Object::COLORABLE))->desired = color;
    (new Object(scene, 200, 380, 50, Object::FIXED | Object::SHADOWCAST | Object::COLLIDER | Object::COLORABLE))->desired = color;
    (new Object(scene, 600, 380, 50, Object::FIXED | Object::SHADOWCAST | Object::COLLIDER | Object::COLORABLE))->desired = color;

    Object *c = new Object(scene, 400, 240, 60, Object::BORDERBOUNCE | Object::COLLIDER | Object::NOGRAVITY, color);
    new ObjectIsLightSourceBehavior(scene, c);
}

void
level81(Scene *scene)
{
    /* INTRO level with 4 targets, 3 colors */
    RGB color1(0xee9933);
    RGB color2(0xee1199);
    RGB color3(0x9977ee);

    int flags = Object::FIXED | Object::COLLIDER | Object::COLORABLE;
    Object *o1 = new Object(scene, 400, 100, 30, flags);
    new ObjectIsLightSourceBehavior(scene, o1);
    o1->desired = color1;
    flags |= Object::SHADOWCAST;
    (new Object(scene, 220, 100, 50, flags))->desired = color2;
    (new Object(scene, 400, 380, 40, flags))->desired = color3;
    (new Object(scene, 610, 380, 60, flags))->desired = color1;

    flags = Object::BORDERBOUNCE | Object::COLLIDER | Object::NOGRAVITY | Object::SHADOWCAST;
    new Object(scene, 400, 240, 60, flags, color1);
    new Object(scene, 600, 160, 60, flags, color2);
    new Object(scene, 200, 300, 60, flags, color3);
}

void
level82(Scene *scene)
{
    /* INTRO level with 8 targets, 2 colors */
    RGB color1(0x460145);
    RGB color2(0x276786);

    scene->set_custom_background_color(RGB::mix(RGB(1.0, 1.0, 1.0), color1, 0.4));

    for (int i=0; i<8; i++) {
        int x = 240 + (i / 4) * 500;
        x += (-i * 30);
        int y = 100 + (i % 4) * 100;
        RGB c = ((i % 2) ^ (i / 4)) ? color1 : color2;
        (new Object(scene, x, y, 50, Object::FIXED | Object::COLLIDER | Object::COLORABLE))->desired = c;
    }

    new Object(scene, 380, 350, 60, Object::BORDERBOUNCE | Object::COLLIDER | Object::NOGRAVITY, color1);
    new Object(scene, 360, 200, 60, Object::BORDERBOUNCE | Object::COLLIDER | Object::NOGRAVITY, color2);
}

void level72(Scene *scene)
{
    RGB targetcolor(0x1166dd);
    RGB stopcolor(0xaa9eef);

    int top_row_y = 120;
    int bottom_row_y = 480 - 120;

    int left_x = 400 - 100;
    int right_x = 400 + 100;

    int size = 50;
    int tsize = 30;
    int target_size = 26;

    int overlap = 5;
    int pendulum_y_min = top_row_y + tsize + target_size - overlap;
    int pendulum_y_max = bottom_row_y - tsize - target_size + overlap;

    Object *target_left = new Object(scene, left_x, bottom_row_y - 100,
            target_size, Object::COLORABLE | Object::COLLIDER | Object::FIXED);
    target_left->desired = targetcolor;

    new AxisConstraintBehavior(scene, target_left, true, left_x, top_row_y+tsize,
            bottom_row_y-tsize);
    new YAxisPendulumBehavior(scene, target_left, pendulum_y_min,
            pendulum_y_max, 0.02, 20.0);

    Object *target_right = new Object(scene, right_x, bottom_row_y - 110,
            target_size, Object::COLORABLE | Object::COLLIDER | Object::FIXED);
    target_right->desired = targetcolor;

    new AxisConstraintBehavior(scene, target_right, true, right_x,
            top_row_y+tsize, bottom_row_y-tsize);
    new YAxisPendulumBehavior(scene, target_right, pendulum_y_min,
            pendulum_y_max, 0.028, 14.0);

    new Object(scene, left_x, top_row_y, tsize,
            Object::FIXED | Object::COLLIDER, stopcolor);

    new Object(scene, left_x, bottom_row_y,
            tsize, Object::FIXED | Object::COLLIDER, stopcolor);

    new Object(scene, right_x, top_row_y, tsize,
            Object::FIXED | Object::COLLIDER, stopcolor);

    new Object(scene, right_x, bottom_row_y,
            tsize, Object::FIXED | Object::COLLIDER, stopcolor);

    new Object(scene, 400, 240, size,
            Object::COLLIDER | Object::NOGRAVITY | Object::BORDERBOUNCE,
            targetcolor);
}

void level71(Scene *scene)
{
    RGB yellow(0.8, 0.9, 0.3);
    RGB cyan(0.1, 0.8, 0.9);
    RGB magenta(0.6, 0.3, 0.9);

    for (int i=0; i<6; i++) {
        Object *target = new Object(scene, 185 + i*90, 240, 60,
                Object::FIXED | Object::COLLIDER | Object::COLORABLE);
        switch (i % 3) {
            case 0: target->desired = yellow; break;
            case 1: target->desired = cyan; break;
            case 2: target->desired = magenta; break;
        }
    }

    int rails_top = 105;
    int rails_bottom = 240 + (240 - rails_top);
    int rails_left = 100;
    int rails_right = 700;

    Object *movable2 = new Object(scene, 120, 0, 80, Object::NOGRAVITY | Object::COLLIDER, cyan);
    new AxisConstraintBehavior(scene, movable2, false, rails_top, rails_left, rails_right);

    Object *movable3 = new Object(scene, 320, 0, 80, Object::NOGRAVITY | Object::COLLIDER, magenta);
    new AxisConstraintBehavior(scene, movable3, false, rails_top, rails_left, rails_right);

    // movable 2 must always be left of movable 3
    new XAxisOrderConstraint(scene, movable2, movable3);

    Object *movable1 = new Object(scene, 500, 0, 80, Object::NOGRAVITY | Object::COLLIDER, yellow);
    new AxisConstraintBehavior(scene, movable1, false, rails_bottom, rails_left, rails_right);

    Object *movable4 = new Object(scene, 680, 0, 80, Object::NOGRAVITY | Object::COLLIDER, cyan);
    new AxisConstraintBehavior(scene, movable4, false, rails_bottom, rails_left, rails_right);

    // movable 1 must always be left of movable 4
    new XAxisOrderConstraint(scene, movable1, movable4);
}

void level70(Scene *scene)
{
    RGB blue1(0x1eafe5);
    RGB blue2(0x0df3d5);
    RGB blue3(0x003b84);

    Object *paint1 = new Object(scene, 400, 200, 40, Object::COLLIDER | Object::NOGRAVITY | Object::BORDERBOUNCE | Object::SHADOWCAST, blue1);
    Object *paint2 = new Object(scene, 350, 280, 40, Object::COLLIDER | Object::NOGRAVITY | Object::BORDERBOUNCE | Object::SHADOWCAST, blue2);
    Object *paint3 = new Object(scene, 450, 280, 40, Object::COLLIDER | Object::NOGRAVITY | Object::BORDERBOUNCE | Object::SHADOWCAST, blue3);

    CenterOfObjectsLightSource *cools = new CenterOfObjectsLightSource(scene);
    cools->add(paint1);
    cools->add(paint2);
    cools->add(paint3);

    new Joint(scene, paint1, paint2);
    new Joint(scene, paint2, paint3);
    new Joint(scene, paint1, paint3);

    Object *grid[7][7];
    for (int x=0; x<7; x++) {
        for (int y=0; y<7; y++) {
            if (x == 0 || x == 6 || y == 0 || y == 6) {
                float xp = 400 + 60 * (x - 3);
                float yp = 240 + 60 * (y - 3);

                Object *o = new Object(scene, xp, yp, 20, Object::NOGRAVITY | Object::COLLIDER | Object::COLORABLE | Object::NODRAG | Object::BORDERBOUNCE | Object::SHADOWCAST);
                if ((std::abs(x - y) < 2) || (std::abs(x - (6-y)) < 2)) {
                    o->desired = blue3;
                } else if (x > y) {
                    o->desired = blue2;
                } else {
                    o->desired = blue1;
                }
                grid[x][y] = o;
            }
        }
    }

    for (int x=0; x<6; x++) {
        new Joint(scene, grid[x][0], grid[x+1][0]);
        new Joint(scene, grid[x][6], grid[x+1][6]);
        new Joint(scene, grid[0][x], grid[0][x+1]);
        new Joint(scene, grid[6][x], grid[6][x+1]);
    }

    /* MEDIUM-HARD 3 color blue/cyan square*/
}

void level62x(Scene *scene)
{
    RGB black(0.2, 0.2, 0.2);
    RGB white(0.8, 0.8, 0.8);

    RGB alpha(0.9, 0.4, 0.2);
    RGB beta(0.5, 0.2, 0.9);

    Object *nail1 = new Object(scene, 100, 50, 20, Object::FIXED, black);
    Object *nail2 = new Object(scene, 700, 50, 20, Object::FIXED, black);

    int width = (nail2->pos.x - nail1->pos.x);
    int steps = 7;
    float spacing2 = width / (steps + 1);

    Object *prev = nail1;
    for (int i=0; i<steps; i++) {
        Object *cur = new Object(scene, nail1->pos.x + (i+1) * spacing2, 50, 20, Object::COLLIDER | Object::NODRAG |
                Object::BORDERBOUNCE, (i < steps / 2) ? alpha : beta);
        if (i == 0) {
            new Joint(scene, prev, cur);
            Object *handle1 = new Object(scene, cur->pos.x, cur->pos.y + 100, 50, Object::BORDERBOUNCE | Object::COLLIDER, white);
            new Joint(scene, handle1, cur, Joint::STRONG);
        } else {
            new Joint(scene, prev, cur, Joint::STRONG);
        }
        prev = cur;
    }
    Object *handle1 = new Object(scene, prev->pos.x, prev->pos.y + 100, 50, Object::BORDERBOUNCE | Object::COLLIDER, white);
    new Joint(scene, handle1, prev, Joint::STRONG);
    new Joint(scene, prev, nail2);

    Object *target1 = new Object(scene, 300, 450, 60, Object::FIXED | Object::COLLIDER | Object::COLORABLE);
    target1->desired = alpha;

    Object *target2 = new Object(scene, 500, 450, 60, Object::FIXED | Object::COLLIDER | Object::COLORABLE);
    target2->desired = beta;
}


void level62(Scene *scene)
{
    RGB black(0.2, 0.2, 0.2);
    RGB white(0.8, 0.8, 0.8);

    RGB alpha(0.2, 0.4, 0.9);
    RGB beta(0.4, 0.9, 0.2);

    Object *nail1 = new Object(scene, 100, 50, 20, Object::FIXED, black);
    Object *nail2 = new Object(scene, 700, 50, 20, Object::FIXED, black);

    int width = (nail2->pos.x - nail1->pos.x);
    int steps = 7;
    float spacing2 = width / (steps + 1);

    Object *prev = nail1;
    for (int i=0; i<steps; i++) {
        Object *cur = new Object(scene, nail1->pos.x + (i+1) * spacing2, 50, 20, Object::COLLIDER | Object::NODRAG |
                Object::BORDERBOUNCE, (i < steps / 2) ? alpha : beta);
        if (i == 0) {
            new Joint(scene, prev, cur, Joint::BREAKABLE);
            Object *handle1 = new Object(scene, cur->pos.x, cur->pos.y + 100, 50, Object::BORDERBOUNCE | Object::COLLIDER, white);
            new Joint(scene, handle1, cur, Joint::STRONG);
        } else {
            new Joint(scene, prev, cur, Joint::STRONG);
        }
        prev = cur;
    }
    Object *handle1 = new Object(scene, prev->pos.x, prev->pos.y + 100, 50, Object::BORDERBOUNCE | Object::COLLIDER, white);
    new Joint(scene, handle1, prev, Joint::STRONG);
    new Joint(scene, prev, nail2, Joint::BREAKABLE);

    Object *target1 = new Object(scene, 300, 450, 60, Object::FIXED | Object::COLLIDER | Object::COLORABLE);
    target1->desired = beta;

    Object *target2 = new Object(scene, 500, 450, 60, Object::FIXED | Object::COLLIDER | Object::COLORABLE);
    target2->desired = alpha;
}

void level61(Scene *scene)
{
    RGB white(0.7, 0.7, 0.7);

    RGB green(0.9, 0.5, 0.1);
    RGB red(0.9, 0.1, 0.5);
    RGB blue(0.7, 0.9, 0.2);

    new Object(scene, 100, 220, 70, Object::FIXED | Object::COLLIDER, white);
    new Object(scene, 250, 220, 70, Object::FIXED | Object::COLLIDER, white);
    new Object(scene, 400, 220, 70, Object::FIXED | Object::COLLIDER, white);
    new Object(scene, 550, 220, 70, Object::FIXED | Object::COLLIDER, white);
    new Object(scene, 700, 220, 70, Object::FIXED | Object::COLLIDER, white);

    Object *first = NULL;
    Object *prev = NULL;
    int count = 15;
    for (int i=0; i<count; i++) {
        float p = (float)i / (count - 1);
        int x = 400 + 200 * cosf(M_PI * p);
        int y = 220 - 120 * sinf(M_PI * p);
        Object *cur = new Object(scene, x, y, 10, Object::COLLIDER | Object::BORDERBOUNCE | Object::NODRAG);
        if (first == NULL) {
            first = cur;
        }
        if (prev != NULL) {
            new Joint(scene, prev, cur, Joint::RUBBERBAND | Joint::STRONG);
        }
        if (i == count / 2) {
            Object *handle = new Object(scene, x, y-70, 50, Object::COLLIDER | Object::BORDERBOUNCE, green);
            new Joint(scene, cur, handle);
        }
        prev = cur;
    }

    Object *paint1 = new Object(scene, first->pos.x, first->pos.y + 30, 20, Object::COLLIDER | Object::BORDERBOUNCE | Object::NODRAG, red);
    new Joint(scene, first, paint1);
    Object *paint2 = new Object(scene, prev->pos.x, prev->pos.y + 30, 20, Object::COLLIDER | Object::BORDERBOUNCE | Object::NODRAG, blue);
    new Joint(scene, prev, paint2);

    new Object(scene, first->pos.x, 450, 30, Object::FIXED | Object::COLLIDER, white);
    new Object(scene, prev->pos.x, 450, 30, Object::FIXED | Object::COLLIDER, white);

    Object *target1 = new Object(scene, first->pos.x + 100, 400, 30, Object::COLLIDER | Object::COLORABLE | Object::BORDERBOUNCE | Object::NODRAG);
    target1->desired = red;
    Object *target2 = new Object(scene, prev->pos.x - 100, 400, 30, Object::COLLIDER | Object::COLORABLE | Object::BORDERBOUNCE | Object::NODRAG);
    target2->desired = blue;
}

void level60(Scene *scene)
{
    RGB black(0.1, 0.1, 0.1);
    RGB grey(0.3, 0.3, 0.3);
    RGB white(0.8, 0.8, 0.8);

    Object *nail1 = new Object(scene, 100, 20, 10, Object::FIXED, black);
    Object *holder1 = new Object(scene, 100, 150, 40, Object::COLLIDER | Object::NODRAG, grey);
    new Joint(scene, nail1, holder1, Joint::FIXED);
    Object *handle1 = new Object(scene, 100, 230, 60, Object::COLLIDER, white);
    new Joint(scene, holder1, handle1);

    Object *nail2 = new Object(scene, 700, 20, 10, Object::FIXED, black);
    Object *holder2 = new Object(scene, 700, 150, 40, Object::COLLIDER | Object::NODRAG, grey);
    new Joint(scene, nail2, holder2, Joint::FIXED);
    Object *handle2 = new Object(scene, 700, 230, 60, Object::COLLIDER, white);
    new Joint(scene, holder2, handle2);

    RGB colors[] = {
        grey,
        grey,
        grey,

        RGB(0.7, 0.9, 0.5), //
        RGB(0.9, 0.7, 0.5), //
        RGB(0.5, 0.7, 0.9), //

        grey,
        grey,
        grey,
    };

    Object *prev = holder1;
    for (int i=0; i<9; i++) {
        Object *cur = new Object(scene, 170 + i * 57, 160, 10, Object::COLLIDER | Object::NODRAG, colors[i]);
        new Joint(scene, prev, cur, Joint::FIXED);
        prev = cur;
    }
    new Joint(scene, prev, holder2, Joint::FIXED);

    Object *target1 = new Object(scene, 300, 100, 40, Object::FIXED | Object::COLLIDER | Object::COLORABLE);
    target1->desired = colors[3];

    Object *target2 = new Object(scene, 400, 80, 40, Object::FIXED | Object::COLLIDER | Object::COLORABLE);
    target2->desired = colors[4];

    Object *target3 = new Object(scene, 500, 100, 40, Object::FIXED | Object::COLLIDER | Object::COLORABLE);
    target3->desired = colors[5];
}

void level50(Scene *scene)
{
    RGB yellow(0.8, 0.6, 0.3);
    RGB blue(0.3, 0.3, 0.8);

    Object *target1 = new Object(scene, 600, 80, 70, Object::COLLIDER | Object::FIXED | Object::COLORABLE);
    target1->desired = yellow;

    Object *target2 = new Object(scene, 600, 400, 70, Object::COLLIDER | Object::FIXED | Object::COLORABLE);
    target2->desired = blue;

    Object *paint1 = new Object(scene, 200, 300, 50, Object::COLLIDER | Object::NOGRAVITY | Object::BORDERBOUNCE, yellow);
    Object *paint2 = new Object(scene, 200, 180, 50, Object::COLLIDER | Object::NOGRAVITY | Object::BORDERBOUNCE, blue);

    new Joint(scene, paint1, paint2, Joint::FIXED);
}

void level51(Scene *scene)
{
    RGB greenish(0.6, 0.9, 0.1);
    RGB veilchen(0xd909b9);

    int flags = Object::COLLIDER | Object::FIXED | Object::COLORABLE;

    Object *target1 = new Object(scene, 300, 90, 50, flags);
    target1->desired = greenish;

    Object *target2 = new Object(scene, 300, 190, 50, flags);
    target2->desired = veilchen;

    Object *target3 = new Object(scene, 300, 290, 50, flags);
    target3->desired = greenish;

    Object *target4 = new Object(scene, 300, 390, 50, flags);
    target4->desired = veilchen;

    flags = Object::COLLIDER | Object::NOGRAVITY | Object::BORDERBOUNCE;
    Object *paint1 = new Object(scene, 400, 240, 50, flags | Object::NODRAG, greenish);
    Object *paint2 = new Object(scene, 570, 240, 50, flags, veilchen);

    new Joint(scene, paint1, paint2, Joint::FIXED);
}

void level52(Scene *scene)
{
    RGB orange(0xffa300);
    RGB lime(0x00ff8a);
    RGB red(0xc30909);

    scene->light_source.enabled = true;
    scene->light_source.pos = Vec2(400, 480);

    int flags = Object::COLLIDER | Object::NOGRAVITY | Object::BORDERBOUNCE | Object::SHADOWCAST;

    new Object(scene, 330, 340, 50, flags, orange);
    new Object(scene, 400, 340, 50, flags, lime);
    new Object(scene, 470, 340, 50, flags, red);

    Object *prev = NULL;
    for (int i=0; i<8; i++) {
        flags = Object::COLORABLE | Object::COLLIDER | Object::NOGRAVITY | Object::BORDERBOUNCE | Object::NODRAG | Object::SHADOWCAST;
        Object *target1 = new Object(scene, 400 + 300 * cosf((float)i/7. * M_PI), 400 - 350 * sinf((float)i/7. * M_PI),
                30, flags);
        if (i == 0 || i == 7) {
            target1->desired = red;
        } else {
            target1->desired = (i%2 == 0) ? lime : orange;
        }
        if (prev != NULL) {
            new Joint(scene, prev, target1);
        }
        prev = target1;
    }
}

void
level45(Scene *scene)
{
    UntangleLevel u(scene);
    new HintDecal(scene, Circle1D::UNTANGLE, 350, 350);

    Object *a = u ^ Vec2(300, 170);
    Object *b = u ^ Vec2(500, 170);
    Object *c = u ^ Vec2(350, 300);
    Object *d = u ^ Vec2(450, 300);

    u << b << d << a << b << c << a >> false;
}

void
level46(Scene *scene)
{
    UntangleLevel u(scene, RGB(0.9, 0.9, 0.7));

    Object *a = u ^ Vec2(393, 73);
    Object *b = u ^ Vec2(459, 408);
    Object *c = u ^ Vec2(574, 208);
    Object *d = u ^ Vec2(251, 336);
    Object *e = u ^ Vec2(524, 114);
    Object *f = u ^ Vec2(342, 402);
    Object *g = u ^ Vec2(282, 134);
    Object *h = u ^ Vec2(563, 344);
    Object *i = u ^ Vec2(217, 227);

    u << a << b << c << d << e << f << g << h >> true;
    u << b << i << c >> false;
    u << h << i << e >> false;
    u << g << e >> true;

    u || a || g || h || b;
}

void
level47(Scene *scene)
{
    UntangleLevel u(scene, RGB(0.4, 0.9, 0.4));

    Object *a = u ^ Vec2(615, 331);
    Object *b = u ^ Vec2(467, 426);
    Object *c = u ^ Vec2(493, 77) ;
    Object *d = u ^ Vec2(297, 427);
    Object *e = u ^ Vec2(339, 73) ;
    Object *f = u ^ Vec2(383, 440);
    Object *g = u ^ Vec2(184, 157);
    Object *h = u ^ Vec2(556, 411);
    Object *i = u ^ Vec2(619, 161);
    Object *j = u ^ Vec2(174, 247);
    Object *k = u ^ Vec2(183, 328);
    Object *l = u ^ Vec2(624, 245);
    Object *m = u ^ Vec2(218, 404);

    u << a << b << c << d << e << f << g << h << i << j << k >> true;
    u << c << m << e >> false;
    u << a << k << l << b >> false;
    u << m << l << g >> false;
    u << j << k << i >> false;

    u || c || e;
}

void
level42(Scene *scene)
{
    UntangleLevel u(scene, RGB(0.9, 0.5, 0.7));

    Object *a = u^Vec2(335, 87);
    Object *b = u^Vec2(276, 275);
    Object *c = u^Vec2(403, 207);
    Object *d = u^Vec2(552, 258);
    Object *e = u^Vec2(331, 375);
    Object *f = u^Vec2(548, 133);
    Object *g = u^Vec2(515, 361);
    Object *h = u^Vec2(241, 185);

    u << a << c << e << d >> true;
    u << c << f << e >> false;
    u << c << b << g << h << f >> false;

    u || b || d || f || h;
}

void
level41(Scene *scene)
{
    UntangleLevel untangle(scene, RGB(0.5, 0.9, 0.7));

    Object *a = untangle ^ Vec2(298, 386);
    Object *b = untangle ^ Vec2(339, 149);
    Object *c = untangle ^ Vec2(199, 62);
                                
    Object *d = untangle ^ Vec2(603, 70);
    Object *e = untangle ^ Vec2(473, 150);
    Object *f = untangle ^ Vec2(519, 385);

    untangle << a << d << b << e << c << f << b << e << a >> false;
    untangle << a << b << c >> false;
    untangle << d << e << f >> false;
}

void
level40(Scene *scene)
{
    UntangleLevel untangle(scene, RGB(0.9, 0.7, 0.5));

    float angle = 0.0;
    float radius = 100.0;
    for (int i=0; i<5; i++) {
        angle += 145.0 / 180.0 * M_PI;
        untangle << Vec2(400 + radius * sinf(angle),
                         240 + radius * cosf(angle));
    }

    untangle >> true;
}

void
level31(Scene *scene)
{
    RGB alpha(0xaacc88);
    RGB beta(0xee66cc);

    Object *target = new Object(scene, 400, 240 + 160, 70, Object::FIXED |
            Object::COLORABLE | Object::COLLIDER);
    target->desired = alpha;

    Object *target2 = new Object(scene, 400, 240 - 160, 70, Object::FIXED |
            Object::COLORABLE | Object::COLLIDER);
    target2->desired = beta;

    new Object(scene, 700, 240 - 100, 60, Object::NOGRAVITY |
            Object::COLLIDER | Object::BORDERBOUNCE, beta);

    new Object(scene, 100, 240 + 100, 60, Object::NOGRAVITY |
            Object::COLLIDER | Object::BORDERBOUNCE, alpha);
}

void
level30(Scene *scene)
{
    RGB alpha(0xaa9977);
    RGB beta(0x557799);

    scene->set_custom_background_color(alpha);

    Object *target = new Object(scene, 600, 240, 50, Object::FIXED |
            Object::COLORABLE | Object::COLLIDER);
    target->desired = alpha;

    new Object(scene, 300, 240 - 120, 60, Object::NOGRAVITY |
            Object::COLLIDER | Object::BORDERBOUNCE, beta);

    new Object(scene, 300, 240 + 120, 60, Object::NOGRAVITY |
            Object::COLLIDER | Object::BORDERBOUNCE, alpha);
}

void
level24(Scene *scene)
{
    RGB topleft(0x8727a7);
    RGB topcenter(0x7a3fc4);
    RGB topright(0xc43fb6);

    RGB bottomleft = topcenter;
    RGB bottomcenter = topright;
    RGB bottomright = topleft;

    RGB targetleft(0x7090f0);
    RGB targetcenter(0x3060e0);
    RGB targetright(0x001090);

    int top_row_y = 90;
    int bottom_row_y = 480 - 90;

    int left_x = 400 - 200;
    int center_x = 400;
    int right_x = 400 + 200;

    int size = 45;
    int target_size = 26;

    int overlap = 5;
    int pendulum_y_min = top_row_y + size + target_size - overlap;
    int pendulum_y_max = bottom_row_y - size - target_size + overlap;

    Object *target_left = new Object(scene, left_x, bottom_row_y - 100,
            target_size, Object::COLORABLE | Object::COLLIDER | Object::FIXED);
    target_left->desired = targetleft;

    new AxisConstraintBehavior(scene, target_left, true, left_x, top_row_y+size,
            bottom_row_y-size);
    new YAxisPendulumBehavior(scene, target_left, pendulum_y_min,
            pendulum_y_max, 0.01);

    Object *target_center = new Object(scene, center_x, top_row_y + 120,
            target_size, Object::COLORABLE | Object::COLLIDER | Object::FIXED);
    target_center->desired = targetcenter;

    new AxisConstraintBehavior(scene, target_center, true, center_x,
            top_row_y+size, bottom_row_y-size);
    new YAxisPendulumBehavior(scene, target_center, pendulum_y_min,
            pendulum_y_max, 0.006, 10.0);

    Object *target_right = new Object(scene, right_x, bottom_row_y - 110,
            target_size, Object::COLORABLE | Object::COLLIDER | Object::FIXED);
    target_right->desired = targetright;

    new AxisConstraintBehavior(scene, target_right, true, right_x,
            top_row_y+size, bottom_row_y-size);
    new YAxisPendulumBehavior(scene, target_right, pendulum_y_min,
            pendulum_y_max, 0.008, 14.0);

    new Object(scene, left_x, top_row_y, size,
            Object::FIXED | Object::COLLIDER, topleft);

    new Object(scene, left_x, bottom_row_y,
            size, Object::FIXED | Object::COLLIDER, bottomleft);

    new Object(scene, center_x, top_row_y, size,
            Object::FIXED | Object::COLLIDER, topcenter);

    new Object(scene, center_x, bottom_row_y,
            size, Object::FIXED | Object::COLLIDER, bottomcenter);

    new Object(scene, right_x, top_row_y, size,
            Object::FIXED | Object::COLLIDER, topright);

    new Object(scene, right_x, bottom_row_y,
            size, Object::FIXED | Object::COLLIDER, bottomright);

    new Object(scene, left_x+10, 180, size,
            Object::COLLIDER | Object::NOGRAVITY | Object::BORDERBOUNCE,
            targetcenter);

    new Object(scene, center_x-10, 240, size,
            Object::COLLIDER | Object::NOGRAVITY | Object::BORDERBOUNCE,
            targetright);

    new Object(scene, right_x-10, 250, size,
            Object::COLLIDER | Object::NOGRAVITY | Object::BORDERBOUNCE,
            targetleft);
}

void
level32(Scene *scene)
{
    RGB yellow(0.9, 0.3, 0.2);
    RGB magenta(0.3, 0.9, 0.2);

    for (int i=0; i<8; i++) {
        Object *target = new Object(scene, 85 + i*90, 240, 40,
                Object::FIXED | Object::COLLIDER | Object::COLORABLE);
        target->desired = (i%2==0)?yellow:magenta;
    }

    int rails_top = 170;
    int rails_bottom = 240 + (240 - rails_top);
    int rails_left = 100;
    int rails_right = 700;

    Object *movable2 = new Object(scene, 220, 0, 40, Object::NOGRAVITY | Object::COLLIDER, magenta);
    new AxisConstraintBehavior(scene, movable2, false, rails_top, rails_left, rails_right);

    Object *movable4 = new Object(scene, 580, 0, 40, Object::NOGRAVITY | Object::COLLIDER, yellow);
    new AxisConstraintBehavior(scene, movable4, false, rails_bottom, rails_left, rails_right);
}

void
level23(Scene *scene)
{
    RGB yellow(0.6, 0.65, 0.2);
    RGB magenta(0.1, 0.4, 0.7);

    Object *target = new Object(scene, 220, 240, 60, Object::FIXED |
            Object::COLLIDER | Object::COLORABLE);
    target->desired = yellow;

    Object *target2 = new Object(scene, 340, 240, 60, Object::FIXED |
            Object::COLLIDER | Object::COLORABLE);
    target2->desired = magenta;

    Object *target3 = new Object(scene, 460, 240, 60, Object::FIXED |
            Object::COLLIDER | Object::COLORABLE);
    target3->desired = yellow;

    Object *target4 = new Object(scene, 580, 240, 60, Object::FIXED |
            Object::COLLIDER | Object::COLORABLE);
    target4->desired = magenta;

    Object *movable2 = new Object(scene, 270, 0, 40, Object::NOGRAVITY | Object::COLLIDER, magenta);
    new AxisConstraintBehavior(scene, movable2, false, 145, 200, 600);

    Object *movable4 = new Object(scene, 530, 0, 40, Object::NOGRAVITY | Object::COLLIDER, yellow);
    new AxisConstraintBehavior(scene, movable4, false, 240 + (240 - 145), 200, 600);
}

void
level22(Scene *scene)
{
    RGB green(0.4, 0.8, 0.4);
    RGB red(0.8, 0.4, 0.3);
    RGB black(0.2, 0.2, 0.2);

    int duration = 1000;
    int a_to_b_ms = 2000;
    int b_to_c_ms = 5000;
    int c_to_d_ms = 7000;

    Object *switch_for_green = build_switch(scene, 360, 300, green, black);
    switch_for_green->target_color = black;

    Object *switch_for_red = build_switch(scene, 520, 300, red, black);
    switch_for_red->target_color = black;

    Object *d = new Object(scene, 200, 300, 30, Object::FIXED | Object::COLORABLE, black);
    EmitterBehavior *c = build_emitter(scene, 200, 100, black,
            c_to_d_ms, duration, d, true);
    EmitterBehavior *b = build_emitter(scene, 400, 100, black,
            b_to_c_ms, duration, c->emitter, true);
    EmitterBehavior *a = build_emitter(scene, 450, 200,
            switch_for_green->target_color,
            a_to_b_ms, duration, b->emitter, true);

    new SwitchForEmitterBehavior(scene, switch_for_green, a, black, true);
    new SwitchForEmitterBehavior(scene, switch_for_red, a, black, true);

    new Joint(scene, switch_for_green, a->emitter);
    new Joint(scene, switch_for_red, a->emitter);

    b->emitter->desired = red;
    b->emitter->flags |= Object::COLORABLE;

    c->emitter->desired = green;
    c->emitter->flags |= Object::COLORABLE;

    d->desired = red;
}

void
level21(Scene *scene)
{
    RGB cyan(0.2, 0.7, 0.9);
    RGB red(0.9, 0.2, 0.4);

    Object *target = new Object(scene, 600, 240, 80,
            Object::FIXED | Object::COLORABLE | Object::NOAUTOCOLOR);
    target->desired = cyan;

    EmitterBehavior *emitter1 = build_emitter(scene, 200, 240, cyan, 0, 3000, target, false);

    Object *switch_object = build_switch(scene, 200, 240, cyan, red);

    new SwitchForEmitterBehavior(scene, switch_object, emitter1, red);
}

void
level20(Scene *scene)
{
    RGB green(0.4, 0.9, 0.2);
    RGB red(0.9, 0.2, 0.4);
    RGB blue(0.2, 0.4, 0.9);

    Object *target = new Object(scene, 400, 240, 80,
            Object::FIXED | Object::COLORABLE | Object::NOAUTOCOLOR);
    target->desired = green;

    EmitterBehavior *emitter1 = build_emitter(scene, 90, 260, green, 0, 3000, target, false);
    Object *switch_object = build_switch(scene, 90, 260, green, red);

    new SwitchForEmitterBehavior(scene, switch_object, emitter1, red);

    build_emitter(scene, 800-60, 60, blue, 1300, 1300, target);
}

void
level19(Scene *scene)
{
    scene->set_custom_background_color(Colors::COLOR_MIX_BACKGROUND_COLOR);

    RGB green(0.4, 0.9, 0.2);
    RGB red(0.9, 0.2, 0.4);
    RGB blue(0.2, 0.4, 0.9);

    RGB c_a = red;
    RGB c_b = green;
    RGB c_c = blue;
    RGB c_d = green;

    RGB c_i = blue;
    RGB c_j = blue;
    RGB c_k = red;
    RGB c_l = green;

    RGB c_e = RGB::mix(c_a, c_b);
    RGB c_f = RGB::mix(c_c, c_d);

    RGB c_g = RGB::mix(c_i, c_j);
    RGB c_h = RGB::mix(c_k, c_l);

    RGB c_m = RGB::mix(c_e, c_f);
    RGB c_n = RGB::mix(c_e, c_h);
    RGB c_o = RGB::mix(c_g, c_h);

    Object *a = build_color_mix(scene, 200, 120, true);
    Object *b = build_color_mix(scene, 200, 200, true);
    Object *c = build_color_mix(scene, 200, 280, true);
    Object *d = build_color_mix(scene, 200, 360, true);

    Object *e = build_color_mix(scene, 300, 180);
    Object *f = build_color_mix(scene, 300, 300);

    Object *g = build_color_mix(scene, 500, 180);
    Object *h = build_color_mix(scene, 500, 300);

    Object *i = build_color_mix(scene, 600, 120, true);
    Object *j = build_color_mix(scene, 600, 200, true);
    Object *k = build_color_mix(scene, 600, 280, true);
    Object *l = build_color_mix(scene, 600, 360, true);

    Object *m = build_color_target(scene, 400, 120, c_m);
    Object *n = build_color_target(scene, 400, 240, c_n);
    Object *o = build_color_target(scene, 400, 360, c_o);

    new ColorCombinationBehavior(scene, e, a, b);
    new ColorCombinationBehavior(scene, f, c, d);
    new ColorCombinationBehavior(scene, g, i, j);
    new ColorCombinationBehavior(scene, h, k, l);
    new ColorCombinationBehavior(scene, m, e, f);
    new ColorCombinationBehavior(scene, n, e, h);
    new ColorCombinationBehavior(scene, o, g, h);

    build_paint_target(scene, 100, 120, red);
    build_paint_target(scene, 100, 240, green);
    build_paint_target(scene, 100, 360, blue);

    build_paint_target(scene, 700, 120, red);
    build_paint_target(scene, 700, 240, green);
    build_paint_target(scene, 700, 360, blue);
}

void
level25(Scene *scene)
{
    RGB ca(0x88aa00);
    RGB cb(0x660000);

    new HintDecal(scene, Circle1D::COLOR_LEFT, 200, 370);
    new HintDecal(scene, Circle1D::COLOR_RIGHT, 600, 370);

    RGB target = RGB::add(ca, cb);

    Object *a = build_color_mix(scene, 300, 300, true);
    Object *b = build_color_mix(scene, 500, 300, true);
    Object *c = build_color_target(scene, 400, 200, target);
    c->size = 70;

    scene->set_custom_background_color(Colors::COLOR_MIX_BACKGROUND_COLOR);
    new BackgroundColorFromObjectBehavior(scene, c);

    new ColorCombinationBehavior(scene, c, a, b, ColorCombinationBehavior::ADD);

    build_paint_target(scene, 100, 360, ca)->size = 60;
    build_paint_target(scene, 700, 360, cb)->size = 60;
}

void
level26(Scene *scene)
{
    RGB c1(0xff0000);
    RGB c2(0x00ff00);
    RGB c3(0x0000ff);
    RGB c4(0xff00ff);
    RGB c5(0x00ffff);
    RGB c6(0xffff00);

    RGB target1 = RGB::mix(c2, c5);
    Object *a1 = build_color_mix(scene, 200, 300, true);
    Object *a2 = build_color_mix(scene, 300, 300, true);
    Object *a3 = build_color_target(scene, 250, 200, target1);
    new ColorCombinationBehavior(scene, a3, a1, a2, ColorCombinationBehavior::MIX);

    RGB target2 = RGB::mix(c1, c6);
    Object *b1 = build_color_mix(scene, 500, 300, true);
    Object *b2 = build_color_mix(scene, 600, 300, true);
    Object *b3 = build_color_target(scene, 550, 200, target2);
    new ColorCombinationBehavior(scene, b3, b1, b2, ColorCombinationBehavior::MIX);

    build_paint_target(scene, 100, 200, c1);
    build_paint_target(scene, 100, 300, c2);
    build_paint_target(scene, 100, 400, c3);
    build_paint_target(scene, 700, 200, c4);
    build_paint_target(scene, 700, 300, c5);
    build_paint_target(scene, 700, 400, c6);

    scene->set_custom_background_color(Colors::COLOR_MIX_BACKGROUND_COLOR);
}

void
level27(Scene *scene)
{
    RGB c1(0xff0000);
    RGB c2(0x00ff00);
    RGB c3(0x0000ff);
    RGB c4(0xff00ff);
    RGB c5(0x00ffff);
    RGB c6(0xffff00);

    RGB target1 = RGB::mix(c1, c2);
    RGB target2 = RGB::mix(c3, c4);
    RGB target3 = RGB::mix(c5, c6);

    build_simple_combiner(scene, target1, 400, 200, 300, 170, 500, 170);
    build_simple_combiner(scene, target2, 340, 300, 200, 200, 200, 300);
    build_simple_combiner(scene, target3, 460, 300, 600, 200, 600, 300);

    int offset = 140;
    build_paint_target(scene, 100, 240 - offset, c1);
    build_paint_target(scene, 100, 240, c2);
    build_paint_target(scene, 100, 240 + offset, c3);

    build_paint_target(scene, 700, 240 - offset, c4);
    build_paint_target(scene, 700, 240, c5);
    build_paint_target(scene, 700, 240 + offset, c6);

    scene->set_custom_background_color(Colors::COLOR_MIX_BACKGROUND_COLOR);
}

void
level18(Scene *scene)
{
    RGB green(0.4, 0.9, 0.2);
    RGB red(0.9, 0.2, 0.4);

    RGB lemon(0.7, 0.9, 0.2);

    /* Arrow */
    for (int i=0; i<4; i++) {
        new Object(scene, 240, 700-(330+i*30), 20, Object::FIXED);

        new Object(scene, 240-(3-i)*20, 700-(390+i*20), 20, Object::FIXED);
        new Object(scene, 240+(3-i)*20, 700-(390+i*20), 20, Object::FIXED);
    }

    Object *gravity_switch = build_switch(scene, 240, 120, green, red);

    Object *falling = new Object(scene, 600, 140, 60,
            Object::NOGRAVITY | Object::NODRAG | Object::COLLIDER, lemon);

    new SwitchForGravity(scene, gravity_switch, falling);

    Object *target = new Object(scene, 600, 340, 60,
            Object::FIXED | Object::COLLIDER | Object::COLORABLE);
    target->desired = lemon;
}

void
level17(Scene *scene)
{
    RGB cyan(0.2, 0.7, 0.9);

    new HintDecal(scene, Circle1D::INTRO_ARROW, 400, 240);

    new Object(scene, 200, 240, 60,
            Object::NOGRAVITY | Object::BORDERBOUNCE | Object::COLLIDER, cyan);

    Object *target = new Object(scene, 600, 240, 60,
            Object::FIXED | Object::COLLIDER | Object::COLORABLE);
    target->desired = cyan;
}

void
level16(Scene *scene)
{
    RGB green(0.4, 0.9, 0.2);
    RGB red(0.9, 0.2, 0.4);
    RGB blue(0.2, 0.4, 0.9);

    Object *switch_a = build_switch(scene, 80, 80, green, red);
    Object *switch_b = build_switch(scene, 800-80, 480-120, green, red);
    new HintDecal(scene, Circle1D::ON_OFF, 80, 80+60);
    new HintDecal(scene, Circle1D::ON_OFF, 800-80, 480-120+60);

    Object *top_rotater = new Object(scene, 200, 80, 40, Object::FIXED | Object::COLLIDER);
    SwitchForRotatingBehavior *top_switch = new SwitchForRotatingBehavior(scene, switch_a);
    top_switch->add_rotate(new RotatingBehavior(scene, 400, 80, top_rotater,
                3.0, true, 1.0, 0.3));

    Object *bottom_rotater = new Object(scene, 600, 480-80, 40, Object::FIXED | Object::COLLIDER);
    SwitchForRotatingBehavior *bottom_switch = new SwitchForRotatingBehavior(scene, switch_b);
    bottom_switch->add_rotate(new RotatingBehavior(scene, 400, 480-80, bottom_rotater,
                3.0, true, 1.0, 0.3));

    Object *blueball = new Object(scene, 400, 240, 60, Object::NODRAG | Object::NOGRAVITY | Object::COLLIDER, blue);
    new Joint(scene, top_rotater, blueball);
    new Joint(scene, bottom_rotater, blueball);

    Object *target_bottomleft = new Object(scene, 60, 420, 40,
            Object::FIXED | Object::COLORABLE | Object::COLLIDER);
    target_bottomleft->desired = blue;

    Object *target_topright = new Object(scene, 800-60, 480-420, 40,
            Object::FIXED | Object::COLORABLE | Object::COLLIDER);
    target_topright->desired = blue;
}

void
level15a(Scene *scene)
{
    RGB red(1.0, 0.2, 0.1);
    RGB blue(0.1, 0.2, 1.0);
    RGB green(0.2, 1.0, 0.1);

    RGB black(0.2, 0.2, 0.2);

    RGB color_c = RGB::mix(RGB::mix(red, blue), green);
    RGB color_d = RGB::mix(RGB::mix(red, red), green);

    Object *target_d = new Object(scene, 200, 140, 80, Object::FIXED | Object::COLLIDER | Object::COLORABLE | Object::NOAUTOCOLOR);
    target_d->desired = color_d;

    Object *target_c = new Object(scene, 200, 340, 80, Object::FIXED | Object::COLLIDER | Object::COLORABLE | Object::NOAUTOCOLOR);
    target_c->desired = color_c;

    Object *target_a = new Object(scene, 420, 140, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::NOAUTOCOLOR | Object::SWITCHONLY, black);
    target_a->desired = black;

    Object *target_b = new Object(scene, 420, 340, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::NOAUTOCOLOR | Object::SWITCHONLY, black);
    target_b->desired = black;

    Object *mix_a = new Object(scene, 550, 80, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::SWITCHONLY, black);
    mix_a->desired = black;
    Object *mix_b = new Object(scene, 550, 160, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::SWITCHONLY, black);
    mix_b->desired = black;

    Object *mix_e = new Object(scene, 550, 240, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::SWITCHONLY, black);
    mix_e->desired = black;

    Object *mix_c = new Object(scene, 550, 320, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::SWITCHONLY, black);
    mix_c->desired = black;
    Object *mix_d = new Object(scene, 550, 400, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::SWITCHONLY, black);
    mix_d->desired = black;

    new ColorCombinationBehavior(scene, target_d, target_a, mix_e,
            ColorCombinationBehavior::MIX);

    new ColorCombinationBehavior(scene, target_c, target_b, mix_e,
            ColorCombinationBehavior::MIX);

    new ColorCombinationBehavior(scene, target_a, mix_a, mix_b,
            ColorCombinationBehavior::MIX);

    new ColorCombinationBehavior(scene, target_b, mix_c, mix_d,
            ColorCombinationBehavior::MIX);

    new Object(scene, 700, 240 - 120, 50, Object::NOGRAVITY | Object::COLLIDER | Object::BORDERBOUNCE, green);
    new Object(scene, 700, 240, 50, Object::NOGRAVITY | Object::COLLIDER | Object::BORDERBOUNCE, red);
    new Object(scene, 700, 240 + 120, 50, Object::NOGRAVITY | Object::COLLIDER | Object::BORDERBOUNCE, blue);

    scene->set_custom_background_color(Colors::COLOR_MIX_BACKGROUND_COLOR);
    new BackgroundColorFromObjectBehavior(scene, target_c);
}


void
level15(Scene *scene)
{
    RGB red(1.0, 0.2, 0.1);
    RGB blue(0.1, 0.2, 1.0);
    RGB green(0.2, 1.0, 0.1);

    RGB black(0.2, 0.2, 0.2);

    RGB a = RGB::mix(red, blue);
    RGB b = RGB::mix(blue, green);
    RGB c = RGB::mix(a, b);

    Object *target_c = new Object(scene, 290, 240, 80, Object::FIXED | Object::COLLIDER | Object::COLORABLE | Object::NOAUTOCOLOR);
    target_c->desired = c;

    Object *target_a = new Object(scene, 420, 140, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::NOAUTOCOLOR | Object::SWITCHONLY, black);
    target_a->desired = black;

    Object *target_b = new Object(scene, 420, 340, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::NOAUTOCOLOR | Object::SWITCHONLY, black);
    target_b->desired = black;

    new ColorCombinationBehavior(scene, target_c, target_a, target_b,
            ColorCombinationBehavior::MIX);

    Object *mix_a = new Object(scene, 550, 80, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::SWITCHONLY, black);
    mix_a->desired = black;
    Object *mix_b = new Object(scene, 550, 200, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::SWITCHONLY, black);
    mix_b->desired = black;

    new ColorCombinationBehavior(scene, target_a, mix_a, mix_b,
            ColorCombinationBehavior::MIX);

    Object *mix_c = new Object(scene, 550, 280, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::SWITCHONLY, black);
    mix_c->desired = black;
    Object *mix_d = new Object(scene, 550, 400, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::SWITCHONLY, black);
    mix_d->desired = black;

    new ColorCombinationBehavior(scene, target_b, mix_c, mix_d,
            ColorCombinationBehavior::MIX);

    new Object(scene, 700, 240 - 120, 50, Object::NOGRAVITY | Object::COLLIDER | Object::BORDERBOUNCE, green);
    new Object(scene, 700, 240, 50, Object::NOGRAVITY | Object::COLLIDER | Object::BORDERBOUNCE, red);
    new Object(scene, 700, 240 + 120, 50, Object::NOGRAVITY | Object::COLLIDER | Object::BORDERBOUNCE, blue);

    scene->set_custom_background_color(Colors::COLOR_MIX_BACKGROUND_COLOR);
    new BackgroundColorFromObjectBehavior(scene, target_c);
}

void
level14(Scene *scene)
{
    RGB green(0.4, 0.9, 0.1);
    RGB red(0.9, 0.3, 0.4);

    RGB desired(0.3, 0.1, 0.4);

    new HintDecal(scene, Circle1D::ON_OFF, 150, 260);

    Object *switch_object1 = build_switch(scene, 150, 190, green, red);
    Object *switch_object2 = build_switch(scene, 150, 340, green, red);

    Object *paint = new Object(scene, 420, 200, 30, Object::COLLIDER | Object::FIXED, desired);

    Object *rotater = new Object(scene, 600, 200, 30, Object::FIXED);

    SwitchForRotatingBehavior *switch1 = new SwitchForRotatingBehavior(scene, switch_object1);
    switch1->add_rotate(new RotatingBehavior(scene, 500, 100, rotater, 1.5, true));

    SwitchForRotatingBehavior *switch2 = new SwitchForRotatingBehavior(scene, switch_object2);
    switch2->add_rotate(new RotatingBehaviorAround(scene, rotater, paint, 1.5));

    new Joint(scene, rotater, paint);

    Object *target = new Object(scene, 400, 400, 80, Object::COLLIDER | Object::FIXED | Object::COLORABLE);
    target->desired = desired;
}

void
level13(Scene *scene)
{
    scene->set_custom_background_color(Colors::COLOR_MIX_BACKGROUND_COLOR);

    RGB red(0.6, 0.2, 0.3);
    RGB blue(0.3, 0.2, 0.6);
    RGB green(0.2, 0.6, 0.3);

    RGB black(0.2, 0.2, 0.2);

    Object *target = new Object(scene, 400, 140, 80, Object::FIXED | Object::COLORABLE | Object::NOAUTOCOLOR | Object::COLLIDER);
    target->desired = RGB::add(red, green);

    Object *left = new Object(scene, 500, 300, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::SWITCHONLY, black);
    left->desired = black;

    Object *right = new Object(scene, 530, 180, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::SWITCHONLY, black);
    right->desired = black;

    new ColorCombinationBehavior(scene, target, left, right, ColorCombinationBehavior::ADD);

    Object *target2 = new Object(scene, 300, 300, 80, Object::FIXED | Object::COLORABLE | Object::NOAUTOCOLOR | Object::COLLIDER);
    target2->desired = RGB::add(blue, red);

    Object *left2 = new Object(scene, 400, 400, 30, Object::FIXED | Object::COLORABLE | Object::COLLIDER | Object::SWITCHONLY, black);
    left2->desired = black;

    new ColorCombinationBehavior(scene, target2, left, left2, ColorCombinationBehavior::ADD);

    new Object(scene, 700, 240-150, 50, Object::NOGRAVITY | Object::COLLIDER | Object::BORDERBOUNCE, red);
    new Object(scene, 700, 240, 50, Object::NOGRAVITY | Object::COLLIDER | Object::BORDERBOUNCE, blue);
    new Object(scene, 700, 240+150, 50, Object::NOGRAVITY | Object::COLLIDER | Object::BORDERBOUNCE, green);
}

void
level12(Scene *scene)
{
    RGB green(0.1, 0.6, 0.2);
    RGB red(0.6, 0.2, 0.2);
    RGB yellow(0.8, 0.7, 0.1);

    new HintDecal(scene, Circle1D::ON_OFF, 400, 270);

    Object *target = new Object(scene, 160, 200, 30,
            Object::FIXED | Object::COLLIDER | Object::COLORABLE);
    target->desired = yellow;

    SwitchForRotatingBehavior *rotating_switch = new SwitchForRotatingBehavior(scene);

    Object *spoke = NULL;
    for (int i=0; i<12; i++) {
        float x = 120.0 * sinf((float)i*30.0 / 180.0 * M_PI);
        float y = 120.0 * cosf((float)i*30.0 / 180.0 * M_PI);
        Object *o = new Object(scene, 400+x, 240+y, 40, Object::FIXED | Object::COLLIDER);
        if (i == 3) {
            spoke = o;
        }
        rotating_switch->add_rotate(new RotatingBehavior(scene, 400, 240, o, 0.8));
    }

    rotating_switch->set_switch(build_switch(scene, 400, 220, green, red));

    Object *last = spoke;
    for (int i=0; i<5; i++) {
        Object *o = new Object(scene, 580, 240+20*i, 10, Object::COLLIDER | Object::NODRAG);
        if (last != NULL) {
            new Joint(scene, last, o, Joint::RUBBERBAND | Joint::FIXED);
        }
        last = o;
    }
    Object *o = new Object(scene, 580, 240+20* /*i=*/5+30, 40, Object::COLLIDER | Object::NODRAG, yellow);
    new Joint(scene, last, o, Joint::RUBBERBAND | Joint::STRONG | Joint::FIXED);
}

void
level11(Scene *scene)
{
    RGB green(0.2, 0.7, 0.4);
    RGB blue(0.5, 0.6, 0.8);
    RGB red(0.8, 0.4, 0.4);

    new HintDecal(scene, Circle1D::PENDULUM, 600, 100);

    Object *a = new Object(scene, 300, 240, 30,
            Object::FIXED | Object::COLLIDER | Object::COLORABLE | Object::SHADOWCAST);
    //new ObjectIsLightSourceBehavior(scene, a);
    a->desired = green;
    new RotatingBehavior(scene, 200, 240, a, 2.0);

    Object *nail = new Object(scene, 400, 10, 10, Object::FIXED);
    Object *handle = new Object(scene, 400, 400, 40, Object::SHADOWCAST | Object::COLLIDER);

    Object *b = new Object(scene, 400, 240, 30,
            Object::COLLIDER | Object::NODRAG |
            Object::COLORABLE | Object::COLORTRANSFER | Object::SHADOWCAST);
    b->desired = red;

    Object *redpaint = new Object(scene, 90, 40, 30,
            Object::COLLIDER | Object::FIXED, red);
    new ObjectIsLightSourceBehavior(scene, redpaint);

    new Joint(scene, nail, b, Joint::STRONG);
    new Joint(scene, b, handle, Joint::STRONG);

    Object *c = new Object(scene, 630, 240, 30,
            Object::FIXED | Object::COLLIDER | Object::SHADOWCAST, green);
    new RotatingBehavior(scene, 700, 240, c, 1.0);

    Object *d = new Object(scene, 450, 240, 30,
            Object::FIXED | Object::COLLIDER | Object::SHADOWCAST, blue);
    new RotatingBehavior(scene, 600, 200, d, 3.0);
}

void scene_square(Scene *scene)
{
    int radius = 60;

    Object *a = new Object(scene, 10, 10, radius, Object::COLLIDER);
    Object *b = new Object(scene, 10, 150, radius, Object::COLLIDER);
    Object *c = new Object(scene, 150, 150, radius, Object::COLLIDER);
    Object *d = new Object(scene, 150, 10, radius, Object::COLLIDER);
    Object *e = new Object(scene, 80, 80, 130, Object::COLLIDER);

    a->compound = b->compound = c->compound = d->compound = e;

    new Joint(scene, a, b, Joint::FIXED);
    new Joint(scene, b, c, Joint::FIXED);
    new Joint(scene, c, d, Joint::FIXED);
    new Joint(scene, d, a, Joint::FIXED);

    new Joint(scene, a, c, Joint::FIXED);
    new Joint(scene, b, d, Joint::FIXED);

    new Joint(scene, a, e, Joint::FIXED);
    new Joint(scene, b, e, Joint::FIXED);
    new Joint(scene, c, e, Joint::FIXED);
    new Joint(scene, d, e, Joint::FIXED);
}

void level3(Scene *scene)
{
    RGB red1(0.9, 0.5, 0.1);
    RGB red2(0.9, 0.1, 0.5);
    RGB red3(0.7, 0.9, 0.2);

    int x = 80;
    int y = 80;
    int i;

    Object *o = new Object(scene, x, y, 20, Object::FIXED | Object::COLLIDER | Object::COLORABLE);
    o->desired = red1;
    for (i=0; i<20; i++) {
        x += 30;
        Object *n = new Object(scene, x, y, (i%10 != 9)?15:20,
                ((i%10 != 9)?Object::NODRAG:Object::FIXED) | Object::COLLIDER | Object::COLORABLE);
        n->desired = (i%3 == 0)?red3:red2;
        if (i%10 == 9) n->desired = red1;
        new Joint(scene, o, n, Joint::STRONG | Joint::RUBBERBAND);
        o = n;
    }

    new Object(scene, 30, 480-30, 100, Object::NOGRAVITY | Object::COLLIDER | Object::STAYINVIEW, red2);
    new Object(scene, 400-15, 480-30, 60, Object::NOGRAVITY | Object::COLLIDER | Object::STAYINVIEW, red1);
    new Object(scene, 800-30, 480-30, 100, Object::NOGRAVITY | Object::COLLIDER | Object::STAYINVIEW, red3);
}

void level1(Scene *scene)
{
    int i;

    RGB red(0.7, 0.2, 0.1);
    RGB green(0.2, 0.4, 0.0);
    RGB blue(0.1, 0.2, 0.7);

#if 0
    /* Level borders for scaling test */
    new Object(scene, 0, 0, 20, Object::FIXED);
    new Object(scene, 800, 0, 20, Object::FIXED);
    new Object(scene, 0, 480, 20, Object::FIXED);
    new Object(scene, 800, 480, 20, Object::FIXED);
#endif
    new Object(scene, 230, 200, 40, Object::COLLIDER | Object::NODRAG);
    new Object(scene, 340, 200, 40, Object::COLLIDER | Object::NODRAG);
    new Object(scene, 460, 200, 40, Object::COLLIDER | Object::NODRAG);

    /* Player's controller */
    //new Object(scene, 400, 100, 80, Object::COLLIDER | Object::NOGRAVITY, green);

    /* Target objects (need to be colored) */
    for (i=0; i<4; i++) {
        Object *o = new Object(scene, 200+120*i, 400, 80, Object::FIXED | Object::COLLIDER | Object::COLORABLE);
        o->desired = red;
    }
    for (i=0; i<3; i++) {
        Object *o = new Object(scene, 190+80+120*i, 100, 80, Object::FIXED | Object::COLLIDER | Object::COLORABLE);
        o->desired = blue;
    }


    /* Chain of colored objects */
    Object *o = NULL;
    for (i=0; i<18; i++) {
        bool is_red = ((i%4==0) && i < 13) || (i == 17);
        int flags = Object::COLLIDER;
        if (!is_red) {
            flags |= Object::NODRAG;
        }
        RGB color = is_red?red:blue;
        Object *n = new Object(scene, 120+30*i, 300, 10, flags, color);
        if (o) {
            new Joint(scene, o, n, Joint::RUBBERBAND);// | Joint::BREAKABLE);
        }
        o = n;
    }
}

void level4(Scene *scene)
{
    int i;

    RGB lightgreen(0.3, 0.8, 0.2);
    RGB darkgreen(0.05, 0.4, 0.1);

    Object *last = NULL;
    Object *first = NULL;
    for (i=0; i<36; i++) {
        Object *o = new Object(scene,
                400+100*sinf(M_PI*(float)(i*10)/180.f),
                240+100*cosf(M_PI*(float)(i*10)/180.f),
                15, Object::COLLIDER | Object::NOGRAVITY |
                Object::COLORABLE | Object::NODRAG | Object::BORDERBOUNCE);
        o->desired = ((i%6) > 3) ? lightgreen : darkgreen;
        if (first == NULL) {
            first = o;
        }
        if (last != NULL) {
            new Joint(scene, o, last, Joint::RUBBERBAND);
        }
        last = o;
    }
    new Joint(scene, last, first, Joint::RUBBERBAND);

    Object *a = new Object(scene, 400, 200, 40, Object::COLLIDER | Object::NOGRAVITY | Object::BORDERBOUNCE | Object::SHADOWCAST, lightgreen);
    Object *b = new Object(scene, 400, 280, 40, Object::COLLIDER | Object::NOGRAVITY | Object::BORDERBOUNCE | Object::SHADOWCAST, darkgreen);

    CenterOfObjectsLightSource *cools = new CenterOfObjectsLightSource(scene);
    cools->add(a);
    cools->add(b);
}

void level5(Scene *scene)
{
    RGB sky(0.3, 0.6, 0.9);
    RGB magenta(0.7, 0.3, 0.8);

    Object *a, *b, *c, *d;

    (a = new Object(scene, 300, 240, 30, Object::COLLIDER | Object::NOGRAVITY | Object::COLORABLE | Object::BORDERBOUNCE))->desired = sky;
    (b = new Object(scene, 400, 180, 30, Object::COLLIDER | Object::NOGRAVITY | Object::COLORABLE | Object::BORDERBOUNCE))->desired = magenta;
    (c = new Object(scene, 500, 240, 30, Object::COLLIDER | Object::NOGRAVITY | Object::COLORABLE | Object::BORDERBOUNCE))->desired = sky;
    (d = new Object(scene, 400, 300, 30, Object::COLLIDER | Object::NOGRAVITY | Object::COLORABLE | Object::BORDERBOUNCE))->desired = magenta;

    new Joint(scene, a, b, Joint::FIXED);
    new Joint(scene, b, c, Joint::FIXED);
    new Joint(scene, c, a, Joint::FIXED);
    new Joint(scene, d, c, Joint::FIXED);
    new Joint(scene, d, a, Joint::FIXED);
    new Joint(scene, b, d, Joint::FIXED);

    new Object(scene, 200, 400, 100, Object::FIXED | Object::COLLIDER, sky);
    new Object(scene, 600, 240, 100, Object::FIXED | Object::COLLIDER, magenta);
    new Object(scene, 200, 100, 100, Object::FIXED | Object::COLLIDER, sky);
}

