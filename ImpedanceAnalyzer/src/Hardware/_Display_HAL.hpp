#pragma once

#include "stm32g4xx_hal.h"

#define CFXS_LCD_SPI_CONSTEXPR

class Pin_RST {
public:
    constexpr void Write(bool state) {
        if (state) {
            GPIOB->BSRR = GPIO_PIN_7;
        } else {
            GPIOB->BSRR = GPIO_PIN_7 << 16;
        }
    }

    constexpr void ConfigureAsOutput() {
    }
};

class Pin_DCSEL {
public:
    constexpr void Write(bool state) {
        if (state) {
            GPIOB->BSRR = GPIO_PIN_9;
        } else {
            GPIOB->BSRR = GPIO_PIN_9 << 16;
        }
    }

    constexpr void ConfigureAsOutput() {
    }
};

class Pin_CS {
public:
    constexpr void Write(bool state) {
        if (state) {
            GPIOB->BSRR = GPIO_PIN_6;
        } else {
            GPIOB->BSRR = GPIO_PIN_6 << 16;
        }
    }

    constexpr void ConfigureAsOutput() {
    }
};

extern SPI_HandleTypeDef hspi3;

class LCD_SPI {
public:
    constexpr LCD_SPI() = default;

    CFXS_LCD_SPI_CONSTEXPR void Initialize() const {
        Pin_CS{}.Write(true);
        Disable();
    }

    /// @brief Configure SPI in master mode
    /// @param mode phase/polarity
    /// @param speed bitrate
    /// @param bits bits per transfer word
    void ConfigureAsMaster(CFXS::HW::SPI::Mode mode, size_t speed, size_t bits) const {
    }

    /// @brief Enable SPI
    CFXS_LCD_SPI_CONSTEXPR void Enable() const {
        SPI3->CR1 |= SPI_CR1_SPE;
        SPI3->CR1 |= USART_CR1_FIFOEN;
    }

    /// @brief Disable SPI
    CFXS_LCD_SPI_CONSTEXPR void Disable() const {
    }

    CFXS_LCD_SPI_CONSTEXPR bool IsEnabled() const {
        return true;
    }

    /// @brief Set chipselect level
    /// @param state true = high
    CFXS_LCD_SPI_CONSTEXPR void SetCS(bool state) const {
        Pin_CS{}.Write(state);
    }

    /// @brief Read all data from RX FIFO
    CFXS_LCD_SPI_CONSTEXPR void Clear_RX_FIFO() const {
        uint32_t temp;
        while (NonBlockingRead(&temp)) {
        }
    }

    /// @brief Write data to SPI
    /// @param data data to write
    /// @param waitUntilTransmitted block until transfer finished
    CFXS_LCD_SPI_CONSTEXPR void Write(uint32_t data) const {
        while (Is_TX_FIFO_Full()) {
        }
        *((__rw uint8_t*)(&(SPI3->DR))) = data;
    }

    /// @brief Write multiple bytes to SPI interface
    /// @param data data array to write
    /// @param len number of bytes to write
    /// @param waitUntilTransmitted block until transfer finished
    CFXS_LCD_SPI_CONSTEXPR void Write(uint8_t* data, size_t len) const {
        for (size_t i = 0; i < len; i++) {
            Write(data[i]);
        }
    }

    /// @brief Write multiple words to SPI interface, unrolled at compile time
    /// @param ...args words to write
    template<class... Args>
    CFXS_LCD_SPI_CONSTEXPR void WriteList(Args... args) {
        _WriteList(
            [this](auto arg) constexpr { Write(arg); }, args...);
    }

    /// @brief Read data from FIFO
    /// @param read write data to here
    CFXS_LCD_SPI_CONSTEXPR void Read(uint8_t* readTo) const {
        while (Is_RX_FIFO_Empty()) {
        }
    }

    /// @brief Attempt to read data from FIFO
    /// @param read write data to here
    /// @return true if data read, false if no data was available
    CFXS_LCD_SPI_CONSTEXPR bool NonBlockingRead(uint32_t* read) const {
        return true;
    }

    /// @brief Check if SPI TX FIFO is full
    /// @return true if FIFO full
    CFXS_LCD_SPI_CONSTEXPR bool Is_TX_FIFO_Full() const {
        return !(SPI3->SR & SPI_SR_TXE);
    }

    /// @brief Check if SPI RX FIFO is empty
    /// @return true if empty
    CFXS_LCD_SPI_CONSTEXPR bool Is_RX_FIFO_Empty() const {
        return true; // return !(__mem32(GetBase() + SPI::BaseOffset::SR) & SPI::Register::SR::RNE);
    }

    /// @brief Check if SPI busy (transfer in progress)
    /// @return true if busy
    CFXS_LCD_SPI_CONSTEXPR bool IsBusy() const {
        return (SPI3->SR & SPI_SR_BSY);
    }

    /// @brief Wait for transfer to finish
    CFXS_LCD_SPI_CONSTEXPR void WaitForTransferFinished() {
        while (IsBusy()) {
        }
    }

private:
    /////////////////////////////////////
    // Write list unroll helpers

    template<class F, class First, class... Rest>
    CFXS_LCD_SPI_CONSTEXPR void _WriteList(F f, First first, Rest... rest) {
        f(first);
        _WriteList(f, rest...);
    }

    template<class F>
    CFXS_LCD_SPI_CONSTEXPR void _WriteList(F f) {
    }
};