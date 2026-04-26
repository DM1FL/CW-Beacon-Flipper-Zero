#include "morse.h"
#include <string.h>

static const char* morse_table[] = {
['A']=".-",['B']="-...",['C']="-.-.",['D']="-..",['E']=".",['F']="..-.",
['G']="--.",['H']="....",['I']="..",['J']=".---",['K']="-.-",['L']=".-..",
['M']="--",['N']="-.",['O']="---",['P']=".--.",['Q']="--.-",['R']=".-.",
['S']="...",['T']="-",['U']="..-",['V']="...-",['W']=".--",['X']="-..-",
['Y']="-.--",['Z']="--..",['/']="-..-.",['0']="-----",['1']=".----",
['2']="..---",['3']="...--",[' '] = " "
};

static uint32_t dot_time(uint16_t wpm){ return 1200 / wpm; }

void morse_encode_and_send(const MorseConfig* cfg, void (*tx_cb)(bool on, uint32_t duration_ms)){
    uint32_t dt = dot_time(cfg->wpm);
    for(size_t i=0;i<strlen(cfg->text);i++){
        char c=cfg->text[i];
        if(c>='a'&&c<='z') c-=32;
        const char* code=morse_table[(int)c];
        if(!code) continue;
        for(size_t j=0;j<strlen(code);j++){
            if(code[j]=='.'){ tx_cb(true,dt); tx_cb(false,dt); }
            else if(code[j]=='-'){ tx_cb(true,dt*3); tx_cb(false,dt); }
            else if(code[j]==' '){ tx_cb(false,dt*7); }
        }
        tx_cb(false,dt*3);
    }
}