// VSR Kernel - Main Entry Point

// VGA text mode buffer at 0xB8000
#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// VGA color codes
#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define COLOR_LIGHT_GRAY 7
#define COLOR_WHITE 15

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

static uint16_t* vga_buffer = (uint16_t*)VGA_MEMORY;
static int cursor_x = 0;
static int cursor_y = 0;

// External functions
extern void idt_init();
extern void pic_remap();

void clear_screen() {
    uint8_t color = (COLOR_BLACK << 4) | COLOR_LIGHT_GRAY;
    uint16_t blank = ' ' | (color << 8);
    
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = blank;
    }
    cursor_x = 0;
    cursor_y = 0;
}

void print_char(char c, uint8_t color) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\b') {
        // Backspace
        if (cursor_x > 0) {
            cursor_x--;
            uint16_t attrib = color << 8;
            vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = ' ' | attrib;
        }
    } else if (c == '\t') {
        // Tab - move to next multiple of 4
        cursor_x = (cursor_x + 4) & ~(4 - 1);
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    } else {
        uint16_t attrib = color << 8;
        vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = c | attrib;
        cursor_x++;
        
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    
    // Scroll if needed
    if (cursor_y >= VGA_HEIGHT) {
        // Shift all lines up
        for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
            vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
        }
        // Clear last line
        uint8_t blank_color = (COLOR_BLACK << 4) | COLOR_LIGHT_GRAY;
        uint16_t blank = ' ' | (blank_color << 8);
        for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
            vga_buffer[i] = blank;
        }
        cursor_y = VGA_HEIGHT - 1;
    }
}

void print_string(const char* str, uint8_t color) {
    for (int i = 0; str[i] != '\0'; i++) {
        print_char(str[i], color);
    }
}

// Main kernel entry point
void kernel_main() {
    clear_screen();
    
    uint8_t title_color = (COLOR_BLACK << 4) | COLOR_CYAN;
    uint8_t text_color = (COLOR_BLACK << 4) | COLOR_LIGHT_GRAY;
    uint8_t prompt_color = (COLOR_BLACK << 4) | COLOR_GREEN;
    
    print_string("VSR Operating System v0.1\n", title_color);
    print_string("Kernel loaded successfully!\n\n", text_color);
    
    // Initialize interrupts
    idt_init();
    pic_remap();
    
    print_string("Interrupt system initialized.\n", text_color);
    print_string("Keyboard driver loaded.\n\n", text_color);
    
    // Enable interrupts
    __asm__ __volatile__("sti");
    
    print_string("Type something: ", prompt_color);
    
    // Halt and wait for interrupts
    while(1) {
        __asm__ __volatile__("hlt");
    }
}
