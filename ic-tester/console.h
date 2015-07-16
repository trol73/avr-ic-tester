//
//  console.h
//  zx-tape-loader
//
//  Created by Oleg Trifonov on 24.05.15.
//
//

#ifndef zx_tape_loader_console_h
#define zx_tape_loader_console_h

#define CONSOLE_WIDTH   14
#define CONSOLE_HEIGHT  5


char console_data[CONSOLE_WIDTH * CONSOLE_HEIGHT];
uint8_t console_line = 0;


void draw_console() {
    glcd_tiny_set_font(Font5x7, 5, 7, 32, 127);
    glcd_clear_buffer();

    uint8_t y = 0;
    for (uint16_t i = 0; i < console_line; i++) {
        glcd_draw_string_xy(0, y, console_data + i*CONSOLE_WIDTH);
        y += 10;
    }
    glcd_write();
}


void print(char *s) {
    if (console_line >= CONSOLE_HEIGHT) {
        for (uint16_t i = 0; i < CONSOLE_WIDTH*(CONSOLE_HEIGHT-1); i++) {
            console_data[i] = console_data[i+CONSOLE_WIDTH];
        }
        console_line--;
    }
    for (uint16_t i = 0; i < CONSOLE_WIDTH; i++) {
        console_data[i + console_line * CONSOLE_WIDTH] = s[i];
    }
    console_line++;

    
    draw_console();
}


#endif
