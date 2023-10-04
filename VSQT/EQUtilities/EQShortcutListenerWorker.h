#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>
#include <QTimer>

class EQShortcutListenerWorker : public QObject
{
	Q_OBJECT

public:
	EQShortcutListenerWorker();
	~EQShortcutListenerWorker();

	const static QVector<int> DEFAULT_CODE;
	const static QString DEFAULT_SHORTCUT;

public slots:
	void startListening();

signals:
	void shortcutTextChanged(const QString newShortcutText);
	void shortcutSelected();
	void shortcutReady(QVector<int> virtualKeys);

private slots:
	void listenLoop();
	void emitShortcutSelected();
	void waitForShortcutRelease();

private:
	static const QMap<int, QString> VIRTUAL_KEYS;
	static const unsigned short HOLD_TIME_TO_CONFIRM{ 2000 };

	bool inputChanged;
	bool active;
	QMap<int, QString> pressedKeys;
	QTimer* timer;
};