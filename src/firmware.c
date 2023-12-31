#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/scb.h>
#include "core/system.h"
#include "core/timer.h"
#include "core/uart.h"


#define LED_PORT            (GPIOA)
#define LED_PIN             (GPIO0)

#define UART2_PORT          (GPIOA)
#define RX2_PIN             (GPIO3)
#define TX2_PIN             (GPIO2)


#define BOOTLOADER_SIZE     (0x8000U)

static void vector_setup(void)  {
    
    // Reset the vector table to the main application
    SCB_VTOR = BOOTLOADER_SIZE;
}

static void gpio_setup(void)    {
    rcc_periph_clock_enable(RCC_GPIOA);
    
    // Set gpio to alternate function, then set the alternate function to AF1 (TIM2)
    gpio_mode_setup(LED_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LED_PIN);
    gpio_set_af(LED_PORT, GPIO_AF1, LED_PIN);

    // Set up UART2 and change the pins to alternate function
    gpio_mode_setup(UART2_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, TX2_PIN | RX2_PIN );
    gpio_set_af(UART2_PORT,GPIO_AF7, TX2_PIN | RX2_PIN );
    
}

int main(void)  {

    // Change the location of vector table to main application
    vector_setup();
    
    // Set up GPIOs, timers, and clocks
    gpio_setup();
    system_setup();
    timer_setup();
    // Set up UART
    uart_setup();

    uint64_t start_time = system_get_ticks();
    float duty_cycle = 0.0f;

    timer_pwm_set_duty_cycle(duty_cycle);

    while(1)    {
        
        if(system_get_ticks() - start_time >= 50) {
            
            // Increment duty cycle then reset
            duty_cycle += 1.0f;
            if(duty_cycle > 100.0f)   {
                duty_cycle = 0.0f;
            }

            // Update PWM duty cycle
            timer_pwm_set_duty_cycle(duty_cycle);
            start_time = system_get_ticks();
        }

        // Check for and receive UART data
        if(uart_data_available())   {
            uint8_t data = uart_read_byte();
            uart_write_byte(data +1);
        }
        system_delay(1000);
    }
    return 0;   // Never return
}