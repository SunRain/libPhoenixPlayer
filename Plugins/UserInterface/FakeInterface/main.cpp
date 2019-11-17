#include "MainWindow.h"
#include <QApplication>

#include <QMutex>
#include <QMutexLocker>
#include <QDebug>
#include <QTimer>

//QList<int> list;

//QList<int> *getlist()
//{
//    return &list;
//}


class Tst : public QObject
{
    Q_OBJECT
public:
    Tst() {  }
    virtual ~Tst() {}

    void stop() {
        qDebug()<<"call stop";
        flag = true;
    }

    void loop() {
        while (true) {
            qApp->processEvents();
            if (flag) {
                qDebug()<<"stop loop";
                break;
            }
            qDebug()<<" loop "<<++cnt;
        }
    }

private:
    bool flag = false;
    int cnt = 0;
};


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QMutex mutex;

//    for(int i=0; i<10; ++i) {
//        list.append(i);
//    }

//    QList<int> ll;

//    for (auto it=getlist()->constBegin(); it != getlist()->constEnd(); ++it) {
//         qDebug()<<Q_FUNC_INFO<<*it;
//         ll.append(*it);
//    }
//    qDebug()<<Q_FUNC_INFO<<ll;

    Tst ts;

    QTimer tm;
    tm.setSingleShot(true);
    tm.setInterval(100);
    QObject::connect(&tm, &QTimer::timeout, [&](){
        qDebug()<<"-----time out";
        ts.stop();
    });
    tm.start();
    ts.loop();


    MainWindow w;
    w.show();

    return a.exec();
}

#include "main.moc"

