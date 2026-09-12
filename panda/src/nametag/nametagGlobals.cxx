// Filename: nametagGlobals.cxx
// Created by:  drose (19Jul01)
// $Id$
////////////////////////////////////////////////////////////////////

#include "nametagGlobals.h"
#include "configVariableBool.h"

// This is the distance toward the camera to slide the Nametag3d, so
// that it doesn't intersect any part of the avatar.
const float NametagGlobals::billboard_offset = 3.0f;

// These numbers control how the Nametag3d is scaled based on its
// distance from the camera.  At the far_distance, the speech balloon
// will be scaled by far_scale.  At other distances, the dropoff is
// based on the exponent--an exponent of 1.0 will cause the balloon to
// keep a fixed size on the screen at all distances, while an exponent
// of 0.0 will cause it to keep a fixed size in the world, scaling on
// the screen in the natural way.  Exponents between 0 and 1 will lie
// somewhere between these two extremes.
const float NametagGlobals::far_distance = 50.0f;
const float NametagGlobals::far_scale = 0.56f;
const float NametagGlobals::scale_exponent = 0.5f;

// These numbers specify how much the little red arrow should be
// scaled, and how far it should be positioned below the name, within
// the Nametag2d's popup.
const float NametagGlobals::arrow_scale = 0.5f;
const float NametagGlobals::arrow_offset = 1.0f;

// This defines the amount of padding applied to the card behind the
// nametags, in each direction (left, right, bottom, top).
const LVecBase4f NametagGlobals::card_pad = LVecBase4f(0.1f, 0.1f, 0.1f, 0.0f);

// This defines the width of the "nominal" avatar, for the purposes of
// defining the MouseWatcherRegion for picking the avatar from the
// screen.  The region will extend from the origin of the Nametag3d
// down to 0.0, at approximately the indicated width.
const float NametagGlobals::nominal_avatar_width = 5.0f;

// These define the widths at which text is wordwrapped for the name
// as well as for the chat bubbles (in both nametag2d and nametag3d).
// This indirectly affects the scale of the text, because the Nametag
// code will scale the text so its wordwrap width exactly fills its
// available space.
const float NametagGlobals::name_wordwrap = 7.5f;  // Wide enough for "Telemarketer"
const float NametagGlobals::building_name_wordwrap = 8.5f;  // Wide enough for "Sidesplitter's"
const float NametagGlobals::house_name_wordwrap = 10.0f;  // Wide enough for "Mizzenscooter's"
// 2d chat -- a message in a margin cell -- is wrapped at this width.  The
// font size is set separately: Nametag2d and WhisperPopup scale the balloon
// by 2 * cell_width / name_wordwrap, so a message always reads at exactly the
// size of the name on the tag above it, whatever this is.  While the scale
// was coupled to the wrap (2 * cell_width / (wordwrap + 1)), the retail 8.0
// made a message 17% smaller than that name, and 6.5 (name_wordwrap - 1) was
// the value that matched it -- at the cost of wrapping a line early, so long
// messages stacked tall instead of running wide.
//
// 7.5 is name_wordwrap, and it is the width at which rendered text stops
// overrunning its cell at all.  A line only outgrows the cell past ~8.2 em, so
// a 7.5-wrap line -- 7.5 plus whatever the last word overshoots by -- stays
// inside it: measured at 1080p landscape against a 205 px cell, text overruns
// by 0.0 px at every sample, against up to 5.8 px at 8.0.  Only the decorative
// balloon border reaches past the cell box, into the gap between cells (or, on
// a side cell, toward the window edge, which ToonBase's MarginCellEdgeInset
// holds it off).
//
// Retail's 8.0 was chosen while the wrap still set the font size, so it
// carries no behaviour worth keeping now that the two are decoupled -- and its
// comment claimed "wide enough for Telemarketer:", which measures 5.71 em.
//
// This is also the wrap of a WhisperPopup, which is the reason the constant
// matters: unlike Nametag2d, WhisperPopup has no Python setter to override it.
const float NametagGlobals::chat_2d_wordwrap = 7.5f;  // = name_wordwrap
const float NametagGlobals::chat_3d_wordwrap = 10.0f;

// This is the width of the chatBalloon geometry models, as they are
// scaled in the model files, for the internal width (i.e. the width
// of the text region), and the external width (the entire thing,
// including the decorations).
const float NametagGlobals::balloon_internal_width = 9.0f;
const float NametagGlobals::balloon_external_width = 10.0f;

// This is the smallest scale factor that may be applied to the chat
// balloon's horizontal axis to make it fit short text.  If the
// balloon must be scaled smaller than this, the text will be centered
// within the balloon instead.
const float NametagGlobals::balloon_min_hscale = 0.25f;

// This is the bottom-left corner of the text within the balloon.
const LPoint3f NametagGlobals::balloon_text_origin = LPoint3f(1.0f, 0.0f, 2.0f);

// These define the spacing of the grid of cells for MarginPopups to
// be placed in.  This is mainly for the benefit of
// MarginManager::add_grid_cell(), which is a more user-friendly way
// of defining cells on the screen than add_cell().
const float NametagGlobals::grid_count_horizontal = 6.0f;
const float NametagGlobals::grid_count_vertical = 6.0f;
const float NametagGlobals::grid_spacing_horizontal = 0.02f;
const float NametagGlobals::grid_spacing_vertical = 0.02f;

// This defines the length of time in seconds for which an empty
// MarginCell (i.e. for a Nametag2d) will remember its previous
// occupant, so that the occupant will be displayed in the same cell
// again should it return.
const double NametagGlobals::cell_memory_time = 30.0;

// These define the length of time, in seconds, that a Whisper message
// will receive priority over all other Nametag2d's competing for
// space onscreen, as well as the total time it will stay onscreen
// when there is plenty of space.
const double NametagGlobals::whisper_priority_time = 5.0;
const double NametagGlobals::whisper_total_time = 15.0;

// How long should we delay before displaying the "click to advance"
// button on a dialog balloon that asks for one?  The reason we have
// any delay at all is to protect against accidental page advances
// from double clicks or something like that.
const double NametagGlobals::button_delay_time = 0.2;

// This is the maximum distance at which a building nametag is still
// visible.  Buildings farther away than this are not displayed in the
// nametag space.  It gates building-coloured tags only -- CC_toon_building,
// CC_suit_building and CC_house_building, in Nametag2d::is_visible -- and is
// the only distance cutoff in the nametag system at all: a toon or NPC tag
// never disappears for being far away.
//
// Retail's 40 put a shop's tag out of reach until you were most of the way to
// its door, which reads as tags popping in rather than a street you can
// navigate by.  Measured off the landmark_building positions in the street
// DNA, a street spans 390 to 650 units, so 40 covered barely a tenth of one.
// 400 covers a whole street from either end on all but the longest.  The
// constant is in world units, compared as 400^2 against the squared distance
// Nametag2d keeps.
//
// This is deliberately wide enough that the cutoff stops being the limiter.
// Two other things still are, and they are why a wide radius is safe:
//
//   - A margin cell is only taken when the tag's Nametag3d is NOT fully
//     onscreen (the NF_onscreen test at the top of Nametag2d::is_visible).
//     Buildings you are looking at cost nothing; only ones beside or behind
//     you compete.
//   - Cells go to the highest Nametag2d::get_score(), which is
//     1000 - distance2 with no bonus for being a toon, so what shows is the
//     nearest handful either way.  Whispers are exempt: an un-rendered
//     WhisperPopup scores 2000, above any nametag.
//
// The cost is that once demand exceeds the cells MarginManager has, a nearer
// building outranks a farther toon, so mid-distance toon tags are what get
// pushed out.  scripts/agent-scripts/gui-fit/measure_building_nametag_distance.py
// re-derives the eligible-tag counts per zone for any radius.
const float NametagGlobals::building_nametag_distance = 400.0f;

// The color of QuickTalker (SpeedChat) balloons unless we specify
// otherwise.
const LColorf NametagGlobals::default_qt_color = LColorf(0.8f, 0.8f, 1.0f, 1.0f);

// The color of QuickTalker (SpeedChat) balloons unless we specify
// otherwise.
const LColorf NametagGlobals::default_balloon_modulation_color = LColorf(1.0f, 1.0f, 1.0f, 1.0f);

// This sequence counter is incremented whenever some global property
// of Nametag2d stuff (or MarginPopups in general) is changed, forcing
// all MarginPopups to regenerate themselves.
UpdateSeq NametagGlobals::margin_prop_seq;


NodePath NametagGlobals::_camera;
NodePath NametagGlobals::_toon;
NodePath NametagGlobals::_arrow_model;
NodePath NametagGlobals::_page_button[max_button_states];
NodePath NametagGlobals::_quit_button[max_button_states];
NodePath NametagGlobals::_nametag_card;
LVecBase4f NametagGlobals::_nametag_card_frame;

PT(AudioSound) NametagGlobals::_rollover_sound;
PT(AudioSound) NametagGlobals::_click_sound;

PT(MouseWatcher) NametagGlobals::_mouse_watcher;

PT(TextNode) NametagGlobals::_text_node;

PT(ChatBalloon) NametagGlobals::_speech_balloon_2d;
PT(ChatBalloon) NametagGlobals::_thought_balloon_2d;
PT(ChatBalloon) NametagGlobals::_speech_balloon_3d;
PT(ChatBalloon) NametagGlobals::_thought_balloon_3d;

bool NametagGlobals::_master_nametags_active = true;
bool NametagGlobals::_master_nametags_visible =
  ConfigVariableBool("nametags-visible", true,
                     PRC_DESC("This is the initial value of "
                              "NametagGlobals::set_master_nametags_visible()."));

bool NametagGlobals::_master_arrows_on = true;
bool NametagGlobals::_onscreen_chat_forced = false;

float NametagGlobals::_max_2d_alpha = 0.6f;
float NametagGlobals::_min_2d_alpha = 0.0f;

float NametagGlobals::_global_nametag_scale = 1.0f;

////////////////////////////////////////////////////////////////////
//     Function: NametagGlobals::Colors::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
NametagGlobals::Colors::
Colors(const LColorf &name_fg, const LColorf &name_bg,
       const LColorf &chat_fg, const LColorf &chat_bg) :
  _name_fg(name_fg),
  _name_bg(name_bg),
  _chat_fg(chat_fg),
  _chat_bg(chat_bg)
{
  balloon_modulation_color = default_balloon_modulation_color;
}

static const int num_button_states = 4;
static const int num_color_codes = 9;
static const int num_whisper_types = 6;

// The following defines the table of colors for the Nametags in
// various states.

// These are the colors for the little arrows only.  Mostly they're
// this orangish-red color.
static LColorf arrow_colors[num_color_codes] = {
  LColorf(1.0f, 0.4f, 0.2f, 1.0f),   // CC_normal
  LColorf(1.0f, 0.4f, 0.2f, 1.0f),   // CC_no_chat
  LColorf(1.0f, 0.4f, 0.2f, 1.0f),   // CC_non_player
  LColorf(1.0f, 0.4f, 0.2f, 1.0f),   // CC_suit
  LColorf(0.3f, 0.6f, 1.0f, 1.0f),   // CC_toon_building
  LColorf(0.55f, 0.55f, 0.55f, 1.0f),// CC_suit_building
  LColorf(0.3f, 0.6f, 1.0f, 1.0f),   // CC_house_building
  LColorf(0.3f, 0.7f, 0.3f, 1.0f),   // CC_speed_chat
  LColorf(0.3f, 0.3f, 0.7f, 1.0f),   // CC_free_chat
};

// Normal avatar: green (TTR).  Note this code is transient -- every toon
// lands on CC_speed_chat or CC_free_chat once considerUnderstandable runs.
static NametagGlobals::Colors normal_colors[num_button_states] = {
  // Normal avatar, not selected.
  NametagGlobals::Colors(LColorf(0.0f, 0.46f, 0.19f, 1.0f),  // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),    // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),    // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),   // chat bg

  // Normal avatar, clicked.
  NametagGlobals::Colors(LColorf(0.5f, 0.8f, 0.6f, 1.0f),   // name fg
                         LColorf(0.2f, 0.2f, 0.2f, 0.6f),   // name bg
                         LColorf(1.0f, 0.5f, 0.5f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Normal avatar, rollover.
  NametagGlobals::Colors(LColorf(0.0f, 0.6f, 0.2f, 1.0f),   // name fg
                         LColorf(1.0f, 1.0f, 1.0f, 0.8f),   // name bg
                         LColorf(0.0f, 0.6f, 0.6f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Normal avatar, inactive.
  NametagGlobals::Colors(LColorf(0.0f, 0.35f, 0.14f, 1.0f),  // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),    // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),    // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),   // chat bg
};

// Quicktalker-only avatar: orange (TTR).
static NametagGlobals::Colors no_chat_colors[num_button_states] = {
  // Quicktalker-only avatar, not selected.
  NametagGlobals::Colors(LColorf(0.85f, 0.38f, 0.0f, 1.0f),  // name fg
                         LColorf(1.0f, 1.0f, 1.0f, 0.5f),    // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),    // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),   // chat bg

  // Quicktalker-only avatar, clicked.
  NametagGlobals::Colors(LColorf(0.93f, 0.69f, 0.5f, 1.0f),  // name fg
                         LColorf(0.2f, 0.2f, 0.2f, 0.6f),    // name bg
                         LColorf(1.0f, 0.5f, 0.5f, 1.0f),    // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),   // chat bg

  // Quicktalker-only avatar, rollover.
  NametagGlobals::Colors(LColorf(0.99f, 0.44f, 0.0f, 1.0f),  // name fg
                         LColorf(1.0f, 1.0f, 1.0f, 0.8f),    // name bg
                         LColorf(0.0f, 0.6f, 0.6f, 1.0f),    // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),   // chat bg

  // Quicktalker-only avatar, inactive.
  NametagGlobals::Colors(LColorf(0.77f, 0.26f, 0.0f, 1.0f),  // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),    // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),    // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),   // chat bg
};

// Non-player avatar -- NPC toons and doodles: orange (TTR).  Orange and
// bluish green, rather than pure red and green, so the two are still
// distinguishable to a colorblind player.
static NametagGlobals::Colors non_player_colors[num_button_states] = {
  // Non-player avatar, not selected.
  NametagGlobals::Colors(LColorf(0.85f, 0.38f, 0.0f, 1.0f),  // name fg
                         LColorf(1.0f, 1.0f, 1.0f, 0.5f),    // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),    // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),   // chat bg

  // Non-player avatar, clicked.
  NametagGlobals::Colors(LColorf(0.93f, 0.69f, 0.5f, 1.0f),  // name fg
                         LColorf(0.2f, 0.2f, 0.2f, 0.6f),    // name bg
                         LColorf(1.0f, 0.5f, 0.5f, 1.0f),    // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),   // chat bg

  // Non-player avatar, rollover.
  NametagGlobals::Colors(LColorf(0.99f, 0.44f, 0.0f, 1.0f),  // name fg
                         LColorf(1.0f, 1.0f, 1.0f, 0.8f),    // name bg
                         LColorf(0.0f, 0.6f, 0.6f, 1.0f),    // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),   // chat bg

  // Non-player avatar, inactive.
  NametagGlobals::Colors(LColorf(0.77f, 0.26f, 0.0f, 1.0f),  // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),    // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),    // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),   // chat bg
};

// A suit, black.
static NametagGlobals::Colors suit_colors[num_button_states] = {
  // Suit, not selected.
  NametagGlobals::Colors(LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Suit, clicked.
  NametagGlobals::Colors(LColorf(1.0f, 1.0f, 1.0f, 1.0f),   // name fg
                         LColorf(0.2f, 0.2f, 0.2f, 0.6f),   // name bg
                         LColorf(0.5f, 1.0f, 0.5f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Suit, rollover.
  NametagGlobals::Colors(LColorf(0.5f, 0.5f, 0.5f, 1.0f),   // name fg
                         LColorf(1.0f, 1.0f, 1.0f, 0.8f),   // name bg
                         LColorf(0.6f, 0.0f, 0.6f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Suit, inactive.
  NametagGlobals::Colors(LColorf(0.2f, 0.2f, 0.2f, 1.0f),   // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg
};

// A toon building, bright blue for now.
static NametagGlobals::Colors toon_building_colors[num_button_states] = {
  // Toon building, not selected.
  NametagGlobals::Colors(LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Toon building, clicked.
  NametagGlobals::Colors(LColorf(1.0f, 1.0f, 1.0f, 1.0f),   // name fg
                         LColorf(0.2f, 0.2f, 0.2f, 0.6f),   // name bg
                         LColorf(0.5f, 1.0f, 0.5f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Toon building, rollover.
  NametagGlobals::Colors(LColorf(0.5f, 0.5f, 0.5f, 1.0f),   // name fg
                         LColorf(1.0f, 1.0f, 1.0f, 0.8f),   // name bg
                         LColorf(0.6f, 0.0f, 0.6f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Toon building, inactive.
  NametagGlobals::Colors(LColorf(0.3f, 0.6f, 1.0f, 1.0f),   // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg
};

// A suit building, gray.
static NametagGlobals::Colors suit_building_colors[num_button_states] = {
  // Suit building, not selected.
  NametagGlobals::Colors(LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Suit building, clicked.
  NametagGlobals::Colors(LColorf(1.0f, 1.0f, 1.0f, 1.0f),   // name fg
                         LColorf(0.2f, 0.2f, 0.2f, 0.6f),   // name bg
                         LColorf(0.5f, 1.0f, 0.5f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Suit building, rollover.
  NametagGlobals::Colors(LColorf(0.5f, 0.5f, 0.5f, 1.0f),   // name fg
                         LColorf(1.0f, 1.0f, 1.0f, 0.8f),   // name bg
                         LColorf(0.6f, 0.0f, 0.6f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Suit building, inactive.
  NametagGlobals::Colors(LColorf(0.55f, 0.55f, 0.55f, 1.0f),// name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg
};

// Speed Chat: green (TTR).
static NametagGlobals::Colors speed_chat_colors[num_button_states] = {
  // Speed Chat, not selected.
  NametagGlobals::Colors(LColorf(0.0f, 0.46f, 0.19f, 1.0f),  // name fg
                         LColorf(1.0f, 1.0f, 1.0f, 0.5f),    // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),    // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),   // chat bg

  // Speed Chat, clicked.
  NametagGlobals::Colors(LColorf(0.5f, 0.8f, 0.6f, 1.0f),   // name fg
                         LColorf(0.2f, 0.2f, 0.2f, 0.6f),   // name bg
                         LColorf(0.5f, 1.0f, 0.5f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Speed Chat, rollover.
  NametagGlobals::Colors(LColorf(0.0f, 0.6f, 0.2f, 1.0f),   // name fg
                         LColorf(1.0f, 1.0f, 1.0f, 0.8f),   // name bg
                         LColorf(0.6f, 0.0f, 0.6f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Speed Chat, inactive.
  NametagGlobals::Colors(LColorf(0.0f, 0.35f, 0.14f, 1.0f),  // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),    // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),    // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),   // chat bg
};

// Free Chat: blue.
static NametagGlobals::Colors free_chat_colors[num_button_states] = {
  // Free Chat, not selected.
  NametagGlobals::Colors(LColorf(0.3f, 0.3f, 0.7f, 1.0f),   // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Free Chat, clicked.
  NametagGlobals::Colors(LColorf(0.2f, 0.2f, 0.5f, 1.0f),   // name fg
                         LColorf(0.2f, 0.2f, 0.2f, 0.6f),   // name bg
                         LColorf(1.0f, 0.5f, 0.5f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Free Chat, rollover.
  NametagGlobals::Colors(LColorf(0.5f, 0.5f, 1.0f, 1.0f),   // name fg
                         LColorf(1.0f, 1.0f, 1.0f, 0.8f),   // name bg
                         LColorf(0.0f, 0.6f, 0.6f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Free Chat, inactive.
  NametagGlobals::Colors(LColorf(0.3f, 0.3f, 0.7f, 1.0f),   // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg
};

static NametagGlobals::Colors *color_table[num_color_codes] = {
  normal_colors,
  no_chat_colors,
  non_player_colors,
  suit_colors,
  toon_building_colors,
  suit_building_colors,
  toon_building_colors,
  speed_chat_colors,
  free_chat_colors,
};


// Whisper messages.
static NametagGlobals::Colors whisper_colors[num_button_states] = {
  // Whisper, not selected.
  NametagGlobals::Colors(LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(0.3f, 0.6f, 0.8f, 0.6f)),  // chat bg

  // Whisper, clicked.
  NametagGlobals::Colors(LColorf(0.5f, 0.5f, 0.5f, 1.0f),   // name fg
                         LColorf(0.2f, 0.2f, 0.2f, 0.6f),   // name bg
                         LColorf(1.0f, 0.5f, 0.5f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Whisper, rollover.
  NametagGlobals::Colors(LColorf(0.5f, 0.5f, 0.5f, 1.0f),   // name fg
                         LColorf(1.0f, 1.0f, 1.0f, 0.8f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(0.4f, 0.8f, 1.0f, 1.0f)),  // chat bg

  // Whisper, inactive.
  NametagGlobals::Colors(LColorf(0.3f, 0.3f, 0.3f, 1.0f),   // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(0.3f, 0.6f, 0.8f, 0.6f)),  // chat bg
};

// Emote whisper messages.
static NametagGlobals::Colors emote_whisper_colors[num_button_states] = {
  // Emote whisper, not selected.
  NametagGlobals::Colors(LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(0.3f, 0.8f, 0.3f, 0.6f)),  // chat bg

  // Emote whisper, clicked.
  NametagGlobals::Colors(LColorf(0.5f, 0.5f, 0.5f, 1.0f),   // name fg
                         LColorf(0.2f, 0.2f, 0.2f, 0.6f),   // name bg
                         LColorf(1.0f, 0.5f, 0.5f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Emote whisper, rollover.
  NametagGlobals::Colors(LColorf(0.5f, 0.5f, 0.5f, 1.0f),   // name fg
                         LColorf(1.0f, 1.0f, 1.0f, 0.8f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(0.4f, 1.0f, 0.4f, 1.0f)),  // chat bg

  // Emote whisper, inactive.
  NametagGlobals::Colors(LColorf(0.3f, 0.3f, 0.3f, 1.0f),   // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(0.3f, 0.8f, 0.3f, 0.6f)),  // chat bg
};

// System messages.
static NametagGlobals::Colors system_colors[num_button_states] = {
  // System, not selected.
  NametagGlobals::Colors(LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(0.8f, 0.3f, 0.6f, 0.6f)),  // chat bg

  // System, clicked.
  NametagGlobals::Colors(LColorf(0.5f, 0.5f, 0.5f, 1.0f),   // name fg
                         LColorf(0.2f, 0.2f, 0.2f, 0.6f),   // name bg
                         LColorf(1.0f, 0.5f, 0.5f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // System, rollover.
  NametagGlobals::Colors(LColorf(0.5f, 0.5f, 0.5f, 1.0f),   // name fg
                         LColorf(1.0f, 1.0f, 1.0f, 0.8f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(0.8f, 0.4f, 1.0f, 1.0f)),  // chat bg

  // System, inactive.
  NametagGlobals::Colors(LColorf(0.3f, 0.3f, 0.3f, 1.0f),   // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(0.8f, 0.3f, 0.6f, 0.6f)),  // chat bg
};

// Toontown Boarding Group Whisper Messages.
static NametagGlobals::Colors toontown_boarding_group_colors[num_button_states] = {
  // Boarding Group whisper, not selected.
  NametagGlobals::Colors(LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(0.97f, 0.43f, 0.1f, 0.6f)),  // chat bg

  // Boarding Group whisper, clicked.
  NametagGlobals::Colors(LColorf(0.5f, 0.5f, 0.5f, 1.0f),   // name fg
                         LColorf(0.2f, 0.2f, 0.2f, 0.6f),   // name bg
                         LColorf(1.0f, 0.5f, 0.5f, 1.0f),   // chat fg
                         LColorf(1.0f, 1.0f, 1.0f, 1.0f)),  // chat bg

  // Boarding Group whisper, rollover.
  NametagGlobals::Colors(LColorf(0.5f, 0.5f, 0.5f, 1.0f),   // name fg
                         LColorf(1.0f, 1.0f, 1.0f, 0.8f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(0.98f, 0.6f, 0.38f, 0.6f)),  // chat bg

  // Boarding Group whisper, inactive.
  NametagGlobals::Colors(LColorf(0.3f, 0.3f, 0.3f, 1.0f),   // name fg
                         LColorf(0.8f, 0.8f, 0.8f, 0.5f),   // name bg
                         LColorf(0.0f, 0.0f, 0.0f, 1.0f),   // chat fg
                         LColorf(0.97f, 0.43f, 0.1f, 0.6f)),  // chat bg
};

static NametagGlobals::Colors *whisper_color_table[num_whisper_types] = {
  whisper_colors,      // WT_normal
  whisper_colors,      // WT_quick_talker
  system_colors,       // WT_system
  system_colors,       // WT_battle_SOS
  emote_whisper_colors, // WT_emote
  toontown_boarding_group_colors //WT_toontown_boarding_group
};

LColorf NametagGlobals::balloon_modulation_color;

////////////////////////////////////////////////////////////////////
//     Function: NametagGlobals::get_text_node
//       Access: Public, Static
//  Description: Returns a pointer to the TextNode object used for all
//               Nametags.
////////////////////////////////////////////////////////////////////
TextNode *NametagGlobals::
get_text_node() {
  if (_text_node == (TextNode *)NULL) {
    _text_node = new TextNode("nametag");
  }
  return _text_node;
}

////////////////////////////////////////////////////////////////////
//     Function: NametagGlobals::get_arrow_color
//       Access: Public, Static
//  Description: Returns the color to draw the little arrow.
////////////////////////////////////////////////////////////////////
const LColorf &NametagGlobals::
get_arrow_color(NametagGroup::ColorCode color_code) {
  nassertr((int)color_code >= 0 && (int)color_code < num_color_codes,
           arrow_colors[0]);
  return arrow_colors[color_code];
}

////////////////////////////////////////////////////////////////////
//     Function: NametagGlobals::get_colors
//       Access: Public, Static
//  Description: Returns the colors in which a Nametag should be
//               rendered, given its color code and button state.
////////////////////////////////////////////////////////////////////
const NametagGlobals::Colors &NametagGlobals::
get_colors(NametagGroup::ColorCode color_code, PGButton::State state) {
  nassertr((int)color_code >= 0 && (int)color_code < num_color_codes,
           color_table[0][0]);
  nassertr((int)state >= 0 && (int)state < num_button_states,
           color_table[0][0]);
  return color_table[color_code][state];
}

////////////////////////////////////////////////////////////////////
//     Function: NametagGlobals::get_whisper_colors
//       Access: Public, Static
//  Description: Returns the colors in which a Whisper should be
//               rendered.
////////////////////////////////////////////////////////////////////
const NametagGlobals::Colors &NametagGlobals::
get_whisper_colors(WhisperPopup::WhisperType type, PGButton::State state) {
  nassertr((int)type >= 0 && (int)type < num_whisper_types,
           whisper_color_table[0][0]);
  nassertr((int)state >= 0 && (int)state < num_button_states,
           whisper_color_table[0][0]);
  return whisper_color_table[type][state];
}
