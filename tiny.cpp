#include "tiny.h"
#include <QString>
#include <QDebug>
#include <QCoreApplication>

Tiny::Tiny()
{//构造函数
    init();
}

void Tiny::init()
{//初始化
    this->root = NULL;
    this->myString = "";
    this->errorString = "";
    this->index = 0;
    this->token = "";
    sign.insert("(");
    sign.insert(")");
    sign.insert(";");
    sign.insert("<");
    sign.insert(">");
    sign.insert("+");
    sign.insert("-");
    sign.insert("@");
    sign.insert("*");
    sign.insert("/");
    sign.insert("%");
    sign.insert(":");
    sign.insert("=");
    sign.insert("^");
    sign.insert("|");
    sign.insert("&");
    sign.insert("#");
}

void Tiny::setString(QString tempString)
{//设置待处理的程序字符串
    QString ts = "";
    //去掉注释
    bool flag = false;
    for(int i=0;i<int(tempString.length());i++)
    {
        if(tempString[i]=='{')
        {//如果是注释
            flag = true;
        }
        if(flag==false)
        {
            ts += tempString[i];
        }

        if(tempString[i]=="}")
        {
            flag = false;
        }


    }

    if(flag == true)
    {
        errorString = "input error" ;
        errorString += "/n";  //输入的语法有误
        return;
    }
    //去掉多余的空格
    ts.replace(QRegExp("[\\s]+"), " ");
    //将空格替换成@，便于后续处理
    ts.replace(QRegExp("\\s"), "@");
    this->myString = ts;
}

QString Tiny::getToken()
{//获取下一个单词，适用于源程序中以空格为分隔符的单词
    QString tempS = "";
    //以@为边界
    if(myString[index]=="@")//前面是@
    {
        index++;
    }
    while (index<int(myString.length())&&sign.find(QString(myString[index]))==sign.end())
    {
        tempS += myString[index];
        index++;
    }
    return tempS;
}

void Tiny::match(QString matchString, QString tempString)
{//匹配函数
    if(matchString!=tempString)
    {
        errorString += "error: unexpected token: ";
        errorString += tempString;
        errorString += "/n";
    }
    return;
}

//program
void Tiny::program()
{

    if(errorString!="")
    {
        return;
    }

    else
    {

        this->root = stmt_sequence();

    }
}

//stmt-sequence
TreeNode* Tiny:: stmt_sequence()
{


    TreeNode* thisNode =  new TreeNode;
    thisNode=statement();


    while(myString[index] == ";")
    {

        index++;
        thisNode->sibling[thisNode->siblingNum]=statement();
        thisNode->siblingNum ++;
        if(myString[index]=="@")
        {
            index++;
        }
    }

    return thisNode;

}


//statement
TreeNode* Tiny:: statement()
{

    TreeNode* thisNode = new TreeNode;

    token = getToken();


    if(token == "if")
    {
        match("if",token);
        thisNode = if_stmt();
        thisNode->data = "If";
        thisNode->type = "If";

    }
    else if(token=="repeat")
    {
        match("repeat",token);
        thisNode = repeat_stmt();
        thisNode->data = "Repeat";
        thisNode->type = "Repeat";

    }
    else if(token == "read")
    {

        match("read",token);
        thisNode = read_stmt();

        thisNode->type = "Read";
    }
    else if(token == "write")
    {
        match("write",token);
        thisNode = write_stmt();

        thisNode->type="Write";
    }
    else
    {
        QString temp = token;


        thisNode = assign_stmt_or_ID_stmt();
        thisNode->data = temp;


    }

    return thisNode;
}


//if_stmt
TreeNode* Tiny::if_stmt()
{

    TreeNode* thisNode = new TreeNode;
    thisNode->data="If";
    thisNode->type = "If";
    /***匹配(***/
    if(myString[index]=="@")
    {
        index++;
    }
    token= myString[index];
    index++;
    match("(",token);//匹配(
   thisNode->child[thisNode->childNum] = this->exp1();

   thisNode->childNum++;
   /***匹配)***/
   if(myString[index]=="@")
   {
       index++;
   }
   token= myString[index];
   index++;
   match(")",token);//匹配)
   thisNode->child[thisNode->childNum] = stmt_sequence();
   thisNode->childNum++;
   if(myString[index]=="@")
   {
       index++;
   }
   if(myString[index]=="e")
   {

       token = "";
       while(myString[index]!="@")
       {
           token += myString[index];
           index++;
       }
       index++;//@后面的字母
       match("else",token);
       thisNode->child[thisNode->childNum] = stmt_sequence();
       thisNode->childNum++;

   }
   return thisNode;
}


//exp1
TreeNode* Tiny::exp1()
{
    TreeNode* thisNode = exp2();
    if(myString[index]=="@")
    {
        index++;
    }
    while(myString[index]=="o")
    {
        token = getToken();
        match("or",token);
        TreeNode* t=new TreeNode;
        t->data = "Or";
        t->type = "OP";
        t->child[t->childNum]=thisNode;
        t->childNum++;
        t->child[t->childNum]=exp1();
        t->childNum++;
        thisNode = t;
    }
    return thisNode;
}

//exp2
TreeNode* Tiny::exp2()
{
    TreeNode* thisNode = exp();
    if(myString[index]=="@")
    {
        index++;
    }
    while(myString[index]=="a")
    {
        token = getToken();
        match("and",token);
        TreeNode* t=new TreeNode;
        t->data = "And";
        t->type = "OP";
        t->child[t->childNum]=thisNode;
        t->childNum++;
        t->child[t->childNum]=exp();
        t->childNum++;
        thisNode = t;
    }
    return thisNode;
}

//repeat_stmt
TreeNode* Tiny::repeat_stmt()
{
    /***stmt-sequence***/
    TreeNode* thisNode =new TreeNode;
    thisNode->data = "Repeat";
    thisNode->type = "Repeat";
    thisNode->child[thisNode->childNum] = stmt_sequence();
    thisNode->childNum++;
    /***匹配until***/
    token = getToken();
    match("until",token);
    /***exp1***/
    thisNode->child[thisNode->childNum] = exp1();
    thisNode->childNum++;
    return thisNode;
}

//assign_stmt or ID_stmt
TreeNode* Tiny::assign_stmt_or_ID_stmt()
{
    TreeNode* thisNode = new TreeNode;
    //判断后面的符号是-=还是=还是:=
    if(myString[index]=="@")
    {
        index++;
    }
    //是=或者-=
    if(myString[index]=="="||myString[index]=="-")
    {
          token="";
          if(myString[index]=="=")
          {
              token+=myString[index];
              index++;
              match("=",token);
          }
          else
          {
              token+=myString[index];
              index++;
              token+=myString[index];
              index++;
              match("-=",token);
          }
          thisNode->type="Assign"+token;

          thisNode->child[thisNode->childNum]=exp1();
          thisNode->childNum++;
    }
    else
    {
        token="";
        token+=myString[index];
        index++;
        token+=myString[index];
        index++;
        match(":=",token);
        thisNode->type="ID_Assign"+token;
        thisNode->child[thisNode->childNum]=regular_expression();
        thisNode->childNum++;
    }
    return thisNode;
}

//read_stmt
TreeNode* Tiny::read_stmt()
{
    TreeNode* thisNode = new TreeNode;
    thisNode->type = "Read";
    /***匹配identifier***/
    token = getToken();
    thisNode->data = token;
    return thisNode;
}

//write_stmt
TreeNode* Tiny::write_stmt()
{
    TreeNode* thisNode = new TreeNode;
    thisNode->type = "Write";
    thisNode->data = "Write";
    thisNode->child[thisNode->childNum]=exp();
    thisNode->childNum++;
    return thisNode;
}

//exp
TreeNode* Tiny::exp()
{
    TreeNode* thisNode = new TreeNode;
    if(myString[index]=="@")
    {
        index++;
    }

    /***是(exp1)***/
    if(myString[index]=="(")
    {
        /***匹配(***/
        token=myString[index];
        index++;
        match("(",token);

        thisNode = exp1();

        /***匹配)***/
        if(myString[index]=="@")
        {
            index++;
        }
        token=myString[index];
        index++;
        match(")",token);
    }

    /***是not exp1***/
    else if(myString[index]=="n")
    {
        /***匹配not***/
        token = getToken();

        match("not",token);

        thisNode->data="not";
        thisNode->type="Op";

        thisNode->child[thisNode->childNum]=exp1();
        thisNode->childNum++;
    }

    /***是simple_exp [ comparison_op simple_exp ]***/
    else
    {
        thisNode = simple_exp();
        if(myString[index]=="@")
        {
            index++;
        }
        if(myString[index]=="<"||myString[index]=="="||myString[index]==">")
        {
            TreeNode* t = comparsion_op();
            t->child[t->childNum]=thisNode;
            t->childNum++;
            t->child[t->childNum]=simple_exp();
            t->childNum++;
            thisNode=t;
        }
    }
    return thisNode;
}

//comparsion_op
TreeNode* Tiny::comparsion_op()
{
    token = "";
    TreeNode* thisNode = new TreeNode;
    thisNode->type = "Op";
    if(myString[index]=="@")
    {
        index++;
    }

    //判断是否为<或者<=或者<>
    if(myString[index]=="<")
    {
        token = myString[index];
        index++;

        if(myString[index]=="="||myString[index]==">")
        {
            token+=myString[index];
            index++;
        }


    }

    //判断是否为>或者>=
    else if(myString[index]==">")
    {
        token = myString[index];
        index++;

        if(myString[index]=="=")
        {
            token+=myString[index];
            index++;
        }
    }

    //判断是否为==

    else if(myString[index]=="="&&myString[index+1]=="=")
    {
        token = myString[index];
        index++;

        token+=myString[index];
            index++;

    }

    else
    {
        //出错
        errorString+="error: Op error";
        return NULL;

    }

    thisNode->data = token;
    return thisNode;

}

//simple_exp
TreeNode* Tiny::simple_exp()
{

    TreeNode* thisNode = term();

    /***匹配addop***/
    if(myString[index]=="@")
    {
        index++;
    }

    while(myString[index]=="+"||myString[index]=="-")
    {

        TreeNode* t= new TreeNode;
        t->data = myString[index];
        index++;
        t->type = "Op";
        t->child[t->childNum]=thisNode;
        t->childNum++;
        t->child[t->childNum]=term();
        t->childNum++;
        thisNode = t;

        if(myString[index]=="@")
        {
            index++;
        }
    }

    return thisNode;

}

//term
TreeNode* Tiny::term()
{

    TreeNode* thisNode = term1();

    /***匹配mulop***/
    if(myString[index]=="@")
    {
        index++;
    }

    while(myString[index]=="*"||myString[index]=="/"||myString[index]=="%")
    {
        qDebug()<<myString[index];

        TreeNode* t= new TreeNode;
        t->data = myString[index];
        index++;
        t->type = "Op";
        t->child[t->childNum]=thisNode;
        t->childNum++;
        t->child[t->childNum]=term1();
        t->childNum++;
        thisNode = t;

        if(myString[index]=="@")
        {
            index++;
        }
    }

    return thisNode;

}

//term1
TreeNode* Tiny::term1()
{

    TreeNode* thisNode = factor();

    /***匹配mulop***/
    if(myString[index]=="@")
    {
        index++;
    }

    while(myString[index]=="^")
    {
        TreeNode* t= new TreeNode;
        t->data = myString[index];
        index++;
        t->type = "Op";
        t->child[t->childNum]=thisNode;
        t->childNum++;
        t->child[t->childNum]=factor();
        t->childNum++;
        thisNode = t;

        if(myString[index]=="@")
        {
            index++;
        }
    }

    return thisNode;

}


//factor
TreeNode* Tiny::factor()
{

    TreeNode* thisNode = new TreeNode;

    /***如果是(exp)***/
    if(myString[index]=="@")
    {
        index++;
    }

    if(myString[index]=="(")
    {
        token = myString[index];
        match(token,"(");
        index++;


        thisNode = exp1();

        if(myString[index]=="@")
        {
            index++;
        }
        token = myString[index];
        match(token,")");
        index++;

    }

    else
    {
        if(myString[index]>="0"&&myString[index]<="9")
        {
          thisNode->type = "Const";
         }

        else
        {
            thisNode->type = "Id";
        }

        token = getToken();
        thisNode->data = token;

    }
    return thisNode;
}


//regular_expression
TreeNode* Tiny::regular_expression()
{

    TreeNode* thisNode = re1();

    /***匹配mulop***/
    if(myString[index]=="@")
    {
        index++;
    }

    while(myString[index]=="|")
    {
        TreeNode* t= new TreeNode;
        t->data = myString[index];
        index++;
        t->type = "Op";
        t->child[t->childNum]=thisNode;
        t->childNum++;
        t->child[t->childNum]=re1();
        t->childNum++;
        thisNode = t;

        if(myString[index]=="@")
        {
            index++;
        }
    }

    return thisNode;
}

//re1
TreeNode* Tiny::re1()
{

    TreeNode* thisNode = re2();

    /***匹配mulop***/
    if(myString[index]=="@")
    {
        index++;
    }

    while(myString[index]=="&")
    {
        TreeNode* t= new TreeNode;
        t->data = myString[index];
        index++;
        t->type = "Op";
        t->child[t->childNum]=thisNode;
        t->childNum++;
        t->child[t->childNum]=re2();
        t->childNum++;
        thisNode = t;

        if(myString[index]=="@")
        {
            index++;
        }
    }

    return thisNode;
}

//re2
TreeNode* Tiny::re2()
{

    TreeNode* thisNode = re();

    if(myString[index]=="@")
    {
        index++;
    }

    if(myString[index]=="#")
    {
        index++;
        TreeNode* t = new TreeNode;
        t->data="#";

        t->type="Op";
        t->child[t->childNum]=thisNode;
        t->childNum++;
        thisNode = t;
    }

    return thisNode;

}

//re
TreeNode* Tiny::re()
{

    TreeNode* thisNode= new TreeNode;
    /***匹配(***/
    if(myString[index]=="@")
    {
        index++;
    }

    if(myString[index]=="(")
    {
        token = myString[index];
        match(token,"(");
        index++;


        thisNode = regular_expression();

        token = myString[index];
        match(token,")");
        index++;

    }

    else
    {
        token = getToken();
        thisNode->type = "Sigma";
        thisNode->data = token;

    }

    return thisNode;


}




/*****打印语法树****/
QString Tiny::PrintTree(TreeNode* t,int deep)
{
    QString tempS;
    //先打印当前节点
    for(int j=0;j<deep;j++)
    {
        tempS+="  ";
    }
    if(t->data==t->type)
    {
        tempS+=t->data;
        tempS+="\n";
    }
    else
    {
        tempS+=t->type;
        tempS+=": ";
        tempS+=t->data;
        tempS+="\n";

    }

    //孩子节点
    for(int i=0;i<t->childNum;i++)
    {
        tempS+=PrintTree(t->child[i],deep+1);
    }

    //再打印相邻节点
    for(int i=0;i<t->siblingNum;i++)
    {
        tempS+=PrintTree(t->sibling[i],deep);
    }

    return tempS;
}
