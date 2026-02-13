# ISON-C 命名规范分析与优化建议

## 1. 概述

本文档对 ISON-C 项目（含 `ison-c` 核心库与 `isonantic-c` 验证库）的命名规范进行全面分析，覆盖文件名、类型名、函数名、变量名、宏/常量名五个维度，识别现有规范的优点与不一致之处，并提出优化建议。

---

## 2. 现有命名规范总结

### 2.1 文件命名

| 分类 | 规范 | 示例 |
|---|---|---|
| 源文件 | 小写单词，`.c` 后缀 | `value.c`, `parse.c`, `dump.c`, `convert.c`, `block.c` |
| 头文件 | 小写单词，`.h` 后缀 | `ison.h`, `isonantic.h` |
| 测试文件 | `snake_case`，`.c` 后缀 | `advanced_tests.c`, `minimal_suite.c`, `parse_test.c`, `test_runner.c` |

**评价**: 源文件和头文件命名简洁、规范。测试文件命名风格基本一致，但命名模式不统一（`xxx_tests.c` vs `xxx_test.c` vs `test_xxx.c`）。

### 2.2 类型命名

#### ison-c（核心库）

采用 **C 标准库风格** `snake_case_t`，搭配 `ison_` 前缀：

| 类别 | 规范 | 示例 |
|---|---|---|
| 枚举类型 | `ison_<name>_t` | `ison_error_t`, `ison_type_t` |
| 枚举值 | `ISON_<CATEGORY>_<NAME>` | `ISON_OK`, `ISON_ERROR_MEMORY`, `ISON_TYPE_NULL` |
| 结构体类型 | `ison_<name>_t` | `ison_value_t`, `ison_block_t`, `ison_document_t` |
| 回调类型 | `isonl_callback_t` | — |
| 内部类型 | `<name>_t`（无前缀） | `parser_t`（`parse.c` 文件内部） |

#### isonantic-c（验证库）

采用 **PascalCase 风格**，搭配 `Isonantic` 前缀：

| 类别 | 规范 | 示例 |
|---|---|---|
| 枚举类型 | `IsonanticValueType` | — |
| 枚举值 | `ISONANTIC_VALUE_<NAME>` | `ISONANTIC_VALUE_NULL`, `ISONANTIC_VALUE_STRING` |
| 结构体类型 | `Isonantic<Name>` | `IsonanticSchema`, `IsonanticValue`, `IsonanticDict` |
| 回调类型 | `Isonantic<Name>Fn` | `IsonanticRefinementFn` |

**⚠️ 不一致**: 两个子库使用了完全不同的类型命名风格。`ison-c` 使用 C 惯用的 `snake_case_t`，`isonantic-c` 使用 PascalCase（更接近 C++/Java 风格）。

### 2.3 函数命名

#### ison-c 公共 API

遵循 `ison_<模块>_<动作>` 模式：

| 模式 | 示例 |
|---|---|
| 值构造器 | `ison_null()`, `ison_bool()`, `ison_int()`, `ison_float()`, `ison_string()`, `ison_ref()` |
| 值访问器 | `ison_value_as_bool()`, `ison_value_as_int()`, `ison_value_as_float()` |
| 值操作 | `ison_value_to_ison()`, `ison_value_to_json()`, `ison_value_free()` |
| 引用操作 | `ison_reference_make()`, `ison_reference_to_ison()`, `ison_reference_free()` |
| 行操作 | `ison_row_create()`, `ison_row_set()`, `ison_row_get()`, `ison_row_free()` |
| 块操作 | `ison_block_create()`, `ison_block_add_field()`, `ison_block_add_row()`, `ison_block_free()` |
| 文档操作 | `ison_document_create()`, `ison_document_add_block()`, `ison_document_get()`, `ison_document_free()` |
| 解析 | `ison_parse()`, `ison_parse_isonl()` |
| 序列化 | `ison_dumps()`, `ison_dump()`, `ison_dumps_isonl()`, `ison_dump_isonl()` |
| 格式转换 | `ison_to_json()`, `ison_to_isonl()`, `ison_from_json()` |
| 文件 I/O | `ison_load()`, `ison_dump()`, `ison_read_file()`, `ison_write_file()` |

**例外**: 以下函数使用 `isonl_` 前缀而非 `ison_` 前缀：

- `isonl_to_ison()` — 与 `ison_to_isonl()` 形成不对称
- `isonl_stream_file()`
- `isonl_stream_buffer()`
- `isonl_callback_t`

#### isonantic-c 公共 API

遵循 `isonantic_<模块>_<动作>` 模式：

| 模式 | 示例 |
|---|---|
| Schema 创建 | `isonantic_string_create()`, `isonantic_number_create()`, `isonantic_boolean_create()` |
| Schema 约束 | `isonantic_string_min()`, `isonantic_number_positive()`, `isonantic_string_email()` |
| 值创建 | `isonantic_value_create_string()`, `isonantic_value_create_number()` |
| Dict 操作 | `isonantic_dict_create()`, `isonantic_dict_set()`, `isonantic_dict_get()` |
| Array 操作 | `isonantic_array_create()`, `isonantic_array_add()`, `isonantic_array_get()` |
| 错误处理 | `isonantic_validation_errors_create()`, `isonantic_validation_errors_add()` |

**特殊**: `I` 命名空间使用 PascalCase 方法名（`I.String()`, `I.Number()`, `I.Boolean()`），与 C 标准风格不一致，但作为 DSL 语法糖，此设计具有合理性。

#### 内部静态函数

两个库的内部函数均使用 `snake_case`：

- ison-c: `strdup_safe()`, `trim_right()`, `split_lines()`, `tokenize()`, `parse_field_def()`, `parse_block()`, `append_string()`, `append_char()`, `skip_ws()`, `parse_json_string()`, `parse_json_value()`, `parse_json_object()`
- isonantic-c: `string_validate()`, `number_validate()`, `boolean_validate()`, `string_free_schema()`, `dict_hash()`, `dict_entry_free()`, `validation_error_free()`, `is_valid_email()`, `is_valid_url()`

### 2.4 变量命名

| 分类 | 规范 | 示例 |
|---|---|---|
| 局部变量 | `snake_case` | `field_count`, `line_count`, `cur_len`, `in_quotes`, `token_count` |
| 简写局部变量 | 单字母（上下文明确时） | `v`, `p`, `s`, `ch`, `i`, `j`, `r` |
| 结构体字段（ison-c） | `snake_case` | `bool_val`, `int_val`, `float_val`, `string_val`, `field_count`, `row_count`, `summary_row` |
| 结构体字段（isonantic-c） | `snake_case` | `min_len`, `max_len`, `is_email`, `is_url`, `min_val`, `max_val`, `is_int`, `is_positive`, `string_value`, `number_value` |
| 函数参数 | `snake_case` | `type_hint`, `out_len`, `user_data`, `error_message` |

**评价**: 变量命名在两个库中保持一致，均使用 `snake_case`，这是好的实践。

### 2.5 宏 / 常量命名

| 分类 | 规范 | 示例 |
|---|---|---|
| 版本宏 | `<PREFIX>_VERSION` | `ISON_VERSION`, `ISONANTIC_VERSION` |
| 头文件守卫 | `<NAME>_H` | `ISON_H`, `ISONANTIC_H` |
| 测试断言宏 | `ASSERT_<NAME>` | `ASSERT_TRUE`, `ASSERT_FALSE`, `ASSERT_NOT_NULL`, `ASSERT_CONTAINS` |

**评价**: 宏和常量命名统一使用 `UPPER_SNAKE_CASE`，符合 C 语言惯例。

### 2.6 资源管理语义

| 动作 | 命名规范 | 示例 |
|---|---|---|
| 创建/分配 | `_create` | `ison_row_create()`, `ison_document_create()` |
| 释放 | `_free` | `ison_row_free()`, `ison_document_free()` |
| 值构造（栈上） | `ison_<type>()` | `ison_null()`, `ison_int(42)` |
| 值构造（堆上） | `_create_<type>` | `isonantic_value_create_string()` |
| 序列化为字符串 | `_to_<format>` | `ison_value_to_json()`, `ison_reference_to_ison()` |
| 从外部构造 | `_make` | `ison_reference_make()` |
| 序列化到内存 | `_dumps` | `ison_dumps()` |
| 序列化到文件 | `_dump` | `ison_dump()` |
| 从文件加载 | `_load` | `ison_load()` |

**评价**: `create`/`free` 配对语义清晰。`_make` 与 `_create` 的区别体现了栈返回和堆分配的不同语义。`dumps`/`dump` 的命名借鉴了 Python json 库风格，在 C 项目中略显非主流。

---

## 3. 关键问题分析

### 3.1 两个子库类型命名风格不一致

**问题**: `ison-c` 使用 `snake_case_t`（如 `ison_value_t`），`isonantic-c` 使用 PascalCase（如 `IsonanticValue`）。两者在同一个仓库中共存，给维护者和使用者带来认知负担。

**影响等级**: ⭐⭐⭐ 高

**示例对比**:
```c
// ison-c 风格
ison_value_t v = ison_int(42);
ison_document_t *doc = ison_document_create();

// isonantic-c 风格
IsonanticSchema* schema = isonantic_string_create();
IsonanticValue* val = isonantic_value_create_string("hello");
```

### 3.2 `isonl_` 前缀与 `ison_` 前缀不统一

**问题**: 大多数函数使用 `ison_` 前缀，但部分 ISONL 相关函数使用 `isonl_` 前缀，导致命名空间不统一。

**影响等级**: ⭐⭐ 中

**涉及标识符**:
- `isonl_to_ison()` — 与之对称的 `ison_to_isonl()` 却使用 `ison_` 前缀
- `isonl_stream_file()`
- `isonl_stream_buffer()`
- `isonl_callback_t`
- `isonl_record_t`

### 3.3 `strdup_safe` 等内部函数重复定义

**问题**: `strdup_safe()` 在 6 个文件中以 `static` 重复定义（`value.c`、`parse.c`、`dump.c`、`block.c`、`document.c`、`reference.c`）。`append_string()` 和 `append_char()` 在 `dump.c` 和 `convert.c` 中重复定义。

**影响等级**: ⭐⭐ 中

虽然 `static` 限制了作用域，但违反了 DRY 原则，增加了维护成本。

### 3.4 部分函数命名语义不够精确

**问题**:

| 函数 | 问题描述 |
|---|---|
| `ison_reference_get_ns()` | 函数名暗示返回 namespace，但实际也可能返回 relationship 字段，语义模糊 |
| `ison_error_string()` | 不符合 `ison_<模块>_<动作>` 的一般模式，建议改为 `ison_error_to_string()` |
| `ison_dumps()` | Python 风格的 `dumps` 在 C 中不常见，可能造成困惑 |
| `ison_string_n()` | `_n` 后缀含义不够直观，可以考虑 `ison_string_with_len()` |

**影响等级**: ⭐ 低

### 3.5 测试文件命名模式不统一

**问题**: 测试文件命名存在多种模式：

- `advanced_tests.c`  — `<描述>_tests.c`
- `minimal_suite.c`   — `<描述>_suite.c`
- `parse_test.c`      — `<描述>_test.c`
- `test_runner.c`     — `test_<描述>.c`

**影响等级**: ⭐ 低

### 3.6 `int` 与 `size_t` 的不一致使用

**问题**: `ison-c` 在表示计数/大小时一致使用 `size_t`（如 `field_count`、`row_count`），而 `isonantic-c` 在类似场景中使用 `int`（如 `IsonanticDict.capacity`、`IsonanticArray.size`、函数参数 `int capacity`、`int index`）。

**影响等级**: ⭐⭐ 中

### 3.7 NULL 检查风格不统一

**问题**: `ison-c` 倾向使用隐式布尔转换 `!ptr`，而 `isonantic-c` 倾向使用显式比较 `ptr == NULL`。

**影响等级**: ⭐ 低（不影响正确性，但影响风格一致性）

```c
// ison-c 风格
if (!value) return false;
if (!str) return NULL;

// isonantic-c 风格
if (schema == NULL) return;
if (value == NULL) return NULL;
```

---

## 4. 优化建议

### 4.1 统一类型命名风格（优先级: 高）

**建议**: 统一为 `snake_case_t` 风格，与 C 标准库和 ison-c 保持一致。

如果希望保留 isonantic-c 的 PascalCase 风格（出于与 Zod/JavaScript 生态的对齐考虑），则至少在文档中明确声明这一设计决策，并添加 typedef 别名：

```c
/* 可选方案: 保留 PascalCase 但提供 snake_case 别名 */
typedef IsonanticSchema isonantic_schema_t;
typedef IsonanticValue  isonantic_value_t;
```

**推荐统一方案**:
```c
/* 统一为 snake_case_t */
typedef struct isonantic_schema isonantic_schema_t;
typedef struct isonantic_value  isonantic_value_t;
typedef struct isonantic_dict   isonantic_dict_t;
typedef enum isonantic_value_type isonantic_value_type_t;
```

### 4.2 统一 ISONL 相关函数前缀（优先级: 中）

**建议**: 将所有 `isonl_` 前缀改为 `ison_isonl_`，保持前缀命名空间的一致性。

```c
/* 当前 */
char *isonl_to_ison(const char *isonl_text, ison_error_t *error);
ison_error_t isonl_stream_file(const char *path, isonl_callback_t cb, void *userdata);

/* 建议 */
char *ison_isonl_to_ison(const char *isonl_text, ison_error_t *error);
ison_error_t ison_isonl_stream_file(const char *path, ison_isonl_callback_t cb, void *userdata);
```

或者，如果认为 `isonl_` 作为独立命名空间是有意为之，应在头文件和文档中明确说明此设计决策。

### 4.3 提取内部公共工具函数（优先级: 中）

**建议**: 在内部头文件 `ison_internal.h`（不作为公共 API 暴露）中声明共享的工具函数，消除重复：

```c
/* ison_internal.h */
#ifndef ISON_INTERNAL_H
#define ISON_INTERNAL_H

#include <stddef.h>

char *ison__strdup_safe(const char *str);
void  ison__append_string(char **buf, size_t *len, size_t *cap, const char *str);
void  ison__append_char(char **buf, size_t *len, size_t *cap, char ch);

#endif
```

使用 `ison__`（双下划线）前缀表示内部 API。在 `util.c` 中实现这些函数，并从其他文件中删除重复的 `static` 定义。

### 4.4 精确化函数命名（优先级: 低）

| 当前名称 | 建议名称 | 理由 |
|---|---|---|
| `ison_reference_get_ns()` | `ison_reference_get_namespace()` 或拆分为 `ison_reference_get_ns()` + `ison_reference_get_relationship()` 分别返回 | 消除语义歧义（当前函数在 relationship 存在时返回 relationship） |
| `ison_error_string()` | `ison_error_to_string()` | 与 `ison_value_to_ison()`、`ison_value_to_json()` 的 `_to_` 模式一致 |
| `ison_string_n()` | `ison_string_with_length()` 或 `ison_stringn()` | C 标准库中 `strncpy` 等函数使用 `n` 后缀是约定俗成，但 `_n` 中间的下划线使其语义不如 `strncpy` 风格直观 |

> 注：`ison_dumps()` 虽然源自 Python 风格，但已成为此 API 的一部分，修改成本较高，建议保留但在文档中注明其含义。

### 4.5 统一测试文件命名模式（优先级: 低）

**建议**: 统一为 `test_<模块>.c` 格式：

| 当前 | 建议 |
|---|---|
| `advanced_tests.c` | `test_advanced.c` |
| `minimal_suite.c` | `test_minimal.c` |
| `parse_test.c` | `test_parse.c` |
| `test_runner.c` | `test_runner.c`（已符合） |

### 4.6 统一计数类型为 `size_t`（优先级: 中）

**建议**: `isonantic-c` 中的 `int capacity`、`int size`、`int index` 等表示大小和索引的字段应统一为 `size_t`，与 `ison-c` 保持一致，并避免潜在的有符号/无符号问题：

```c
/* 当前 */
struct IsonanticDict {
    struct IsonanticDictEntry** buckets;
    int capacity;
    int size;
};

/* 建议 */
struct IsonanticDict {
    struct IsonanticDictEntry** buckets;
    size_t capacity;
    size_t size;
};
```

### 4.7 统一 NULL 检查风格（优先级: 低）

**建议**: 选择一种风格并在全项目中统一。两种风格各有优劣：

- `!ptr`：更简洁，C 惯用风格
- `ptr == NULL`：更显式，易于阅读

建议在项目中统一采用显式比较 `ptr == NULL` / `ptr != NULL` 风格，因为：
1. 更清晰地表达意图
2. 减少误将赋值 `=` 写成比较 `==` 的风险
3. 对 C 语言新手更友好

如果偏好简洁风格，则统一使用 `!ptr`。关键是项目内保持一致。

---

## 5. 命名规范推荐总结

基于以上分析，建议本项目采用以下统一的命名规范：

| 类别 | 规范 | 示例 |
|---|---|---|
| **文件名** | 小写 `snake_case` | `value.c`, `parse.c`, `test_parse.c` |
| **公共类型名** | `snake_case_t` 带前缀 | `ison_value_t`, `isonantic_schema_t` |
| **枚举类型名** | `snake_case_t` 带前缀 | `ison_type_t`, `isonantic_value_type_t` |
| **枚举值** | `UPPER_SNAKE_CASE` 带前缀 | `ISON_TYPE_NULL`, `ISONANTIC_VALUE_STRING` |
| **公共函数名** | `<prefix>_<module>_<verb>` | `ison_document_create()`, `isonantic_string_min()` |
| **内部函数名** | `snake_case` 或 `<prefix>__<name>` | `parse_block()`, `ison__strdup_safe()` |
| **局部变量名** | `snake_case` | `field_count`, `line_count` |
| **结构体字段名** | `snake_case` | `bool_val`, `field_count`, `summary_row` |
| **宏 / 常量** | `UPPER_SNAKE_CASE` | `ISON_VERSION`, `ASSERT_TRUE` |
| **回调类型名** | `snake_case_t` 带前缀 | `ison_isonl_callback_t` |
| **头文件守卫** | `<NAME>_H` | `ISON_H`, `ISONANTIC_H` |

---

## 6. 优化路线图

按优先级排列的实施建议：

### Phase 1 — 高优先级（不破坏 API）

1. **提取内部公共工具函数**: 创建 `ison_internal.h`，消除 `strdup_safe` / `append_string` / `append_char` 的重复定义
2. **文档化命名决策**: 在 README 或 CONTRIBUTING.md 中明确声明两个子库的命名风格差异（如果有意为之）

### Phase 2 — 中优先级（Minor API 变更）

3. **统一 `isonl_` 前缀**: 通过 deprecated 宏逐步迁移为 `ison_isonl_` 前缀
4. **统一计数类型**: 将 isonantic-c 中的 `int` 计数字段改为 `size_t`
5. **修正 `ison_reference_get_ns()`**: 拆分为明确语义的两个函数

### Phase 3 — 低优先级（Major API 变更，建议在 2.0 版本）

6. **统一 isonantic-c 类型命名**: 将 PascalCase 改为 `snake_case_t`，或提供 typedef 别名
7. **统一 NULL 检查风格**: 全项目统一
8. **统一测试文件命名**: 采用 `test_<module>.c` 模式

---

## 7. 现有规范优点

在分析不一致之处的同时，也应当认可项目中已有的良好命名实践：

1. **前缀命名空间隔离**: `ison_` 和 `isonantic_` 前缀有效避免了符号冲突
2. **`create`/`free` 配对语义**: 堆分配资源的生命周期管理语义明确
3. **一致的枚举值命名**: 所有枚举值均采用 `UPPER_SNAKE_CASE`，前缀统一
4. **函数参数的输出标记**: 使用 `out` 或 `out_` 前缀标识输出参数（如 `out_len`、`*out`）
5. **C++ 兼容**: 头文件包含 `extern "C"` 守卫
6. **值类型名与类型枚举对应**: `ison_value_t.type` 的枚举值 `ISON_TYPE_INT` 与值构造器 `ison_int()` 形成清晰对应关系
7. **链式 API 返回 self**: isonantic-c 的约束函数返回 `IsonanticSchema*`，支持链式调用

---

## 8. 结论

ISON-C 项目整体命名质量较好，`ison-c` 核心库的命名高度一致，遵循 C 语言最佳实践。主要改进机会集中在：

- **两个子库之间的风格统一**（`snake_case_t` vs PascalCase）
- **`isonl_` 前缀命名空间统一**
- **内部工具函数的去重**

建议按照路线图分阶段实施优化，优先处理不破坏 API 的改进项（Phase 1），在后续版本中逐步统一类型命名风格。
