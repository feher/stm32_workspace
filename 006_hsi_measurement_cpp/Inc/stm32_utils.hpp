#ifndef STM32_UTILS_HPP_
#define STM32_UTILS_HPP_

#include <stdint.h>

static inline volatile uint32_t& get_reg(uint32_t address) {
    return *reinterpret_cast<volatile uint32_t*>(address);
}

static inline void set_reg(uint32_t address, uint32_t value) {
    get_reg(address) = value;
}

static inline void set_bit(uint32_t address, int bit_offset) {
    auto& reg = get_reg(address);
    reg = reg | (1 << bit_offset);
}

static inline void clear_bit(uint32_t address, int bit_offset) {
    auto& reg = get_reg(address);
    reg = reg & ~(1 << bit_offset);
}

static inline void set_bits(
        uint32_t address,
        int lsb_offset,
        int bit_count,
        uint32_t bit_values) {
    constexpr uint32_t ones = ~((uint32_t) 0x0);
    // bit_count = 2
    // lsb_offest = 3
    // 0x11111111111 -> 0x11111111100 -> 0x00000000011 -> 0x00000011000 -> 0x11111100111 (clear_mask)
    const uint32_t clear_mask = ~((~(ones << bit_count)) << lsb_offset);
    uint32_t val = get_reg(address);
    val &= clear_mask; // clear out the affected bits
    val |= (bit_values << lsb_offset); // set the affected bits
    set_reg(address, val);
}

static inline uint32_t get_bits(
        uint32_t address,
        int lsb_offset,
        int bit_count) {
    constexpr uint32_t ones = ~((uint32_t) 0x0);
    // bit_count = 2
    // lsb_offest = 3
    // 0x11111111111 -> 0x11111111100 -> 0x00000000011 -> 0x00000011000 (mask)
    const uint32_t mask = (~(ones << bit_count)) << lsb_offset;
    uint32_t val = get_reg(address);
    val &= mask; // clear out the unwanted bits
    return val >> lsb_offset;
}

static constexpr uint32_t rcc_base_addr = 0x40023800UL;

template<uint32_t address, int offset>
struct bit_t {
    static_assert(0 <= offset && offset <= 31);
    static inline void set() {
        set_bit(address, offset);
    }
    static inline void clear() {
        clear_bit(address, offset);
    }
    static inline bool is_set() {
        return (get_reg(address) & (1 << offset)) != 0;
    }
};

template<uint32_t address, int lsb_offset, int bit_count>
struct bits_t {
    static_assert(0 <= lsb_offset && lsb_offset <= 31);
    static_assert(bit_count < 32 - lsb_offset);
    static inline void set(uint32_t value) {
        set_bits(address, lsb_offset, bit_count, value);
    }
    static inline uint32_t get() {
        return get_bits(address, lsb_offset, bit_count);
    }
};

template<uint32_t address>
struct reg_t {
    static inline uint32_t get_value() {
        return *reinterpret_cast<volatile uint32_t*>(address);
    }
    static inline uint32_t set_value(uint32_t value) {
        return *reinterpret_cast<volatile uint32_t*>(address) = value;
    }
};

template<uint32_t address>
struct apb2enr_t : public reg_t<address> {
    static constexpr uint32_t reset_value = 0x0;
    [[no_unique_address]] bit_t<address, 0> tim1en;
    [[no_unique_address]] bit_t<address, 1> tim2en;
    [[no_unique_address]] bit_t<address, 4> usart1en;
    [[no_unique_address]] bit_t<address, 5> usart6en;
    [[no_unique_address]] bit_t<address, 8> adc1en;
    [[no_unique_address]] bit_t<address, 9> adc2en;
    [[no_unique_address]] bit_t<address, 10> adc3en;
};

template<uint32_t address>
struct ahb1enr_t : public reg_t<address> {
    static constexpr uint32_t reset_value = 0x0;
    [[no_unique_address]] bit_t<address, 0> gpioaen;
    [[no_unique_address]] bit_t<address, 1> gpioben;
    [[no_unique_address]] bit_t<address, 2> gpiocen;
};

template<uint32_t address>
struct rcc_cfgr_t : public reg_t<address> {
    static constexpr uint32_t reset_value = 0x0;
    [[no_unique_address]] bit_t<address, 0> gpioaen;
    [[no_unique_address]] bit_t<address, 1> gpioben;
    [[no_unique_address]] bit_t<address, 2> gpiocen;

    [[no_unique_address]] bits_t<address, 21, 2> mco1;
    static constexpr uint32_t mco1_hsi_value = 0x00;
};

template<uint32_t address>
struct rcc_t {
    [[no_unique_address]] rcc_cfgr_t<address + 0x8> cgfr;
    [[no_unique_address]] ahb1enr_t<address + 0x30> ahb1enr;
    [[no_unique_address]] apb2enr_t<address + 0x44> apb2enr;
};

static const rcc_t<rcc_base_addr>* rcc = reinterpret_cast<rcc_t<rcc_base_addr>*>(rcc_base_addr);

template<uint32_t address>
struct adc_cr1_t : public reg_t<address> {
    [[no_unique_address]] bit_t<address, 8> scan;
};

template<uint32_t address>
struct adc1_t {
    [[no_unique_address]] adc_cr1_t<address + 0x4> cr1;
};

static constexpr uint32_t adc1_base_addr = 0x40012000UL;
static const adc1_t<adc1_base_addr>* adc1 = reinterpret_cast<adc1_t<adc1_base_addr>*>(adc1_base_addr);

template<uint32_t address>
struct gpio_moder_t : public reg_t<address> {
    [[no_unique_address]] bits_t<address, 16, 2> moder8;
    static constexpr uint32_t moder_input_value = 0x00;
    static constexpr uint32_t moder_output_value = 0x01;
    static constexpr uint32_t moder_altfunc_value = 0x10;
    static constexpr uint32_t moder_analog_value = 0x11;
};

template<uint32_t address>
struct gpio_afrh_t : public reg_t<address> {
    [[no_unique_address]] bits_t<address, 0, 4> afr8;
    static constexpr uint32_t afr_af0_value = 0x00;
};

template<uint32_t address>
struct gpioa_t {
    static constexpr uint32_t reset_value = 0b10101000_00000000_00000000_00000000;
    [[no_unique_address]] gpio_moder_t<address + 0x0> moder;
    [[no_unique_address]] gpio_afrh_t<address + 0x24> afrh;
};

static const uint32_t gpioa_base_addr = 0x40020000UL;
static const gpioa_t<gpioa_base_addr>* gpioa = reinterpret_cast<gpioa_t<gpioa_base_addr>*>(gpioa_base_addr);

#endif /* STM32_UTILS_HPP_ */
