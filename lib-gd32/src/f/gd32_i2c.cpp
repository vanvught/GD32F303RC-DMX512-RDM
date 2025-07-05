/**
 * @file gd32_i2c.cpp
 *
 */
/* Copyright (C) 2021-2025 by Arjan van Vught mailto:info@gd32-dmx.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#if defined(CONFIG_I2C_OPTIMIZE_O2) || defined(CONFIG_I2C_OPTIMIZE_O3)
#pragma GCC push_options
#if defined(CONFIG_I2C_OPTIMIZE_O2)
#pragma GCC optimize("O2")
#else
#pragma GCC optimize("O3")
#endif
#endif

#include <cstdint>

#include "gd32.h"
#include "gd32_i2c.h"

static constexpr int32_t TIMEOUT = 0xfff;

static uint8_t s_address;
static uint8_t s_address1;

/**
 * i2c master sends start signal only when the bus is idle
 */
template <uint32_t PERIPH> static int32_t send_start()
{
    auto nTimeout = TIMEOUT;

    while (i2c_flag_get(PERIPH, I2C_FLAG_I2CBSY))
    {
        if (--nTimeout <= 0)
        {
            return -GD32_I2C_NOK_TOUT;
        }
    }

    i2c_start_on_bus(PERIPH);

    nTimeout = TIMEOUT;
    /* wait until SBSEND bit is set */
    while (!i2c_flag_get(PERIPH, I2C_FLAG_SBSEND))
    {
        if (--nTimeout <= 0)
        {
            return -GD32_I2C_NOK_TOUT;
        }
    }

    return GD32_I2C_OK;
}

template <uint32_t PERIPH> static int32_t send_slaveaddr()
{
    if constexpr (PERIPH == I2C1)
    {
        i2c_master_addressing(PERIPH, s_address1, I2C_TRANSMITTER);
    }
    else
    {
        i2c_master_addressing(PERIPH, s_address, I2C_TRANSMITTER);
    }

    auto nTimeout = TIMEOUT;

    while (!i2c_flag_get(PERIPH, I2C_FLAG_ADDSEND))
    {
        if (--nTimeout <= 0)
        {
            return -GD32_I2C_NOK_TOUT;
        }
    }

    i2c_flag_clear(PERIPH, I2C_FLAG_ADDSEND);

    nTimeout = TIMEOUT;

    while (SET != i2c_flag_get(PERIPH, I2C_FLAG_TBE))
    {
        if (--nTimeout <= 0)
        {
            return -GD32_I2C_NOK_TOUT;
        }
    }

    return GD32_I2C_OK;
}

/**
 * send a stop condition to I2C bus
 */
template <uint32_t PERIPH> static int32_t send_stop()
{
    auto nTimeout = TIMEOUT;

    i2c_stop_on_bus(PERIPH);

    /* wait until the stop condition is finished */
    while (I2C_CTL0(PERIPH) & I2C_CTL0_STOP)
    {
        if (--nTimeout <= 0)
        {
            return -GD32_I2C_NOK_TOUT;
        }
    }

    return GD32_I2C_OK;
}

template <uint32_t PERIPH> static int32_t SendData(const uint8_t* pData, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        i2c_data_transmit(PERIPH, *pData);
        pData++;
        auto nTimeout = TIMEOUT;

        while (!i2c_flag_get(PERIPH, I2C_FLAG_BTC))
        {
            if (--nTimeout <= 0)
            {
                return -GD32_I2C_NOK_TOUT;
            }
        }
    }

    return GD32_I2C_OK;
}

template <uint32_t PERIPH> static int32_t WriteImplementation(const char* buffer, uint32_t length)
{
    if (send_start<PERIPH>() != GD32_I2C_OK)
    {
        send_stop<PERIPH>();
        return -1;
    }

    if (send_slaveaddr<PERIPH>() != GD32_I2C_OK)
    {
        send_stop<PERIPH>();
        return -1;
    }

    if (SendData<PERIPH>(reinterpret_cast<const uint8_t*>(buffer), length) != GD32_I2C_OK)
    {
        send_stop<PERIPH>();
        return -1;
    }

    send_stop<PERIPH>();

    return 0;
}

template <uint32_t PERIPH> static uint8_t ReadImplementation(char* buffer, uint32_t length)
{
    auto nTimeout = TIMEOUT;

    while (i2c_flag_get(PERIPH, I2C_FLAG_I2CBSY))
    {
        if (--nTimeout <= 0)
        {
            send_stop<PERIPH>();
            return GD32_I2C_NOK_TOUT;
        }
    }

    if (2 == length)
    {
        i2c_ackpos_config(PERIPH, I2C_ACKPOS_NEXT);
    }

    i2c_start_on_bus(PERIPH);

    nTimeout = TIMEOUT;

    while (!i2c_flag_get(PERIPH, I2C_FLAG_SBSEND))
    {
        if (--nTimeout <= 0)
        {
            send_stop<PERIPH>();
            return GD32_I2C_NOK_TOUT;
        }
    }

    if constexpr (PERIPH == I2C1)
    {
        i2c_master_addressing(PERIPH, s_address1, I2C_RECEIVER);
    }
    else
    {
        i2c_master_addressing(PERIPH, s_address, I2C_RECEIVER);
    }

    if (length < 3)
    {
        i2c_ack_config(PERIPH, I2C_ACK_DISABLE);
    }

    nTimeout = TIMEOUT;

    while (!i2c_flag_get(PERIPH, I2C_FLAG_ADDSEND))
    {
        if (--nTimeout <= 0)
        {
            send_stop<PERIPH>();
            return GD32_I2C_NOK_TOUT;
        }
    }

    i2c_flag_clear(PERIPH, I2C_FLAG_ADDSEND);

    if (1 == length)
    {
        i2c_stop_on_bus(PERIPH);
    }

    auto nTimeoutLoop = TIMEOUT;

    while (length)
    {
        if (3 == length)
        {
            nTimeout = TIMEOUT;

            while (!i2c_flag_get(PERIPH, I2C_FLAG_BTC))
            {
                if (--nTimeout <= 0)
                {
                    send_stop<PERIPH>();
                    return GD32_I2C_NOK_TOUT;
                }
            }

            i2c_ack_config(PERIPH, I2C_ACK_DISABLE);
        }

        if (2 == length)
        {
            nTimeout = TIMEOUT;

            while (!i2c_flag_get(PERIPH, I2C_FLAG_BTC))
            {
                if (--nTimeout <= 0)
                {
                    send_stop<PERIPH>();
                    return GD32_I2C_NOK_TOUT;
                }
            }

            i2c_stop_on_bus(PERIPH);
        }

        if (i2c_flag_get(PERIPH, I2C_FLAG_RBNE))
        {
            *buffer = i2c_data_receive(PERIPH);
            buffer++;
            length--;
            nTimeoutLoop = TIMEOUT;
        }

        if (--nTimeoutLoop <= 0)
        {
            send_stop<PERIPH>();
            return GD32_I2C_NOK_TOUT;
        }
    }

    nTimeout = TIMEOUT;

    while (I2C_CTL0(PERIPH) & I2C_CTL0_STOP)
    {
        if (--nTimeout <= 0)
        {
            return GD32_I2C_NOK_TOUT;
        }
    }

    i2c_ack_config(PERIPH, I2C_ACK_ENABLE);
    i2c_ackpos_config(PERIPH, I2C_ACKPOS_CURRENT);

    return GD32_I2C_OK;
}

template <uint32_t PERIPH> void write_register_implementation(const uint8_t reg, const uint8_t value)
{
    char buffer[2];

    buffer[0] = static_cast<char>(reg);
    buffer[1] = static_cast<char>(value);

    WriteImplementation<PERIPH>(buffer, 2);
}

template <uint32_t PERIPH> void read_register_implementation(const uint8_t reg, uint8_t& value)
{
    char buffer[1];

    buffer[0] = static_cast<char>(reg);

    WriteImplementation<PERIPH>(buffer, 1);
    ReadImplementation<PERIPH>(buffer, 1);

    value = buffer[0];
}

static void RcuConfigI2c()
{
    rcu_periph_clock_enable(I2C_RCU_I2Cx);
    rcu_periph_clock_enable(I2C_SCL_RCU_GPIOx);
    rcu_periph_clock_enable(I2C_SDA_RCU_GPIOx);
}

static void GpioConfigI2c()
{
#if defined(GPIO_INIT)
    gpio_init(I2C_SCL_GPIOx, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C_SCL_GPIO_PINx);
    gpio_init(I2C_SDA_GPIOx, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C_SDA_GPIO_PINx);

#if defined(I2C_REMAP)
    if (I2C_REMAP == GPIO_I2C0_REMAP)
    {
        gpio_pin_remap_config(GPIO_I2C0_REMAP, ENABLE);
    }
    else
    {
        assert(0);
    }
#endif
#else
    gpio_af_set(I2C_SCL_GPIOx, I2C_GPIO_AFx, I2C_SCL_GPIO_PINx);
    gpio_mode_set(I2C_SCL_GPIOx, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SCL_GPIO_PINx);
    gpio_output_options_set(I2C_SCL_GPIOx, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCL_GPIO_PINx);

    gpio_af_set(I2C_SDA_GPIOx, I2C_GPIO_AFx, I2C_SDA_GPIO_PINx);
    gpio_mode_set(I2C_SDA_GPIOx, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SDA_GPIO_PINx);
    gpio_output_options_set(I2C_SDA_GPIOx, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SDA_GPIO_PINx);
#endif
}

#if defined(CONFIG_ENABLE_I2C1)
static void RcuConfigI2c1()
{
    rcu_periph_clock_enable(RCU_I2C1);
    rcu_periph_clock_enable(I2C1_SCL_RCU_GPIOx);
    rcu_periph_clock_enable(I2C1_SDA_RCU_GPIOx);
}

static void GpioConfigI2c1()
{
#if defined(GPIO_INIT)
    gpio_init(I2C1_SCL_GPIOx, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C1_SCL_GPIO_PINx);
    gpio_init(I2C1_SDA_GPIOx, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C1_SDA_GPIO_PINx);

#if defined(I2C1_REMAP)
    if ((I2C1_REMAP == AFIO_PCF5_I2C1_REMAP) || (I2C1_REMAP == GPIO_PCF5_I2C1_REMAP0) || (I2C1_REMAP == GPIO_PCF5_I2C1_REMAP1))
    {
        gpio_pin_remap_config(I2C1_REMAP, ENABLE);
    }
    else
    {
        assert(0);
    }
#endif
#else
    gpio_af_set(I2C1_SCL_GPIOx, I2C1_GPIO_AFx, I2C1_SCL_GPIO_PINx);
    gpio_mode_set(I2C1_SCL_GPIOx, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C1_SCL_GPIO_PINx);
    gpio_output_options_set(I2C1_SCL_GPIOx, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C1_SCL_GPIO_PINx);

    gpio_af_set(I2C1_SDA_GPIOx, I2C1_GPIO_AFx, I2C1_SDA_GPIO_PINx);
    gpio_mode_set(I2C1_SDA_GPIOx, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C1_SDA_GPIO_PINx);
    gpio_output_options_set(I2C1_SDA_GPIOx, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C1_SDA_GPIO_PINx);
#endif
}
#endif

template <uint32_t PERIPH> static void I2cConfig()
{
    i2c_clock_config(PERIPH, gd32::kI2CFullSpeed, I2C_DTCY_2);
    i2c_enable(PERIPH);
    i2c_ack_config(PERIPH, I2C_ACK_ENABLE);
}

/*
 * Public API's
 */

void Gd32I2cBegin()
{
    RcuConfigI2c();
    GpioConfigI2c();
    I2cConfig<I2C_PERIPH>();
}

void Gd32I2cSetBaudrate(uint32_t baudrate)
{
    i2c_clock_config(I2C_PERIPH, baudrate, I2C_DTCY_2);
}

void Gd32I2cSetAddress(uint8_t address)
{
    s_address = address << 1;
}

uint8_t Gd32I2cWrite(const char* pBuffer, uint32_t length)
{
    const auto ret = WriteImplementation<I2C_PERIPH>(pBuffer, length);
    return static_cast<uint8_t>(-ret);
}

uint8_t Gd32I2cWrite(uint8_t address, const char* pBuffer, uint32_t length)
{
    s_address = address << 1;
    const auto ret = WriteImplementation<I2C_PERIPH>(pBuffer, length);
    return static_cast<uint8_t>(-ret);
}

uint8_t Gd32I2cRead(char* pBuffer, uint32_t length)
{
    return ReadImplementation<I2C_PERIPH>(pBuffer, length);
}

uint8_t Gd32I2cRead(uint8_t address, char* pBuffer, uint32_t length)
{
    s_address = address << 1;
    return ReadImplementation<I2C_PERIPH>(pBuffer, length);
}

bool Gd32I2cIsConnected(uint8_t address, uint32_t baudrate)
{
    Gd32I2cSetAddress(address);
    Gd32I2cSetBaudrate(baudrate);

    uint8_t nResult;
    char buffer;

    if ((address >= 0x30 && address <= 0x37) || (address >= 0x50 && address <= 0x5F))
    {
        nResult = Gd32I2cRead(&buffer, 1);
    }
    else
    {
        /* This is known to corrupt the Atmel AT24RF08 EEPROM */
        nResult = Gd32I2cWrite(nullptr, 0);
    }

    return (nResult == 0);
}

void Gd32I2cWriteReg(uint8_t reg, uint8_t value)
{
    write_register_implementation<I2C_PERIPH>(reg, value);
}

void Gd32I2cWriteReg(uint8_t address, uint8_t reg, uint8_t value)
{
    s_address = address << 1;
    write_register_implementation<I2C_PERIPH>(reg, value);
}

void Gd32I2cReadReg(uint8_t reg, uint8_t& value)
{
    read_register_implementation<I2C_PERIPH>(reg, value);
}

void Gd32I2cReadReg(uint8_t address, uint8_t reg, uint8_t& value)
{
    s_address = address << 1;
    read_register_implementation<I2C_PERIPH>(reg, value);
}

#if defined(CONFIG_ENABLE_I2C1)
void Gd32I2c1Begin()
{
    RcuConfigI2c1();
    GpioConfigI2c1();
    I2cConfig<I2C1>();
}

void Gd32I2c1SetBaudrate(uint32_t baudrate)
{
    i2c_clock_config(I2C1, baudrate, I2C_DTCY_2);
}

void Gd32I2c1SetAddress(uint8_t address)
{
    s_address1 = address << 1;
}

uint8_t Gd32I2c1Write(const char* pBuffer, uint32_t length)
{
    const auto ret = WriteImplementation<I2C1>(pBuffer, length);
    return static_cast<uint8_t>(-ret);
}

uint8_t Gd32I2c1Write(uint8_t address, const char* pBuffer, uint32_t length)
{
    s_address1 = address << 1;
    const auto ret = WriteImplementation<I2C1>(pBuffer, length);
    return static_cast<uint8_t>(-ret);
}

uint8_t Gd32I2c1Read(char* pBuffer, uint32_t length)
{
    return ReadImplementation<I2C1>(pBuffer, length);
}

uint8_t Gd32I2c1Read(uint8_t address, char* pBuffer, uint32_t length)
{
    s_address1 = address << 1;
    return ReadImplementation<I2C1>(pBuffer, length);
}

bool Gd32I2c1IsConnected(uint8_t address, uint32_t baudrate)
{
    Gd32I2c1SetAddress(address);
    Gd32I2c1SetBaudrate(baudrate);

    uint8_t nResult;
    char buffer;

    if ((address >= 0x30 && address <= 0x37) || (address >= 0x50 && address <= 0x5F))
    {
        nResult = Gd32I2c1Read(&buffer, 1);
    }
    else
    {
        /* This is known to corrupt the Atmel AT24RF08 EEPROM */
        nResult = Gd32I2c1Write(nullptr, 0);
    }

    return (nResult == 0);
}

void Gd32I2c1WriteReg(uint8_t reg, uint8_t value)
{
    write_register_implementation<I2C1>(reg, value);
}

void Gd32I2c1ReadReg(uint8_t reg, uint8_t& value)
{
    read_register_implementation<I2C1>(reg, value);
}
#endif
