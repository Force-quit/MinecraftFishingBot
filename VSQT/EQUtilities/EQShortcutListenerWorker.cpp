#include "EQShortcutListenerWorker.h"
#include <QThread>
#include <QTimer>
#include <QMapIterator>
#include <Windows.h>

const QString EQShortcutListenerWorker::DEFAULT_SHORTCUT{ "RCONTROL" };
const QVector<int> EQShortcutListenerWorker::DEFAULT_CODE{ 0xA3 };

EQShortcutListenerWorker::EQShortcutListenerWorker()
	: timer(), pressedKeys(), active(), inputChanged()
{
	
}

void EQShortcutListenerWorker::startListening()
{
	pressedKeys.clear();
	if (timer == nullptr)
	{
		timer = new QTimer;
		timer->setSingleShot(true);
		connect(timer, &QTimer::timeout, this, &EQShortcutListenerWorker::emitShortcutSelected);
	}

	// Clear current input buffer
	QMapIterator<int, QString> i(VIRTUAL_KEYS);
	while (i.hasNext()) 
	{
		i.next();
		GetAsyncKeyState(i.key());
	}

	active = true;
	QTimer::singleShot(10, this, &EQShortcutListenerWorker::listenLoop);
}

void EQShortcutListenerWorker::listenLoop()
{
	inputChanged = false;

	QMapIterator<int, QString> i(VIRTUAL_KEYS);
	while (i.hasNext()) 
	{
		i.next();
		if (GetAsyncKeyState(i.key()))
		{
			if (!pressedKeys.contains(i.key()))
			{
				inputChanged = true;
				pressedKeys[i.key()] = i.value();
				timer->start(HOLD_TIME_TO_CONFIRM);
			}
		}
		else if (pressedKeys.contains(i.key()))
		{
			inputChanged = true;
			pressedKeys.remove(i.key());
			if (pressedKeys.size() == 0)
				timer->stop();
			else
				timer->start(HOLD_TIME_TO_CONFIRM);
		}
	}

	if (inputChanged)
	{
		QString textToShow;
		for (QString text : pressedKeys.values())
			textToShow += text + " + ";
		textToShow.chop(3);
		emit shortcutTextChanged(textToShow);
	}

	if (active)
		QTimer::singleShot(10, this, &EQShortcutListenerWorker::listenLoop);
}

void EQShortcutListenerWorker::emitShortcutSelected()
{
	active = false;
	emit shortcutSelected();
	QTimer::singleShot(10, this, &EQShortcutListenerWorker::waitForShortcutRelease);
}

void EQShortcutListenerWorker::waitForShortcutRelease()
{
	for (auto& i : pressedKeys.keys())
	{
		if (!GetAsyncKeyState(i))
		{
			emit shortcutReady(pressedKeys.keys());
			return;
		}
	}

	QTimer::singleShot(10, this, &EQShortcutListenerWorker::waitForShortcutRelease);
}

const QMap<int, QString> EQShortcutListenerWorker::VIRTUAL_KEYS{
	{0x01,	"LBUTTON"},
	{0x02,	"RBUTTON"},
	{0x03,	"CANCEL"},
	{0x04,	"MBUTTON"},
	{0x05,	"XBUTTON1"},
	{0x06,	"XBUTTON2"},
	{0x07,	"Undefined"},
	{0x08,	"BACKSPACE"},
	{0x09,	"TAB"},
	{0x0A,	"Reserved"},
	{0x0B,  "Reserved"},
	{0x0C,	"CLEAR"},
	{0x0D,	"ENTER"},
	{0x0E,	"Undefined"},
	{0x0F,  "Undefined"},
	//{0x10,	"SHIFT"},
	//{0x11,	"CTRL"},
	//{0x12,	"ALT"},
	{0x13,	"PAUSE"},
	{0x14,	"CAPS LOCK"},
	{0x15,	"HANGUL"},
	{0x16,	"IME_ON"},
	{0x17,	"JUNJA"},
	{0x18,	"FINAL"},
	{0x19,	"HANJA/KANJI"},
	{0x1A,	"IME_OFF"},
	{0x1B,	"ESC"},
	{0x1C,	"CONVERT"},
	{0x1D,	"NONCONVERT"},
	{0x1E,	"ACCEPT"},
	{0x1F,	"MODECHANGE"},
	{0x20,	"SPACEBAR"},
	{0x21,	"PAGE UP"},
	{0x22,	"PAGE DOWN"},
	{0x23,	"END"},
	{0x24,	"HOME"},
	{0x25,	"LEFT ARROW"},
	{0x26,	"UP ARROW"},
	{0x27,	"RIGHT ARROW"},
	{0x28,	"DOWN ARROW"},
	{0x29,	"SELECT"},
	{0x2A,	"PRINT"},
	{0x2B,	"EXECUTE"},
	{0x2C,	"PRINT SCREEN"},
	{0x2D,	"INS"},
	{0x2E,	"DEL"},
	{0x2F,	"HELP"},
	{0x30,	"0"},
	{0x31,	"1"},
	{0x32,	"2"},
	{0x33,	"3"},
	{0x34,	"4"},
	{0x35,	"5"},
	{0x36,	"6"},
	{0x37,	"7"},
	{0x38,	"8"},
	{0x39,	"9"},
	{0x3A,	"Undefined"},
	{0x3B,	"Undefined"},
	{0x3C,	"Undefined"},
	{0x3D,	"Undefined"},
	{0x3E,	"Undefined"},
	{0x3F,	"Undefined"},
	{0x40,	"Undefined"},
	{0x41,	"A"},
	{0x42,	"B"},
	{0x43,	"C"},
	{0x44,	"D"},
	{0x45,	"E"},
	{0x46,	"F"},
	{0x47,	"G"},
	{0x48,	"H"},
	{0x49,	"I"},
	{0x4A,	"J"},
	{0x4B,	"K"},
	{0x4C,	"L"},
	{0x4D,	"M"},
	{0x4E,	"N"},
	{0x4F,	"O"},
	{0x50,	"P"},
	{0x51,	"Q"},
	{0x52,	"R"},
	{0x53,	"S"},
	{0x54,	"T"},
	{0x55,	"U"},
	{0x56,	"V"},
	{0x57,	"W"},
	{0x58,	"X"},
	{0x59,	"Y"},
	{0x5A,	"Z"},
	{0x5B,	"LWIN"},
	{0x5C,	"RWIN"},
	{0x5D,	"APPS"},
	{0x5E,	"Reserved"},
	{0x5F,	"SLEEP"},
	{0x60,	"NUMPAD0"},
	{0x61,	"NUMPAD1"},
	{0x62,	"NUMPAD2"},
	{0x63,	"NUMPAD3"},
	{0x64,	"NUMPAD4"},
	{0x65,	"NUMPAD5"},
	{0x66,	"NUMPAD6"},
	{0x67,	"NUMPAD7"},
	{0x68,	"NUMPAD8"},
	{0x69,	"NUMPAD9"},
	{0x6A,	"Multiply"},
	{0x6B,	"Add"},
	{0x6C,	"Separator"},
	{0x6D,	"Subtract"},
	{0x6E,	"Decimal"},
	{0x6F,	"Divide"},
	{0x70,	"F1"},
	{0x71,	"F2"},
	{0x72,	"F3"},
	{0x73,	"F4"},
	{0x74,	"F5"},
	{0x75,	"F6"},
	{0x76,	"F7"},
	{0x77,	"F8"},
	{0x78,	"F9"},
	{0x79,	"F10"},
	{0x7A,	"F11"},
	{0x7B,	"F12"},
	{0x7C,	"F13"},
	{0x7D,	"F14"},
	{0x7E,	"F15"},
	{0x7F,	"F16"},
	{0x80,	"F17"},
	{0x81,	"F18"},
	{0x82,	"F19"},
	{0x83,	"F20"},
	{0x84,	"F21"},
	{0x85,	"F22"},
	{0x86,	"F23"},
	{0x87,	"F24"},
	{0x88,	"Unassigned"},
	{0x89,	"Unassigned"},
	{0x8A,	"Unassigned"},
	{0x8B,	"Unassigned"},
	{0x8C,	"Unassigned"},
	{0x8D,	"Unassigned"},
	{0x8E,	"Unassigned"},
	{0x8F,	"Unassigned"},
	{0x90,	"NUMLOCK"},
	{0x91,	"SCROLL"},
	{0x92,	"OEM specific"},
	{0x93,	"OEM specific"},
	{0x94,	"OEM specific"},
	{0x95,	"OEM specific"},
	{0x96,	"OEM specific"},
	{0x97,	"Unassigned"},
	{0x98,	"Unassigned"},
	{0x99,	"Unassigned"},
	{0x9A,	"Unassigned"},
	{0x9B,	"Unassigned"},
	{0x9C,	"Unassigned"},
	{0x9D,	"Unassigned"},
	{0x9E,	"Unassigned"},
	{0x9F,	"Unassigned"},
	{0xA0,	"LSHIFT"},
	{0xA1,	"RSHIFT"},
	{0xA2,	"LCONTROL"},
	{0xA3,	"RCONTROL"},
	{0xA4,	"LALT"},
	{0xA5,	"RALT"},
	{0xA6,	"Browser Back"},
	{0xA7,	"Browser Forward"},
	{0xA8,	"Browser Refresh"},
	{0xA9,	"Browser Stop"},
	{0xAA,	"Browser Search"},
	{0xAB,	"Browser Favorites"},
	{0xAC,	"BROWSER_HOME"},
	{0xAD,	"Volume Mute"},
	{0xAE,	"Volume Down"},
	{0xAF,	"Volume Up"},
	{0xB0,	"Next Track"},
	{0xB1,	"Previous Track"},
	{0xB2,	"Stop Media"},
	{0xB3,	"MEDIA_PLAY_PAUSE"},
	{0xB4,	"Start Mail"},
	{0xB5,	"Select Media"},
	{0xB6,	"LAUNCH_APP1"},
	{0xB7,	"LAUNCH_APP2"},
	{0xB8,	"Reserved"},
	{0xB9,  "Reserved"},
	{0xBA,	"OEM_1"},
	{0xBB,	"OEM_PLUS"},
	{0xBC,	"OEM_COMMA"},
	{0xBD,	"OEM_MINUS"},
	{0xBE,	"OEM_PERIOD"},
	{0xBF,	"OEM_2"},
	{0xC0,	"OEM_3"},
	{0xC1,	"Reserved"},
	{0xC2,	"Reserved"},
	{0xC3,	"Reserved"},
	{0xC4,	"Reserved"},
	{0xC5,	"Reserved"},
	{0xC6,	"Reserved"},
	{0xC7,	"Reserved"},
	{0xC8,	"Reserved"},
	{0xC9,	"Reserved"},
	{0xCA,	"Reserved"},
	{0xCB,	"Reserved"},
	{0xCC,	"Reserved"},
	{0xCD,	"Reserved"},
	{0xCE,	"Reserved"},
	{0xCF,	"Reserved"},
	{0xD0,	"Reserved"},
	{0xD1,	"Reserved"},
	{0xD2,	"Reserved"},
	{0xD3,	"Reserved"},
	{0xD4,	"Reserved"},
	{0xD5,	"Reserved"},
	{0xD6,	"Reserved"},
	{0xD7,	"Reserved"},
	{0xD8,  "Unassigned"},
	{0xD9,  "Unassigned"},
	{0xDA,  "Unassigned"},
	{0xDB,	"OEM_4"},
	{0xDC,	"OEM_5"},
	{0xDD,	"OEM_6"},
	{0xDE,	"OEM_7"},
	{0xDF,	"OEM_8"},
	{0xE0,	"Reserved"},
	{0xE1,	"OEM specific"},
	{0xE2,	"OEM_102"},
	{0xE3,  "OEM specific"},
	{0xE4,	"OEM specific"},
	{0xE5,	"IME PROCESS"},
	{0xE6,	"OEM specific"},
	{0xE7,	"Unicode"},
	{0xE8,	"Unassigned"},
	{0xE9,  "OEM specific"},
	{0xEA,  "OEM specific"},
	{0xEB,  "OEM specific"},
	{0xEC,  "OEM specific"},
	{0xED,  "OEM specific"},
	{0xEE,  "OEM specific"},
	{0xEF,  "OEM specific"},
	{0xF0,  "OEM specific"},
	{0xF1,  "OEM specific"},
	{0xF2,  "OEM specific"},
	{0xF3,  "OEM specific"},
	{0xF4,  "OEM specific"},
	{0xF5,  "OEM specific"},
	{0xF6,	"Attn"},
	{0xF7,	"CrSel"},
	{0xF8,	"ExSel"},
	{0xF9,	"Erase EOF"},
	{0xFA,	"Play"},
	{0xFB,	"Zoom"},
	{0xFC,	"Reserved"},
	{0xFD,	"PA1"},
	{0xFE,	"Clear"} 
};

EQShortcutListenerWorker::~EQShortcutListenerWorker() {}