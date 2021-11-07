#pragma once

#ifdef __cplusplus
extern "C" {
#endif
void DMA2_Stream4_IRQHandler() {
  static const constexpr SD::Hardware::DMAPort kDMA_port = {
      .device = SD::Hardware::DMADevice::DMA_2,
      .stream = SD::Hardware::DMAStream::Stream4
  };

  if (LL_DMA_IsActiveFlag_TC4(DMA2) == 1) {
    SD::Specific::GetMasterInstance().DMACallback(kDMA_port);
    LL_DMA_ClearFlag_TC4(DMA2);
  }
  if (LL_DMA_IsActiveFlag_TE4(DMA2) == 1) {
    LL_DMA_ClearFlag_TE4(DMA2);
  }
}

void DMA2_Stream7_IRQHandler() {
  static const constexpr SD::Hardware::DMAPort kDMA_port = {
      .device = SD::Hardware::DMADevice::DMA_2,
      .stream = SD::Hardware::DMAStream::Stream7
  };

  if (LL_DMA_IsActiveFlag_TC7(DMA2) == 1) {
    SD::Specific::GetMasterInstance().DMACallback(kDMA_port);
    LL_DMA_ClearFlag_TC7(DMA2);
  }
  if (LL_DMA_IsActiveFlag_TE7(DMA2) == 1) {
    LL_DMA_ClearFlag_TE7(DMA2);
  }
}

#ifdef __cplusplus
}
#endif
