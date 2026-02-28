#include "PageTurnEtaEstimator.h"

#include <cmath>

PageTurnEtaEstimator::PageTurnEtaEstimator(const float alpha) : smoothingFactor(alpha) {}

void PageTurnEtaEstimator::reset() {
  hasCurrentPageKey = false;
  currentSectionIndex = -1;
  currentPageIndex = -1;
  lastPageTimestampMs = 0UL;
  emaInitialized = false;
  emaPageTurnIntervalMs = 0.0f;
  validTurnCount = 0;
}

void PageTurnEtaEstimator::onPageRendered(const int sectionIndex, const int pageIndex, const unsigned long nowMs) {
  if (!hasCurrentPageKey) {
    hasCurrentPageKey = true;
    currentSectionIndex = sectionIndex;
    currentPageIndex = pageIndex;
    lastPageTimestampMs = nowMs;
    return;
  }

  if (currentSectionIndex == sectionIndex && currentPageIndex == pageIndex) {
    return;
  }

  const unsigned long intervalMs = nowMs - lastPageTimestampMs;

  currentSectionIndex = sectionIndex;
  currentPageIndex = pageIndex;
  lastPageTimestampMs = nowMs;

  if (intervalMs < MIN_VALID_TURN_INTERVAL_MS || intervalMs > MAX_VALID_TURN_INTERVAL_MS) {
    return;
  }

  if (!emaInitialized) {
    emaPageTurnIntervalMs = static_cast<float>(intervalMs);
    emaInitialized = true;
  } else {
    emaPageTurnIntervalMs = (smoothingFactor * static_cast<float>(intervalMs)) +
                            ((1.0f - smoothingFactor) * emaPageTurnIntervalMs);
  }

  validTurnCount++;
}

int PageTurnEtaEstimator::estimateMinutesToEnd(const int remainingPages) const {
  if (remainingPages <= 0) {
    return 0;
  }

  // Cold start: require at least 2 valid page turns before showing ETA.
  if (!emaInitialized || validTurnCount < 2) {
    return -1;
  }

  const float etaMs = emaPageTurnIntervalMs * static_cast<float>(remainingPages);
  const int minutes = static_cast<int>(std::ceil(etaMs / (60.0f * 1000.0f)));
  return minutes < 0 ? 0 : minutes;
}

