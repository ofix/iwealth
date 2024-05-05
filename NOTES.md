### mingw-x64 版本选择
> 选择 `x86_64-posix-sjlj`，因为 Win32 线程模型不支持 C++ 11 的 `std::thread` 抽象，而 posix 版本支持； seh 异常模型采用windows操作系统内置的异常捕获机制，效率比 sjlj 要高，但存在跨平台不利影响。

### 正确设置c++工程 includePath 路径
```

```