#ifndef TINY_H
#define TINY_H
#include <bits/stdc++.h>
#include <QString>
using namespace std;

struct TreeNode
{//定义语法树节点
    QString type;//当前节点的类型
    QString data;//当前节点的值
    TreeNode* child[8];//孩子节点
    TreeNode* sibling[8];//兄弟节点
    int childNum = 0;
    int siblingNum = 0;
};

class Tiny
{
public:
    void init();
    Tiny();
    void setString(QString tempString);//设置待处理的程序字符串
    QString getToken();//获取下一个单词
    void match(QString matchString, QString tempString);
    void program();
    TreeNode* re();
    TreeNode* re2();
    TreeNode* re1();
    TreeNode* regular_expression();
    TreeNode* factor();
    TreeNode* term1();
    TreeNode* mulop();
    TreeNode* term();
    TreeNode* addop();
    TreeNode* simple_exp();
    TreeNode* comparsion_op();
    TreeNode* exp();
    TreeNode* write_stmt();
    TreeNode* read_stmt();
    TreeNode* assign_op();
    TreeNode* assign_stmt_or_ID_stmt();
    TreeNode* repeat_stmt();
    TreeNode* exp2();
    TreeNode* exp1();
    TreeNode* if_stmt();
    TreeNode* statement();
    TreeNode* stmt_sequence();
    QString PrintTree(TreeNode *t, int deep);//打印语法树
    /********待分析的字符串******/
    QString myString;
    QString errorString;
    TreeNode* root;
    int index;//正在判断的字符下标
    QString token;
    /***存储一些用于分割单词的符号***/
    set<QString>sign;
};

#endif // TINY_H
