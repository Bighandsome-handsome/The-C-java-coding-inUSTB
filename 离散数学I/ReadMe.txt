离散数学I期末作业要求：（占期末成绩的10%）
注意ATTENTION：源代码上交之后会有查重，查重较高可能会判0分，影响期末成绩。
解析一阶谓词公式的项
1. 项的层数定义
定义NL中项t的层数level(t)如下：
(1)当 t 为个体符号（个体变元或个体常元）时，level(t)=0；
(2)当 t=f(t1,···,tn)时（其中 f 为 n 元函数，n∈N 且 n≥1），level(t)=max(level(t1),···,level(tn))+1。
2. 程序功能
读入项的表达式（不含空格）。
首先输出项的层数，再按照层数由小到大输出表达式中的项，每层的项按照在表达式中的顺序输出
（如同一项在表达式中多次出现，则要多次输出），中间用英文分号分开，每层输出完毕输出回车符。
不同的项（函数、个体常元、个体变元）可能使用相同的标识符。
3. 样例
输入：f(f(g(a,b),x,h(c)),a(j,c),h(a))
输出：
3
a;b;x;c;j;c;a
g(a,b);h(c);a(j,c);h(a)
f(g(a,b),x,h(c))
f(f(g(a,b),x,h(c)),a(j,c),h(a))