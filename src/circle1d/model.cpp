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


#include "model.h"
#include "../platform.h"
#include "../constants.h"
#include "../colors.h"
#include "../util.h"
#include "serialize.h"
#include "resources_util.h"

static const RGB EDIT_SWATCH[10] = {
    RGB(),
    RGB(0.2, 0.7, 0.9),
    RGB(0xe70b92),
    RGB(0.4, 0.9, 0.2),
    RGB(0xc0d700),
    RGB(0.5, 0.9, 0.7),
    RGB(0.6, 0.2, 0.6),
    RGB(0x406849),
    RGB(0.5, 0.9, 0.7),
    RGB(0.7, 0.9, 0.5),
};

HintDecal::HintDecal(Scene *scene, enum Circle1D::Decal decal, int x, int y)
    : Circle1DType(Circle1DType::DECAL)
    , decal(decal)
    , opacity(1.f)
    , x(x)
    , y(y)
{
    if (scene) {
        scene->add_decal(this);
    }
}

Object::Object(Scene *scene, float x, float y, float size, int flags, RGB color)
    : Circle1DType(Circle1DType::OBJECT)
    , scene(scene),
      pos(x, y),
      last_pos(x, y),
      size(size),
      color(color),
      target_color(color),
      desired(color),
      flags(flags),
      velocity(0, 0),
      force(0, 0),
      grabbed(-1),
      old_mouse_pos(0, 0),
      mouse(NULL),
      mouse_joint(NULL),
      compound(NULL),
      last_color_update(0)
{
    if (scene) {
        scene->add_object(this);
    }
}

std::string
Object::describe()
{
    std::string flags_str = "";

#define DESCRIBE(x) if (flags & x) flags_str += #x " ";
    DESCRIBE(COLLIDER)
    DESCRIBE(SLOW)
    DESCRIBE(NOGRAVITY)
    DESCRIBE(FIXED)
    DESCRIBE(COLORABLE)
    DESCRIBE(NODRAG)
    DESCRIBE(COLORTRANSFER)
    DESCRIBE(SWITCHONLY)
    DESCRIBE(NOAUTOCOLOR)
    DESCRIBE(BORDERBOUNCE)
    DESCRIBE(STAYINVIEW)
    DESCRIBE(CANGRAB)
    DESCRIBE(NOOVERLAP)
    DESCRIBE(SHADOWCAST)
    DESCRIBE(SILENT)
    DESCRIBE(INVISIBLE)
#undef DESCRIBE

    return Util::format("Object (%.2f, %.2f), size=%.2f, flags=[%s]", pos.x, pos.y, size, flags_str.c_str());
}

bool
Object::handle(Circle1DEvent *event)
{
    if (event->type == Circle1DEvent::MOUSEDOWN) {
        Vec2 pos(event->x, event->y);
        // FIXED objects behave just like NODRAG objects
        // in terms of mouse dragging, but when CANGRAB is
        // set, these objects can be (invisibly) grabbed
        // (this is used for activating switch objects)
        if ((grabbed == -1) && (this->pos - pos).length() < size &&
                !(flags & NODRAG) &&
                (!(flags & FIXED) || (flags & CANGRAB))) {
            grabbed = event->finger;
            if (!(flags & CANGRAB)) {
                if (!(scene->simulation_running)) {
                    Platform::play(Sound::MOVABLE_SPHERE_PRESS);
                }
            }
            old_mouse_pos = pos;
            if (!(flags & FIXED)) {
                mouse = new Object(scene, this->pos.x,
                        this->pos.y, 10, Object::FIXED | Object::INVISIBLE);
                mouse_joint = new Joint(scene, this, mouse, Joint::MOUSE);
            }
            return true;
        }
    } else if (event->type == Circle1DEvent::MOUSEMOTION) {
        if (grabbed == event->finger) {
            Vec2 pos(event->x, event->y);
            Vec2 rel = pos - old_mouse_pos;
            if (mouse) {
                mouse->pos += rel;
            }
            old_mouse_pos = pos;
            return true;
        }
    } else if (event->type == Circle1DEvent::MOUSEUP) {
        if (grabbed == event->finger) {
            if (!(flags & CANGRAB)) {
                if (!(scene->simulation_running)) {
                    Platform::play(Sound::MOVABLE_SPHERE_RELEASE);
                }
            }
            if (mouse) {
                scene->remove_object(mouse);
                delete mouse;
                mouse = NULL;
            }
            if (mouse_joint) {
                scene->remove_object(mouse_joint);
                delete mouse_joint;
                mouse_joint = NULL;
            }
            grabbed = -1;
            return true;
        }
    } else if (event->type == Circle1DEvent::TICK) {
        simulate();
    }

    return false;
}

void
Object::take_color(Object *b)
{
    if (last_color_update < scene->time - 10) {
        if (!target_color.equals(b->target_color)) {
            // Do not play sounds if the scene is simulating
            if (!(scene->simulation_running)) {
                if (!(flags & (NOAUTOCOLOR | SILENT))) {
                    if (b->target_color.equals(desired)) {
                        Platform::play(Sound::COLOR_CHANGES_TO_RIGHT);
                    } else {
                        Platform::play(Sound::COLOR_CHANGES_TO_WRONG);
                    }
                }
            }
        }
        last_color_update = scene->time;
    }

    if ((flags & NOAUTOCOLOR) == 0) {
        target_color = b->target_color;
    }
}

void
Object::handle_collision(Object *b)
{
    Object *a = this;
    Vec2 dist = (a->pos - b->pos);
    if ((dist.x*dist.x + dist.y*dist.y) < (a->size + b->size)*(a->size + b->size)) {
        float diff = (a->size + b->size) - dist.length();
        Vec2 a_dir = (b->pos - a->pos).normalize();
        Vec2 b_dir = dist.normalize();
        if (a->flags & NOOVERLAP || b->flags & NOOVERLAP) {
            // Move the other object out of the way (the one that is not fixed)
            // Assumption: We only set NOOVERLAP on FIXED objects
            float alpha = 0.1;
            if (a->flags & FIXED) {
                b->pos = b->pos * alpha + (a->pos - b_dir * (a->size + b->size)) * (1.0 - alpha);
            } else {
                a->pos = a->pos * alpha + (b->pos - a_dir * (a->size + b->size)) * (1.0 - alpha);
            }

            // Apply a slightly larger force to the objects
            float force = diff*std::min(a->size, b->size) * 0.6;
            a->apply_force(b_dir*force);
            b->apply_force(a_dir*force);
        } else {
            float force = diff*2.9*std::min(a->size, b->size)/10.;
            a->apply_force(b_dir*force);
            b->apply_force(a_dir*force);
        }

        if ((a->flags & COLORABLE) && (b->flags & COLORABLE)) {
            if ((b->flags & COLORTRANSFER) != 0 && !(b->target_color.isgray())) {
                a->take_color(b);
            } else if ((a->flags & COLORTRANSFER) != 0 && (!a->target_color.isgray())) {
                b->take_color(a);
            }
        } else if ((a->flags & COLORABLE) != 0 && !(b->target_color.isgray())) {
            a->take_color(b);
        } else if ((b->flags & COLORABLE) != 0 && !(a->target_color.isgray())) {
            b->take_color(a);
        }
    }
}

void
Object::simulate()
{
    if (!(flags & (Object::NOGRAVITY | Object::FIXED))) {
        /* Gravity force */
        force += Vec2(0, 16.2) * weight() / 2000.;
    }

    velocity += force / weight() * 100.;
    velocity *= .9;
    if (flags & Object::SLOW) {
        velocity *= .5;
    }
    pos += velocity;
    force = Vec2(0, 0);
    if (flags & (Object::BORDERBOUNCE | Object::STAYINVIEW)) {
        float bounce = 0.;
        float xmin = 0, ymin = 0;
        float xmax = Constants::WORLD_WIDTH, ymax = Constants::WORLD_HEIGHT;

        if (flags & Object::BORDERBOUNCE) {
            xmin += size;
            ymin += size;
            xmax -= size;
            ymax -= size;
            bounce = -0.8;
        }

        bool bounced = false;

        if (pos.x < xmin) {
            velocity.x *= bounce;
            pos.x = xmin;

            if (last_pos.x > xmin) {
                bounced = true;
            }
        }

        if (pos.x > xmax) {
            velocity.x *= bounce;
            pos.x = xmax;
            if (last_pos.x < xmax) {
                bounced = true;
            }
        }

        if (pos.y < ymin) {
            velocity.y *= bounce;
            pos.y = ymin;
            if (last_pos.y > ymin) {
                bounced = true;
            }
        }

        if (pos.y > ymax) {
            velocity.y *= bounce;
            pos.y = ymax;
            if (last_pos.y < ymax) {
                bounced = true;
            }
        }

        if (bounced && size >= 30) {
            // XXX: Volume based on velocity.length()
            if (!(scene->simulation_running)) {
                Platform::play(Sound::SPHERE_HITS_WALL);
            }
        }
    }

    color.fade_to(target_color);

    last_pos = pos;
}

void
Object::restack_on_top()
{
    scene->remove_object(this);
    scene->add_object(this);
}

Joint::Joint(Scene *scene, Object *a, Object *b, int flags)
    : Circle1DType(Circle1DType::JOINT),
      scene(scene),
      a(a),
      b(b),
      flags(flags),
      knot_at(0.0),
      knot_color(),
      distance((a->pos - b->pos).length())
{
    scene->add_object(this);
}

Joint::~Joint()
{
}

std::string
Joint::describe()
{
    std::string flags_str = "";

#define DESCRIBE(x) if (flags & x) flags_str += #x " ";
    DESCRIBE(RUBBERBAND)
    DESCRIBE(STRONG)
    DESCRIBE(BREAKABLE)
    DESCRIBE(BROKEN)
    DESCRIBE(FIXED)
    DESCRIBE(HAS_KNOT)
    DESCRIBE(IS_RAIL)
    DESCRIBE(MOUSE)
    DESCRIBE(UNTANGLE)
#undef DESCRIBE

    return Util::format("Joint flags=[%s]", flags_str.c_str());
}


void
Joint::simulate()
{
    if (flags & Joint::BROKEN) {
        return;
    }

    if (flags & Joint::UNTANGLE) {
        // Untangle joints don't have any physical effect
        return;
    }

    float current_distance = (a->pos - b->pos).length();

    float diff = distance - current_distance;
    if (fabsf(diff) < 0.001) {
        return;
    }

    if ((flags & Joint::BREAKABLE) && fabsf(diff) > fabsf(distance) * 4.) {
        flags |= Joint::BROKEN;
        if (!(scene->simulation_running)) {
            Platform::play(Sound::CHAIN_BREAKS);
        }
        return;
    }

    Vec2 a_dir = (b->pos - a->pos).normalize();
    Vec2 b_dir = (a->pos - b->pos).normalize();

    if (flags & Joint::FIXED) {
        if (a->flags & Object::FIXED) {
            b->move(a->pos + a_dir * distance);
        } else if (b->flags & Object::FIXED) {
            a->move(b->pos + b_dir * distance);
        } else {
            Vec2 center = Vec2::mean(a->pos, b->pos);
            a->move(center + b_dir*distance/2.);
            b->move(center + a_dir*distance/2.);
        }
        return;
    }

    float strength = fabsf(diff) * 2.;
    if (flags & Joint::RUBBERBAND) {
        strength *= .2;
    }
    if (flags & Joint::STRONG) {
        strength *= 3.;
    }

    if (flags & Joint::MOUSE) {
        Vec2 center = Vec2::mean(a->pos, b->pos);
        if (!(a->flags & Object::FIXED)) {
            a->move(center + b_dir*distance/2.);
        }
        if (!(b->flags & Object::FIXED)) {
            b->move(center + a_dir*distance/2.);
        }
    }

    if (diff < 0.) {
        /* diff must be made smaller */
        a->apply_force(a_dir*strength);
        b->apply_force(b_dir*strength);
    } else {
        /* diff must be made bigger */
        if (!(flags & Joint::RUBBERBAND)) {
            a->apply_force(b_dir*strength);
            b->apply_force(a_dir*strength);
        }
    }
}

Circle1DBehavior::Circle1DBehavior(Scene *scene)
    : Circle1DType(Circle1DType::BEHAVIOR)
    , scene(scene)
{
    scene->add_behavior(this);
}

static bool
lines_intersect(Vec2 p1, Vec2 p2, Vec2 p3, Vec2 p4)
{
    // Check if line (p1->p2) intersects with line (p3->p4)
    // http://paulbourke.net/geometry/pointlineplane/

    float nn = (p4.y-p3.y)*(p2.x-p1.x)-(p4.x-p3.x)*(p2.y-p1.y);
    if (nn == 0) {
        return false;
    }

    float za = (p4.x-p3.x)*(p1.y-p3.y)-(p4.y-p3.y)*(p1.x-p3.x);
    float ua = za / nn;

    float zb = (p2.x-p1.x)*(p1.y-p3.y)-(p2.y-p1.y)*(p1.x-p3.x);
    float ub = zb / nn;

    float epsilon = 0.01;
    float min = 0.0 + epsilon;
    float max = 1.0 - epsilon;

    return (min < ua && ua < max && min < ub && ub < max);

}

bool
Scene::checkUntanglement()
{
    if (untangle_joints.size() == 0) {
        // Objective reached
        return true;
    }

    bool result = true;
    std::list<Joint*>::iterator jit;
    for (jit = untangle_joints.begin(); jit != untangle_joints.end(); ++jit) {
        (*jit)->knot_color = RGB(1.0, 1.0, 1.0);
    }

    // check joint tangling
    for (jit = untangle_joints.begin(); jit != untangle_joints.end(); ++jit) {
        Joint *a = *jit;
        std::list<Joint*>::iterator jit2;
        for (jit2 = jit, ++jit2; jit2 != untangle_joints.end(); ++jit2) {
            Joint *b = *jit2;
            if (lines_intersect(a->a->pos, a->b->pos, b->a->pos, b->b->pos)) {
                a->knot_color = b->knot_color = RGB(1.0, 0.0, 0.0);
                result = false;
            }
        }
    }

    return result;
}

void
Scene::background_color_from_content()
{
    if (background_color_set) {
        return;
    }
    std::list<Object*>::iterator it;
    for (it=objects.begin(); it!=objects.end(); ++it) {
        Object *o = *it;
        if (!(o->flags & Object::COLORABLE) && !(o->color.isgray())) {
            background_color = RGB::mix(RGB(0x000000), o->color);
            return;
        }
    }

    background_color = RGB::background();
    background_color_set = true;
}

void
Scene::save(const std::string &filename)
{
    SerializeBuffer sav;

    sav << time;

    sav << background_color << int(background_color_set);

    sav << int(objects.size());

    std::list<Object*>::iterator it;
    for (it=objects.begin(); it!=objects.end(); ++it) {
        Object *o = *it;
        sav << o->pos << o->size << o->flags;
        sav << o->color << o->target_color << o->desired;
        sav << o->last_pos << o->velocity << o->force;
    }

    sav << int(joints.size());

    std::list<Joint*>::iterator jit;
    for (jit=joints.begin(); jit!=joints.end(); ++jit) {
        Joint *j = *jit;

        sav << object_id(j->a) << object_id(j->b) << j->flags << j->distance;
    }

    sav << int(decals.size());

    std::list<HintDecal*>::iterator hit;
    for (hit=decals.begin(); hit!=decals.end(); ++hit) {
        HintDecal *h = *hit;

        sav << int(h->decal) << h->x << h->y;
    }

    sav << int(behaviors.size());

    std::list<Circle1DBehavior*>::iterator bit;
    for (bit=behaviors.begin(); bit!=behaviors.end(); ++bit) {
        Circle1DBehavior *b = *bit;

        Circle1DBehaviorIO *io = Circle1DBehaviorRegistration::lookup(b->name());
        if (io) {
            sav << b->name();
            io->save(this, b, sav);
        } else {
            sav << "";
        }
    }

    sav.write_file(filename);
}

void
Scene::load(const std::string &filename)
{
    SerializeBuffer sav;
    sav.read_file(filename);

    if (!sav.available()) {
        SHADY_DEBUG_PRINTF("Could not load %s\n", filename.c_str());
        return;
    }

    reset();

    sav >> time;

    sav >> background_color;
    background_color_set = sav.read_int32();

    int objects;
    sav >> objects;

    for (int i=0; i<objects; i++) {
        Vec2 pos;
        float size;
        int flags;

        sav >> pos >> size >> flags;
        Object *o = new Object(this, pos.x, pos.y, size, flags);
        sav >> o->color >> o->target_color >> o->desired;
        sav >> o->last_pos >> o->velocity >> o->force;
    }

    int joints;
    sav >> joints;

    for (int i=0; i<joints; i++) {
        int a_id, b_id, flags;
        float distance;

        sav >> a_id >> b_id >> flags >> distance;

        Joint *j = new Joint(this, object_from_id(a_id), object_from_id(b_id), flags);
        j->distance = distance;
    }

    int decals;
    sav >> decals;

    for (int i=0; i<decals; i++) {
        int id, x, y;
        sav >> id >> x >> y;
        new HintDecal(this, Circle1D::Decal(id), x, y);
    }

    int behaviors;
    sav >> behaviors;

    for (int i=0; i<behaviors; i++) {
        std::string name;
        sav >> name;
        Circle1DBehaviorIO *io = Circle1DBehaviorRegistration::lookup(name);
        if (io) {
            io->load(this, sav);
        }
    }
}

int
Scene::object_id(Object *o)
{
    int i = 0;
    for (auto &object: objects) {
        if (o == object) {
            return i;
        }
        i++;
    }

    SHADY_ASSERT(false);
    return -1;
}

int
Scene::joint_id(Joint *j)
{
    int i = 0;
    for (auto &joint: joints) {
        if (j == joint) {
            return i;
        }
        i++;
    }

    SHADY_ASSERT(false);
    return -1;
}

int
Scene::behavior_id(Circle1DBehavior *b)
{
    int i = 0;
    for (auto &behavior: behaviors) {
        if (b == behavior) {
            return i;
        }
        i++;
    }

    SHADY_ASSERT(false);
    return -1;
}

int
Scene::decal_id(HintDecal *d)
{
    int i = 0;
    for (auto &decal: decals) {
        if (d == decal) {
            return i;
        }
        i++;
    }

    SHADY_ASSERT(false);
    return -1;
}

Object *
Scene::object_from_id(int id)
{
    if (id < 0) {
        return NULL;
    }

    auto it = objects.begin();
    while (id--) {
        SHADY_ASSERT(it != objects.end());
        it++;
    }
    return *it;
}

Joint *
Scene::joint_from_id(int id)
{
    if (id < 0) {
        return NULL;
    }

    auto it = joints.begin();
    while (id--) {
        SHADY_ASSERT(it != joints.end());
        it++;
    }
    return *it;
}

Circle1DBehavior *
Scene::behavior_from_id(int id)
{
    if (id < 0) {
        return NULL;
    }

    auto it = behaviors.begin();
    while (id--) {
        SHADY_ASSERT(it != behaviors.end());
        it++;
    }
    return *it;
}

HintDecal *
Scene::decal_from_id(int id)
{
    if (id < 0) {
        return NULL;
    }

    auto it = decals.begin();
    while (id--) {
        SHADY_ASSERT(it != decals.end());
        it++;
    }
    return *it;
}

void
Scene::handle_editing(Circle1DEvent *event)
{
    Vec2 pos(event->x, event->y);
    Vec2 rel(pos - editing_old_mouse_pos);

    if (event->type == Circle1DEvent::KEYDOWN) {
        switch (event->finger) {
            case Circle1DEvent::DELETE:
                if (edit_object) {
                    remove_object(edit_object);
                    edit_object = NULL;
                } else if (edit_joint) {
                    remove_object(edit_joint);
                    edit_joint = NULL;
                }
                break;
            case Circle1DEvent::TOGGLE_COLLIDER:
                if (edit_object) {
                    edit_object->flags ^= Object::COLLIDER;
                    object_flags_changed(edit_object);
                }
                break;
            case Circle1DEvent::TOGGLE_GRAVITY:
                if (edit_object) {
                    edit_object->flags ^= Object::NOGRAVITY;
                    object_flags_changed(edit_object);
                }
                break;
            case Circle1DEvent::TOGGLE_BORDERBOUNCE:
                if (edit_object) {
                    edit_object->flags ^= Object::BORDERBOUNCE;
                    object_flags_changed(edit_object);
                }
                break;
            case Circle1DEvent::TOGGLE_FIXED:
                if (edit_object) {
                    edit_object->flags ^= Object::FIXED;
                    object_flags_changed(edit_object);
                } else if (edit_joint) {
                    edit_joint->flags ^= Joint::FIXED;
                    object_flags_changed(edit_joint);
                }
                break;
            case Circle1DEvent::TOGGLE_SHADOWCAST:
                if (edit_object) {
                    edit_object->flags ^= Object::SHADOWCAST;
                    object_flags_changed(edit_object);
                }
                break;
            case Circle1DEvent::SET_COLOR:
                editing_color_index = event->button;
                break;
            case Circle1DEvent::SET_TARGET_COLOR:
                if (edit_object) {
                    edit_object->color = edit_object->target_color = EDIT_SWATCH[editing_color_index];
                    if (!edit_object->desired.equals(edit_object->target_color)) {
                        edit_object->flags |= Object::COLORABLE;
                    } else {
                        edit_object->flags &= ~Object::COLORABLE;
                    }
                    object_flags_changed(edit_object);
                }
                break;
            case Circle1DEvent::SET_DESIRED_COLOR:
                if (edit_object) {
                    edit_object->desired = EDIT_SWATCH[editing_color_index];
                    if (!edit_object->desired.equals(edit_object->target_color)) {
                        edit_object->flags |= Object::COLORABLE;
                    } else {
                        edit_object->flags &= ~Object::COLORABLE;
                    }
                    object_flags_changed(edit_object);
                }
                break;
            default:
                break;
        }
    } else if (event->type == Circle1DEvent::MOUSEMOTION && event->button == 0) {
        edit_object = object_at(pos);
        edit_joint = joint_at(pos);
    } else if (event->button == 1) {
        switch (event->type) {
            case Circle1DEvent::MOUSEDOWN:
                if (!edit_object) {
                    edit_object = new Object(this, pos.x, pos.y, 10);
                }
                editing_old_mouse_pos = pos;
                break;
            case Circle1DEvent::MOUSEMOTION:
                if (edit_object) {
                    edit_object->pos += rel;
                    editing_old_mouse_pos = pos;
                }
                break;
            case Circle1DEvent::MOUSEUP:
                break;
            default:
                break;
        }
    } else if (event->button == 2) {
        switch (event->type) {
            case Circle1DEvent::MOUSEDOWN:
                if (edit_object) {
                    edit_joint_tmp = new Object(this, pos.x, pos.y, 10);
                    edit_joint = new Joint(this, edit_object, edit_joint_tmp);
                }
                break;
            case Circle1DEvent::MOUSEMOTION:
                if (edit_joint && edit_joint_tmp) {
                    edit_joint_tmp->pos = pos;
                    edit_object = object_at(edit_joint_tmp->pos);
                    edit_joint->b = edit_object;
                }
                break;
            case Circle1DEvent::MOUSEUP:
                if (edit_joint && edit_joint_tmp) {
                    if (edit_joint->b == edit_joint_tmp) {
                        remove_object(edit_joint);
                    } else {
                        edit_joint->reset_distance();
                    }
                    remove_object(edit_joint_tmp);
                    edit_joint_tmp = NULL;
                }
                break;
            default:
                break;
        }
    } else if (event->button == 3) {
        switch (event->type) {
            case Circle1DEvent::MOUSEDOWN:
                if (!edit_object) {
                    edit_object = new Object(this, pos.x, pos.y, 1);
                    editing_old_mouse_pos = pos;
                }
                break;
            case Circle1DEvent::MOUSEMOTION:
                if (edit_object) {
                    edit_object->size = (pos - edit_object->pos).length();
                    editing_old_mouse_pos = pos;
                }
                break;
            case Circle1DEvent::MOUSEUP:
                break;
            default:
                break;
        }
    }

    if (event->is_mouse_event()) {
        editing_label_pos = pos + Vec2(10, 0);
    }

    if (edit_object) {
        editing_label = edit_object->describe();
    } else if (edit_joint) {
        editing_label = edit_joint->describe();
    } else {
        editing_label = "";
    }
}

Object *
Scene::object_at(Vec2 pos)
{
    for (auto &o: objects) {
        if ((pos - o->pos).length() < o->size) {
            return o;
        }
    }

    return NULL;
}

Joint *
Scene::joint_at(Vec2 pos)
{
    for (auto &j: joints) {
        Vec2 j_pos = (j->a->pos + j->b->pos) / 2.0;
        if (((pos - j_pos).length()) < 10.0) {
            return j;
        }
    }

    return NULL;
}

void
Scene::render(Circle1DRenderer *renderer)
{
    std::list<HintDecal*>::iterator dit;
    for (dit = decals.begin(); dit != decals.end(); ++dit) {
        HintDecal *d = *dit;
        renderer->decal(d->decal, d->x, d->y, d->opacity);
    }

    std::list<Object*>::iterator oit;
    if (light_source.enabled) {
        for (oit = objects.begin(); oit != objects.end(); ++oit) {
            Object *o = *oit;
            if (o->flags & Object::SHADOWCAST) {
                renderer->shadow(light_source.pos, o->pos, o->size);
            }
        }
        renderer->flush_shadows();
    }

    std::list<Joint*>::iterator jit;
    for (jit = joints.begin(); jit != joints.end(); ++jit) {
        Joint *j = *jit;
        if (!(j->flags & (Joint::BROKEN | Joint::MOUSE))) {
            renderer->line(j);
        }
    }
    renderer->flush_lines();

    for (oit = objects.begin(); oit != objects.end(); ++oit) {
        Object *o = *oit;
        if (!(o->flags & Object::INVISIBLE)) {
            renderer->circle(o);
        }
    }
    renderer->flush_circles();

    if (editing) {
        renderer->text_render("[EDITING]", 10, 10, FONT_SMALL, 1.0, EDIT_SWATCH[editing_color_index]);

        Vec2 pos = editing_label_pos;
        float w, h;
        renderer->text_measure(editing_label.c_str(), &w, &h, FONT_TINY);

        if (pos.x + w > Constants::WORLD_WIDTH) {
            pos.x = Constants::WORLD_WIDTH - w;
        }
        if (pos.y + h > Constants::WORLD_HEIGHT) {
            pos.y = Constants::WORLD_HEIGHT - h;
        }

        renderer->text_render(editing_label.c_str(), pos.x, pos.y, FONT_TINY);
    }
}

std::map<std::string, Circle1DBehaviorIO *> *
Circle1DBehaviorRegistration::m_ios = 0;
