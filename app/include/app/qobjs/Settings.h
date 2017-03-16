#ifndef SETTINGS_H_I3SW6BOJ
#define SETTINGS_H_I3SW6BOJ

#include <QSettings>

class Settings : public QSettings
{
	Q_OBJECT

	public:
	explicit Settings(QObject *parent = nullptr)
		: QSettings(
			QSettings::IniFormat,
			QSettings::UserScope,
			//QCoreApplication::instance()->organizationName(),
			//QCoreApplication::instance()->applicationName(),
			"", "",
			parent
		)
	{}

	~Settings() = default;

	Q_INVOKABLE inline void setValue(const QString& key, const QVariant& value) { QSettings::setValue(key, value); }
	Q_INVOKABLE inline QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const { return QSettings::value(key, defaultValue); }
};

#endif /* end of include guard: SETTINGS_H_I3SW6BOJ */

/* vim: set ts=4 sw=4 sts=4 expandtab ffs=unix,dos : */
