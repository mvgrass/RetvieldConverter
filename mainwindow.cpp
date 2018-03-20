#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTextStream>
#include <QDir>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QByteArray>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void solve(QString dir,QString &l){

    QDir Curr_dir(dir);
    QFileInfoList files = Curr_dir.entryInfoList();

    foreach(QFileInfo file,files){
        if(file.isDir()&&file.absoluteFilePath()!=dir&&file.absolutePath()==dir){
            solve(QString(file.absoluteFilePath()),l);
        }else if(file.isFile()&&(file.fileName().right(3)=="RAW"||file.fileName().right(3)=="raw"||file.fileName().right(3)=="RBG"||file.fileName().right(3)=="rbg"||file.fileName().right(3)=="RK1"||file.fileName().right(3)=="rk1"||file.fileName().right(3)=="RSM"||file.fileName().right(3)=="rsm")){
            bool exist = false;
            foreach (QFileInfo file1, files){
                if(file1.fileName()==(file.fileName()+".dat")){
                    exist = true;
                    break;
                }
            }

            if(!exist){
                QFile f(file.absoluteFilePath());
                if(!f.open(QFile::ReadOnly)){
                    l+="Ошибка чтения "+file.absoluteFilePath()+"\n";
                }
                QByteArray data;
                QFile f1(file.absoluteFilePath()+".dat");
                if(f1.open(QFile::WriteOnly)){
                    data = f.readAll();
                    f.close();
                    QTextStream stream(&f1);
                    stream.setFieldWidth(10);
                    stream.setPadChar(' ');
                    stream.setFieldAlignment(QTextStream::AlignLeft);

                    QString t1;
                    QString t2;
                    QString t3;
                    QString t4;
                    bool ok;

                    QString initialCorner;
                    t1 = QString::number((unsigned char)data.at(611),16);
                    t2 = QString::number((unsigned char)data.at(610),16);
                    t3 = QString::number((unsigned char)data.at(609),16);
                    t4 = QString::number((unsigned char)data.at(608),16);
                    if(t2.size()==1)
                        t2="0"+t2;
                    if(t3.size()==1)
                        t3="0"+t3;
                    if(t4.size()==1)
                        t4="0"+t4;
                    t1 += t2+t3+t4;
                    initialCorner = QString::number((double)(t1.toLongLong(&ok,16))/10000);
                    if(initialCorner.indexOf(".")==-1)
                        initialCorner+=".";
                    for(int i  = initialCorner.size()-initialCorner.indexOf(".");i<5;i++)
                        initialCorner+="0";

                    QString nstr;
                    t1 = QString::number((unsigned char)data.at(626),16);
                    t2 = QString::number((unsigned char)data.at(625),16);
                    t3 = QString::number((unsigned char)data.at(624),16);
                    if(t2.size()==1)
                        t2="0"+t2;
                    if(t3.size()==1)
                        t3="0"+t3;//(?)
                    nstr = t1+t2+t3;
                    int n = nstr.toLongLong(&ok,16);

                    QString step;
                    t1 = QString::number((unsigned char)data.at(603),16);
                    t2 = QString::number((unsigned char)data.at(602),16);
                    t3 = QString::number((unsigned char)data.at(601),16);
                    t4 = QString::number((unsigned char)data.at(600),16);
                    if(t2.size()==1)
                        t2="0"+t2;
                    if(t3.size()==1)
                        t3="0"+t3;
                    if(t4.size()==1)
                        t4="0"+t4;
                    t1+= t2+t3+t4;
                    step = QString::number((double)(t1.toLongLong(&ok,16))/10000);
                    for(int i  = step.size()-step.indexOf(".");i<6;i++)
                        step+="0";

                    QString finishCorner = QString::number(initialCorner.toDouble()+(n-1)*step.toDouble());
                    if(finishCorner.indexOf(".")==-1)
                        finishCorner+=".0";
                    for(int i = finishCorner.size() - finishCorner.indexOf(".");i<5;i++)
                        finishCorner+="0";

                    stream<<initialCorner<<step<<finishCorner;
                    stream.setFieldWidth(2);
                    stream<<"\r\n";
                    stream.setFieldWidth(10);

                    int counter = 0;
                    for(int i = 968;i<data.size();i=i+4){
                        QString t1 = QString::number((unsigned char)data.at(i+1),16);
                        QString t2 = QString::number((unsigned char)data.at(i),16);
                        if(t2.size()==1)
                            t2="0"+t2;
                        QString currentIntension = t1+t2;
                        if(counter<4){
                            stream<<currentIntension.toInt(&ok,16);
                            counter++;
                        }else{
                            stream<<currentIntension.toInt(&ok,16);
                            stream.setFieldWidth(2);
                            stream<<"\r\n";
                            stream.setFieldWidth(10);
                            counter = 0;
                        }
                    }
                    f1.close();
                    l+=file.absoluteFilePath()+".dat был записан\n";
                }else{
                    l+="Не удалось записать "+file.absoluteFilePath()+".dat\n";
                }
            }else{
                l+=file.absoluteFilePath()+".dat уже существует\n";
            }
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString dir  = QFileDialog::getExistingDirectory(this,tr("Open Directory"),"/home",QFileDialog::ShowDirsOnly);

    if(dir!=""){
        ui->label->setText("Обработка файлов...");
        QString log = "";
        solve(dir,log);
        ui->textBrowser->setText(log);
        ui->label->setText("Обработка завершена. ");
    }

}
