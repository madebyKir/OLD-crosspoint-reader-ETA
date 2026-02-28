#pragma once

#include <stdint.h>

class PageTurnEtaEstimator {
 public:
  static constexpr unsigned long MIN_VALID_TURN_INTERVAL_MS = 800UL;
  static constexpr unsigned long MAX_VALID_TURN_INTERVAL_MS = 10UL * 60UL * 1000UL;
  static constexpr float DEFAULT_ALPHA = 0.125f;

  explicit PageTurnEtaEstimator(float alpha = DEFAULT_ALPHA);

  void reset();
  void onPageRendered(int sectionIndex, int pageIndex, unsigned long nowMs);
  int estimateMinutesToEnd(int remainingPages) const;

 private:
  bool hasCurrentPageKey = false;
  int currentSectionIndex = -1;
  int currentPageIndex = -1;
  unsigned long lastPageTimestampMs = 0UL;

  bool emaInitialized = false;
  float emaPageTurnIntervalMs = 0.0f;
  int validTurnCount = 0;
  float smoothingFactor = DEFAULT_ALPHA;
};

