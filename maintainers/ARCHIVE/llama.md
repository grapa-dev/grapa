# Mac

xcode-select --install
brew install cmake

rm -rf build dist

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -DLLAMA_METAL=ON \
  -DGGML_BLAS=ON \
  -DGGML_BACKEND_DL=OFF \
  -DGGML_METAL_EMBED_LIBRARY=ON

cmake --build build -j

cmake --install build --prefix ./dist

copy dist/include/*.h ../../source/llama
copy dist/lib/libggml*.a ../../source/llama-lib/mac-arm64
rm ../../source/llama-lib/mac-arm64/libggml.a

-framework Metal -framework Foundation -framework Accelerate

target_link_libraries(grapa_app PRIVATE
  llama
  ggml-base
  ggml-cpu
  ggml-metal
  ggml-blas
  mtmd
  "-framework Metal"
  "-framework Foundation"
  "-framework Accelerate"
)


•    Linux (CPU-only + BLAS): link llama, ggml-base, ggml-cpu, ggml-blas, plus -lopenblas -lpthread -lm -ldl (or your BLAS/MKL choice).
•    Linux (CUDA): llama, ggml-base, ggml-cpu, ggml-cuda, plus CUDA libs (-lcublas -lcublasLt -lcudart …).
•    Windows (CPU-only): llama.lib, ggml-base.lib, ggml-cpu.lib (and ggml-blas.lib if enabled), plus bcrypt.lib; use /MT if you want static CRT.

pip install -U huggingface_hub
/// huggingface-cli login

mkdir -p ./hf_cache
export HF_HOME="$PWD/hf_cache"

hf download Qwen/Qwen2.5-7B-Instruct-GGUF \
  --include "qwen2.5-7b-instruct-q5_k_m-*.gguf" \
  --local-dir ./models/qwen2.5-7b-instruct

rm -rf build

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLAMA_BUILD_TESTS=ON \
  -DLLAMA_BUILD_EXAMPLES=ON \
  -DLLAMA_BUILD_TOOLS=ON

cmake --build build --target gguf-split -j

 ./build/bin/gguf-split --merge \
  ./models/qwen2.5-7b-instruct/qwen2.5-7b-instruct-q5_k_m-00001-of-00002.gguf \
  ./models/qwen2.5-7b-instruct/qwen2.5-7b-instruct-q5_k_m.gguf
   

# Linux

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -DLLAMA_CUBLAS=OFF \
  -DGGML_BLAS=ON \
  -DGGML_BACKEND_DL=OFF

cmake --build build -j
cmake --install build --prefix ./dist

    •    Produces libllama.a, libggml-base.a, libggml-cpu.a, libggml-blas.a.
    •    Link also against your system BLAS (-lopenblas -lm -lpthread -ldl).
    
# Linux + CUDA

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -DLLAMA_CUBLAS=ON \
  -DGGML_BLAS=ON \
  -DGGML_BACKEND_DL=OFF

cmake --build build -j
cmake --install build --prefix ./dist

    •    Produces libllama.a, libggml-base.a, libggml-cpu.a, libggml-blas.a, libggml-cuda.a.
    •    You’ll need CUDA toolkit installed, and link against cublas, cublasLt, cudart (and culibos depending on CUDA version).
    
# Windows

cmake -S . -B build -A x64 ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DBUILD_SHARED_LIBS=OFF ^
  -DLLAMA_CUBLAS=OFF ^
  -DGGML_BLAS=ON ^
  -DGGML_BACKEND_DL=OFF

cmake --build build --config Release
cmake --install build --prefix dist

    •    Produces llama.lib, ggml-base.lib, ggml-cpu.lib, ggml-blas.lib.
    •    Link also against bcrypt.lib and your BLAS implementation (OpenBLAS, MKL, etc.).

py -m pip install -U huggingface_hub
huggingface-cli login
    
# Windows + CUDA

cmake -S . -B build -A x64 ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DBUILD_SHARED_LIBS=OFF ^
  -DLLAMA_CUBLAS=ON ^
  -DGGML_BLAS=ON ^
  -DGGML_BACKEND_DL=OFF

cmake --build build --config Release
cmake --install build --prefix dist

    •    Produces llama.lib, ggml-base.lib, ggml-cpu.lib, ggml-blas.lib, ggml-cuda.lib.
    •    Link also against CUDA libs (cublas.lib, cublasLt.lib, cudart.lib).
    


