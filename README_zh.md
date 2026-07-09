# 随机数生成器 (RNG)

一个基于终端界面的 C 语言随机数生成器。
零外部依赖，仅使用 ANSI 转义序列和 C 标准库。

![](C:\Users\gfd\Documents\GitHub\Random_Number_Generator\Images\screenshot%202026-07-09%20120825.png)

## 功能

- 三栏 TUI 布局：控制面板 | 分布图 | 结果列表
- 均匀分布：在 [min, max] 范围内生成随机整数
- 正态分布：Box-Muller 变换，可配置均值和标准差
- 伯努利分布：按概率 p 生成 0/1 结果
- 实时 ASCII 概率密度/质量函数图形
- CSV 导出，文件名带时间戳
- 可指定随机种子，结果可复现

## 编译

```bash
gcc -Wall -Wextra -std=c99 -pedantic -Isrc -o rng-tui \
    src/tui.c src/term.c src/controls.c src/random.c \
    src/stats.c src/output.c src/graph/graph.c -lm
```

> 安装了 make 的话也可以直接 `make`（使用项目自带的 Makefile）。

## 使用

```bash
./rng-tui
```

### 按键说明

| 按键          | 功能        |
| ----------- | --------- |
| `G`         | 生成随机数     |
| `E`         | 导出结果为 CSV |
| `U`         | 切换到均匀分布   |
| `N`         | 切换到正态分布   |
| `B`         | 切换到伯努利分布  |
| `Tab`       | 切换输入框焦点   |
| `↑` `↓`     | 滚动结果列表    |
| `←` `→`     | 在输入框中移动光标 |
| `Backspace` | 删除输入框中的字符 |
| `Q` / `Esc` | 退出        |

## 项目结构

```
src/
├── tui.c/h           主事件循环、画面渲染、结果面板
├── term.c/h          终端控制 (ANSI) + 键盘输入
├── controls.c/h      左栏：参数字段、字段编辑
├── graph/graph.c/h   ASCII 图形绘制 (均匀/正态/伯努利)
├── random.c/h        随机数生成 (基于 rand)
├── stats.c/h         统计计算 (最小值、最大值、均值、标准差)
└── output.c/h        输出到控制台 / CSV 文件
```

## 协议

MIT
