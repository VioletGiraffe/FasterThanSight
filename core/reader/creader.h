#pragma once

#include "../text/cstructuredtext.h"
#include "cpausehandler.h"
#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include <QTimer>
RESTORE_COMPILER_WARNINGS

#include <vector>

class CReader : private CPauseHandler::OnPauseValuesChangedListener
{
public:
	struct TextFragmentWithPause {
		TextFragment _textFragment;
		size_t _pauseAfter;
	};

	enum State {Reading, Paused};

	struct ReaderInterface {
		virtual void updateDisplay(const size_t currentTextFragmentIndex) = 0;
		virtual void updateInfo() = 0; // Time remaining etc.
		virtual void stateChanged(const State newState) = 0;
	};

	explicit CReader(ReaderInterface* interface);

// Preparation
	void load(const CStructuredText& text);
	bool loadFromFile(const QString& filePath);

	void setClearScreenAfterSentenceEnd(bool clear);
	void setLongWordPauseScaling(unsigned lengthThreshold, float scaleFactor);

// State
	State state() const;
	QString filePath() const;
	size_t position() const; // Current word index
	size_t totalNumWords() const;
	long double progress() const; // [0.0; 1.0]

	struct ChapterProgress {

		inline int progressPercentage() const // [0, 100]
		{
			return totalNumWords > 0 ? wordsRead * 100 / totalNumWords : 0;
		}

		size_t wordsRead;
		size_t totalNumWords;
	};

	size_t timeRemainingSeconds() const;
	const ChapterProgress currentChapterProgress() const;
	size_t currentChapterTimeRemainingSeconds() const;

	// Notify the client of current reader state via ReaderInterface
	void updateInterface() const;

// Data
	inline TextFragmentWithPause textFragment(const size_t fragmentIndex) const {
		assert_and_return_r(_text.totalFragmentsCount() == _pauseForFragment.size(), (TextFragmentWithPause{TextFragment(), 0}));
		const TextFragment& fragment = _text.fragment(fragmentIndex);
		return {fragment, _pauseForFragment[fragmentIndex]};
	}

	const CStructuredText& text() const;

// Control
	void resumeReading();
	void pauseReading();
	void togglePause();
	void resetAndStop();
	// Specify 0-based word index to go to
	void goToWord(size_t wordIndex);

	void toPreviousChapter();
	void toPreviousParagraph();
	void toNextParagraph();
	void toNextChapter();

// Settings
	size_t readingSpeed() const;
	void setReadingSpeed(size_t wpm);

private:
	void readNextFragment();

	// Calculates the pause after the specific fragment in ms
	size_t pauseForFragment(const TextFragment& fragment) const;
	// Recalculate all the pauses for the entire text
	void updatePauseValues();

	void onPauseScalingValuesChanged() override;

private:
	ReaderInterface * const _interface;

	QString _filePath;

	CStructuredText     _text;
	std::vector<size_t> _pauseForFragment;

	size_t              _position = 0;
	bool                _currentWordRead = false;

	size_t              _speedWpm = 250;

	State _state = Paused;

	QTimer _readingTimer;

// Pause-related stuff
	bool _clearScreenAfterSentenceEnd = false;
	unsigned _longWordThreshold = 7;
	float _longWordDelayScaleFactor = 1.1f;
};
