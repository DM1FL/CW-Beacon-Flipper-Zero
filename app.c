#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_region.h>
#include <lib/subghz/devices/cc1101_configs.h>

/* --- KONFIGURATION --- */
#define TARGET_FREQ 432225000      // Deine gewünschte Ziel-Frequenz
#define CORRECTION 50000           // Hardware-Korrektur (50kHz), da der Flipper sonst daneben liegt
#define CW_FREQ (TARGET_FREQ - CORRECTION)

#define DOT_MS 150                 // Länge eines Punktes (Dit) in Millisekunden
#define DASH_MS (DOT_MS * 3)       // Länge eines Strichs (Dah)
#define CARRIER_TOTAL_MS 30000     // Gesamtdauer des Dauerträgers (30 Sek.)
#define RECALIBRATE_MS 2000        // Intervall für PLL-Refresh gegen Frequenz-Drift

/* Region-Unlock: Erlaubt das Senden außerhalb der Standard-Frequenzen */
static FuriHalRegion unlockedRegion = {
    .country_code = "FTW",
    .bands_count = 1,
    .bands = {{.start = 200000000, .end = 1000000000, .power_limit = 20, .duty_cycle = 100}},
};

/* Prüft, ob die Zurück-Taste gedrückt wurde, um das Programm zu beenden */
static bool should_exit() {
    return !furi_hal_gpio_read(&gpio_button_back);
}

/* Schaltet den Sender ein oder aus und erzwingt maximale Leistung */
static void set_cw(bool on) {
    if(on) {
        // Antennen-Pfad für 433 MHz physisch schalten
        furi_hal_subghz_set_path(FuriHalSubGhzPath433);
        // Sendevorgang auf Chip-Ebene starten
        furi_hal_subghz_tx();
        // PA-Table (Power Amplifier) komplett mit Maximalwerten (0xC0) fluten
        uint8_t pwr[] = {0x3E, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0};
        furi_hal_subghz_write_packet(pwr, 9);
        // Rote LED signalisiert: SENDER AKTIV
        furi_hal_light_set(LightRed, 255);
    } else {
        // Sender in Ruhezustand versetzen
        furi_hal_subghz_idle();
        furi_hal_light_set(LightRed, 0);
    }
}

/* Sendet ein einzelnes Morse-Element (Punkt oder Strich) */
static bool send_element(uint32_t dur) {
    if(should_exit()) return false;
    set_cw(true);
    furi_delay_ms(dur);
    set_cw(false);
    furi_delay_ms(DOT_MS); // Kurze Pause nach jedem Element
    return true;
}

/* Eine Wartefunktion, die in kleinen Schritten prüft, ob abgebrochen werden soll */
static bool wait_ms(uint32_t ms) {
    for(uint32_t i = 0; i < ms / 50; i++) {
        if(should_exit()) return false;
        furi_delay_ms(50);
    }
    return true;
}

/* Wandelt einen String aus Punkten und Strichen in Morse-Signale um */
static bool m(const char* dots_dashes) {
    for(size_t i = 0; i < strlen(dots_dashes); i++) {
        if(should_exit()) return false;
        uint32_t d = (dots_dashes[i] == '-') ? DASH_MS : DOT_MS;
        if(!send_element(d)) return false;
    }
    return wait_ms(DOT_MS * 2); // Pause nach jedem Buchstaben
}

/* Hauptfunktion der App */
int32_t cw_beacon_app(void* p_ptr) {
    UNUSED(p_ptr);
    
    // 1. Hardware vorbereiten
    furi_hal_region_set(&unlockedRegion);
    furi_hal_subghz_reset();
    furi_hal_subghz_idle();
    // Nutzt ein Standard-Funkprofil als Basis (2-FSK, asynchron)
    furi_hal_subghz_load_custom_preset(subghz_device_cc1101_preset_2fsk_dev47_6khz_async_regs);
    
    // Haupt-Loop der Bake
    while(!should_exit()) {
        // Frequenz sicherheitshalber im IDLE setzen
        furi_hal_subghz_idle(); 
        furi_hal_subghz_set_frequency(CW_FREQ);

        // --- TEIL 1: Rufzeichen DM1FL/B (2 Durchgänge) ---
        for(int l = 0; l < 2; l++) {
            if(!m("-..")) break;  // D
            if(!m("--")) break;   // M
            if(!m(".----")) break;// 1
            if(!m("..-.")) break; // F
            if(!m(".-..")) break; // L
            if(!m("-..-.")) break;// /
            if(!m("-...")) break; // B
            if(!wait_ms(DOT_MS * 4)) break; // Wortpause
        }

        // --- TEIL 2: Locator LOC JO30MS (2 Durchgänge) ---
        for(int l = 0; l < 2; l++) {
            if(!m(".-..")) break; // L
            if(!m("---")) break;  // O
            if(!m("-.-.")) break; // C
            if(!wait_ms(DOT_MS * 2)) break;
            
            if(!m(".---")) break; // J
            if(!m("---")) break;  // O
            if(!m("...--")) break;// 3
            if(!m("-----")) break;// 0
            if(!m("--")) break;   // M
            if(!m("...")) break;  // S
            if(!wait_ms(DOT_MS * 4)) break;
        }

        // --- TEIL 3: Dauerträger (30 Sekunden) ---
        if(!should_exit()) {
            furi_hal_light_set(LightBlue, 255); // Blaue LED signalisiert Dauerträger
            uint32_t elapsed = 0;
            
            while(elapsed < CARRIER_TOTAL_MS && !should_exit()) {
                // Crash-Prävention: Erst IDLE, dann Frequenz-Fix, dann wieder TX
                furi_hal_subghz_idle(); 
                furi_hal_subghz_set_frequency(CW_FREQ);
                set_cw(true); 
                
                furi_delay_ms(RECALIBRATE_MS);
                elapsed += RECALIBRATE_MS;
            }
            set_cw(false);
            furi_hal_light_set(LightBlue, 0);
        }
        
        // Kurze Pause zwischen den kompletten Durchläufen
        if(!wait_ms(2000)) break;
    }

    // Beim Beenden alles sicher ausschalten
    set_cw(false);
    furi_hal_subghz_idle();
    
    return 0;
}