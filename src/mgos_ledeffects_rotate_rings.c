#include "mgos.h"
#include "led_master.h"

static int direction = 0;
static int ring_size = 2;

static void mgos_intern_rotate_rings_init( mgos_rgbleds* leds) {
    leds->timeout = mgos_sys_config_get_ledeffects_rotate_rings_timeout();
    leds->dim_all = mgos_sys_config_get_ledeffects_rotate_rings_dim_all();
    direction = mgos_sys_config_get_ledeffects_rotate_rings_direction();
    leds->pix_pos = direction > 0 ? 0 : leds->pic_width - 1 - leds->panel_width;
    leds->fit_horz = mgos_sys_config_get_ledeffects_rotate_rings_fit_horz();
    leds->fit_vert = mgos_sys_config_get_ledeffects_rotate_rings_fit_vert();
    leds->color_file = mgos_sys_config_get_ledeffects_rotate_rings_color_file();
    ring_size = mgos_sys_config_get_ledeffects_rotate_rings_ring_size();    
    mgos_universal_led_get_bitmap(leds);
    mgos_universal_led_scale_bitmap(leds);
    
    mgos_universal_led_log_data(leds, LL_DEBUG, "[DATA] Rotate Columns LEDs");
}

static void mgos_intern_rotate_rings_loop(mgos_rgbleds* leds)
{
    LOG(LL_VERBOSE_DEBUG, ("Rotate columns: offset = %d", leds->pix_pos));
    mgos_universal_led_set_rings_from_buffer(leds, leds->pix_pos, 0, true, mgos_sys_config_get_ledeffects_rotate_rings_toggle_odd(), (uint8_t) ring_size);
    mgos_universal_led_show(leds);
    
    leds->pix_pos += direction;
    leds->pix_pos = leds->pix_pos >= leds->pic_width ? 0 : leds->pix_pos < 0 ? leds->pic_width - 1 : leds->pix_pos;
}

void mgos_ledeffects_rotate_rings(void* param, mgos_rgbleds_action action)
{
    static bool do_time = false;
    static uint32_t max_time = 0;
    uint32_t time = (mgos_uptime_micros() / 1000);
    mgos_rgbleds* leds = (mgos_rgbleds*)param;

    switch (action) {
    case MGOS_RGBLEDS_ACT_INIT:
        LOG(LL_INFO, ("mgos_ledeffects_rotate_rings: called (init)"));
        mgos_intern_rotate_rings_init(leds);
        break;
    case MGOS_RGBLEDS_ACT_EXIT:
        LOG(LL_INFO, ("mgos_ledeffects_rotate_rings: called (exit)"));
        break;
    case MGOS_RGBLEDS_ACT_LOOP:
        mgos_intern_rotate_rings_loop(leds);
        if (do_time) {
            time = (mgos_uptime_micros() /1000) - time;
            max_time = (time > max_time) ? time : max_time;
            LOG(LL_VERBOSE_DEBUG, ("Rotate rings loop duration: %d milliseconds, max: %d ...", time / 1000, max_time / 1000));
        }
        break;
    }
}

bool mgos_ledeffects_rotate_rings_init(void) {
  LOG(LL_INFO, ("mgos_rotate_rings_init ..."));
  ledmaster_add_effect("ANIM_ROTATE_RINGS", mgos_ledeffects_rotate_rings);
  return true;
}
