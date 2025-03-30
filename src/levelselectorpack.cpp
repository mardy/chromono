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


#include "levelselectorpack.h"

#include "opengl_renderer.h"

class PressablePack : public Pressable {
    public:
        PressablePack(LevelSelectorPack *pack, Object *o, int id)
            : Pressable()
            , pack(pack)
            , o(o)
            , id(id)
        {
        }

        virtual ~PressablePack()
        {
        }

        virtual bool contains(Vec2 pos)
        {
            return ((o->pos - pos).length() < o->size);
        }

        virtual bool on_pressed()
        {
            return pack->select_pack(id);
        }

    private:
        LevelSelectorPack *pack;
        Object *o;
        int id;
};

LevelSelectorPack::LevelSelectorPack(Game *game, level_pack_select_func select_func,
        void *select_func_user_data)
    : game(game)
    , select_func(select_func)
    , select_func_user_data(select_func_user_data)
    , level_manager(game->get_level_manager())
    , score_manager(game->get_score_manager())
    , scene()
    , title("Chapters")
    , packs_count(0)
    , instantiated_behaviors(NULL)
{
    LevelPack *pack = packs;
    Object *last = NULL;
    int i = 0;
    while (pack->name) {
        SHADY_ASSERT(i < MAX_LEVEL_PACKS);

        Object *current = new Object(&scene, pack->layout.x, pack->layout.y,
                pack->layout.size, Object::FIXED | Object::COLLIDER,
                RGB(0.7, 0.7, 0.7));
        if (last != NULL && pack->layout.connected_to_previous) {
            new Joint(&scene, last, current);
        }

        last = current;
        pack++;

        level_pack_object[i] = current;
        pressable_pack[i] = new PressablePack(this, current, i);
        i++;
    }
    packs_count = i;

    instantiated_behaviors = new int[packs_count];
    for (i=0; i<packs_count; i++) {
        instantiated_behaviors[i] = 0;
    }
}

LevelSelectorPack::~LevelSelectorPack()
{
    delete [] instantiated_behaviors;

    for (int i=0; i<packs_count; i++) {
        delete pressable_pack[i];
    }
}

void
LevelSelectorPack::render(OpenGLRenderer *renderer)
{
    Vec2 offset = game->get_offset();

    float w, h;
    renderer->text_measure(title, &w, &h, FONT_LARGE);
    renderer->text_render(title, (Constants::WORLD_WIDTH - w) / 2.0, 10.0 - offset.y, FONT_LARGE);
    scene.render(renderer);

    for (int i=0; i<packs_count; i++) {
        LevelPack *pack = &(packs[i]);
        RGB color = RGB::mix(RGB(0.0, 0.0, 0.0), pack->layout.color);

        Object *o = level_pack_object[i];

        int levels = (pack->last_level - pack->first_level + 1);
        int completed = 0;
        int locked = 0;
        for (int i=pack->first_level; i<=pack->last_level; i++) {
            LevelInfo *info = level_manager->get(i);
            if (score_manager->get_score(i) != -1) {
                completed++;
            }
            if (score_manager->stars() < info->required_stars && !Constants::UNLOCK_ALL) {
                locked++;
            }
        }
        int available = levels - locked;

        if (available > 0) {
            renderer->text_measure(pack->name, &w, &h, FONT_MEDIUM);
            renderer->text_render(pack->name, o->pos.x - w/2.0, o->pos.y + o->size + 2.0,
                    FONT_MEDIUM, 1.0, RGB::mix(RGB(1.0, 1.0, 1.0), pack->layout.color));
        }

        float w, h;
        char tmp[1024];
        if (available == 0) {
            w = h = o->size * 1.5;
            float x = o->pos.x - w / 2.05;
            float y = o->pos.y - h / 1.9;
            renderer->icon(Icons::LOCK, x, y, w, h, RGB(0.0, 0.0, 0.0), 0.2);
        } else if (completed < levels) {
            sprintf(tmp, "%d/%d", completed, levels);
            renderer->text_measure(tmp, &w, &h, FONT_LARGE);
            renderer->text_render(tmp, o->pos.x - w/2.0, o->pos.y - h/2.0, FONT_LARGE, 1.0, color);
        } else {
            //w = h = 60;
            //float x = o->pos.x - w / 2.0;
            //float y = o->pos.y - h / 2.0;
            //renderer->icon(Icons::STAR, x, y, w, h, RGB(0.0, 0.0, 0.0), 0.2);
            // done
            //sprintf(tmp, ":)");//%d", levels);
        }
    }
}

void
LevelSelectorPack::before_render(OpenGLRenderer *renderer)
{
    bool do_preroll = false;

    for (int i=0; i<packs_count; i++) {
        LevelPack *pack = &(packs[i]);
        Object *o = level_pack_object[i];

        int levels = (pack->last_level - pack->first_level + 1);
        int locked = 0;
        for (int i=pack->first_level; i<=pack->last_level; i++) {
            LevelInfo *info = level_manager->get(i);
            if (score_manager->stars() < info->required_stars && !Constants::UNLOCK_ALL) {
                locked++;
            }
        }

        int available = levels - locked;
        if (available > 0) {
            if (!instantiated_behaviors[i]) {
                o->target_color = o->color = pack->layout.color;
                // Instantiate new behavior for that pack
                if (pack->behavior_func) {
                    pack->behavior_func(o);
                    do_preroll = true;
                }
                instantiated_behaviors[i] = 1;
            }
        }
    }

    if (do_preroll) {
        scene.simulate(60); // preroll
    }
}

void
LevelSelectorPack::handle(Circle1DEvent *event)
{
    if (event->type == Circle1DEvent::TICK) {
        scene.handle(event);
    }

    for (int i=0; i<packs_count; i++) {
        pressable_pack[i]->handle(event);
    }
}

LevelPack *
LevelSelectorPack::find_pack(int level)
{
    for (int i=0; i<packs_count; i++) {
        LevelPack *pack = &(packs[i]);
        if ((pack->first_level <= level) &&
            (pack->last_level >= level)) {
            return pack;
        }
    }

    return NULL;
}


bool
LevelSelectorPack::select_pack(int pack_id)
{
    LevelPack *pack = &(packs[pack_id]);

    int minimum_required = -1;
    for (int i=pack->first_level; i<=pack->last_level; i++) {
        LevelInfo *info = level_manager->get(i);
        if (minimum_required == -1 || minimum_required > info->required_stars) {
            minimum_required = info->required_stars;
        }
    }

    int missing_stars = (minimum_required - score_manager->stars());

    if (missing_stars > 0 && !Constants::UNLOCK_ALL) {
        char tmp[1024];
        sprintf(tmp, (missing_stars == 1) ? "Need %d more star to unlock" :
                "Need %d more stars to unlock", missing_stars);
        game->show_message(tmp);
        Platform::play(Sound::LEVEL_LOCKED_MESSAGE_BOX);
        return true;
    } else {
        select_func(pack, select_func_user_data);
        return false;
    }
}
