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

#ifndef CIRCLE1D_BEHAVIORS_H
#define CIRCLE1D_BEHAVIORS_H

#include "content.h"

#include "../constants.h"
#include "../colors.h"
#include "../platform.h"

#include <vector>
#include <utility>

#include <stdio.h>

class ObjectIsLightSourceBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(ObjectIsLightSourceBehavior)

        ObjectIsLightSourceBehavior(Scene *scene, Object *o)
            : Circle1DBehavior(scene)
            , o(o)
        {
            scene->light_source.enabled = true;
            scene->light_source.pos = o->pos;
        }

        virtual ~ObjectIsLightSourceBehavior()
        {
        }

        virtual void simulate()
        {
            scene->light_source.pos = o->pos;
        }

    private:
        Object *o;

        friend class ObjectIsLightSourceBehaviorIO;
};

class ObjectIsLightSourceBehaviorIO : public Circle1DBehaviorIO {
public:
    virtual void save(Scene *scene, Circle1DBehavior *behavior, SerializeBuffer &buffer) {
        ObjectIsLightSourceBehavior *b = dynamic_cast<ObjectIsLightSourceBehavior *>(behavior);
        int oid = scene->object_id(b->o);
        buffer << oid;
    }

    virtual Circle1DBehavior *load(Scene *scene, SerializeBuffer &buffer) {
        int oid;
        buffer >> oid;

        return new ObjectIsLightSourceBehavior(scene, scene->object_from_id(oid));
    }
};

CIRCLE1D_BEHAVIOR_REGISTER(ObjectIsLightSourceBehavior, ObjectIsLightSourceBehaviorIO)


class CenterOfObjectsLightSource : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(CenterOfObjectsLightSource)

        CenterOfObjectsLightSource(Scene *scene)
            : Circle1DBehavior(scene)
            , objects()
        {
        }

        void add(Object *object) {
            objects.push_back(object);
            scene->light_source.enabled = true;
            simulate();
        }

        virtual ~CenterOfObjectsLightSource()
        {
        }

        virtual void simulate()
        {
            Vec2 center(0, 0);
            for (size_t i=0; i<objects.size(); i++) {
                center += objects[i]->pos;
            }
            center /= objects.size();

            scene->light_source.pos = center;
        }

    private:
        std::vector<Object*> objects;

        friend class CenterOfObjectsLightSourceIO;
};

class CenterOfObjectsLightSourceIO : public Circle1DBehaviorIO {
public:
    virtual void save(Scene *scene, Circle1DBehavior *behavior, SerializeBuffer &buffer) {
        CenterOfObjectsLightSource *c = dynamic_cast<CenterOfObjectsLightSource *>(behavior);
        int count = c->objects.size();
        buffer << count;
        for (int i=0; i<count; i++) {
            buffer << scene->object_id(c->objects[i]);
        }
    }

    virtual Circle1DBehavior *load(Scene *scene, SerializeBuffer &buffer) {
        int count = buffer.read_int32();

        CenterOfObjectsLightSource *result = new CenterOfObjectsLightSource(scene);
        for (int i=0; i<count; i++) {
            result->add(scene->object_from_id(buffer.read_int32()));
        }
        return result;
    }
};

CIRCLE1D_BEHAVIOR_REGISTER(CenterOfObjectsLightSource, CenterOfObjectsLightSourceIO)

class RotatingBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(RotatingBehavior)

        RotatingBehavior(Scene *scene, int cx, int cy,
                Object *o, float velocity=2.0, bool draw_spoke=false,
                float sx=1.0, float sy=1.0)
            : Circle1DBehavior(scene)
            , cx(cx)
            , cy(cy)
            , sx(sx)
            , sy(sy)
            , o(o)
            , phase(0)
            , radius(0)
            , velocity(velocity)
            , running(true)
        {
            // Debug: Show center of rotation + draw joint
            if (draw_spoke) {
                new Joint(scene, o, new Object(scene, cx, cy, 10, Object::FIXED));
            }

            float x = (o->pos.x - cx);
            float y = (o->pos.y - cy);
            phase = atan2f(y, x);
            radius = sqrtf(x*x + y*y);
        }

        virtual ~RotatingBehavior()
        {
        }

        virtual void simulate()
        {
            if (running) {
                phase += velocity / 180.0 * M_PI;
            }

            o->pos.x = cx + sx * radius * cosf(phase);
            o->pos.y = cy + sy * radius * sinf(phase);
        }

        void set_running(bool running)
        {
            this->running = running;
        }

        virtual bool save(SerializeBuffer &buffer) { return false; }

    protected:
        int cx;
        int cy;
        float sx;
        float sy;
        Object *o;
        float phase;
        float radius;
        float velocity;
        bool running;

        friend class RotatingBehaviorIO;
};

class RotatingBehaviorIO : public Circle1DBehaviorIO {
public:
    virtual void save(Scene *scene, Circle1DBehavior *behavior, SerializeBuffer &buffer) {
        RotatingBehavior *r = dynamic_cast<RotatingBehavior *>(behavior);
        int oid = scene->object_id(r->o);
        buffer << r->cx << r->cy << r->sx << r->sy << oid << r->phase << r->radius << r->velocity << int(r->running);
    }

    virtual Circle1DBehavior *load(Scene *scene, SerializeBuffer &buffer) {
        int cx, cy, object_id, running;
        float sx, sy, phase, radius, velocity;

        buffer >> cx >> cy >> sx >> sy >> object_id >> phase >> radius >> velocity >> running;

        RotatingBehavior *result = new RotatingBehavior(scene, cx, cy,
                scene->object_from_id(object_id), velocity, false, sx, sy);
        result->phase = phase;
        result->radius = radius;
        result->velocity = velocity;
        result->running = running;
        return result;
    }
};

CIRCLE1D_BEHAVIOR_REGISTER(RotatingBehavior, RotatingBehaviorIO)

class RotatingBehaviorAround : public RotatingBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(RotatingBehaviorAround)

        RotatingBehaviorAround(Scene *scene, Object *center,
                Object *o, float velocity=2.0)
            : RotatingBehavior(scene, center->pos.x, center->pos.y, o, velocity)
            , center(center)
        {
        }

        virtual ~RotatingBehaviorAround()
        {
        }

        virtual void simulate()
        {
            cx = center->pos.x;
            cy = center->pos.y;
            RotatingBehavior::simulate();
        }

    private:
        Object *center;
};

class SwitchForRotatingBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(SwitchForRotatingBehavior)

        SwitchForRotatingBehavior(Scene *scene,
                Object *switch_object=NULL, bool is_on=false)
            : Circle1DBehavior(scene)
            , rotates()
            , switch_object(switch_object)
            , is_on(is_on)
        {
        }

        virtual ~SwitchForRotatingBehavior()
        {
        }

        virtual void simulate()
        {
            if (switch_object == NULL) {
                return;
            }

            bool new_on = switch_object->target_color.equals(switch_object->desired);
            if (new_on != is_on) {
                std::vector<RotatingBehavior*>::iterator it;
                for (it=rotates.begin(); it != rotates.end(); ++it) {
                    (*it)->set_running(new_on);
                }
                is_on = new_on;
            }
        }

        void set_switch(Object *switch_object)
        {
            this->switch_object = switch_object;
        }

        void add_rotate(RotatingBehavior *rotate)
        {
            rotates.push_back(rotate);
            rotate->set_running(is_on);
        }

    private:
        std::vector<RotatingBehavior*> rotates;
        Object *switch_object;
        bool is_on;
};

class SwitchForGravity : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(SwitchForGravity)

        SwitchForGravity(Scene *scene, Object *switch_object,
                Object *target_object)
            : Circle1DBehavior(scene)
            , switch_object(switch_object)
            , target_object(target_object)
            , is_on(!(target_object->flags & Object::NOGRAVITY))
        {
        }

        virtual void simulate()
        {
            bool new_on = switch_object->target_color.equals(switch_object->desired);
            if (new_on != is_on) {
                if (new_on) {
                    target_object->flags &= ~(Object::NOGRAVITY);
                } else {
                    target_object->flags |= Object::NOGRAVITY;
                }
                is_on = new_on;
            }
        }

    private:
        Object *switch_object;
        Object *target_object;
        bool is_on;
};

class ColorCombinationBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(ColorCombinationBehavior)

        enum Method {
            MIX,
            ADD,
        };

        ColorCombinationBehavior(Scene *scene, Object *target, Object *a, Object *b, enum Method method=MIX)
            : Circle1DBehavior(scene)
            , target(target)
            , a(a)
            , b(b)
            , method(method)
        {
            // TODO: a and b should have SWITCHONLY flags
            new Joint(scene, a, target);
            new Joint(scene, b, target);
        }

        virtual ~ColorCombinationBehavior()
        {
        }

        virtual void simulate()
        {
            switch (method) {
                case MIX:
                    target->target_color = RGB::mix(a->target_color, b->target_color);
                    break;
                case ADD:
                    target->target_color = RGB::add(a->target_color, b->target_color);
                    break;
            }
        }

    private:
        Object *target;
        Object *a;
        Object *b;
        enum Method method;

        friend class ColorCombinationBehaviorIO;
};

class ColorCombinationBehaviorIO : public Circle1DBehaviorIO {
public:
    virtual void save(Scene *scene, Circle1DBehavior *behavior, SerializeBuffer &buffer) {
        ColorCombinationBehavior *c = dynamic_cast<ColorCombinationBehavior *>(behavior);
        int target_id = scene->object_id(c->target);
        int a_id = scene->object_id(c->a);
        int b_id = scene->object_id(c->b);
        buffer << target_id << a_id << b_id << int(c->method);
    }

    virtual Circle1DBehavior *load(Scene *scene, SerializeBuffer &buffer) {
        int target_id, a_id, b_id, method;
        buffer >> target_id >> a_id >> b_id >> method;

        return new ColorCombinationBehavior(scene, scene->object_from_id(target_id),
                scene->object_from_id(a_id), scene->object_from_id(b_id),
                ColorCombinationBehavior::Method(method));
    }
};

CIRCLE1D_BEHAVIOR_REGISTER(ColorCombinationBehavior, ColorCombinationBehaviorIO)

class SwitchToggleBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(SwitchToggleBehavior)

        SwitchToggleBehavior(Scene *scene, Object *switch_object, RGB green, RGB red)
            : Circle1DBehavior(scene)
            , switch_object(switch_object)
            , green(green)
            , red(red)
            , grabbed(false)
        {
        }

        virtual ~SwitchToggleBehavior()
        {
        }

        virtual void simulate()
        {
            bool new_grabbed = (switch_object->grabbed != -1);
            if (new_grabbed != grabbed) {
                grabbed = new_grabbed;
                if (grabbed) {
                    if (switch_object->target_color.equals(green)) {
                        switch_object->target_color = red;
                        Platform::play(Sound::TOGGLE_SWITCH_OFF);
                    } else {
                        switch_object->target_color = green;
                        Platform::play(Sound::TOGGLE_SWITCH_ON);
                    }
                }
            }
        }

    private:
        Object *switch_object;
        RGB green;
        RGB red;
        bool grabbed;
};

Object *
build_switch(Scene *scene, int x, int y, RGB green, RGB red)
{
    Object *switch_object = new Object(scene, x, y, 30,
            Object::COLORABLE | Object::FIXED | Object::COLLIDER |
            Object::SWITCHONLY | Object::CANGRAB, red);
    switch_object->desired = green;

#if 0
    Object *green_object = new Object(scene, x-30, y+30, 20,
            Object::COLLIDER, green);
    new Joint(scene, switch_object, green_object, Joint::RUBBERBAND);

    Object *red_object = new Object(scene, x+30, y+30, 20,
            Object::COLLIDER, red);
    new Joint(scene, switch_object, red_object, Joint::RUBBERBAND);
#endif

    new SwitchToggleBehavior(scene, switch_object, green, red);

    return switch_object;
}

Object *
build_color_mix(Scene *scene, int x, int y, bool is_leaf=false)
{
    RGB black(0.2, 0.2, 0.2);

    int flags = Object::FIXED | Object::COLORABLE |
        Object::COLLIDER |  Object::SWITCHONLY;

    if (!is_leaf) {
        flags |= Object::NOAUTOCOLOR;
    }

    Object *mix = new Object(scene, x, y, 30, flags, black);
    mix->desired = black;

    return mix;
}

Object *
build_color_target(Scene *scene, int x, int y, RGB color)
{
    Object *target = new Object(scene, x, y, 40,
            Object::FIXED | Object::COLLIDER |
            Object::COLORABLE | Object::NOAUTOCOLOR);
    target->desired = color;

    return target;
}

Object *
build_paint_target(Scene *scene, int x, int y, RGB color)
{
    return new Object(scene, x, y, 40, Object::NOGRAVITY |
            Object::COLLIDER | Object::BORDERBOUNCE, color);
}

class EmitterBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(EmitterBehavior)

        EmitterBehavior(Scene *scene, Object *emitter, Joint *joint, Object *target,
                bool auto_emit, Circle1DTime interval, Circle1DTime duration)
            : Circle1DBehavior(scene)
            , emitter(emitter)
            , joint(joint)
            , target(target)
            , auto_emit(auto_emit)
            , interval(interval)
            , duration(duration)
            , last_emission(-duration)
            , colorized(true)
        {
        }

        virtual ~EmitterBehavior()
        {
        }

        bool emit(bool force=false)
        {
            if (!force && (last_emission + duration >= scene->time)) {
                // Can't emit new one until current one has arrived
                return false;
            }

            last_emission = scene->time;
            colorized = false;

            return true;
        }

        virtual void simulate()
        {
            Circle1DTime now = scene->time;

            if (!colorized && (last_emission + duration < now)) {
                // arrived - color target to emitter target
                target->target_color = emitter->target_color;
                colorized = true;
            }

            if (auto_emit && (last_emission + interval < now)) {
                emit();
            }

            joint->knot_color = emitter->target_color;
            joint->knot_at = 1.0 - ((float)(now - last_emission) / (float)duration);
        }

    public:
        Object *emitter;

    private:
        Joint *joint;
        Object *target;
        bool auto_emit;
        Circle1DTime interval;
        Circle1DTime duration;

        Circle1DTime last_emission;
        bool colorized;
};

EmitterBehavior *
build_emitter(Scene *scene, int x, int y, RGB color,
        int interval_ms, int duration_ms, Object *target, bool auto_emit=true)
{
    // Right now we don't support more than one knot!
    SHADY_ASSERT(!auto_emit || duration_ms <= interval_ms);

    Object *emitter = new Object(scene, x, y, 30, Object::FIXED, color);
    Joint *joint = new Joint(scene, emitter, target, Joint::HAS_KNOT);

    return new EmitterBehavior(scene, emitter, joint, target, auto_emit,
            interval_ms / Constants::TICK_MS, duration_ms / Constants::TICK_MS);
}

class SwitchForEmitterBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(SwitchForEmitterBehavior)

        SwitchForEmitterBehavior(Scene *scene,
                Object *switch_object,
                EmitterBehavior *emitter_object,
                RGB color_after_emit,
                bool copy_color_on_emit=false)
            : Circle1DBehavior(scene)
            , switch_object(switch_object)
            , emitter_object(emitter_object)
            , color_after_emit(color_after_emit)
            , copy_color_on_emit(copy_color_on_emit)
        {
        }

        virtual ~SwitchForEmitterBehavior()
        {
        }

        virtual void simulate()
        {
            if (switch_object->color.equals(switch_object->desired)) {
                if (emitter_object->emit()) {
                    if (copy_color_on_emit) {
                        // When emitting, copy the switch color to the emitter
                        emitter_object->emitter->target_color = switch_object->target_color;
                    }
                    switch_object->target_color = color_after_emit;
                }
            }
        }

    private:
        Object *switch_object;
        EmitterBehavior *emitter_object;
        RGB color_after_emit;
        bool copy_color_on_emit;
};

class CopyColorBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(CopyColorBehavior)

        CopyColorBehavior(Scene *scene, Object *from, Object *to)
            : Circle1DBehavior(scene)
            , from(from)
            , to(to)
        {
        }

        virtual ~CopyColorBehavior()
        {
        }

        virtual void simulate()
        {
            to->target_color = from->target_color;
        }

    private:
        Object *from;
        Object *to;
};

class AxisConstraintBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(AxisConstraintBehavior)

        AxisConstraintBehavior(Scene *scene, Object *constrained,
                bool major_is_xaxis, int major_axis_value, int minor_min, int minor_max,
                bool make_joint=true)
            : Circle1DBehavior(scene)
            , constrained(constrained)
            , major_is_xaxis(major_is_xaxis)
            , major_axis_value(major_axis_value)
            , minor_min(minor_min)
            , minor_max(minor_max)
            , topleft(0, 0)
            , bottomright(0, 0)
        {
            float size = 10;
            float overlap = 4;

            int minvalue = minor_min-constrained->size-size+overlap;
            int maxvalue = minor_max+constrained->size+size-overlap;

            float x1, y1, x2, y2;
            if (major_is_xaxis) {
                x1 = x2 = major_axis_value;
                y1 = minvalue;
                y2 = maxvalue;

                topleft = Vec2(major_axis_value, minor_min);
                bottomright = Vec2(major_axis_value, minor_max);
            } else {
                y1 = y2 = major_axis_value;
                x1 = minvalue;
                x2 = maxvalue;

                topleft = Vec2(minor_min, major_axis_value);
                bottomright = Vec2(minor_max, major_axis_value);
            }

            if (make_joint) {
                new Joint(scene,
                        new Object(scene, x1, y1, size, Object::FIXED),
                        new Object(scene, x2, y2, size, Object::FIXED),
                        Joint::IS_RAIL);
            }

            simulate();
        }

        virtual ~AxisConstraintBehavior()
        {
        }

        virtual void simulate()
        {
            constrained->pos.x = std::max(topleft.x, std::min(bottomright.x, constrained->pos.x));
            constrained->pos.y = std::max(topleft.y, std::min(bottomright.y, constrained->pos.y));
        }

    private:
        Object *constrained;
        bool major_is_xaxis;
        int major_axis_value;
        int minor_min;
        int minor_max;
        Vec2 topleft;
        Vec2 bottomright;

        friend class AxisConstraintBehaviorIO;
};

class AxisConstraintBehaviorIO : public Circle1DBehaviorIO {
public:
    virtual void save(Scene *scene, Circle1DBehavior *behavior, SerializeBuffer &buffer) {
        AxisConstraintBehavior *a = dynamic_cast<AxisConstraintBehavior *>(behavior);
        buffer << scene->object_id(a->constrained) << int(a->major_is_xaxis) <<
            a->major_axis_value << a->minor_min << a->minor_max;
    }

    virtual Circle1DBehavior *load(Scene *scene, SerializeBuffer &buffer) {
        int oid, major_is_xaxis, major_axis_value, minor_min, minor_max;
        buffer >> oid >> major_is_xaxis >>
            major_axis_value >> minor_min >> minor_max;

        return new AxisConstraintBehavior(scene, scene->object_from_id(oid),
                major_is_xaxis, major_axis_value, minor_min, minor_max, false);
    }
};

CIRCLE1D_BEHAVIOR_REGISTER(AxisConstraintBehavior, AxisConstraintBehaviorIO)


class XAxisOrderConstraint : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(XAxisOrderConstraint)

        XAxisOrderConstraint(Scene *scene, Object *left, Object *right)
            : Circle1DBehavior(scene)
            , left(left)
            , right(right)
        {
        }

        virtual ~XAxisOrderConstraint()
        {
        }

        virtual void simulate()
        {
            if (left->pos.x > right->pos.x) {
                float center = (left->pos.x + right->pos.x) / 2.f;
                left->pos.x = center - 1.f;
                right->pos.x = center + 1.f;
            }
        }

    private:
        Object *left;
        Object *right;

        friend class XAxisOrderConstraintIO;
};

class XAxisOrderConstraintIO : public Circle1DBehaviorIO {
public:
    virtual void save(Scene *scene, Circle1DBehavior *behavior, SerializeBuffer &buffer) {
        XAxisOrderConstraint *x = dynamic_cast<XAxisOrderConstraint *>(behavior);
        buffer << scene->object_id(x->left) << scene->object_id(x->right);
    }

    virtual Circle1DBehavior *load(Scene *scene, SerializeBuffer &buffer) {
        int left_id, right_id;
        buffer >> left_id >> right_id;

        return new XAxisOrderConstraint(scene, scene->object_from_id(left_id),
                scene->object_from_id(right_id));
    }
};

CIRCLE1D_BEHAVIOR_REGISTER(XAxisOrderConstraint, XAxisOrderConstraintIO)


class YAxisPendulumBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(YAxisPendulumBehavior)

        YAxisPendulumBehavior(Scene *scene, Object *object,
                int ymin, int ymax, float factor=1.0, float phase=0.0)
            : Circle1DBehavior(scene)
            , object(object)
            , ymin(ymin)
            , ymax(ymax)
            , factor(factor)
            , phase(phase)
        {
        }

        virtual ~YAxisPendulumBehavior()
        {
        }

        virtual void simulate() {
            float value = sinf(phase + scene->time * factor);
            float center = (ymin + ymax) / 2.0;
            float radius = (ymax - ymin) / 2.0;
            object->pos.y = center + radius * value;
        }

    private:
        Object *object;
        int ymin;
        int ymax;
        float factor;
        float phase;

        friend class YAxisPendulumBehaviorIO;
};

class YAxisPendulumBehaviorIO : public Circle1DBehaviorIO {
public:
    virtual void save(Scene *scene, Circle1DBehavior *behavior, SerializeBuffer &buffer) {
        YAxisPendulumBehavior *y = dynamic_cast<YAxisPendulumBehavior *>(behavior);
        buffer << scene->object_id(y->object) << y->ymin << y->ymax << y->factor << y->phase;
    }

    virtual Circle1DBehavior *load(Scene *scene, SerializeBuffer &buffer) {
        int oid, ymin, ymax;
        float factor, phase;

        buffer >> oid >> ymin >> ymax >> factor >> phase;

        return new YAxisPendulumBehavior(scene, scene->object_from_id(oid),
                ymin, ymax, factor, phase);
    }
};

CIRCLE1D_BEHAVIOR_REGISTER(YAxisPendulumBehavior, YAxisPendulumBehaviorIO)

class XAxisPendulumBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(XAxisPendulumBehavior)

        XAxisPendulumBehavior(Scene *scene, Object *object,
                int xmin, int xmax, float factor=1.0, float phase=0.0)
            : Circle1DBehavior(scene)
            , object(object)
            , xmin(xmin)
            , xmax(xmax)
            , factor(factor)
            , phase(phase)
        {
        }

        virtual ~XAxisPendulumBehavior()
        {
        }

        virtual void simulate() {
            float value = sinf(phase + scene->time * factor);
            float center = (xmin + xmax) / 2.0;
            float radius = (xmax - xmin) / 2.0;
            object->pos.x = center + radius * value;
        }

    private:
        Object *object;
        int xmin;
        int xmax;
        float factor;
        float phase;

        friend class XAxisPendulumBehaviorIO;
};

class XAxisPendulumBehaviorIO : public Circle1DBehaviorIO {
public:
    virtual void save(Scene *scene, Circle1DBehavior *behavior, SerializeBuffer &buffer) {
        XAxisPendulumBehavior *x = dynamic_cast<XAxisPendulumBehavior *>(behavior);
        buffer << scene->object_id(x->object) << x->xmin << x->xmax << x->factor << x->phase;
    }

    virtual Circle1DBehavior *load(Scene *scene, SerializeBuffer &buffer) {
        int oid, xmin, xmax;
        float factor, phase;

        buffer >> oid >> xmin >> xmax >> factor >> phase;

        return new XAxisPendulumBehavior(scene, scene->object_from_id(oid),
                xmin, xmax, factor, phase);
    }
};

CIRCLE1D_BEHAVIOR_REGISTER(XAxisPendulumBehavior, XAxisPendulumBehaviorIO)

class UntangleLevel {
    public:
        UntangleLevel(Scene *scene, RGB color=RGB(0.5, 0.7, 0.9))
            : scene(scene)
            , first(NULL)
            , previous(NULL)
            , color(color)
        {
        }

        Object *operator^(Vec2 pos) {
            return new Object(scene, pos.x, pos.y, 40, Object::NOGRAVITY |
                    Object::BORDERBOUNCE | Object::COLLIDER, color);
        }

        UntangleLevel &operator<<(Vec2 pos) {
            return (*this) << ((*this) ^ pos);
        }

        UntangleLevel &operator<<(Object *current) {
            if (previous) {
                new Joint(scene, previous, current, Joint::UNTANGLE);
            }

            if (!first) {
                first = current;
            }

            previous = current;

            return *this;
        }

        void operator>>(bool loop) {
            if (loop && first && previous && first != previous) {
                new Joint(scene, previous, first, Joint::UNTANGLE);
            }

            first = NULL;
            previous = NULL;
        }

        UntangleLevel &operator||(Object *o) {
            o->flags |= Object::NODRAG;
            o->target_color = o->color = RGB();
            return *this;
        }

    private:
        Scene *scene;
        Object *first;
        Object *previous;
        RGB color;
};

std::vector<Object*>
build_chain(Scene *scene, Object *from_object, Object *to_object, int count, int flags, int object_flags=0)
{
    std::vector<Object*> result;

    Vec2 a_to_b = (from_object->pos - to_object->pos).normalize();

    Vec2 a = from_object->pos - a_to_b * from_object->size;
    Vec2 b = to_object->pos + a_to_b * to_object->size;

    Object *last = from_object;
    for (int i=0; i<count; i++) {
        float position = 1.0 - ((float)(i+1.0) / (float)(count+1.0));

        Vec2 p = a * position + b * (1.0 - position);
        Object *current = new Object(scene, p.x, p.y, 10, Object::NODRAG | Object::COLLIDER | Object::BORDERBOUNCE | object_flags);

        result.push_back(current);
        new Joint(scene, last, current, Joint::RUBBERBAND | flags);
        last = current;
    }
    new Joint(scene, last, to_object, Joint::RUBBERBAND | flags);

    return result;
}

void
bezier_chain(Scene *scene, Object *from_object, Object *to_object, Vec2 ctrl, int count, int flags=0)
{
    std::vector<Object*> result;

    Object *last = from_object;
    for (int i=0; i<count; i++) {
        float position = 1.0 - ((float)(i+1.0) / (float)(count+1.0));

        Vec2 a = from_object->pos * position + ctrl * (1.0 - position);
        Vec2 b = ctrl * position + to_object->pos * (1.0 - position);
        Vec2 p = a * position + b * (1.0 - position);

        Object *current = new Object(scene, p.x, p.y, 10, Object::NODRAG | Object::COLLIDER | Object::SHADOWCAST);

        result.push_back(current);
        new Joint(scene, last, current, flags);
        last = current;
    }
    new Joint(scene, last, to_object, flags);
}

class BackgroundColorFromObjectBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(BackgroundColorFromObjectBehavior)

        BackgroundColorFromObjectBehavior(Scene *scene, Object *o)
            : Circle1DBehavior(scene)
            , o(o)
        {
            simulate();
        }

        virtual ~BackgroundColorFromObjectBehavior()
        {
        }

        virtual void simulate()
        {
            scene->background_color = RGB::mix(RGB(0x000000), o->color);
        }

    private:
        Object *o;

        friend class BackgroundColorFromObjectBehaviorIO;
};

class BackgroundColorFromObjectBehaviorIO : public Circle1DBehaviorIO {
public:
    virtual void save(Scene *scene, Circle1DBehavior *behavior, SerializeBuffer &buffer) {
        BackgroundColorFromObjectBehavior *b = dynamic_cast<BackgroundColorFromObjectBehavior *>(behavior);
        int oid = scene->object_id(b->o);
        buffer << oid;
    }

    virtual Circle1DBehavior *load(Scene *scene, SerializeBuffer &buffer) {
        int oid;
        buffer >> oid;

        return new BackgroundColorFromObjectBehavior(scene, scene->object_from_id(oid));
    }
};

CIRCLE1D_BEHAVIOR_REGISTER(BackgroundColorFromObjectBehavior, BackgroundColorFromObjectBehaviorIO)

Object *
build_simple_combiner(Scene *scene, RGB target,
        int x, int y, int x1, int y1, int x2, int y2,
        ColorCombinationBehavior::Method method=ColorCombinationBehavior::MIX)
{
    Object *o1 = build_color_mix(scene, x1, y1, true);
    Object *o2 = build_color_mix(scene, x2, y2, true);
    Object *o3 = build_color_target(scene, x, y, target);
    new ColorCombinationBehavior(scene, o3, o1, o2, method);
    return o3;
}

#endif /* CIRCLE1D_BEHAVIORS_H */
