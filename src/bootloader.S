// This is some special magic to take advantage of ASM in order to load our bootloader as a binary for the linker script
.section .bootloader_section
    .incbin "../stm32_bootloader/bootloader.bin"