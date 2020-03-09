#include "pch.h"
#include "ViewTracker.hpp"

void ViewTracker::TrackView(std::wstring_view aumid, HWND hwnd)
{
	if (hwnd != 0) {
		knownHwnds_.push_back(hwnd);
	} else {
		knownAumids_.push_back(aumid.data());
	}
}

void ViewTracker::RemoveView(std::wstring_view aumid, HWND hwnd)
{
	if (hwnd != 0) {
		knownHwnds_.remove(hwnd);
	} else {
		knownAumids_.remove(aumid.data());
	}
}

bool ViewTracker::IsViewKnown(std::wstring_view aumid, HWND hwnd)
{
	if (hwnd != 0) {
		return std::find(knownHwnds_.begin(), knownHwnds_.end(), hwnd) != knownHwnds_.end();
	}
	else {
		return std::find(knownAumids_.begin(), knownAumids_.end(), aumid) != knownAumids_.end();
	}
}


// Exempt views are typically system views, e.g. RunDialog, that shouldn't be tracked
bool ViewTracker::IsViewExempt(std::wstring& aumid)
{
	return std::find(exemptViews_.begin(), exemptViews_.end(), aumid) != exemptViews_.end();
}


