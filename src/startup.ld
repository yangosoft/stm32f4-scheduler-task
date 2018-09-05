MEMORY
{
    mem : ORIGIN = 0x08000000, LENGTH = 2048K
    data : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS
{
    .text : { *(.text*) } > mem
    .data : { *(.text*) } > data
}
