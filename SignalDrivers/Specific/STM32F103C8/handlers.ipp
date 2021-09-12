#pragma once

#ifdef __cplusplus
extern "C" {
#endif
void DMA1_Channel1_IRQHandler() {
  static const constexpr auto dma_device = SD::Hardware::DMADevice::DMA_1;
  static const constexpr auto dma_channel = SD::Hardware::DMAChannel::Channel1;


  if (LL_DMA_IsActiveFlag_TC1(DMA1) == 1) {
    SD::Specific::GetMasterInstance().DMACallback(dma_device, dma_channel);
    LL_DMA_ClearFlag_TC1(DMA1);
  }
  if (LL_DMA_IsActiveFlag_TE1(DMA1) == 1) {
    LL_DMA_ClearFlag_TE1(DMA1);
  }
}

void DMA1_Channel4_IRQHandler() {
  static const constexpr auto dma_device = SD::Hardware::DMADevice::DMA_1;
  static const constexpr auto dma_channel = SD::Hardware::DMAChannel::Channel4;


  if (LL_DMA_IsActiveFlag_TC4(DMA1) == 1) {
    SD::Specific::GetMasterInstance().DMACallback(dma_device, dma_channel);
    LL_DMA_ClearFlag_TC4(DMA1);
  }
  if (LL_DMA_IsActiveFlag_TE4(DMA1) == 1) {
    LL_DMA_ClearFlag_TE4(DMA1);
  }
}

#ifdef __cplusplus
}
#endif
