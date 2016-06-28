#pragma once

#include "../text/ctextfragment.h"

#include <map>
#include <vector>

class CPauseHandler
{
public:
	struct OnPauseValuesChangedListener {
		virtual void onPauseScalingValuesChanged() = 0;
	};

	void addOnPauseValuesChangedListener(OnPauseValuesChangedListener* listener);

	static CPauseHandler& instance();

	float pauseFactorForDelimiter(const TextFragment::Delimiter delimiter) const;
	void updatePauseFactor(const TextFragment::Delimiter delimiter, const float factor);
	const std::map<TextFragment::Delimiter, float /*pauseFactor*/> pauseScalingValues() const;

	void loadFromSettings();
	void storeToSettings() const;

private:
	CPauseHandler();

	void sendPauseScalingValuesChangedNotification() const;

private:
	std::map<TextFragment::Delimiter, float /*pauseFactor*/> _pauseForDelimiter;
	std::vector<OnPauseValuesChangedListener*> _pauseChangedNotificationListeners;
};
