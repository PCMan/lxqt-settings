#include <QCoreApplication>
#include "liblxqt-settings.h"


int main(int argc, char** argv) {
  QCoreApplication app(argc, argv);
  
  LxQt::Settings settings("lxde", "setting_test");

  settings.beginGroup("testGroup");
  QVariant v = settings.value("Test");
  qDebug("value: %d", v.isValid());
  settings.endGroup();

  settings.beginGroup("testGroup2");
  settings.setValue("write TestStr", "Test");
  settings.setValue("write TestInt", 123);
  settings.setValue("write TestBool", bool(true));
  settings.setValue("write TestFloat", 123.45);
  qDebug("group: %s", settings.group().toLatin1().constData());
  settings.endGroup();

  settings.sync();

  settings.beginGroup("testGroup2");
  qDebug("read TestStr: %s", settings.value("TestStr").toString().toUtf8().constData());
  qDebug("read TestInt: %d", settings.value("TestInt").toInt());
  qDebug("read TestBool: %d", settings.value("TestBool").toBool());
  qDebug("read TestFloat: %f", settings.value("TestFloat").toFloat());
  settings.endGroup();

  return app.exec();
}
