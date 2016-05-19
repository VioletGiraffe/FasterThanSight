#pragma once

#include "../textfragment/ctextfragment.h"

#include <QTimer>

#include <vector>

class CReader
{
public:
	enum State {Reading, Paused};

	struct ReaderInterface {
		virtual void displayText(const TextFragment& text) = 0;
		virtual void stateChanged(const State newState) = 0;
	};

	explicit CReader(ReaderInterface* interface);

// Preparation
	void load(const std::vector<TextFragment>& textFragments);
	void loadFromFile(const QString& filePath);

// State
	State state() const;

// Control
	void resumeReading();
	void pauseReading();
	void resetAndStop();

// Settings
	size_t readingSpeed() const;
	void setReadingSpeed(size_t wpm);

private:
	void readNextFragment();

private:
	ReaderInterface * const _interface;

	std::vector<TextFragment> _textFragments;
	size_t                    _position = 0;
	size_t                    _speedWpm = 250;

	State _state = Paused;

	QTimer _readingTimer;
};
