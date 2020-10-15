
 #include <QtGui>

 #include "mainwindow.h"

 int main(int argv, char *args[])
 {
     Q_INIT_RESOURCE(tv14);

     QApplication app(argv, args);
     MainWindow mainWindow;
     return app.exec();
 }
