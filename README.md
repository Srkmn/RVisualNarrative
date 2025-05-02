# RVisualNarrative
[![license](https://img.shields.io/badge/license-MIT-blue)](https://github.com/Srkmn/RVisualNarrative/blob/main/LICENSE)
[![QQ群](https://img.shields.io/badge/QQ群-1039586349-800020?logo=tencentqq&style=flat-square)](https://qm.qq.com/q/ATfliiSGxG)
[![GitHub release](https://img.shields.io/github/v/release/Srkmn/RVisualNarrative?color=228B22&style=flat-square)](https://github.com/Srkmn/RVisualNarrative/releases)  

## 简介

RVisualNarrative 是一款为虚幻引擎(Unreal Engine)开发的跨版本对话状态机编辑器插件，旨在提供可视化、灵活且高效的剧情对话编辑或者状态机解决方案。

## 支持版本

- 支持 Unreal Engine 5.0 到 5.5 版本

## 特性

### 多模式编辑

- **编辑器模式**：支持复制、粘贴、删除，以及完整的 Redo 和 Undo 功能
- **蓝图模式**：无缝集成虚幻引擎蓝图系统

### 数据驱动

- 与行为树一致的黑板上下文
- 黑板装饰器，控制Task的执行

### 逻辑组装

- 自由拖拽 Condition 到任意 StateNode 和 TaskNode
- 状态内置状态机，可以拖动Task实现需求
- 支持同步和异步任务
- 灵活的逻辑编排机制

### 组件复用

- Decorator 可以灵活放置于对话组件
- 响应资产变更（增删改查）
- 支持资产状态实时监听

### 快速创建

- 支持通过蓝图创建 Decorator
- 提供友好的创建窗口
- C++ 创建支持
- 性能优化，确保高效调用

### 性能优化

- 使用深度优先搜索(DFS)算法对节点重排序
- 对话组件蓝图提供数据压缩功能
- 黑板数据密集，缓存命中率高

## 安装

1. 克隆仓库到 Unreal Engine 项目的 Plugins 目录
2. 重新生成项目文件
3. 编译项目

## 文档

- [开始上手](https://zhuanlan.zhihu.com/p/1888638258621163028)

## 社区

- [官方QQ群：1039586349](https://qm.qq.com/q/ATfliiSGxG)

## Contributors

**<a href="https://github.com/Srkmn/RVisualNarrative/graphs/contributors">**
**<img src="https://contrib.rocks/image?repo=Srkmn/RVisualNarrative"/>**
**</a>**

---

**RVisualNarrative** - 让对话编辑变得简单而高效！


## 放一个吉祥物在这里，不要对他做坏事
![Logo](Resources/Mascot.png)
