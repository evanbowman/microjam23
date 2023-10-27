#include "tmg_test_game.h"

#include <string.h>

#include "mj/mj_game_list.h"

#include "bn_regular_bg_items_ppick_background.h"
#include "bn_regular_bg_items_tmg_press_a.h"
#include "bn_regular_bg_items_tmg_you_lose.h"
#include "bn_regular_bg_items_tmg_you_win.h"



namespace
{
constexpr bn::string_view code_credits[] = {"Evan Bowman"};
constexpr bn::string_view graphics_credits[] = {"Evan Bowman"};
} // namespace

MJ_GAME_LIST_ADD(tmg::test_game)
MJ_GAME_LIST_ADD_CODE_CREDITS(code_credits)
MJ_GAME_LIST_ADD_GRAPHICS_CREDITS(graphics_credits)
// MJ_GAME_LIST_ADD_MUSIC_CREDITS(music_credits)
// MJ_GAME_LIST_ADD_SFX_CREDITS(sfx_credits)



namespace tmg
{

static const unsigned char map_circ[7][12]
{
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 2, 0, 0, 1, 0, 1, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};


static const unsigned char map_bar[7][12]
{
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
};




test_game::test_game(int completed_games, const mj::game_data& data)
    : bg_(bn::regular_bg_items::ppick_background.create_bg((256 - 240) / 2,
                                                           (256 - 160) / 2)),
      total_frames_(play_jingle(mj::game_jingle_type::METRONOME_16BEAT,
                                completed_games,
                                data))
{
    switch (data.random.get() % 2) {
    default:
    case 0:
        load_map(map_circ);
        break;

    case 1:
        load_map(map_bar);
        break;
    }

    theif_.emplace();

    theif_->set_position(start_x_, start_y_);
    theif_->move(0, 0);
}



void test_game::fade_in([[maybe_unused]] const mj::game_data& data)
{
}



mj::game_result test_game::play(const mj::game_data& data)
{
    mj::game_result result;
    result.exit = data.pending_frames == 0;

    for (auto it = pumpkins_.begin(); it not_eq pumpkins_.end();) {
        if (has_intersection(*it, *theif_)) {
            auto spr = it->sprite_;
            spr.set_item(bn::sprite_items::ppick_tileset, 2);
            spr.put_below();
            decorations_.push_back(spr);
            it = pumpkins_.erase(it);
            theif_->add_pumpkin();
            // TODO: add effects
        } else {
            ++it;
        }
    }

    theif_->update(not victory_);

    if (pumpkins_.empty()) {
        if (not victory_ and not defeat_) {
            victory_ = true;
            result.remove_title = true;
        } else {
            if (--show_result_frames_ == 0) {
                result.exit = true;
            }
        }
    }

    return result;
}



void test_game::fade_out([[maybe_unused]] const mj::game_data& data)
{
}


} // namespace tmg
