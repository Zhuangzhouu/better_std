# 贡献指南

[← 首页](Home.md)

欢迎新增模块、修 bug、补测试或改进文档。

## 代码风格（必须遵守）

1. **命名空间**：`better_std`。
2. **include guard**：ISO 风格 `BETTER_STD_XXX_HPP`，不用 `#pragma once`。
3. **头文件自包含**：每个头文件 `#include` 它需要的标准头，不依赖包含顺序。
4. **ODR 安全**：自由函数 / 全局变量一律 `inline`。
5. **C++11 基线**：不用 C++17 特性（见 [编译与兼容](Build-Guide.md)）。
6. **`std::` 限定**：调用标准数学函数写 `std::fabs` / `std::max` 等，避免被库内高精度重载遮蔽。
7. **不碰 `namespace std`**：不加 abs/div/gcd 重载（UB）。
8. **求值顺序**：容器递归 `push_back` + 索引写回，先算局部变量再赋值。

## 新增模块流程

以新增 `std_foo.hpp` 为例：

**1. 写头文件**

```cpp
#ifndef BETTER_STD_FOO_HPP
#define BETTER_STD_FOO_HPP
#include<vector>   // 自带所需标准头
namespace better_std{
    // 实现……自由函数用 inline
}
#endif // BETTER_STD_FOO_HPP
```

**2. 接入总入口**：在 `std_better.hpp` 末尾追加一行 `#include"std_foo.hpp"`（纯增量，不动旧行）。

**3. 写临时测试** `_foo.cpp`，与暴力/标准库对拍：

```bash
export PATH="/d/msys2/ucrt64/bin:$PATH"
g++ -std=c++11 -pedantic-errors -Wall -Wextra -static -o _foo.exe _foo.cpp
./_foo.exe
```

**4. 全标准验证**：把对拍加入 `tests/test_all.cpp`，然后：

```bash
cd tests && bash run_tests.sh      # 10 种标准
```

**5. 独立包含检查**：

```bash
printf '#include"std_foo.hpp"\nint main(){return 0;}\n' > _one.cpp
g++ -std=c++11 -pedantic-errors -o _one.exe _one.cpp
```

**6. 清理临时文件**，更新文档（Module-Catalog / API-Reference / README）。

**7. 提交推送**：

```bash
git add -A && git commit -m "feat: add std_foo"
GIT_SSL_NO_VERIFY=true git -c http.schannelCheckRevoke=false push origin main
```

> 最后那条 TLS 参数只在本机 Windows 需要；Linux 直接 `git push` 即可。

## 验证清单

提交前确认：

- [ ] 10 种标准（ISO + GNU）编译 + 对拍全过
- [ ] 头文件独立包含通过
- [ ] 多 TU 链接通过（两个 .cpp 同时 include）
- [ ] `examples/` 全部仍能编译运行
- [ ] CI 绿灯
- [ ] 临时文件已清理（`_*.cpp` 等被 .gitignore 忽略，但也要删）

## 报告问题

发现 bug 请给出：最小复现代码、期望/实际结果、编译器与 `-std` 参数。最好附一个暴力对照（本库的测试哲学就是"对拍出真知"）。

仓库：<https://github.com/Zhuangzhouu/better_std>
