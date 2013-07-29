/*
    Copyright (C) 2013  Hong Jen yee (PCMan) <pcman.tw@gmail.com>
    Copyright (C) 2013  Digia Plc and/or its subsidiary(-ies).

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

#include "liblxqt-settings.h"

#include <QSize>
#include <QPoint>
#include <QRect>

using namespace LxQt;

Settings::Settings(const QString& organization, const QString& application):
  // arrayIndex_(0),
  QObject() {

  prefix_ = "/org/";
  prefix_ += organization.toLatin1();
  prefix_ += "/";
  if(application.isEmpty()) {
    prefix_ += application.toLatin1();
    prefix_ += "/";
  }

  g_type_init();
  client_ = dconf_client_new();
}

Settings::~Settings() {
  if(client_)
    g_object_unref(client_);
}

QStringList Settings::allKeys() const {
  QStringList result;

  return result;
}

QString Settings::applicationName() const {
  return applicationName_;
}

void Settings::beginGroup(const QString& prefix) {
  prefix_.append(prefix);
  prefix_.append('/');

  qDebug("beginGroup, current prefix: %s", prefix_.constData());
}

/*
int Settings::beginReadArray(const QString& prefix) {
}

void Settings::beginWriteArray(const QString& prefix, int size) {
}

void Settings::endArray() {
}

void Settings::setArrayIndex(int i) {
}
*/

QStringList Settings::childGroups() const {
  QStringList result;
  int len;
  char** keys = dconf_client_list(client_, prefix_.constData(), &len);

  if(keys) {
    for(char** key = keys; *key; ++key) {
      if(dconf_is_dir(*key, NULL))
        result.append(*key);
    }

    g_strfreev(keys);
  }

  return result;
}

QStringList Settings::childKeys() const {
  QStringList result;
  int len;
  char** keys = dconf_client_list(client_, prefix_.constData(), &len);

  if(keys) {
    for(char** key = keys; *key; ++key) {
      if(!dconf_is_key(*key, NULL)) // if this is not a dir
        result.append(*key);
    }

    g_strfreev(keys);
  }

  return result;
}

void Settings::clear() {

}

bool Settings::contains(const QString& key) const {
  return false;
}

void Settings::endGroup() {
  if(prefix_.length() > 2) {
    int sep = prefix_.lastIndexOf('/', prefix_.length() - 2);
    if(sep != -1) {
      prefix_ = prefix_.left(sep + 1);
    }
    qDebug("endGroup, current prefix: %s", prefix_.constData());
  }
}

bool Settings::fallbacksEnabled() const {
  return false;
}

QString Settings::fileName() const {
  return QString();
}

QString Settings::group() const {
  if(prefix_.length() > 2) {
    int sep = prefix_.lastIndexOf('/', prefix_.length() - 2);
    if(sep != -1) {
      ++sep;
      return prefix_.mid(sep, prefix_.length() - sep - 1);
    }
  }
  return QString();
}

bool Settings::isWritable() const {
  return bool(dconf_client_is_writable(client_, prefix_.constData()));
}

QString Settings::organizationName() const {
  return organizationName_;
}

void Settings::remove(const QString& key) {
  QByteArray path = prefix_ + key.toLatin1();
  dconf_client_write_sync(client_, path.constData(), NULL, NULL, NULL, NULL);
}

QSettings::Scope Settings::scope() const {
  return QSettings::UserScope; // FIXME: will this cause problems?
}

void Settings::setFallbacksEnabled(bool b) {
  qWarning("setFallbacksEnabled(bool b) is not supported.");
}

void Settings::setValue(const QString& key, const QVariant& value) {
  GError* err = NULL;
  QString str = variantToString(value);
  QByteArray path = prefix_ + key.toLatin1();
  GVariant* val = g_variant_new_string(str.toUtf8().constData());
  g_variant_ref_sink(val);
  qDebug("setValue: path: %s, value: %s", path.constData(), g_variant_get_string(val, NULL));
  dconf_client_write_fast(client_, path.constData(), val, &err);
  if(val)
    g_variant_unref(val);

  if(err) {
    qDebug("error: %s", err->message);
    g_error_free(err);
  }
}

QSettings::Status Settings::status() const {
  return QSettings::NoError; // FIXME: should we omit errors?
}

void Settings::sync() {
  dconf_client_sync(client_);
}

// taken from Qt source code (src/corelib/io/qsettings.cpp).
// Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
// license: LGPL
QString Settings::variantToString(const QVariant& v) {
  QString result;

  switch(v.type()) {
    case QVariant::Invalid:
      result = QLatin1String("@Invalid()");
      break;

    case QVariant::ByteArray: {
      QByteArray a = v.toByteArray();
      result = QLatin1String("@ByteArray(");
      result += QString::fromLatin1(a.constData(), a.size());
      result += QLatin1Char(')');
      break;
    }

    case QVariant::String:
    case QVariant::LongLong:
    case QVariant::ULongLong:
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::Bool:
    case QVariant::Double:
    case QVariant::KeySequence: {
      result = v.toString();

      if(result.startsWith(QLatin1Char('@')))
        result.prepend(QLatin1Char('@'));

      break;
    }
#ifndef QT_NO_GEOM_VARIANT
    case QVariant::Rect: {
      QRect r = qvariant_cast<QRect>(v);
      result += QLatin1String("@Rect(");
      result += QString::number(r.x());
      result += QLatin1Char(' ');
      result += QString::number(r.y());
      result += QLatin1Char(' ');
      result += QString::number(r.width());
      result += QLatin1Char(' ');
      result += QString::number(r.height());
      result += QLatin1Char(')');
      break;
    }
    case QVariant::Size: {
      QSize s = qvariant_cast<QSize>(v);
      result += QLatin1String("@Size(");
      result += QString::number(s.width());
      result += QLatin1Char(' ');
      result += QString::number(s.height());
      result += QLatin1Char(')');
      break;
    }
    case QVariant::Point: {
      QPoint p = qvariant_cast<QPoint>(v);
      result += QLatin1String("@Point(");
      result += QString::number(p.x());
      result += QLatin1Char(' ');
      result += QString::number(p.y());
      result += QLatin1Char(')');
      break;
    }
#endif // !QT_NO_GEOM_VARIANT

    default: {
#ifndef QT_NO_DATASTREAM
      QByteArray a;
      {
        QDataStream s(&a, QIODevice::WriteOnly);
        s.setVersion(QDataStream::Qt_4_0);
        s << v;
      }

      result = QLatin1String("@Variant(");
      result += QString::fromLatin1(a.constData(), a.size());
      result += QLatin1Char(')');
#else
      Q_ASSERT(!"QSettings: Cannot save custom types without QDataStream support");
#endif
      break;
    }
  }

  return result;
}


// taken from Qt source code (src/corelib/io/qsettings.cpp).
// Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
// license: LGPL
QVariant Settings::stringToVariant(const QString& s) {
  if(s.startsWith(QLatin1Char('@'))) {
    if(s.endsWith(QLatin1Char(')'))) {
      if(s.startsWith(QLatin1String("@ByteArray("))) {
        return QVariant(s.toLatin1().mid(11, s.size() - 12));
      }
      else if(s.startsWith(QLatin1String("@Variant("))) {
#ifndef QT_NO_DATASTREAM
        QByteArray a(s.toLatin1().mid(9));
        QDataStream stream(&a, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_4_0);
        QVariant result;
        stream >> result;
        return result;
#else
        Q_ASSERT(!"QSettings: Cannot load custom types without QDataStream support");
#endif
#ifndef QT_NO_GEOM_VARIANT
      }
      else if(s.startsWith(QLatin1String("@Rect("))) {
        QStringList args = splitArgs(s, 5);

        if(args.size() == 4)
          return QVariant(QRect(args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt()));
      }
      else if(s.startsWith(QLatin1String("@Size("))) {
        QStringList args = splitArgs(s, 5);

        if(args.size() == 2)
          return QVariant(QSize(args[0].toInt(), args[1].toInt()));
      }
      else if(s.startsWith(QLatin1String("@Point("))) {
        QStringList args = splitArgs(s, 6);

        if(args.size() == 2)
          return QVariant(QPoint(args[0].toInt(), args[1].toInt()));

#endif
      }
      else if(s == QLatin1String("@Invalid()")) {
        return QVariant();
      }

    }

    if(s.startsWith(QLatin1String("@@")))
      return QVariant(s.mid(1));
  }

  return QVariant(s);
}

// taken from Qt source code (src/corelib/io/qsettings.cpp).
// Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
// license: LGPL
QStringList Settings::splitArgs(const QString &s, int idx)
{
    int l = s.length();
    Q_ASSERT(l > 0);
    Q_ASSERT(s.at(idx) == QLatin1Char('('));
    Q_ASSERT(s.at(l - 1) == QLatin1Char(')'));

    QStringList result;
    QString item;

    for (++idx; idx < l; ++idx) {
        QChar c = s.at(idx);
        if (c == QLatin1Char(')')) {
            Q_ASSERT(idx == l - 1);
            result.append(item);
        } else if (c == QLatin1Char(' ')) {
            result.append(item);
            item.clear();
        } else {
            item.append(c);
        }
    }

    return result;
}

QVariant Settings::value(const QString& key, const QVariant& defaultValue) const {
  QByteArray path = prefix_ + key.toLatin1();
  qDebug("value(), path: %s", path.constData());
  GVariant* val = dconf_client_read(client_, path.constData());
  if(val) {
    const char* str = g_variant_get_string(val, NULL);
    if(str) {
      QVariant qv = stringToVariant(str);
      // g_variant_unref(val);
      return qv;
    }
    // g_variant_unref(val);
  }
  return defaultValue;
}
