# ISON-C 项目分析报告

## 1. 项目概述

**ISON-C** 是 ISON（Interchange Simple Object Notation）格式的 C 语言实现。ISON 是一种极简、token 高效的数据格式，专为大语言模型（LLM）和 Agentic AI 工作流优化设计。

- **仓库地址**: [Nixy4/ison-c](https://github.com/Nixy4/ison-c)
- **上游规范**: [ISON-format/ison](https://github.com/ISON-format/ison)
- **许可证**: MIT License（Copyright © 2020 指令集）
- **版本**: 1.0.0（ison-c 与 isonantic-c 均为 1.0.0）
- **编程语言**: C (C99 标准)
- **外部依赖**: 无（仅使用 C 标准库 + POSIX regex）

---

## 2. 目录结构

```
ison-c/                         # 仓库根目录
├── .gitignore
├── LICENSE                     # MIT 许可证
├── README.md                   # 项目说明与使用示例
├── ison-c/                     # 核心 ISON 解析/序列化库
│   ├── Makefile                # 构建 libison.a 静态库
│   ├── include/
│   │   └── ison.h              # 公共 API 头文件 (231 行)
│   ├── src/
│   │   ├── parse.c             # ISON/ISONL 解析器 (463 行)
│   │   ├── dump.c              # ISON/ISONL 序列化 (185 行)
│   │   ├── convert.c           # 格式转换 ISON↔ISONL↔JSON (363 行)
│   │   ├── value.c             # 值类型系统 (247 行)
│   │   ├── block.c             # Block (表/对象/元数据) 管理 (127 行)
│   │   ├── document.c          # Document 容器管理 (83 行)
│   │   ├── row.c               # Row 键值对链表 (83 行)
│   │   ├── reference.c         # 引用类型操作 (61 行)
│   │   ├── file.c              # 文件 I/O (93 行)
│   │   └── util.c              # 工具函数 (15 行)
│   └── tests/
│       ├── advanced_tests.c    # 综合测试 (145 行)
│       ├── minimal_suite.c     # 基础功能测试 (101 行)
│       └── parse_test.c        # 解析调试测试 (36 行)
└── isonantic-c/                # Zod 风格的模式验证库
    ├── Makefile                # 构建 libisonantic.a 静态库
    ├── include/
    │   └── isonantic.h         # 验证 API 头文件 (334 行)
    ├── src/
    │   ├── schema.c            # 模式定义与验证逻辑 (734 行)
    │   ├── value.c             # 通用值容器与工具集合 (288 行)
    │   └── complex.c           # 复合类型验证 (Object/Array/Table/Document) (132 行)
    └── tests/
        └── test_runner.c       # 验证库测试 (249 行)
```

**总代码量**: 约 3,970 行（.c + .h 文件）

---

## 3. 架构设计

### 3.1 双库架构

项目由两个独立的静态库组成：

| 库 | 产物 | 功能 |
|---|---|---|
| **ison-c** | `libison.a` | ISON 格式的解析、序列化、格式转换、文件 I/O |
| **isonantic-c** | `libisonantic.a` | 类 Zod 的模式验证，对 ISON 数据进行类型安全校验 |

### 3.2 核心数据模型

```
ison_document_t          # 文档：多个 Block 的容器
  └── ison_block_t[]     # Block：表/对象/元数据，包含字段定义 + 行数据
       ├── ison_field_info_t[]   # 字段定义（名称 + 类型提示）
       ├── ison_row_t[]          # 数据行数组
       │    └── ison_row_entry_t*  # 键值对链表
       │         └── ison_value_t  # 值：tagged union
       └── ison_row_t*           # 可选的汇总行 (summary_row)
```

### 3.3 值类型系统

`ison_value_t` 使用 tagged union 表示 6 种基本类型：

| 类型枚举 | C 存储类型 | ISON 字面量示例 |
|---|---|---|
| `ISON_TYPE_NULL` | （无数据） | `~`, `null` |
| `ISON_TYPE_BOOL` | `bool` | `true`, `false` |
| `ISON_TYPE_INT` | `int64_t` | `42`, `-1` |
| `ISON_TYPE_FLOAT` | `double` | `3.14`, `1e10` |
| `ISON_TYPE_STRING` | `char*` | `Alice`, `"hello world"` |
| `ISON_TYPE_REFERENCE` | `ison_reference_t` | `:1`, `:user:42`, `:OWNS:5` |

### 3.4 引用系统

ISON 的引用（Reference）是一个独特特性，支持三种形式：

- **简单引用**: `:id` — 如 `:1`
- **命名空间引用**: `:namespace:id` — 如 `:user:42`
- **关系引用**: `:RELATIONSHIP:id` — 如 `:OWNS:5`（全大写被识别为关系）

---

## 4. 支持的数据格式

### 4.1 ISON（多行格式）

```
table.users
id:int name:string active:bool
1 Alice true
2 Bob false
```

- Block 以 `kind.name` 开头（如 `table.users`）
- 第二行为字段定义（可带类型提示 `name:type`）
- 后续行为数据行，空格分隔
- 支持 `---` 分隔符引入汇总行
- `#` 开头为注释

### 4.2 ISONL（单行流式格式）

```
table.users|id:int name:string|1 Alice
table.users|id:int name:string|2 Bob
```

每行自包含 `header|fields|data`，适合流式处理。

### 4.3 JSON（互转支持）

- `ison_to_json()`: ISON → JSON
- `ison_from_json()`: JSON → ISON Document

---

## 5. 公共 API 概览

### 5.1 ison-c 核心 API

#### 值构造与访问

```c
// 构造
ison_value_t ison_null(void);
ison_value_t ison_bool(bool value);
ison_value_t ison_int(int64_t value);
ison_value_t ison_float(double value);
ison_value_t ison_string(const char *value);
ison_value_t ison_ref(const ison_reference_t *ref);

// 访问
bool ison_value_as_bool(const ison_value_t *value, bool *out);
bool ison_value_as_int(const ison_value_t *value, int64_t *out);
bool ison_value_as_float(const ison_value_t *value, double *out);
bool ison_value_as_string(const ison_value_t *value, const char **out);
```

#### 文档操作

```c
ison_document_t *ison_document_create(void);
void ison_document_add_block(ison_document_t *doc, ison_block_t *block);
ison_block_t *ison_document_get(const ison_document_t *doc, const char *name);
void ison_document_free(ison_document_t *doc);
```

#### 解析与序列化

```c
ison_document_t *ison_parse(const char *text, ison_error_t *error);
ison_document_t *ison_parse_isonl(const char *text, ison_error_t *error);
char *ison_dumps(const ison_document_t *doc);
char *ison_dumps_isonl(const ison_document_t *doc);
```

#### 格式转换

```c
char *ison_to_json(const char *ison_text, ison_error_t *error);
ison_document_t *ison_from_json(const char *json_text, ison_error_t *error);
char *ison_to_isonl(const char *ison_text, ison_error_t *error);
char *isonl_to_ison(const char *isonl_text, ison_error_t *error);
```

#### 文件 I/O

```c
ison_document_t *ison_load(const char *path, ison_error_t *error);
ison_error_t ison_dump(const ison_document_t *doc, const char *path);
```

#### 流式处理

```c
typedef void (*isonl_callback_t)(const isonl_record_t *record, void *userdata);
ison_error_t isonl_stream_file(const char *path, isonl_callback_t cb, void *userdata);
ison_error_t isonl_stream_buffer(const char *buffer, size_t len, isonl_callback_t cb, void *userdata);
```

### 5.2 isonantic-c 验证 API

提供类似 JavaScript Zod 库风格的链式验证：

```c
// 基本类型 schema
IsonanticSchema* isonantic_string_create(void);
IsonanticSchema* isonantic_number_create(void);
IsonanticSchema* isonantic_int_create(void);
IsonanticSchema* isonantic_boolean_create(void);
IsonanticSchema* isonantic_null_create(void);
IsonanticSchema* isonantic_ref_create(void);

// 链式约束
isonantic_string_min(schema, 5);
isonantic_string_max(schema, 100);
isonantic_string_email(schema);
isonantic_string_url(schema);
isonantic_string_regex(schema, "^[A-Z]");
isonantic_number_min(schema, 0);
isonantic_number_positive(schema);

// 验证
IsonanticValidationErrors* err = schema->validate(schema, value);
```

---

## 6. 构建系统

### 6.1 构建工具

- **GNU Make** + **GCC**
- 编译标准: C99 (`-std=c99`)
- 编译选项: `-Wall -Wextra -O2`
- 产物: 静态库 (`.a`)

### 6.2 构建命令

```bash
# ison-c 核心库
cd ison-c && make clean && make       # → bin/libison.a
cd ison-c && make test                # 运行测试

# isonantic-c 验证库
cd isonantic-c && make clean && make  # → bin/libisonantic.a
cd isonantic-c && make test           # 运行测试

# 系统安装
cd ison-c && sudo make install        # → /usr/local/include/ison.h + /usr/local/lib/libison.a
```

---

## 7. 测试

### 7.1 测试框架

- 无外部测试框架，使用自定义断言宏
- ison-c 使用 C 标准库 `assert()`
- isonantic-c 使用自定义 `ASSERT_TRUE` / `ASSERT_NOT_NULL` 等宏

### 7.2 测试覆盖

| 测试文件 | 覆盖内容 |
|---|---|
| `ison-c/tests/minimal_suite.c` | 版本检查、值类型构造、Document/Block/Row CRUD |
| `ison-c/tests/advanced_tests.c` | 表解析、类型字段、序列化输出、ISONL 解析、JSON 转换、引用解析 |
| `ison-c/tests/parse_test.c` | 基础解析调试 |
| `isonantic-c/tests/test_runner.c` | String/Number/Boolean/Ref schema 验证、optional、min/max、email |

### 7.3 测试入口

- ison-c: `make test` 运行 `advanced_tests.c`（Makefile 中指定）
- isonantic-c: `make test` 运行 `test_runner.c`

---

## 8. 代码质量分析

### 8.1 优点

1. **零外部依赖**: 仅依赖 C 标准库和 POSIX regex，可移植性强
2. **API 设计清晰**: 命名一致（`ison_` 前缀），资源管理语义明确（`create`/`free` 配对）
3. **多格式支持**: ISON / ISONL / JSON 三种格式的互转
4. **内存管理**: 每个 `create` 都有对应的 `free`，堆内存生命周期可控
5. **错误处理**: 统一的 `ison_error_t` 错误码 + `ison_error_string()` 转换
6. **C++ 兼容**: 头文件包含 `extern "C"` 保护

### 8.2 待改进项

1. **`strdup_safe` 重复定义**: 在 `parse.c`、`dump.c`、`document.c`、`block.c`、`reference.c`、`value.c` 中均有重复的 static `strdup_safe` 实现，建议提取为共享的内部工具函数
2. **complex.c 未完成**: `object_validate` 函数中有大量 `/* Simplified */` 注释占位，实际验证逻辑未实现，`object_validate` 函数末尾缺少 `return` 语句
3. **流式 API 未实现**: `isonl_stream_file()` 和 `isonl_stream_buffer()` 在头文件声明但未在源码中找到实现
4. **ISONL dumps 可能有 bug**: `ison_dumps_isonl()` 中字段头部被重复写入了两次（一次在循环外，一次在每行的循环内）
5. **JSON 解析中嵌套对象处理粗糙**: `ison_from_json()` 中遇到嵌套 JSON 对象时将其转为 `ison_ref`（引用），这种映射语义不够直观
6. **无 CI/CD**: 缺少 GitHub Actions 或其他 CI 配置
7. **测试覆盖不足**: 没有针对边界情况（空输入、超大文件、畸形数据）的测试
8. **缺少动态库构建**: 仅支持静态库 `.a`，不支持 `.so` / `.dylib`

---

## 9. 代码统计

| 模块 | 源码行数 | 头文件行数 | 测试行数 |
|---|---|---|---|
| ison-c | 1,712 | 231 | 282 |
| isonantic-c | 1,154 | 334 | 249 |
| **合计** | **2,866** | **565** | **531** |
| **总计** | | | **3,970** |

### 源码文件大小排名（前 5）

1. `isonantic-c/src/schema.c` — 734 行（验证逻辑主体）
2. `ison-c/src/parse.c` — 463 行（ISON/ISONL 解析器）
3. `ison-c/src/convert.c` — 363 行（格式转换与 JSON 处理）
4. `isonantic-c/include/isonantic.h` — 334 行（验证库 API 定义）
5. `isonantic-c/src/value.c` — 288 行（值容器 + Dict + Array + 错误处理）

---

## 10. 总结

ISON-C 是一个**轻量级、零依赖的 C 库**，用于处理面向 LLM 优化的 ISON 数据格式。项目由两个子库组成：

- **ison-c** 提供完整的解析/序列化/格式转换能力
- **isonantic-c** 提供类 Zod 的模式验证能力

项目整体代码质量良好，API 设计规范，适合嵌入式或性能敏感的 AI 工作流场景。主要改进方向包括：完善 isonantic-c 中的复合类型验证逻辑、消除代码重复、增加 CI/CD 配置和更全面的测试覆盖。
