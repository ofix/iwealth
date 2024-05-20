### 模板类，声明和定义必须在同一个文件中，一般文件后缀名为 `hpp`

### 类的定义中如果有 union 成员，一定要定义 union 的构造函数和析构函数，否则 union 成员初始化会遇到问题，各种报错

参考链接 (https://stackoverflow.com/questions/19191211/constructor-initialization-of-a-named-union-member)[https://stackoverflow.com/questions/19191211/constructor-initialization-of-a-named-union-member]
