#include <furi.h>
#include <furi_hal.h>
#include "morse.h"

#define CW_FREQ 432225000

typedef struct {
    bool running;
    uint16_t wpm;
} BeaconState;

/* Momentum 012 HAL scaffold:
   Replace carrier_on/off internals if your branch symbols differ slightly. */

static void carrier_on() {
    if(!furi_hal_subghz_is_tx_allowed(CW_FREQ)) return; // Sicherheitscheck

    furi_hal_subghz_idle();
    furi_hal_subghz_load_preset(FuriHalSubGhzPresetIdRfPotRaw);
    furi_hal_subghz_set_frequency_and_path(CW_FREQ);
    furi_hal_subghz_start_packet_tx(); // Manche Firmware-Versionen bevorzugen dies für Träger
}

static void carrier_off() {
    furi_hal_subghz_idle();
}

static void tx_cb(bool on, uint32_t dur_ms) {
    if(on) carrier_on(); else carrier_off();
    furi_delay_ms(dur_ms);
}

static int32_t beacon_worker(void* ctx) {
    BeaconState* st = ctx;
    MorseConfig cfg = {
        .text = "DM1FL/B DM1FL/B LOC JO30MS",
        .wpm = st->wpm
    };

    while(st->running) {
        morse_encode_and_send(&cfg, tx_cb);
        furi_delay_ms(1500);
    }
    carrier_off();
    return 0;
}

int32_t cw_beacon_app(void* p) {
    UNUSED(p);
    BeaconState state = {.running=true, .wpm=15};

    FuriThread* th = furi_thread_alloc_ex("cw_beacon", 4096, beacon_worker, &state);
    furi_thread_start(th);

    while(state.running) {
        if(furi_hal_gpio_read(&gpio_button_back)==false) {
            state.running=false;
        }
        furi_delay_ms(100);
    }

    furi_thread_join(th);
    furi_thread_free(th);
    return 0;
}
