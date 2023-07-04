#ifndef STM32_UTILS_H_
#define STM32_UTILS_H_

#include <stdint.h>

static const uint32_t rcc_base_addr = 0x40023800UL;
static const int rcc_apb2enr_offset = 0x44;
static const int rcc_cfgr_offset = 0x08;
static const uint32_t rcc_apb2enr_addr = rcc_base_addr + rcc_apb2enr_offset;
static const uint32_t rcc_cfgr_addr = rcc_base_addr + rcc_cfgr_offset;

static const uint32_t adc1_base_addr = 0x40012000UL;
static const int adc_cr1_offset = 0x4;
static const uint32_t adc1_cr1_addr = adc1_base_addr + adc_cr1_offset;

static const int rcc_cfgr_mco1_bit = 21;
static const int rcc_cfgr_mco1_bitcnt = 2;
static const int rcc_cfgr_mco1_hsi_value = 0x00;
static const int adc1_en_bit = 8;
static const int adc_cr1_scan_bit = 8;

static inline void set_bit(uint32_t address, int bit_offset) {
    *((volatile uint32_t*) address) |= (1 << bit_offset);
}

static inline void clear_bit(uint32_t address, int bit_offset) {
    *((volatile uint32_t*) address) &= ~(1 << bit_offset);
}

static inline void set_bits(
        uint32_t address,
        int lsb_bit_offset,
        int bit_count,
        uint32_t bit_values) {
    static const uint32_t ones = ~((uint32_t) 0x0);
    // 0x11111111111 -> 0x11111111100 -> 0x00000000011 -> 0x00000001100 -> 0x11111110011 (clear_mask)
    const uint32_t clear_mask = ~((~(ones << bit_count)) << lsb_bit_offset);
    uint32_t val = (uint32_t*) address;
    val &= clear_mask; // clear out the affected bits
    val |= (bit_values << lsb_bit_offset); // set the affected bits
    *((uint32_t*) address) = val;
}

#endif /* STM32_UTILS_H_ */
