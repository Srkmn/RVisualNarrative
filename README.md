# RVisualNarrative

## 简介

RVisualNarrative 是一款为虚幻引擎(Unreal Engine)开发的跨版本对话编辑器插件，旨在提供可视化、灵活且高效的剧情对话编辑解决方案。

## 支持版本  

- 支持 Unreal Engine 5.0 到 5.5 版本

## 特性

### 多模式编辑

- **编辑器模式**：支持复制、粘贴、删除，以及完整的 Redo 和 Undo 功能
- **蓝图模式**：无缝集成虚幻引擎蓝图系统

### 逻辑组装

- 自由拖拽 Condition 和 Task 到任意 StateNode
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

### 多人协作

- 支持多用户同时编辑不同 Decorator
- 互不干扰的编辑体验

### 性能优化

- 使用深度优先搜索(DFS)算法对节点重排序
- 对话组件蓝图提供数据压缩功能
- 增量属性更新，仅同步变更数据

## 安装

1. 克隆仓库到 Unreal Engine 项目的 Plugins 目录
2. 重新生成项目文件
3. 编译项目

## 使用示例

TODO


## 许可证

本项目采用 MIT 许可证，详情请参见 LICENSE 文件。

## 贡献

欢迎提交 Issues 和 Pull Requests！

## 联系

如有任何问题，请提交 GitHub Issues。

---

**RVisualNarrative** - 让对话编辑变得简单而高效！


