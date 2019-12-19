#include "pch.h"
#include "ApplicationViewManager.hpp"

//IFACEMETHODIMP AppViewListener::OnApplicationViewChanged(
//    _In_ IApplicationView* appView,
//    _In_ APPLICATION_VIEW_CHANGE appViewChange,
//    _In_opt_ IUnknown* /*eventArgs*/) noexcept try
//{
//    wil::com_ptr<IAppViewExecutionData> appViewExecutionData;
//    switch (appViewChange)
//    {
//    case APPLICATION_VIEW_CHANGE::AVC_ADDED:
//    case APPLICATION_VIEW_CHANGE::AVC_SHOW_IN_SWITCHERS:
//    {
//        appViewExecutionData = TryCreateAppExecutionDataForAppState(AppViewExecutionState::AppViewExecutionState_Started, appView);
//    }
//    break;
//
//    case APPLICATION_VIEW_CHANGE::AVC_REMOVED:
//    case APPLICATION_VIEW_CHANGE::AVC_REMOVED_FROM_SWITCHERS:
//    case APPLICATION_VIEW_CHANGE::AVC_FORGOTTEN:
//    {
//        appViewExecutionData = TryCreateAppExecutionDataForAppState(AppViewExecutionState::AppViewExecutionState_Closed, appView);
//    }
//    break;
//
//    case APPLICATION_VIEW_CHANGE::AVC_FOCUS:
//    {
//        appViewExecutionData = TryCreateAppExecutionDataForAppState(AppViewExecutionState::AppViewExecutionState_Focused, appView);
//    }
//    break;
//
//    case APPLICATION_VIEW_CHANGE::AVC_UNFOCUS:
//    {
//        appViewExecutionData = TryCreateAppExecutionDataForAppState(AppViewExecutionState::AppViewExecutionState_Unfocused, appView);
//    }
//    break;
//
//    case APPLICATION_VIEW_CHANGE::AVC_WINDOW_CHANGED:
//    {
//        appViewExecutionData = TryCreateAppExecutionDataForAppState(AppViewExecutionState::AppViewExecutionState_WindowChanged, appView);
//    }
//    break;
//    }
//
//    if (appViewExecutionData)
//    {
//        ListenerTelemetrySignalTracker telemetrySignalTracker;
//        RETURN_IF_FAILED(m_arbiter->HandleListenerData(appViewExecutionData.get()));
//        telemetrySignalTracker.SetSuccess();
//    }
//
//    return S_OK;
//}