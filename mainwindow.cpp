#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextCodec>
#include <QCoreApplication>
#include <QTextStream>
#include "tiny.h"

Tiny myTINY;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{//打开源程序，读取文件内容并展示在文本框中
    ui->textEdit->clear();
    QTextCodec *codeC =QTextCodec::codecForName("UTF-8");
    QString file_path = QFileDialog::getOpenFileName(this,"选择一个源程序", ".", "Text(*.txt);;all(*.*)");
    QFile file(file_path);
    if(!file.open(QIODevice::ReadOnly)) return;
    QTextStream in(&file);
    //读取文件内容
    QByteArray array = file.readAll();
    ui->textEdit->append(codeC->toUnicode(array));//展示文件内容
    file.close();
}

void MainWindow::on_pushButton_2_clicked()
{//保存源程序，将文本框中的内容写入用户指定的文件中
        QString file_path = QFileDialog::getSaveFileName(this,"保存文件","","Text(*.txt);;all(*.*)");
        QFile f1(file_path);
        if(!f1.open(QIODevice::Append)){
            QMessageBox::warning(this,tr("错误"),tr("打开文件失败,数据保存失败"));
            return ;
        }
        else{
            QTextStream stream(&f1);
            //设置写入文件的编码
            stream.setCodec("GBK");
            QString line=ui->textEdit->toPlainText();
            stream <<line;
        }
        f1.close();
}

void MainWindow::on_pushButton_3_clicked()
{//生成语法树，读取源程序文本框中的内容并生成语法树
    //读取文本框中的内容
    QString tempString = ui->textEdit->toPlainText();
    //生成语法树
    myTINY.init();
    myTINY.setString(tempString);
    myTINY.program();

}

void MainWindow::on_pushButton_4_clicked()
{//查看语法树，将生成的语法树展示在文本框中
    QString tempS = myTINY.PrintTree(myTINY.root,0);
    ui->textEdit_2->setText(tempS);
}
