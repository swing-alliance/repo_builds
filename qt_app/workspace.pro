TEMPLATE = subdirs

# 启用有序构建，确保按照依赖顺序编译
CONFIG += ordered

# 将你的三个子项目文件夹名称填入 SUBDIRS
SUBDIRS += \
    app_time \
    fund_card \
    entry

# 如果项目之间有依赖关系（例如 entry 依赖前面两者），可以显式声明：
# entry.depends = app_time fund_card