#pragma once
#include "ApplicationId.g.h"


namespace winrt::VirtualDesktops::implementation
{
    struct ApplicationId : ApplicationIdT<ApplicationId>
    {
        ApplicationId() = default;

        hstring AUMID();
        void AUMID(hstring const& value);
    };
}
