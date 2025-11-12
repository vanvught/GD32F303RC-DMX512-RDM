/**
 * @file dmx_dma_check.h
 *
 */

#if defined(GD32F10X_HD) || defined(GD32F10X_CL)
static_assert(DMX_MAX_PORTS <= 4, "Too many ports defined");
#endif
#if defined(GD32F20X_CL)
static_assert(DMX_MAX_PORTS <= 6, "Too many ports defined");
#endif

/**
 * DMA channel check
 */
#if defined(GD32F10X_HD) || defined(GD32F10X_CL)
#if defined(DMX_USE_UART4)
#error There is no DMA channel for UART4
#endif
#if defined(DMX_USE_USART5)
#error USART5 is not available
#endif
#if defined(DMX_USE_UART6)
#error UART6 is not available
#endif
#if defined(DMX_USE_UART7)
#error UART7 is not available
#endif
#endif

#if defined(GD32F20X_CL)
#if defined(DMX_USE_UART4) && defined(DMX_USE_UART7)
#error DMA1 Channel 3
#endif
#if defined(DMX_USE_UART3) && defined(DMX_USE_UART6)
#error DMA1 Channel 4
#endif
#endif
