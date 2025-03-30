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

#ifndef CARPHONE_MODEL_H
#define CARPHONE_MODEL_H

#include "vector.h"
#include <math.h>

#include "../constants.h"
#include "../platform.h"

#include "renderer.h"
#include "event.h"
#include "rgb.h"
#include "serialize.h"

#include <list>
#include <algorithm>
#include <string>
#include <functional>
#include <map>

#include <stdio.h>

class Scene;
class Joint;

typedef int32_t Circle1DTime;

struct LightSource {
    public:
        LightSource()
            : enabled(false)
            , pos(0, 0)
        {
        }

        bool enabled;
        Vec2 pos;
};

namespace Circle1D {
    enum Decal {
        INTRO_ARROW = 0,
        UNTANGLE,
        ON_OFF,
        COLOR_LEFT,
        COLOR_RIGHT,
        PENDULUM,
        EASTEREGG,
        DECALS_COUNT, /* Must always be last */
    };
};

// All types that are passed to the JS engine must subclass from
// Circle1DType, for run-time type detection (RTTI can't dynamic
// cast from a void * type, so this is the common ancestor).
class Circle1DType {
public:
    enum Type {
        INVALID = 0,
        OBJECT,
        JOINT,
        SCENE,
        BEHAVIOR,
        DECAL,
    };

    Circle1DType(enum Type type)
        : type_tag(type)
    {
    }

    virtual ~Circle1DType()
    {
    }

    enum Type type_tag;
};

class HintDecal : public Circle1DType {
    public:
        HintDecal(Scene *scene, enum Circle1D::Decal decal, int x, int y);

        enum Circle1D::Decal decal;
        float opacity;
        int x;
        int y;
};

class Object : public Circle1DType {
    public:
        enum Flag {
            COLLIDER        = 1 <<  0,
            SLOW            = 1 <<  1,
            NOGRAVITY       = 1 <<  2,
            FIXED           = 1 <<  3,
            COLORABLE       = 1 <<  4,
            NODRAG          = 1 <<  5,
            COLORTRANSFER   = 1 <<  6,
            SWITCHONLY      = 1 <<  7, // a colorable object that isn't essential for the objective
            NOAUTOCOLOR     = 1 <<  8, // a colorable object that isn't changed on collision
            BORDERBOUNCE    = 1 <<  9, // stay fully in view and bounce from border
            STAYINVIEW      = 1 << 10, // stay at least partially in view
            CANGRAB         = 1 << 11, // can be grabbed, i.e. for switches that need to react to events
            NOOVERLAP       = 1 << 12, // for fixed objects in chain levels to prevent "chain go through"
            SHADOWCAST      = 1 << 13, // object will cast shadows when rendering shadow effect
            SILENT          = 1 << 14, // no sounds
            INVISIBLE       = 1 << 15, // will not be drawn (for mouse joint)
        };

        Object(Scene *scene, float x, float y, float size, int flags=0, RGB color=RGB());
        bool handle(Circle1DEvent *event);

        bool objectiveReached()
        {
            if (grabbed != -1) {
                // Don't finish level until all touches are gone
                return false;
            }

            if ((flags & COLORABLE) != 0 && (flags & SWITCHONLY) == 0) {
                return desired.equals(color);
            }

            return true;
        }

        float weight()
        {
            return size*size*M_PI;
        }

        bool can_drag()
        {
            return ((!(flags & NODRAG)) && (!(flags & FIXED)));
        }

        void simulate();

        void restack_on_top();

        std::string describe();

        void apply_force(Vec2 direction)
        {
            if (!(flags & Object::FIXED)) {
                force += direction;
            }
        }

        void move(Vec2 destination)
        {
            if (!(flags & Object::FIXED)) {
                pos = destination;
            }
        }

        void take_color(Object *b);

        static void handle_collisions(std::list<Object*> &objects)
        {
            std::list<Object*>::iterator a;
            for (a=objects.begin(); a!=objects.end(); ++a) {
                // OPTIMIZATIONS
                std::list<Object*>::iterator b = a;
                for (++b; b!=objects.end(); ++b) {
                    // OPTIMIZATIONS
                    if ((*a)->compound != (*b) && (*b)->compound != (*a)) {
                        (*a)->handle_collision(*b);
                    }
                }
            }
        }

        void handle_collision(Object *b);

    public:
        Scene *scene;
        Vec2 pos;
        Vec2 last_pos;
        float size;
        RGB color;
        RGB target_color;
        RGB desired; // desired color
        int flags;

    private:
        Vec2 velocity;
        Vec2 force;

    public:
        int grabbed;

    private:
        Vec2 old_mouse_pos;
        Object *mouse;
        Joint *mouse_joint;
        Object *compound;
        Circle1DTime last_color_update;

    private:
        friend class Joint;
        friend class Scene;
        friend void scene_square(Scene*);
};

class Joint : public Circle1DType {
    public:
        enum Flag {
            RUBBERBAND      = 1 <<  0,
            STRONG          = 1 <<  1,
            BREAKABLE       = 1 <<  2,
            BROKEN          = 1 <<  3,
            FIXED           = 1 <<  4,
            HAS_KNOT        = 1 <<  5, // knot that "transports" color
            IS_RAIL         = 1 <<  6, // rail - display in darker color
            MOUSE           = 1 <<  7, // only for mouse joints (drag stuff)
            UNTANGLE        = 1 <<  8, // untangle joint - no force, but color + objectiveReached() check
        };

        Joint(Scene *scene, Object *a, Object *b, int flags=0);
        ~Joint();

        void simulate();

        void reset_distance() {
            distance = (a->pos - b->pos).length();
        }

        std::string describe();

    public:
        Scene *scene;
        Object *a;
        Object *b;
        int flags;

        float knot_at;
        RGB knot_color;

    private:
        float distance;

        friend class Scene;
};

class Circle1DBehavior : public Circle1DType {
    public:
        Circle1DBehavior(Scene *scene);

        virtual ~Circle1DBehavior() {}

        virtual void simulate() = 0;

        virtual const char *name() = 0;

    protected:
        Scene *scene;
};

class Circle1DBehaviorIO {
public:
    virtual void save(Scene *scene, Circle1DBehavior *behavior, SerializeBuffer &buffer) = 0;
    virtual Circle1DBehavior *load(Scene *scene, SerializeBuffer &buffer) = 0;
};

class Circle1DBehaviorRegistration {
public:
    Circle1DBehaviorRegistration(std::string name, Circle1DBehaviorIO *io)
    {
        if (!m_ios) {
            m_ios = new std::map<std::string, Circle1DBehaviorIO *>();
        }
        (*m_ios)[name] = io;
    }

    static Circle1DBehaviorIO *lookup(std::string name)
    {
        if (m_ios) {
            return (*m_ios)[name];
        } else {
            return 0;
        }
    }

private:
    static std::map<std::string, Circle1DBehaviorIO *> *m_ios;
};

#define CIRCLE1D_BEHAVIOR_BODY(klass) virtual const char *name() { return #klass; }
#define CIRCLE1D_BEHAVIOR_REGISTER(klass, io) static Circle1DBehaviorRegistration \
    _registration_##klass(#klass, new io());

class Scene : public Circle1DType, public Circle1DEventHandler {
    public:
        Scene()
            : Circle1DType(Circle1DType::SCENE)
            , time(0),
              simulation_running(false)
            , background_color(RGB::background())
            , background_color_set(false)
            , light_source()
            , editing(false)
            , editing_label()
            , editing_label_pos()
            , editing_old_mouse_pos()
            , edit_object(NULL)
            , edit_joint(NULL)
            , edit_joint_tmp(NULL)
            , editing_color_index(0)
            , objects(),
              joints(),
              fixed_joints(),
              untangle_joints(),
              colliders()
            , behaviors()
            , decals()
        {
        }

        ~Scene()
        {
            reset();
        }

        void reset()
        {
            std::list<Joint*>::iterator jit;
            for (jit = joints.begin(); jit != joints.end(); ++jit) {
                Joint *j = *jit;
                delete j;
            }
            joints.clear();
            fixed_joints.clear();
            untangle_joints.clear();

            std::list<Object*>::iterator oit;
            for (oit = objects.begin(); oit != objects.end(); ++oit) {
                Object *o = *oit;
                delete o;
            }
            objects.clear();
            colliders.clear();

            std::list<Circle1DBehavior*>::iterator bit;
            for (bit = behaviors.begin(); bit != behaviors.end(); ++bit) {
                Circle1DBehavior *b = *bit;
                delete b;
            }
            behaviors.clear();

            std::list<HintDecal*>::iterator dit;
            for (dit = decals.begin(); dit != decals.end(); ++dit) {
                HintDecal *d = *dit;
                delete d;
            }
            decals.clear();

            time = 0;
            background_color = RGB::background();
            background_color_set = false;

            light_source.enabled = false;
            light_source.pos = Vec2(0, 0);
        }

        void object_flags_changed(Object *o) {
            remove_object(o);
            add_object(o);
        }

        void object_flags_changed(Joint *o) {
            remove_object(o);
            add_object(o);
        }

        void add_object(Object *o)
        {
            objects.push_back(o);
            if (o->flags & Object::COLLIDER) {
                colliders.push_back(o);
            }
        }

        void add_object(Joint *o)
        {
            joints.push_back(o);
            if (o->flags & Joint::FIXED) {
                fixed_joints.push_back(o);
            }
            if (o->flags & Joint::UNTANGLE) {
                untangle_joints.push_back(o);
            }
        }

        void remove_object(Object *o)
        {
            // TODO: Behaviors might have references to the object
            objects.remove(o);
            if (o->flags & Object::COLLIDER) {
                colliders.remove(o);
            }
        }

        void remove_object(Joint *o)
        {
            joints.remove(o);
            if (o->flags & Joint::FIXED) {
                fixed_joints.remove(o);
            }
            if (o->flags & Joint::UNTANGLE) {
                untangle_joints.remove(o);
            }
        }

        void add_behavior(Circle1DBehavior *b)
        {
            behaviors.push_back(b);
        }

        void remove_behavior(Circle1DBehavior *b)
        {
            behaviors.remove(b);
        }

        void add_decal(HintDecal *d)
        {
            decals.push_back(d);
        }

        void remove_decal(HintDecal *d)
        {
            decals.remove(d);
        }

        void render(Circle1DRenderer *renderer);

        bool checkUntanglement();

        bool objectiveReached() {
            if (editing) {
                return false;
            }

            bool result = checkUntanglement();

            std::list<Object*>::iterator oit;
            for (oit = objects.begin(); oit != objects.end(); ++oit) {
                if (!((*oit)->objectiveReached())) {
                    return false;
                }
            }

            return result;
        }

        void simulate(int iterations=1)
        {
            simulation_running = true;
            Circle1DEvent event(Circle1DEvent::TICK);
            for (int i=0; i<iterations; i++) {
                handle(&event);
            }
            simulation_running = false;
        }

        void background_color_from_content();

        void set_custom_background_color(RGB color)
        {
            background_color = RGB::mix(RGB(0x000000), color);
            background_color_set = true;
        }

        void save(const std::string &filename);
        void load(const std::string &filename);

        int object_id(Object *o);
        int joint_id(Joint *j);
        int behavior_id(Circle1DBehavior *b);
        int decal_id(HintDecal *d);

        int objects_count() { return objects.size(); }

        Object *object_from_id(int id);
        Joint *joint_from_id(int id);
        Circle1DBehavior *behavior_from_id(int id);
        HintDecal *decal_from_id(int id);

        Object *object_at(Vec2 pos);
        Joint *joint_at(Vec2 pos);

        void handle_editing(Circle1DEvent *event);

        void handle(Circle1DEvent *event)
        {
            if (event->type == Circle1DEvent::KEYDOWN) {
                switch (event->finger) {
                    case Circle1DEvent::SAVE_SCENE:
                        save("level.chromono");
                        break;
                    case Circle1DEvent::LOAD_SCENE:
                        load("level.chromono");
                        break;
                    case Circle1DEvent::EDIT:
                        editing = !editing;
                        break;
                    default:
                        break;
                }
            }

            if (editing) {
                handle_editing(event);
                return;
            }

            std::list<Object*>::reverse_iterator oit;
            // Must distribute events in reverse order, so that for overlapping
            // objects the "frontmost" (as rendered) item gets the event first
            for (oit = objects.rbegin(); oit != objects.rend(); ++oit) {
                if ((*oit)->handle(event)) {
                    break;
                }
            }

            if (event->type == Circle1DEvent::TICK) {
                std::list<Joint*>::iterator jit;
                for (int i=0; i<20; i++) {
                    for (jit = fixed_joints.begin();
                            jit != fixed_joints.end(); ++jit) {
                        (*jit)->simulate();
                    }
                }

                for (jit = joints.begin(); jit != joints.end(); ++jit) {
                    (*jit)->simulate();
                }

                std::list<Circle1DBehavior*>::iterator bit;
                for (bit=behaviors.begin(); bit != behaviors.end(); ++bit) {
                    (*bit)->simulate();
                }

                time += 1;
                Object::handle_collisions(colliders);
            }
        }

    public:
        Circle1DTime time;
        bool simulation_running;
        RGB background_color;
        bool background_color_set;
        LightSource light_source;

        bool editing;
        std::string editing_label;
        Vec2 editing_label_pos;
        Vec2 editing_old_mouse_pos;
        Object *edit_object;
        Joint *edit_joint;
        Object *edit_joint_tmp;
        int editing_color_index;

    private:
        /* real contents */
        std::list<Object*> objects;
        std::list<Joint*> joints;

        /* derived from above contents for faster access */
        std::list<Joint*> fixed_joints;
        std::list<Joint*> untangle_joints;
        std::list<Object*> colliders;

        /* enabled behaviors */
        std::list<Circle1DBehavior*> behaviors;

        /* visible decals */
        std::list<HintDecal*> decals;
};

#endif /* CARPHONE_MODEL_H */
