// Interrupt Descriptor Table (IDT) setup

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

// IDT entry structure
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

// IDT pointer structure
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;

// External assembly interrupt handlers
extern void isr0();
extern void isr1();
extern void irq1();

// Set an IDT gate
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

// Initialize IDT
void idt_init() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;
    
    // Clear IDT
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    
    // Set up keyboard interrupt (IRQ1)
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    
    // Load IDT
    __asm__ __volatile__("lidt %0" : : "m"(idtp));
}

// Remap PIC (Programmable Interrupt Controller)
void pic_remap() {
    // ICW1 - Initialize
    __asm__ __volatile__("outb %0, %1" : : "a"((uint8_t)0x11), "Nd"((uint16_t)0x20));
    __asm__ __volatile__("outb %0, %1" : : "a"((uint8_t)0x11), "Nd"((uint16_t)0xA0));
    
    // ICW2 - Remap IRQs to 32-47
    __asm__ __volatile__("outb %0, %1" : : "a"((uint8_t)0x20), "Nd"((uint16_t)0x21));
    __asm__ __volatile__("outb %0, %1" : : "a"((uint8_t)0x28), "Nd"((uint16_t)0xA1));
    
    // ICW3 - Setup cascading
    __asm__ __volatile__("outb %0, %1" : : "a"((uint8_t)0x04), "Nd"((uint16_t)0x21));
    __asm__ __volatile__("outb %0, %1" : : "a"((uint8_t)0x02), "Nd"((uint16_t)0xA1));
    
    // ICW4 - 8086 mode
    __asm__ __volatile__("outb %0, %1" : : "a"((uint8_t)0x01), "Nd"((uint16_t)0x21));
    __asm__ __volatile__("outb %0, %1" : : "a"((uint8_t)0x01), "Nd"((uint16_t)0xA1));
    
    // Mask all IRQs except keyboard (IRQ1)
    __asm__ __volatile__("outb %0, %1" : : "a"((uint8_t)0xFD), "Nd"((uint16_t)0x21));
    __asm__ __volatile__("outb %0, %1" : : "a"((uint8_t)0xFF), "Nd"((uint16_t)0xA1));
}
