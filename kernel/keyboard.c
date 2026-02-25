// Keyboard driver

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

extern void print_char(char c, uint8_t color);

// US QWERTY scancode to ASCII map
const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

// Read from keyboard port
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Keyboard interrupt handler
void keyboard_handler() {
    uint8_t scancode = inb(0x60);
    
    // Only handle key press (not release)
    if (scancode & 0x80) {
        return;
    }
    
    // Convert scancode to ASCII
    if (scancode < sizeof(scancode_to_ascii)) {
        char c = scancode_to_ascii[scancode];
        if (c != 0) {
            print_char(c, 0x0F); // White text
        }
    }
}
