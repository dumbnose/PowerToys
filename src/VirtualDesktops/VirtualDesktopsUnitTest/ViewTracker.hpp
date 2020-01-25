#pragma once
class ViewTracker
{
public:
	void TrackView(std::wstring_view aumid, HWND hwnd);
	void RemoveView(std::wstring_view aumid, HWND hwnd);
	bool IsViewKnown(std::wstring_view aumid, HWND hwnd);
	bool IsViewExempt(std::wstring& aumid);

private:
	std::list<std::wstring> knownAumids_;
	std::list<HWND> knownHwnds_;
	std::list<std::wstring> exemptViews_ = { L"", L"Microsoft.Windows.Shell.RunDialog" };
};

