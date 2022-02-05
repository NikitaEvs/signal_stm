#pragma once

#include <cstdlib>
#include <functional>

namespace SD {

namespace Utils {

template <typename U, typename V>
class Transformer {
 public:
  using TransformerBefore = std::function<void(U*, std::size_t)>;
  using TransformerAfter = std::function<void(V*, std::size_t)>;
  using TransformerMain = std::function<void(U*, V*, std::size_t, std::size_t)>;

 public:
  Transformer() = default;
  ~Transformer() = default;

  Transformer(const Transformer&) = delete;
  Transformer& operator=(const Transformer&) = delete;

  Transformer(Transformer&&) noexcept = default;
  Transformer& operator=(Transformer&&) noexcept = default;

  void operator()(U* in, V* out, std::size_t in_size, std::size_t out_size) {
    if (transformer_before_) {
      transformer_before_(in, in_size);
    }

    if (transformer_main_) {
      transformer_main_(in, out, in_size, out_size);
    }

    if (transformer_after_) {
      transformer_after_(out, out_size);
    }
  }

  void Before(TransformerBefore&& transformer) {
    transformer_before_ = std::move(transformer);
  }

  void SetMainFunction(TransformerMain&& transformer) {
    transformer_main_ = std::move(transformer);
  }

  void After(TransformerAfter&& transformer) {
    transformer_after_ = std::move(transformer);
  }

 private:
  TransformerBefore transformer_before_;
  TransformerAfter transformer_after_;
  TransformerMain transformer_main_;
};

template <typename U, typename V>
class TransformerFactory {
 public:
  using ProcessingFunction =
    std::function<void(U*, V*, std::size_t, std::size_t)>;

 public:
  TransformerFactory() = default;
  ~TransformerFactory() = default;

  TransformerFactory(const TransformerFactory&) = delete;
  TransformerFactory& operator=(const TransformerFactory&) = delete;

  TransformerFactory(TransformerFactory&&) noexcept = default;
  TransformerFactory& operator=(TransformerFactory&&) noexcept = default;

  virtual Transformer<U, V> CreateTransformer() = 0;
};

template <typename U, typename V>
class BasicTransformerFactory : public TransformerFactory<U, V> {
 public:
  using ProcessingFunction = typename TransformerFactory<U, V>::ProcessingFunction;
 public:
  BasicTransformerFactory() = delete;

  explicit BasicTransformerFactory(ProcessingFunction&& func) {
    proc_func_ = std::move(func);
  }

  // One-shot function, TODO: add some protection about it
  Transformer<U, V> CreateTransformer() override {
    Transformer<U, V> transformer;
    transformer.SetMainFunction(std::move(proc_func_));

    return transformer;
  }

 private:
  ProcessingFunction proc_func_;
};

} // namespace Utils
} // namespace SD
