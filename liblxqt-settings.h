/*
    Copyright (C) 2013  Hong Jen yee (PCMan) <pcman.tw@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef liblxqt_settings_H
#define liblxqt_settings_H

extern "C" { // dconf does not do extern "C" properly in its header
#include <dconf/dconf.h>
}

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QStringList>

#include <QSettings>

namespace LxQt {

class Settings: public QObject {
  Q_OBJECT
public:

  explicit Settings(const QString& organization,
                   const QString& application = QString());
  // Settings(QSettings::Scope scope, const QString& organization,
  //          const QString& application = QString());

  // Settings(QSettings::Format format, Scope scope, const QString& organization,
  //          const QString& application = QString());

  // Settings(const QString& fileName, Format format);

  ~Settings();

  void clear();
  void sync();
  QSettings::Status status() const;

  void beginGroup(const QString& prefix);
  void endGroup();
  QString group() const;

  // int beginReadArray(const QString& prefix);
  // void beginWriteArray(const QString& prefix, int size = -1);
  // void endArray();
  // void setArrayIndex(int i);

  QStringList allKeys() const;
  QStringList childKeys() const;
  QStringList childGroups() const;
  bool isWritable() const;

  void setValue(const QString& key, const QVariant& value);
  QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

  void remove(const QString& key);
  bool contains(const QString& key) const;

  void setFallbacksEnabled(bool b);
  bool fallbacksEnabled() const;

  QString fileName() const;
  QSettings::Format format() const;
  QSettings::Scope scope() const;
  QString organizationName() const;
  QString applicationName() const;

  /*
  static void setDefaultFormat(Format format);
  static Format defaultFormat();
  static void setPath(Format format, Scope scope, const QString& path);
  */
private:
  static QString variantToString(const QVariant& v);
  static QVariant stringToVariant(const QString& s);
  static QStringList splitArgs(const QString &s, int idx);

private Q_SLOTS:

private:
  DConfClient* client_;
  QString organizationName_;
  QString applicationName_;
  QByteArray prefix_;
  // int arrayIndex_;
  Q_DISABLE_COPY(Settings)
};

}

#endif // liblxqt_settings_H
